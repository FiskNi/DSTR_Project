#ifndef _SHADER_H
#define _SHADER_H
#include <Pch/Pch.h>
#include <GFX/MaterialMap.h>
#include <Texture/stb_image.h>

class Shader
{
public:
	Shader();
	Shader(std::string compute);
	Shader(std::string vertex, std::string fragment);
	Shader(std::string vertex, std::string geometry, std::string fragment);
	~Shader();

	void use();
	void unuse();

	void clearBinding(); /* CLEAR THIS BINDING AFTER EACH BATCH OF RENDERING*/

	void setMat3(std::string name, glm::mat3 mat);
	void setMat4(std::string name, glm::mat4 mat);
	void setVec2(std::string name, glm::vec2 vec);
	void setVec3(std::string name, glm::vec3 vec);
	void setVec4(std::string name, glm::vec4 vec);
	void setFloat(std::string name, float num);
	void setInt(std::string name, int num);
	void setName(std::string name);
	//Sets a standard material
	void SetMaterial(const std::string& materialName);
	void SetMaterial(Material* material);
	void unbindMaterial(const std::string& materialName);
	void unbindMaterial(Material* material);
	bool getValid() const;
	int getShaderID() const;
	std::string getName() const;
	std::vector<std::string> getShaderNames() const;

	Shader& operator=(const Shader& other);

private:

	std::string m_oldMaterial;
	int m_totalBoundTextures; //Keep track of the largest set of textures bound at once. 

	void shaderSetup(std::string shaderName, unsigned int& shader);
	bool m_valid;
	std::string m_name;
	int m_shaderProg;
	std::vector<std::string> m_shaderNames; //We need to save the name of the shaders. 
};

#endif