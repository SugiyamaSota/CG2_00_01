#pragma once
#include "../interface/IScene.h" // ISceneをインクルード

#include"../bonjin/BonjinEngine.h"

namespace BonjinEngine {

	class GameScene : public IScene {
	public:
		// --- オーバーライド関数 --- 
		virtual ~GameScene() = default;

		void Initialize(Camera* camera) override;

		void Unload()override;

		void Update(float deltaTime) override;

		void Draw() override;

		void DrawImGui()override;

		SceneType GetNextScene() const override;

	private:
		// --- ゲーム固有の変数 ---
		//Particle* particle_ = nullptr;

	private:
		// --- ゲーム固有の関数 ---
	};
}