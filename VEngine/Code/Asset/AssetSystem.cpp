#include "AssetSystem.h"
#include <cstdio>
#include "FBXImporter.h"
#include "MeshAssetHeader.h"
#include "Editor/Editor.h"
#include "Profile.h"

AssetSystem assetSystem;

AssetSystem::AssetSystem() : System("AssetSystem")
{
}

void AssetSystem::WriteAllMeshDataToMeshAssetFiles()
{
	FILE* file = nullptr;

	uint32_t numberOfMeshFilesBuilt = 0;

	auto startTime = Profile::QuickStart();

	for (auto meshIt : fbxImporter.existingMeshDataMap)
	{
		MeshData* meshData = meshIt.second;

		MeshAssetHeader header = {};
		header.sourceMeshFormat = SourceMeshFormat::FBX;
		header.indexCount = meshData->indices.size();
		header.vertexCount = meshData->vertices.size();

		const std::string& filename = meshIt.first;
		const std::string meshName = filename.substr(0, filename.find("."));
		const std::string meshFilePath = "Meshes/" + meshName + ".vmesh";

		fopen_s(&file, meshFilePath.c_str(), "wb");
		assert(file);

		fwrite(&header, sizeof(MeshAssetHeader), 1, file);
		fwrite(meshData->vertices.data(), sizeof(Vertex), meshData->vertices.size(), file);
		fwrite(meshData->indices.data(), sizeof(MeshData::indexDataType), meshData->indices.size(), file);

		fclose(file);
		file = nullptr;

		numberOfMeshFilesBuilt++;
	}

	double elapsedTime = Profile::QuickEnd(startTime);

	editor->Log("Mesh asset build complete.\n\tNum meshes built: %d\n\tTime taken: %f", numberOfMeshFilesBuilt, elapsedTime);
}

//TODO: This is just testing code. Get rid of it or replace.
void AssetSystem::ReadAllMeshAssetsFromFile()
{
	FILE* file = nullptr;
	fopen_s(&file, "Meshes/cube.vmesh", "rb");
	assert(file);

	MeshAssetHeader header;
	MeshData data;

	fread(&header, sizeof(MeshAssetHeader), 1, file);

	data.vertices.resize(header.vertexCount);
	data.indices.resize(header.indexCount);

	fread(data.vertices.data(), sizeof(Vertex), header.vertexCount, file);
	fread(data.indices.data(), sizeof(MeshData::indexDataType), header.indexCount, file);

	return;
}
