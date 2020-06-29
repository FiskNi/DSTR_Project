#pragma once
#include <vector>

#define NAME_SIZE_L 256

// Format that both the exporter and importer share
namespace LoaderFormat
{
	struct BGHeader
	{
		int meshCount;
		int groupCount;
		int materialCount;
		int pointLightCount;
		int dirLightCount;
	};

	struct MeshGroup
	{
		char groupName[NAME_SIZE_L];
	
		float translation[3];
		float rotation[3];
		float scale[3];

		bool isChild;
		char parentName[NAME_SIZE_L];
		int parentType;
	};

	struct Vertex
	{
		float position[3];
		float uv[2];
		float normal[3];
		float tangent[3];
		float bitangent[3];

		float bone[4];
		float weight[4];
	};

	struct Face
	{
		int indices[3];
	};

	struct PhongMaterial
	{
		char name[NAME_SIZE_L];
		float ambient[3];
		float diffuse[3];
		float specular[3];
		float emissive[3];
		float opacity;

		char albedo[NAME_SIZE_L];
		char normal[NAME_SIZE_L];
	};

	struct Skeleton
	{
		char name[NAME_SIZE_L];
		int jointCount;
		int aniCount;
	};

	struct LoaderMesh
	{
		char name[NAME_SIZE_L];
		int materialID;

		float translation[3];
		float rotation[3];
		float scale[3];

		bool isChild;
		char parentName[NAME_SIZE_L];
		int parentType;

		int vertexCount;
		int faceCount;

		Skeleton skeleton;
	};

	struct Joint
	{
		char name[NAME_SIZE_L];
		int parentIndex;
		float invBindPose[16];
	};

	struct Animation
	{
		char name[NAME_SIZE_L];
		int keyframeFirst;
		int keyframeLast;
		float duration;
		float rate;
		int keyframeCount;

		//char mesh[NAME_SIZE_L];
	};

	struct KeyFrame
	{
		int id;
		int transformCount;
	};

	struct Transform
	{
		int joinId;
		float transform[3];
		float rotate[4];
		float scale[3];
	};

	struct DirLight
	{
		float position[3];
		float rotation[3];
		float color[3];
		float intensity;
	};

	struct PointLight 
	{
		float position[3];
		float color[3];
		float intensity;
	};


	struct MeshSkeleton
	{
		std::vector<LoaderFormat::Joint> joint;
	};

	struct MeshAnis
	{
		struct MeshAnimation
		{
			struct KeyFrameL
			{
				struct TransformL
				{
					LoaderFormat::Transform t;
				};

				LoaderFormat::KeyFrame key;
				std::vector<TransformL> transforms;
			};


			LoaderFormat::Animation ani;
			std::vector<KeyFrameL> keyFrames;
		};

		std::vector<MeshAnimation> animations;
	};
}

