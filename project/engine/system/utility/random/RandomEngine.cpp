#include "RandomEngine.h"
#include <limits>
#include <algorithm>

RandomEngine* RandomEngine::GetInstance() {
    static RandomEngine instance;
    return &instance;
}

RandomEngine::RandomEngine() {
    // 乱数エンジンの初期化（シード生成）
    std::random_device seed_gen;
    engine_ = std::mt19937(seed_gen());
}


Vector3 RandomEngine::RandVector3(const Vector3& range) {
    // 各軸に対して RandFloat を呼び出す
    Vector3 result = {
        Rand(-range.x, range.x),
        Rand(-range.y, range.y),
        Rand(-range.z, range.z)
    };
    return result;
}
