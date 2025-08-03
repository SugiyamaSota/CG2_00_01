#pragma once
#include "../../engine/bonjin/BonjinEngine.h"

#include"../../struct.h"

class Anchor {
public:
    Anchor(const Vector2& position, const Vector2& velocity);
    void Update(Camera* camera);
    void Draw() const;

    bool isCollisionMap(const CollisionMapInfo& info); // マップとの当たり判定

private:
    Vector2 position_;
    Vector2 velocity_;

    Model* model_;
    WorldTransform worldTransform_;
};