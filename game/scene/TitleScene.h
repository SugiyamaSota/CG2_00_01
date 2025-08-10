#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

class TitleScene{
private:
	//--- カメラ ---
	Camera camera_;

	//
	bool isFinished_;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	bool GetIsFinished() { return isFinished_; }
};

