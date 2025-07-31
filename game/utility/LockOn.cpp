#include "LockOn.h"
#include"../player/Player.h"
#include"../enemy/Enemy.h"

void LockOn::Initialize() {
	lockOnWorldTransform_ = InitializeWorldTransform();
	lockOnSprite_ = new Sprite;
	lockOnSprite_->Initialize({ 0,0,0 }, Color::White, { 0.5f,0.5f ,0.0f }, { 128,128 }, "uvChecker.png");
}

LockOn::~LockOn() {
	delete lockOnSprite_;
}

void LockOn::Update(Player* player, std::list<Enemy*>& enemies, const Camera& camera) {
	std::list<std::pair<float, Enemy*>>targets;

	Vector3 playerWorldPosition = player->GetWorldPosition();

	Matrix4x4 playerWorldMatrix = MakeAffineMatrix({ 1,1,1 }, { 0,0,0 }, playerWorldPosition);

	Matrix4x4 playerViewMatrix = Inverse(playerWorldMatrix);

	for (Enemy* enemy : enemies) {
		//敵一体ぶんのロックオン
		Vector3 positionWorld = enemy->GetWorldPosition();

		Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, 1280, 720, camera.GetViewProjectionMatrix());

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1,1,1 }, { 0,0,0 }, positionWorld);

		Matrix4x4 viewMatrix = Inverse(worldMatrix);

		if (viewMatrix.m[3][2] <= playerViewMatrix.m[3][2]) {
			continue;
		}

		Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
		//
		float distance = Distance(player->GetReticlePosition(), positionScreenV2);
		//
		const float kDistanceLockOn = 1000.0f;
		//
		if (distance <= kDistanceLockOn) {
			targets.emplace_back(std::make_pair(distance, enemy));
		}
	}

	target_ = nullptr;

	if (!targets.empty()) {
		targets.sort();

		target_ = targets.front().second;

		Vector3 positionWorld = target_->GetWorldPosition();

		Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, 1280, 720, camera.GetViewProjectionMatrix());

		lockOnSprite_->Update(positionScreen, Color::White);
	}
}

void LockOn::Draw() {
	lockOnSprite_->Draw();
}
