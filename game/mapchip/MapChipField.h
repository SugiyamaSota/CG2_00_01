#pragma once
#include "../../engine/bonjin/BonjinEngine.h"

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};



class MapChipField {
private:
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	static inline const uint32_t kNumBlockVirtical = 10;
	static inline const uint32_t kNumBlockHorizontal = 25;

	MapChipData mapChipData_;



public:
	//
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 矩形
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	/// <summary>
	/// データリセット
	/// </summary>
	void ResetMapChipData();

	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadmapChipCsv(const std::string& filePath);

	//セッター

	//ゲッター
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; };
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; };
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);
};