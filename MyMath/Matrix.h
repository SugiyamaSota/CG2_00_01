#pragma once

///// 前方宣言 /////
struct Matrix4x4;

///// 計算関数 /////
// 加法
Matrix4x4 Add(const  Matrix4x4& matrix1, const  Matrix4x4& matrix2);

// 減法
Matrix4x4 Subtract(const  Matrix4x4& matrix1, const  Matrix4x4& matrix2);

//積
Matrix4x4 Multiply(const  Matrix4x4& matrix1, const  Matrix4x4& matrix2);

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& matrix);

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& matrix);

// 単位行列
Matrix4x4 MakeIdentity4x4();
