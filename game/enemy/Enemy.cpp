#include "Enemy.h"
#include"../player/Player.h"
#include"../GameScene.h" // GameSceneヘッダーを含める

void Enemy::Initialize(Model* model, const Vector3& startPosition) {
	model_ = model;
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = startPosition;

	// 初期状態を設定
	state_ = new EnemyStateApproach(); // 初期状態
	fireFunction_ = std::bind(&Enemy::Fire, this);

	timedCalls_.push_back(
		new TimedCall(std::bind(&Enemy::FireAndReset, this), kFireInterval));

	SetCollisionAttibute(kCollisionAttibuteEnemy);
	SetCollisionMask(kCollisionAttibutePlayer);
}

Enemy::~Enemy() {
	delete state_;
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void Enemy::Update(Camera* camera) {
	if (state_) {
		state_->Update(this); // 現在の状態のUpdateメソッドを呼び出す
	}

	worldTransform_.worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	if (worldTransform_.parent) {
		worldTransform_.parent->worldMatrix = MakeAffineMatrix(worldTransform_.parent->scale, worldTransform_.parent->rotate, worldTransform_.parent->translate);
		worldTransform_.worldMatrix = Multiply(worldTransform_.worldMatrix, worldTransform_.parent->worldMatrix);
	}

	model_->Update(worldTransform_, camera, false);

	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
		});

	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

}

void Enemy::Draw() {
	model_->Draw();
}

void Enemy::ChangeState(BaseEnemyState* newState) {
	if (state_) {
		EnemyStateLeave* leaveState = dynamic_cast<EnemyStateLeave*>(newState);
		if (leaveState != nullptr) {
			// 新しい状態がLeaveの場合、すべての予約されたTimedCallをキャンセル（削除）
			for (TimedCall* timedCall : timedCalls_) {
				delete timedCall;
			}
			timedCalls_.clear(); // リストもクリア
		}
		delete state_; // 古い状態を削除
	}
	state_ = newState; // 新しい状態を設定

}

void Enemy::Fire() {
	const float kBulletSpeed = 0.5f;

	Vector3 playerWorldPosition = player_->GetWorldPosition();
	Vector3 enemyWorldPosition = GetWorldPosition();

	Vector3 direction = playerWorldPosition - enemyWorldPosition;

	float length = Length(direction);
	Vector3 normalizedDirection;
	if (length != 0.0f) {
		normalizedDirection = Normalize(direction);
	} else {
	}

	Vector3 velocity = normalizedDirection * kBulletSpeed;

	Model* newModel = new Model();
	newModel->LoadModel("cube");

	
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(newModel, worldTransform_.translate, velocity, *player_);

	// GameSceneに弾丸を追加するよう通知
	if (gameScene_) {
		gameScene_->AddEnemyBullet(newBullet, newModel);
	}
	// Removed: bullets_.push_back(newBullet);
}

void EnemyStateApproach::Update(Enemy* enemy) {
	// 速度を加算
	enemy->MoveApproach();
	// 一定座標に来たら離脱
	if (enemy->GetPosition().z < 0.0f) {
		enemy->ChangeState(new EnemyStateLeave()); // 新しい状態に遷移
	}
}

// EnemyStateLeave クラスのUpdateメソッドの実装
void EnemyStateLeave::Update(Enemy* enemy) {
	// 速度を加算
	//enemy->MoveLeave();
}

void Enemy::FireAndReset() {
	Fire();

	timedCalls_.push_back(
		new TimedCall(std::bind(&Enemy::FireAndReset, this), kFireInterval));
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.worldMatrix.m[3][0];
	worldPos.y = worldTransform_.worldMatrix.m[3][1];
	worldPos.z = worldTransform_.worldMatrix.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() {
	// 
	isDead_ = true;
}