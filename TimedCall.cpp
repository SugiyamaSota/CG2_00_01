#include "TimedCall.h"

TimedCall::TimedCall(const std::function<void()>& func, uint32_t time) : function_(func) {
    time_ = time;
}

void TimedCall::Update() {
    if (isFinished_) {
        return;
    }
    time_--;
    if (time_ <= 0) {
        isFinished_ = true;
        function_();
    }
}