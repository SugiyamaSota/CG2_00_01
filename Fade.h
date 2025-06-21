#pragma once
#include"engine/bonjin/BonjinEngine.h"

class Fade{
private:
	Sprite* sprite_ = nullptr;

	WorldTransform transformSprite_;
	Matrix4x4 viewMatrixSprite_;
	Matrix4x4 projectionMatrixSprite_;

	

	

	float duration_ = 0.0f;
	float counter_ = 0.0f;
public:
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	Status status_ = Status::None;

	void Initialize();

	~Fade();

	void Update();

	void Draw();

	void Start(Status status, float duration);

	void Stop();

	bool IsFinished()const;
};

