#pragma once
#include "../base/SceneBase.h" // SceneBaseをインクルード

#include"../bonjin/BonjinEngine.h"

namespace BonjinEngine {

    class TitleScene : public SceneBase {
    public:
        // 💡 仮想デストラクタは必須
        virtual ~TitleScene() = default;

        /// <summary>
        /// シーン初期化
        /// </summary>
        void Initialize(Camera* camera) override;

        /// <summary>
        /// 毎フレーム更新
        /// </summary>
        /// <param name="deltaTime">デルタタイム</param>
        void Update(float deltaTime) override;

        /// <summary>
        /// 毎フレーム描画
        /// </summary>
        void Draw() override;

        /// <summary>
        /// 次のシーンを取得
        /// </summary>
        /// <returns>次のシーンタイプ</returns>
        SceneType GetNextScene() const override;

    private:
        Model* model_ = nullptr;
    };
}