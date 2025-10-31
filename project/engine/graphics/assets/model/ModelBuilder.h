#pragma once
#include"BonjinEngine.h"

class ModelBuilder
{
public:
	/// <summary>
	/// objファイルの読み込み
	/// </summary>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

private:
	/// <summary>
	/// mtlファイルの読み込み
	/// </summary>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
};

