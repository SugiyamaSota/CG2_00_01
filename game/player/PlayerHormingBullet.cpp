#include "PlayerHormingBullet.h"
#include <cassert>

// スケールとターゲット引数を追加
void PlayerHormingBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, const Vector3& scale, Enemy* target) { //
    // モデル
    assert(model);
    model_ = model;

    model_->SetColor({ 0,1,0,1 });

    // ワールド変換
    worldTransform_ = InitializeWorldTransform();
    worldTransform_.scale = scale; // 引数で受け取ったスケールを設定
    worldTransform_.translate = position;

    velocity_ = velocity;
    target_ = target; // ターゲットを保持

    // 初期速度から向きを設定
    worldTransform_.rotate.y = std::atan2f(velocity_.x, velocity_.z);
    float velocityXZ = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
    worldTransform_.rotate.x = std::atan2(velocity_.y, velocityXZ);

    SetCollisionAttibute(kCollisionAttibutePlayer);
    SetCollisionMask(kCollisionAttibuteEnemy);

}

PlayerHormingBullet::~PlayerHormingBullet() {
    // model_はPlayerクラス側で管理・解放されているはずなので、ここではdeleteしない
    // もしホーミング弾が自身のモデルを独自にnewしている場合はここでdelete model_;が必要
}

void PlayerHormingBullet::Update(Camera* camera) {
    // 一定時間でデス
    if (--deathTimer_ <= 0) {
        isDead_ = true;
    }

    // ホーミングロジック
    // ターゲットが存在し、かつデス状態でない場合のみ追尾する
    // ホーミング対象が死んだ、またはnullptrになった場合は、ホーミングを中止し、現在の速度で直進させる
    if (target_ != nullptr && !target_->IsDead()) { // target_がnullptrでないことをまず確認
        Vector3 targetPos = target_->GetWorldPosition();
        Vector3 currentPos = GetWorldPosition();
        Vector3 directionToTarget = targetPos - currentPos; // ターゲットへの方向ベクトル

        // ターゲットが十分に近く、かつ方向ベクトルが0ベクトルでないことを確認
        // ターゲットに到達した後にdirectionToTargetが0ベクトルになると、NormalizeでNaNが発生する可能性があるため
        if (LengthSq(directionToTarget) > 0.0001f) { // 非常に小さい値より大きいか確認
            // 速度ベクトルをターゲット方向へ徐々に調整する
            const float kHomingStrength = 0.05f; // ホーミングの強さ（0.0～1.0で調整）

            // 現在の速度ベクトルの長さを維持しつつ、方向を調整
            Vector3 normalizedCurrentVelocity = Normalize(velocity_);
            Vector3 normalizedDirectionToTarget = Normalize(directionToTarget);

            // 線形補間を使って速度を更新 (Slerpは球面線形補間、Lerpが単純な線形補間)
            // Slerpが未定義の場合は、単純なLerpを使用するか、自前で実装が必要
            // Slerp関数が利用できない場合、または単に方向を混ぜるだけならLerpでも可
            // 例: velocity_ = Lerp(normalizedCurrentVelocity, normalizedDirectionToTarget, kHomingStrength) * Length(velocity_);

            // Slerp関数が利用できる前提
            velocity_ = Slerp(normalizedCurrentVelocity, normalizedDirectionToTarget, kHomingStrength) * Length(velocity_);
        }
    } else {
        // ターゲットが死んだ、またはnullptrになった場合、ホーミングを中止
        // そのまま現在の速度で直進を続ける (velocity_は変更しない)
        // もしくは、特定のデバッグメッセージを表示するなど
        // target_ = nullptr; // 一度無効になったターゲットはnullptrにしておく
    }

    worldTransform_.translate += velocity_; // 移動

    // 弾の向きを速度ベクトルに合わせる
    if (LengthSq(velocity_) > 0) { // 速度が0ベクトルでないことを確認
        Vector3 normalizedVelocity = Normalize(velocity_);
        worldTransform_.rotate.y = std::atan2f(normalizedVelocity.x, normalizedVelocity.z);
        float velocityXZ = std::sqrtf(normalizedVelocity.x * normalizedVelocity.x + normalizedVelocity.z * normalizedVelocity.z);
        worldTransform_.rotate.x = std::atan2(normalizedVelocity.y, velocityXZ);
    }

    worldTransform_.worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
    if (worldTransform_.parent) {
        worldTransform_.parent->worldMatrix = MakeAffineMatrix(worldTransform_.parent->scale, worldTransform_.parent->rotate, worldTransform_.parent->translate);
        worldTransform_.worldMatrix = Multiply(worldTransform_.worldMatrix, worldTransform_.parent->worldMatrix);
    }

    model_->Update(worldTransform_, camera, false);
}

void PlayerHormingBullet::Draw() {
    model_->Draw();
}

void PlayerHormingBullet::OnCollision() {
    // デスフラグをたてる
    isDead_ = true;
}

Vector3 PlayerHormingBullet::GetWorldPosition() {
    Vector3 worldPos;

    worldPos.x = worldTransform_.worldMatrix.m[3][0];
    worldPos.y = worldTransform_.worldMatrix.m[3][1];
    worldPos.z = worldTransform_.worldMatrix.m[3][2];

    return worldPos;
}