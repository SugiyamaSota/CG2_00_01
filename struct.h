#pragma once
#include"engine/bonjin/BonjinEngine.h"

struct CollisionMapInfo {
	bool isHotTop_ = false;
	bool isLandin_ = false;
	bool isHitWall_ = false;
	Vector3 movement_;
};
