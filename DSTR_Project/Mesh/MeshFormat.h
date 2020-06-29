#pragma once
#include <Pch/Pch.h>

struct Transform 
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform()
	{
		position = glm::vec3();
		rotation = glm::quat();
		scale = glm::vec3(1.0f);
	}
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;
	//glm::vec3 Tangent;
};

struct Vertex2
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 Normals;

	glm::ivec4 bone;
	glm::vec4 weight;
};

struct Face
{
	int indices[3];
};

struct Material
{
	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int rimLighting;
	bool texture;
	bool normalMap;
	std::vector<GLuint> textureID;	
	GLuint normalMapID;

	Material(std::string n)
	{
		name = n;
		ambient = glm::vec3(0.0f);
		diffuse = glm::vec3(0.5f);
		specular = glm::vec3(0.5f);

		rimLighting = 0;
		texture = false;
		normalMap = false;
	}

	Material()
	{
		name = "";
		ambient = glm::vec3(0.5f);
		diffuse = glm::vec3(0.5f);
		specular = glm::vec3(0.5f);

		rimLighting = 0;
		texture = false;
		normalMap = false;
	}
};

struct Buffers
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	int nrOfFaces;
};

struct Skeleton
{
	struct Joint
	{
		std::string name;
		int parentIndex;
		glm::mat4 invBindPose;
	};

	std::string name;
	std::vector<Joint> joints;

	Skeleton()
	{
		name = "";
	}
};
struct AnimationState
{
	bool running = false;
	bool casting = false;
	bool jumping = false;
	bool deflecting = false;
	bool idle = false;
	bool casTripple = false;
	bool castPotion = false;
	
};


struct Animation
{
	// Skeleton animation
	struct skKeyframe
	{
		struct skTransform
		{
			int jointid;
			Transform transform;
		};
		int id;
		std::vector<skTransform> local_joint_t;
	};

	// Standards
	std::string name;
	int keyframeFirst;
	int keyframeLast;
	float duration;
	float rate;
	std::vector<skKeyframe> keyframes;

};

struct BonePalleteBuffer
{
	glm::mat4 bones[64]{ glm::mat4(1.0f) };
};

