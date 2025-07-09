#include"engine/bonjin/BonjinEngine.h"

using namespace BonjinEngine;

// グリッド描画用の頂点構造体
struct GridVertex {
	Vector4 position;
};

// ワールドビュープロジェクション行列用の定数バッファ
struct GridTransformationMatrix {
	Matrix4x4 worldviewProjection;
};

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize(hInstance, kClientWidth, kClientHeight);

	Camera* camera = new Camera();
	camera->Initialize(kClientWidth, kClientHeight);


	WorldTransform worldTransform = InitializeWorldTransform();
	Model* model = new Model();
	model->LoadModel("axis");
	model->Initialize(worldTransform);

	// Gridの頂点データを生成
	std::vector<GridVertex> gridVertices;
	const float gridSize = 10.0f; // グリッドの範囲
	const int numDivisions = 10;   // 分割数
	const float step = gridSize / static_cast<float>(numDivisions);

	// X軸に平行な線
	for (int i = 0; i <= numDivisions; ++i) {
		float z = -gridSize / 2.0f + i * step;
		gridVertices.push_back({ { -gridSize / 2.0f, 0.0f, z, 1.0f } });
		gridVertices.push_back({ { gridSize / 2.0f, 0.0f, z, 1.0f } });
	}
	// Z軸に平行な線
	for (int i = 0; i <= numDivisions; ++i) {
		float x = -gridSize / 2.0f + i * step;
		gridVertices.push_back({ { x, 0.0f, -gridSize / 2.0f, 1.0f } });
		gridVertices.push_back({ { x, 0.0f, gridSize / 2.0f, 1.0f } });
	}

	// グリッド用頂点バッファの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> gridVertexBuffer = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(GridVertex) * gridVertices.size());
	GridVertex* mappedGridVertices = nullptr;
	HRESULT hr = gridVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedGridVertices));
	assert(SUCCEEDED(hr));
	std::copy(gridVertices.begin(), gridVertices.end(), mappedGridVertices);
	gridVertexBuffer->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW gridVbView{};
	gridVbView.BufferLocation = gridVertexBuffer->GetGPUVirtualAddress();
	gridVbView.SizeInBytes = static_cast<UINT>(sizeof(GridVertex) * gridVertices.size());
	gridVbView.StrideInBytes = sizeof(GridVertex);

	// グリッド用定数バッファの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> gridTransformationMatrixResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(GridTransformationMatrix));
	GridTransformationMatrix* gridTransformationMatrixData = nullptr;
	hr = gridTransformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&gridTransformationMatrixData));
	assert(SUCCEEDED(hr));

	//ウィンドウの×ボタンが押されるまでループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			NewFrame();
			///
			/// 更新処理ここから
			///
			
			camera->Update(Camera::CameraType::kDebug);

			model->Update(worldTransform, camera, false);

			gridTransformationMatrixData->worldviewProjection = Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix());
			

			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			model->Draw();

			// グリッドの描画
			DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(DirectXCommon::GetInstance()->GetPSO()->GetLineRootSignature());
			DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(DirectXCommon::GetInstance()->GetPSO()->GetLinePipelineState());
			DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &gridVbView);
			DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, gridTransformationMatrixResource->GetGPUVirtualAddress()); // b0 レジスタに設定
			DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // ラインリストとして描画
			DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(static_cast<UINT>(gridVertices.size()), 1, 0, 0);

			///
			/// 描画処理ここまで
			///
			EndFrame();
		}
	}

	/////  解放処理 /////
	delete model;
	delete camera;
	Finalize();
	return 0;
}