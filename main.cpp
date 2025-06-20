#include"engine/bonjin/BonjinEngine.h"


//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	BonjinEngine* bonjin = new BonjinEngine();
	bonjin->Initialize(hInstance, kClientWidth, kClientHeight);

	//ウィンドウの×ボタンが押されるまでループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			bonjin->NewFrame();
			///
			/// 更新処理ここから
			///
			

			///
			/// 更新処理ここまで
			/// 

			///
			/// 描画処理ここから

			///
			/// 描画処理ここまで
			///
			bonjin->EndFrame();
		}
	}

	/////  解放処理 /////
	delete bonjin;
	return 0;
}