#pragma once
#include "../base/SceneBase.h"
#include"../game/GameScene.h"
#include"../title/TitleScene.h"
#include"../../engine/bonjin/BonjinEngine.h"
#include <map>

namespace BonjinEngine {

    class SceneManager {
    public:
        // シングルトン化 (エンジンの中核なので)
        static SceneManager* GetInstance();
        static void DestroyInstance();

    public:
        //
        void Initialize();
        // 💡 全シーンを登録
        void AddScene(SceneType type, SceneBase* scene);

        // 💡 エンジンのループから呼ばれる更新関数
        void Update(float deltaTime);

        // 💡 エンジンのループから呼ばれる描画関数
        void Draw();

    private:
        static SceneManager* instance;

        // シングルトン関連の禁止
        SceneManager() = default;
        ~SceneManager() = default;
        // ... (コピー禁止など)

        // 💡 現在アクティブなシーン
        SceneBase* currentScene_ = nullptr;
        // 💡 登録されたすべてのシーンを保持するマップ
        std::map<SceneType, SceneBase*> scenes_;

        // 💡 シーン切り替えロジック
        void ChangeScene(SceneType nextSceneType);

        Camera* camera = nullptr;
    };

}