#pragma once

#include <random>
#include <limits>
#include <algorithm>
#include <type_traits>

#include"../math/Struct.h"

class RandomEngine
{
public:
    /// <summary>
    /// シングルトンインスタンスを取得します。
    /// </summary>
    static RandomEngine* GetInstance();

     /// <summary>
    /// 整数値の乱数
    /// </summary>
    /// <param name="min">最小値</param>
    /// <param name="max">最大値</param>
    /// <returns>最小値から最大値の間の乱数</returns>
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
        Rand(T min, T max)
    {
        if (min > max) std::swap(min, max);

        std::uniform_int_distribution<T> distribution(min, max);
        return distribution(engine_);
    }

    /// <summary>
    /// 浮動小数点の乱数
    /// </summary>
    /// <param name="min">最小値</param>
    /// <param name="max">最大値</param>
    /// <returns>最小値から最大値の間の乱数</returns>
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
        Rand(T min, T max) 
    {
        if (min > max) std::swap(min, max);

        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(engine_);
    }

    /// <summary>
    /// 3次元の範囲 [-range, +range] の Vector3 乱数
    /// </summary>
    Vector3 RandVector3(const Vector3& range);

private:

    // シングルトン化のためのプライベートコンストラクタ/デストラクタ
    RandomEngine();
    ~RandomEngine() = default;
    RandomEngine(const RandomEngine&) = delete;
    RandomEngine& operator=(const RandomEngine&) = delete;

private:

    // 乱数エンジン
    std::mt19937 engine_;

};