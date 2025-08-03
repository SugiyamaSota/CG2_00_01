#include "anchor.h"

Anchor::Anchor(const Vector2& position, const Vector2& velocity)
    : position_(position), velocity_(velocity) {
    model_ = new Model();
    model_->LoadModel("cube");
    worldTransform_ = InitializeWorldTransform();
    worldTransform_.translate = { position.x,position.y,0 };
}

void Anchor::Update(Camera* camera) {
    position_.x += velocity_.x;
    position_.y += velocity_.y;

    worldTransform_.translate = { position_.x,position_.y,0 };

    model_->Update(worldTransform_, camera);
}

void Anchor::Draw() const {
    model_->Draw();
}

// Player.h の CollisionMapInfo を使って衝突判定を行う
bool Anchor::isCollisionMap(const CollisionMapInfo& info) {
    // 簡略化のために、中心点のタイルをチェックする例
    // 実際のマップ情報に合わせて tileSize や mapData を取得
    // ここでは仮にマップ情報が CollisionMapInfo 経由で取得できると想定
    const int tileSize = 32; // 仮の値
    // const int** mapData = ...; // 仮にマップデータ

    int tileX = static_cast<int>(position_.x / tileSize);
    int tileY = static_cast<int>(position_.y / tileSize);

    return false;
}