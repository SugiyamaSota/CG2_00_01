#pragma once
#include<functional>

class TimedCall{
private:
	const std::function<void()> function_;

	uint32_t time_;

	bool isFinished_ = false;

public:
	TimedCall(const std::function<void()>& func, uint32_t time);

	void Update();

	bool IsFinished() { return isFinished_; }
};

