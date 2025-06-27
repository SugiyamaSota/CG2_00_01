#include "DirectXCommon.h"
#include<cassert>
#include"../function/Utility.h"
#include<filesystem>

// 静的メンバ変数の実体を定義
DirectXCommon* DirectXCommon::instance_ = nullptr;

// GetInstance メソッドの実装
DirectXCommon* DirectXCommon::GetInstance(HINSTANCE hInstance, int32_t kClientWidth, int32_t ClientHeight) {
	if (instance_ == nullptr) {
		// 初回呼び出し時のみインスタンスを生成
		instance_ = new DirectXCommon(hInstance, kClientWidth, ClientHeight);
		instance_->Initialize(); // 必要であればInitializeを呼び出す
	}
	return instance_;
}
void DirectXCommon::DestroyInstance() { // ★追加★
    delete instance_; // シングルトンインスタンスを解放
    instance_ = nullptr;
}

LRESULT CALLBACK DirectXCommon::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LONG WINAPI DirectXCommon::ExportDump(EXCEPTION_POINTERS* exception) {
	//時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//processID(このexeのid)とクラッシュの発生したthreadidを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//ほかに関連付けられているSEH例外ハンドラがあれば実行。通常はprocessを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}

Microsoft::WRL::ComPtr<ID3D12Resource>  DirectXCommon::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(resource.GetAddressOf()));
	assert(SUCCEEDED(hr));

	return resource;
}

// コンストラクタ（privateに変更）
DirectXCommon::DirectXCommon(HINSTANCE hInstance, int32_t clientWidth, int32_t clientHeight) {
	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;

	RECT wrc = { 0,0,clientWidth ,clientHeight };

	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ログのディレクトリを用意
	std::filesystem::create_directory("logs");

	// ダンプファイルの設定
	SetUnhandledExceptionFilter(ExportDump);

	WNDCLASS wc{};
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	//インスタンスハンドル
	wc.hInstance = hInstance;
	//カーソル
	wc.hCursor = (LoadCursor(nullptr, IDC_ARROW));

	//ウィンドウクラスを登録する
	RegisterClass(&wc);

	//ウィンドウの生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	// Initializeメソッドに移動する初期化処理をここに書いても良いが、
	// シングルトン化に伴いInitialize()を呼び出す形がより一般的。
	// ここではコンストラクタで全て完結させているため、Initialize()は空で問題ない。
	// 必要に応じて、Initialize()に移動してください。
}

// 初期化メソッド（GetInstanceから呼び出されるか、必要に応じて手動で呼び出す）
void DirectXCommon::Initialize() {
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		//デバックレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPUでもチェックを行う
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	CreateDevice();
	CreateCommand();
	CreateSwapChain();
	CreateFence();

	// PSOクラスを使用
	pso.Initialize(
		device_.Get(),
		// logStream, // LogStreamをPSOに渡す場合
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_D24_UNORM_S8_UINT
	);

	CreateDepth();
	CreateLight();

	viewport_.Width = float(clientWidth_);
	viewport_.Height = float(clientHeight_);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	scissorRect_.left = 0;
	scissorRect_.right = LONG(clientWidth_);
	scissorRect_.top = 0;
	scissorRect_.bottom = LONG(clientHeight_);

}


DirectXCommon::~DirectXCommon() {
	if (fenceEvent_ != nullptr) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}
	CoUninitialize();
	CloseWindow(hwnd_);
}

void DirectXCommon::NewFeame() {

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//描画先のRTVとDSVを設定
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);
	commandList_->SetGraphicsRootSignature(pso.GetRootSignature());
	commandList_->SetPipelineState(pso.GetPipelineState());//PSOを設定 

	//TransitionBarrierの設定
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	//遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//蒼っぽい色。RGBAの順番
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList_->RSSetViewports(1, &viewport_);//Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_);//Scirssorを設定

	//光
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_.Get()->GetGPUVirtualAddress());
}

void DirectXCommon::EndFrame() {

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	//コマンドリストの内容を確定させる。すべのコマンドを積んでからcloseする
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));
	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知
	swapChain_->Present(1, 0);

	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//Fenceの値が指定したsignal値のたどり着いてるか確認
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_.Get()->GetCompletedValue() < fenceValue_) {
		//指定したsignakにたどり着いてないので、たどり着くまで待つようにイベントを設定する
		fence_.Get()->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
	//ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

void DirectXCommon::WaitAndResetCommandList() {
	HRESULT hr;

	// commandListをcloseし、キックする
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// Fenceの値をインクリメント
	UINT64 currentFenceValue = fenceValue_; // シグナルを送る前のフェンス値
	IncrementFencevalue(); // 次のフレーム用のフェンス値に更新

	// コマンドキューにFenceのシグナルを送る
	hr = commandQueue_->Signal(fence_.Get(), fenceValue_); // 更新されたフェンス値をシグナルに使う
	assert(SUCCEEDED(hr));

	// 指定したsignalにたどり着いてないので、たどり着くまで待つようにイベントを設定する
	// GetCompletedValue()は、最後に完了したコマンドのフェンス値を取得する
	if (fence_->GetCompletedValue() < fenceValue_) { // currentFenceValueではなく、signalに送ったfenceValue_と比較する
		hr = fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		assert(SUCCEEDED(hr));
		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateDevice() {
	//DXGIファクトリーの作成
	dxgiFactory_ = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	//使用するアダプタ用の変数。最初にnullptrを入れる
	useAdapter_ = nullptr;
	//いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter_.GetAddressOf())) != DXGI_ERROR_NOT_FOUND; ++i) {
		//あだぷたーの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用!

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力、
			//Log(logStream, ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;
	}
	//適切なアダプタがないので起動不可
	assert(useAdapter_ != nullptr);

	device_ = nullptr;
	//昨日レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLvels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLvels); ++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLvels[i], IID_PPV_ARGS(device_.GetAddressOf())); // Get() と GetAddressOf() を使用
		//指定した昨日レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			//静背できたのでる＾ぷを抜ける
			//Log(logStream, std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイス生成がうまくいかず起動できない
	assert(device_ != nullptr);
	//Log(logStream, "Complete create D3D12Device!!!\n");//初期化完了ログ

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//解放 (ComPtr が自動でReleaseするため不要)
		//infoQueue->Release(); 
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com\questions\69805245\directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif
}

void DirectXCommon::CreateCommand() {
	//コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDisc{};
	HRESULT hr = device_->CreateCommandQueue(&commandQueueDisc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	assert(SUCCEEDED(hr));//コマンドキュー生成の可否

	//コマンドアロケータの生成
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	//コマンドアロケータがうまく生成できなかった
	assert(SUCCEEDED(hr));

	//コマンドリストの生成
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf())); // Get() と GetAddressOf() を使用
	//コマンドリストがうまく生成できなかった
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateSwapChain() {
	//スワップチェーンの生成
	swapChainDesc_.Width = clientWidth_;
	swapChainDesc_.Height = clientHeight_;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成
	HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), hwnd_, &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf())); // Get() と GetAddressOf() を使用
	assert(SUCCEEDED(hr));

	//スワップチェーンからリソースを引っ張る
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(swapChainResources_[0].GetAddressOf()));
	//うまく取得できないと起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(swapChainResources_[1].GetAddressOf()));
	assert(SUCCEEDED(hr));

	//ディスクリプタヒープの生成
	//ディスクリプタサイズを取得する
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//RTVの設定
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//ディスクリプらの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	//RTVを2ツ作るのでディスクリプタを2つ用意
	//まず一つ目を作る。一つ目は最初のところに作る。作る場所をこっちで指定する
	rtvHandles_[0] = GetCPUDescriptorHandle(rtvDescriptorHeap_.Get(), descriptorSizeRTV_, 0);
	device_.Get()->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	//2つ目のディスクリプタハンドルを得る(自力で)
	rtvHandles_[1] = GetCPUDescriptorHandle(rtvDescriptorHeap_.Get(), descriptorSizeRTV_, 1);
	//二つ目を得る
	device_.Get()->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);

	srvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, GetSRVSize() * 100, true);
}

void DirectXCommon::CreateFence() {
	//初期値0でFenceを作る

	HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	//FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::CreateDepth() {
	//DepthStencilTextureをウィンドウのサイズで作成
	depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), clientWidth_, clientHeight_);
	//DSV
	dsvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//DSVの設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//DSVHeapの先頭にDSVをつくる
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc_, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

	//DepthStencilStateの設定
	//Depthの機能を有効化
	depthStencilDesc_.DepthEnable = true;
	//書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void DirectXCommon::CreateLight() {
	///// 光用のリソース /////
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.5f,-1.0f,0.0f };
	directionalLightData_->intentity = 1.0f;
}