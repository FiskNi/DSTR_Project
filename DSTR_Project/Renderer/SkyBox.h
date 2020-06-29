#ifndef _SKYBOX_h
#define _SKYBOX_h
#include <Pch/Pch.h>

struct SkyBuffer {
	GLuint VAO;
	GLuint VBO;
	GLuint CubemapTextureID;
};

class SkyBox
{
private:
	//Order of the texture are: Right, Left, Top, Bottom, Front and Back.
	std::vector<std::string> faces
	{
		"right.png",
		"left.png",
		"top.png",
		"bottom.png",
		"front.png",
		"back.png"
	};

	SkyBuffer m_buffer;
	std::string m_skyboxPath;


	glm::vec3 skyboxVertices[36] = {
		// positions          
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),

		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),

		glm::vec3(-1.0f,  1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f, -1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		 glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		 glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f),
		 glm::vec3(1.0f, -1.0f,  1.0f)
	};

public:
	SkyBox();
	~SkyBox();

	unsigned int createCubeMap(std::vector<std::string> faces);
	const GLuint& getVAO() const;
	unsigned int getCubeMapTexture() const;
	void prepareBuffers();
	glm::mat4 getModelMatrix() const;
};

#endif