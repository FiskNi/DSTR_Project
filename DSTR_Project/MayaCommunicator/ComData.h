#ifndef _COMDATA_H
#define _COMDATA_H

#define NAME_SIZE_L 256


namespace ComData
{
	enum DataType : int
	{
		NONE,

		MESH,
		VERTEX,
		FACE,
		CAMERA,

		MESHADDED,
		MESHREMOVED,
		MESHRENAMED,
		MESHTRANSFORMED,
		CAMERAMOVE
	};

	struct Header
	{
		DataType dataType = NONE;
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
	};

	struct Face
	{
		int indices[3];
	};

	struct TransformChange
	{
		char name[NAME_SIZE_L];
		float translation[3];
		float rotation[4];
		float scale[3];
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


	struct Mesh
	{
		char name[NAME_SIZE_L];
		float matrix[4][4];

		int vertexCount = 0;
		int faceCount = 0;

		// Potential junk
		int materialID = 0;
		float translation[3];
		float rotation[4];
		float scale[3];
		bool isChild = 0;
		char parentName[NAME_SIZE_L];
		int parentType = 0;
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



};



#endif