#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include<list> // std::list を使うために追加
#include<vector> // スプライトの再利用などを考慮する場合、vectorも検討

class Player;
class Enemy;

class LockOn {
private:
    WorldTransform lockOnWorldTransform_; // 個々のスプライトで持つ必要があれば変更
    // Sprite* lockOnSprite_ = nullptr; // 単一のスプライトからリストに変更
    std::list<Sprite*> lockOnSprites_; // ロックオン対象のスプライトリスト
    std::list<WorldTransform> lockOnSpriteTransforms_; // スプライトのワールド変換リスト (任意)

    // 対象
    // Enemy* target_ = nullptr; // 単一のターゲットからリストに変更
    std::list<Enemy*> targets_; // ロックオン対象の敵リスト

public:
    void Initialize();

    ~LockOn();

    void Update(Player* player, std::list<Enemy*>& enemies, const Camera& camera);

    void Draw();

    // 単一のターゲットではなく、リスト全体を返すように変更 (または、最も近いターゲットなどを返すメソッドを追加)
    const std::list<Enemy*>& GetTargets()const { return targets_; }
};