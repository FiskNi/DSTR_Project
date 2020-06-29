#pragma once
#include <Pch/Pch.h>
#include "LoaderFormat.h"
#include "Mesh/MeshFormat.h"

//Example usage:
/*
	BGLoader testLoader;
	testLoader.LoadMesh("SexyCube.meh");
	testLoader.Unload();
*/


class BGLoader
{
private:
	std::string	fileName;

	int meshCount;
	int matCount;

	struct bggMeshData
	{
		std::vector<Vertex> bggVertices;
		std::vector<Vertex2> bggSkeleVertices;
		std::vector<Face> bggFaces;
		glm::vec3 bggPositions;
		glm::quat bggRotation;
		glm::vec3 bggScale;
		Transform bggTransforms;
		Skeleton bggSkeleton;
		std::vector<Animation> bggAnimation;
	};
	std::vector<bggMeshData> bggMeshes;
	std::vector<Material> bggMaterials;

	// TODO: Direction lights, Point Lights

	// Temporary shared format to load data into
	LoaderFormat::BGHeader	fileHeader;
	LoaderFormat::MeshGroup* meshGroup;
	LoaderFormat::PhongMaterial* material;
	LoaderFormat::LoaderMesh* loaderMesh;
	LoaderFormat::DirLight* dirLight;
	LoaderFormat::PointLight* pointLight;
	std::vector<LoaderFormat::Vertex*> meshVert;
	std::vector<LoaderFormat::Face*> meshFace;
	std::vector<LoaderFormat::MeshAnis> animationsD;
	std::vector<LoaderFormat::MeshSkeleton> skeletonsD;

public:
	BGLoader();
	// Default constructor to directly load a file
	BGLoader(std::string filesName);		
	~BGLoader();
	void Unload();
	void BGFormatData();

	bool LoadMesh(std::string fileName);	// Load a file

	std::string GetFileName() const { return fileName; }
	const int GetMeshCount() const { return fileHeader.meshCount; }

	const std::string GetMeshName(int meshID) { return (std::string)loaderMesh[meshID].name; }
	const std::vector<Vertex> GetVertices(int meshId) { return bggMeshes[meshId].bggVertices;  }
	const std::vector<Vertex2> GetSkeleVertices(int meshId) { return bggMeshes[meshId].bggSkeleVertices;  }
	const glm::vec3& GetPosition(int meshId) const { return bggMeshes[meshId].bggPositions; }
	const glm::quat& GetRotation(int meshId) const { return bggMeshes[meshId].bggRotation; }
	const glm::vec3& GetScale(int meshId) const { return bggMeshes[meshId].bggScale; }
	const Transform& GetTransform(int meshId) const { return bggMeshes[meshId].bggTransforms; }
	const std::vector<Face> GetFaces(int meshId) { return bggMeshes[meshId].bggFaces; }
	const Material GetMaterial(int meshId) { return bggMaterials[loaderMesh[meshId].materialID]; }
	const std::string GetAlbedo(int meshId) { return (std::string)material[loaderMesh[meshId].materialID].albedo; }
	const std::string GetNormalMap(int meshId) { return (std::string)material[meshId].normal; }
	const int GetVertexCount(int meshId) { return loaderMesh[meshId].vertexCount; }
	const int GetFaceCount(int meshId)  { return loaderMesh[meshId].faceCount; }
	const Skeleton GetSkeleton(int meshId) { return bggMeshes[meshId].bggSkeleton; }
	const std::vector<Animation>& GetAnimation(int meshId) { return bggMeshes[meshId].bggAnimation; }


	// Returns the first (default) data
	const std::string GetMeshName() { return (std::string)loaderMesh[0].name; }
	const std::vector<Vertex> GetVertices() { return bggMeshes[0].bggVertices; }
	const std::vector<Vertex2> GetSkeleVertices() { return bggMeshes[0].bggSkeleVertices;  }
	const glm::vec3& GetPosition() const { return bggMeshes[0].bggPositions; }
	const glm::quat& GetRotation() const { return bggMeshes[0].bggRotation; }
	const glm::vec3& GetScale() const { return bggMeshes[0].bggScale; }
	const Transform& GetTransform() const { return bggMeshes[0].bggTransforms; }
	const std::vector<Face> GetFaces() { return bggMeshes[0].bggFaces; }
	const Material GetMaterial() { return bggMaterials[0]; }
	const std::string GetAlbedo() { return (std::string)material[0].albedo; }
	const std::string GetNormalMap() { return (std::string)material[0].normal; }
	const int GetVertexCount() { return loaderMesh[0].vertexCount; }
	const int GetFaceCount()  { return loaderMesh[0].faceCount; }




	// Ignore below, will be reformated or removed
	LoaderFormat::LoaderMesh GetLoaderMesh(int meshId) const { return loaderMesh[meshId]; }

	const LoaderFormat::PhongMaterial GetMaterial(int meshID) const { return material[meshID]; }
	const LoaderFormat::PhongMaterial GetMaterial() const { return material[0]; }

	const LoaderFormat::Vertex* GetLoaderVertices(int meshId) { return meshVert[meshId]; }
	const LoaderFormat::Face* GetLoaderFaces(int meshId) { return meshFace[meshId]; }
	LoaderFormat::Skeleton GetSkeleton(int index) const { return loaderMesh[index].skeleton; }
	LoaderFormat::Joint GetJoint(int mIndex, int jIndex) const { return skeletonsD[mIndex].joint[jIndex]; }
	LoaderFormat::MeshAnis GetAnimation(int mIndex) const { return animationsD[mIndex]; }
	LoaderFormat::Animation GetAnimationHeader(int mIndex, int aIndex) const { return animationsD[mIndex].animations[aIndex].ani; }
	LoaderFormat::KeyFrame GetKeyFrame(int mIndex, int aIndex, int kIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].key; }
	LoaderFormat::Transform GetTransform(int mIndex, int aIndex, int kIndex, int tIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].transforms[tIndex].t; }
	LoaderFormat::MeshGroup GetMeshGroup(int index) const { return meshGroup[index]; }


	float* GetDirLightPos(int index) const { return dirLight[index].position; }
	float GetDirLightIntensity(int index) const { return dirLight[index].intensity; }
	float* GetDirLightRotation(int index) const { return dirLight[index].rotation; }
	float* GetDirLightColor(int index) const { return dirLight[index].color; }
	int GetDirLightCount() const { return fileHeader.dirLightCount; }

	float* GetPointLightPos(int index) const { return pointLight[index].position; }
	float* GetPointLightColor(int index) const { return pointLight[index].color; }

	float GetPointLightIntensity(int index) const { return pointLight[index].intensity; }
	int GetPointLightCount() const { return fileHeader.pointLightCount; }

	unsigned int GetMaterialID(int meshIndex) const { return loaderMesh[meshIndex].materialID; }
	int GetMaterialCount() const { return fileHeader.materialCount; }

	int GetMeshGroupCount() const { return fileHeader.groupCount; }
};

