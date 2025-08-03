#pragma once
#include "../../engine/bonjin/BonjinEngine.h"
#include "../../struct.h"
#include "../mapchip/MapChipField.h" // マップチップフィールドをインクルード

class Anchor {
public:
    // コンストラクタにマップの参照を追加
    Anchor(const Vector3& position, const Vector3& velocity,MapChipField* mapChipField);

    ~Anchor();

    void Update(Camera& camera);
    void Draw() const;

    // 当たり判定メソッドを追加
    bool isCollisionMap();

    bool GetStandBy()const { return isStandBy; }

    const Vector3& GetPosition() const { return worldTransform_.translate; }
    float GetAngle() const { return worldTransform_.rotate.z; }
private:
    Vector3 position_;
    Vector3 velocity_;

    Model* model_;
    WorldTransform worldTransform_;

    MapChipField* mapChipField_ = nullptr; // マップへの参照

    // プレイヤーと同様に当たり判定サイズを定義
    static inline const float kWidth = 0.5f;
    static inline const float kHeight = 0.5f;

    enum Corner {
        kRightBottom,
        kLeftBottom,
        kRightTop,
        kLeftTop,
        kNumCorner
    };

    // 角の座標を取得するヘルパーメソッド
    Vector3 CornerPosition(const Vector3& center, Corner corner);

    bool isStandBy;
};