#include "LockOn.h"
#include"../player/Player.h"
#include"../enemy/Enemy.h"
#include <algorithm> // std::sort, std::find などに必要

void LockOn::Initialize() {
    // lockOnWorldTransform_ = InitializeWorldTransform(); // 各スプライトで持つなら不要
    // lockOnSprite_ = new Sprite; // リストで管理するので不要
    // lockOnSprite_->Initialize({ 0,0,0 }, Color::White, { 0.5f,0.5f ,0.0f }, { 128,128 }, "uvChecker.png");
    // ここでは初期スプライトの生成は行わず、Updateで動的に生成する
}

LockOn::~LockOn() {
    for (Sprite* sprite : lockOnSprites_) { // スプライトリストを解放
        delete sprite;
    }
    lockOnSprites_.clear();
}

void LockOn::Update(Player* player, std::list<Enemy*>& enemies, const Camera& camera) {
    std::list<std::pair<float, Enemy*>> potentialTargets; // 候補となるターゲット
    std::list<Enemy*> newTargets; // 今回ロックオンされるターゲット

    Vector3 playerWorldPosition = player->GetWorldPosition();
    Matrix4x4 playerWorldMatrix = MakeAffineMatrix({ 1,1,1 }, { 0,0,0 }, playerWorldPosition);
    Matrix4x4 playerViewMatrix = Inverse(playerWorldMatrix);

    for (Enemy* enemy : enemies) {
        Vector3 positionWorld = enemy->GetWorldPosition();
        Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, 1280, 720, camera.GetViewProjectionMatrix());

        Matrix4x4 worldMatrix = MakeAffineMatrix({ 1,1,1 }, { 0,0,0 }, positionWorld);
        Matrix4x4 viewMatrix = Inverse(worldMatrix);

        // プレイヤーより奥にいる敵は対象外
        if (viewMatrix.m[3][2] <= playerViewMatrix.m[3][2]) {
            continue;
        }

        Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
        float distance = Distance(player->GetReticlePosition(), positionScreenV2);
        const float kDistanceLockOn = 1000.0f; // ロックオン可能距離

        if (distance <= kDistanceLockOn) {
            potentialTargets.emplace_back(std::make_pair(distance, enemy));
        }
    }

    // 距離でソートし、近いものからロックオン対象にする
    potentialTargets.sort();

    // 最大ロックオン数 (例: 3体まで)
    const size_t kMaxLockOnTargets = 3;
    size_t currentLockOnCount = 0;

    // 新しいターゲットリストを構築
    for (const auto& pair : potentialTargets) {
        if (currentLockOnCount >= kMaxLockOnTargets) {
            break; // 最大ロックオン数に達したら終了
        }
        newTargets.push_back(pair.second);
        currentLockOnCount++;
    }

    // 古いターゲットリストと新しいターゲットリストを比較し、スプライトの生成・削除を行う
    // ターゲットリストの差分を取り、対応するスプライトを調整するロジック

    // 現在のスプライトとターゲットの関連付けを解除（シンプル化のため、一旦全て削除して再生成）
    for (Sprite* sprite : lockOnSprites_) {
        delete sprite;
    }
    lockOnSprites_.clear();
    targets_.clear(); // 古いターゲットリストもクリア

    // 新しいターゲットに基づいてスプライトを生成し、リストに追加
    for (Enemy* targetEnemy : newTargets) {
        targets_.push_back(targetEnemy); // 新しいターゲットリストに登録

        // 新しいスプライトを生成して初期化
        Sprite* newSprite = new Sprite;
        // lockOnSprite_と同じ画像をロード
        newSprite->Initialize({ 0,0,0 }, Color::White, { 0.5f,0.5f ,0.0f }, { 128,128 }, "uvChecker.png");
        lockOnSprites_.push_back(newSprite);
    }

    // スプライトの位置を更新
    auto spriteIt = lockOnSprites_.begin();
    for (Enemy* targetEnemy : targets_) {
        if (spriteIt == lockOnSprites_.end()) break; // 念のため

        Vector3 positionWorld = targetEnemy->GetWorldPosition();
        Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, 1280, 720, camera.GetViewProjectionMatrix());
        (*spriteIt)->Update(positionScreen, Color::White);

        ++spriteIt;
    }
}

void LockOn::Draw() {
    for (Sprite* sprite : lockOnSprites_) {
        sprite->Draw();
    }
}