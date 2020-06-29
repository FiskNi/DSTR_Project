#ifndef _SHADERMAP_H
#define _SHADERMAP_H
#include <Pch/Pch.h>

class ShaderMap {
public:
	static ShaderMap* GetInstance();
	//@ vsName & fsName enter just the name of the shader.
	Shader* createShader(std::string name, std::string vsName, std::string fsName);
	//@ csName enter just the name of the compute shader
	Shader* createShader(std::string name, std::string csName);
	//gs for particles
	Shader* createShader(std::string name, std::string vsName, std::string gsName, std::string fsName);

	Shader* getShader(std::string name);
	void cleanUp();
	bool existsWithName(std::string name);
	void reload();
	Shader* useByName(std::string name); //Return the object, so that we don't have to get the pointer then use, or vicea versa

	void destroy();
private:
	ShaderMap();
	static ShaderMap* m_shaderMapInstance;
	std::map<std::string, Shader*> m_shaderMap;
};




#endif