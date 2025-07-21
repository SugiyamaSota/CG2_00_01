#include "Enemy.h"



void Enemy::Initialize(Model* model, const Vector3& startPosition) {
    model_ = model;
    worldTransform_ = InitializeWorldTransform();
    worldTransform_.translate = startPosition;

    // 初期状態を設定
    state_ = new EnemyStateApproach(); // 初期状態
    fireFunction_ = std::bind(&Enemy::Fire, this);

    timedCalls_.push_back(
        new TimedCall(std::bind(&Enemy::FireAndReset, this), kFireInterval));
}

Enemy::~Enemy() {
    delete state_; // 現在の状態オブジェクトを削除
    for (EnemyBullet* bullet : bullets_) {
        delete bullet;
    }
    for (Model* model : bulletModel_) {
        delete model;
    }
    for (TimedCall* timedCall : timedCalls_) {
        delete timedCall;
    }
}

void Enemy::Update(Camera* camera) {
    if (state_) {
        state_->Update(this); // 現在の状態のUpdateメソッドを呼び出す
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


    for (EnemyBullet* bullet : bullets_) {
        bullet->Update(camera);
    }

}

void Enemy::Draw() {
    model_->Draw();

    for (EnemyBullet* bullet : bullets_) {
        bullet->Draw();
    }
}

void Enemy::ChangeState(BaseEnemyState* newState) {
    if (state_) {
        delete state_; // 古い状態を削除
    }
    state_ = newState; // 新しい状態を設定
}

void Enemy::Fire() {
    const float kBulletSpeed = -1.0f;
    Vector3 velocity = { 0,0,kBulletSpeed };

    Matrix4x4 worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

    velocity = TransformNormal(velocity, worldMatrix);

    Model* newModel = new Model();
    newModel->LoadModel("cube");

    bulletModel_.push_back(newModel);

    EnemyBullet* newBullet = new EnemyBullet();
    newBullet->Initialize(newModel, worldTransform_.translate, velocity);

    bullets_.push_back(newBullet);
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
    enemy->MoveLeave();
}

void Enemy::FireAndReset() {
    Fire();

    timedCalls_.push_back(
        new TimedCall(std::bind(&Enemy::FireAndReset, this), kFireInterval));
}