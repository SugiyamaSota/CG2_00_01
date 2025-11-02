#pragma once
#include"../interface/IScene.h"

#include"../bonjin/BonjinEngine.h"

namespace BonjinEngine
{

    class TitleScene : public IScene
    {
    public:
        // --- オーバーライド関数 ---
        virtual ~TitleScene() = default;

        void Initialize(Camera* camera) override;

        void Unload()override;

        void Update(float deltaTime) override;

        void Draw() override;

        void DrawImGui()override;

        SceneType GetNextScene() const override;

    private:
        // --- ゲーム固有の変数 ---

        // 
        Model* model_ = nullptr;

    private:
        // --- ゲーム固有の関数 ---

    };
}