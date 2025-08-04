#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

// 衝突情報
struct CollisionMapInfo {
	bool isHotTop_ = false;
	bool isLandin_ = false;
	bool isHitWall_ = false;
	Vector3 movement_;
};

// オブジェクトの４隅
enum Corner {
    kRightBottom,
    kLeftBottom,
    kRightTop,
    kLeftTop,
    kNumCorner
};

// オブジェクトの向き
enum class LRDirection {
	kRight,
	kLeft,
};
