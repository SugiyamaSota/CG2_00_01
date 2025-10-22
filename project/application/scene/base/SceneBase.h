#pragma once
#include "../bonjin/BonjinEngine.h" 

namespace BonjinEngine {

    // シーンの種類を識別するための列挙型 (enum)
    enum class SceneType {
        kTitle,
        kGame,
        kResult,
        kExit // ゲーム終了を意味する特別なシーン
    };

    // シーンの基底クラス (SceneBase)
    class SceneBase {
    protected:
        Camera* camera_ = nullptr;
    public:
        // 💡 仮想デストラクタ: 派生クラスのインスタンスをSceneBase*でdeleteするために必須
        virtual ~SceneBase() = default;

        // 💡 初期化: シーンがロードされたときに一度だけ呼ばれる
        virtual void Initialize(Camera* camera) {
            camera_ = camera; 
        } 

        // 💡 更新処理: 毎フレーム呼ばれる (deltaTimeを受け取るのが理想)
        virtual void Update(float deltaTime) = 0; // = 0 で純粋仮想関数

        // 💡 描画処理: 毎フレーム呼ばれる
        virtual void Draw() = 0; // = 0 で純粋仮想関数

        // 💡 次のシーンを取得: SceneManagerが遷移先を決定するために使う
        virtual SceneType GetNextScene() const = 0; // = 0 で純粋仮想関数

        SceneType GetCurrentSceneType() const { return currentSceneType_; }

    protected:
        // 派生クラスでのみ書き換え可能な現在のシーンタイプ
        SceneType currentSceneType_ = SceneType::kTitle;
        // 次に遷移したいシーンタイプ
        SceneType nextSceneType_ = SceneType::kTitle;
    };

}