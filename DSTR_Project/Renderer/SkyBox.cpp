#include <Pch/Pch.h>
#include <Texture/stb_image.h>
#include "SkyBox.h"

SkyBox::SkyBox()
{
	m_skyboxPath = TEXTUREPATH + "Skybox/";
	m_buffer.CubemapTextureID = createCubeMap(faces);
}

SkyBox::~SkyBox()
{
	glDeleteVertexArrays(1, &m_buffer.VAO);
	glDeleteBuffers(1, &m_buffer.VBO);
	glDeleteTextures(1, &m_buffer.CubemapTextureID);
}

unsigned int SkyBox::createCubeMap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);	

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width, height, nrOfChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{	
		faces[i] = m_skyboxPath + faces[i];
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrOfChannels, STBI_rgb_alpha);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			logError("Loading Skybox Texture Failed");
		}
	}
	return textureID;
}

const GLuint& SkyBox::getVAO() const
{
	return m_buffer.VAO;
}

unsigned int SkyBox::getCubeMapTexture() const
{
 	return m_buffer.CubemapTextureID;
}

void SkyBox::prepareBuffers()
{
	glGenVertexArrays(1, &m_buffer.VAO);
	glGenBuffers(1, &m_buffer.VBO);

	glBindVertexArray(m_buffer.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
}

glm::mat4 SkyBox::getModelMatrix() const
{
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, (float)glfwGetTime() * 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
	return model;
}
