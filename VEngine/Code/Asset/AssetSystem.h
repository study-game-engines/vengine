#pragma once

#include "Render/RenderTypes.h"

namespace AssetSystem
{
	void BuildAllVMeshDataFromFBXImport();
	MeshDataProxy ReadVMeshAssetFromFile(const std::string filename);
	void BuildAllGameplayMapFiles();
};
