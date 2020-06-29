#include <Pch/Pch.h>
#include "ShaderMap.h"

//std::map<std::string, Shader*> ShaderMap::m_shaderMap;
ShaderMap* ShaderMap::m_shaderMapInstance = 0;

ShaderMap::ShaderMap() {}


ShaderMap* ShaderMap::GetInstance()
{
	if (m_shaderMapInstance == 0) {
		m_shaderMapInstance = new ShaderMap();
	}
	return m_shaderMapInstance;
}

Shader* ShaderMap::createShader(std::string name, std::string vsName, std::string fsName)
{
	//If we have already reserved the name
	if (existsWithName(name)) {
		return NULL;
	}

	Shader* shader = new Shader(vsName, fsName);
	m_shaderMap[name] = shader;
	return shader;
}

Shader* ShaderMap::createShader(std::string name, std::string csName) {
	if (existsWithName(name)) {
		return NULL;
	}

	Shader* shader = new Shader(csName);
	m_shaderMap[name] = shader;
	return shader;
}

Shader* ShaderMap::createShader(std::string name, std::string vsName, std::string gsName, std::string fsName)
{
	if (existsWithName(name)) {
		return NULL;
	}

	Shader* shader = new Shader(vsName, gsName, fsName);
	m_shaderMap[name] = shader;
	return shader;
}

Shader* ShaderMap::getShader(std::string name)
{
	if (existsWithName(name)) 
	{
		return m_shaderMap[name];
	}

	return nullptr;
}

void ShaderMap::cleanUp()
{
	std::map<std::string, Shader*>::iterator it;

	for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
		delete it->second;
	}

	m_shaderMap.clear();
}

bool ShaderMap::existsWithName(std::string name)
{
	if (m_shaderMap.find(name) != m_shaderMap.end()) {
		return true;
	}
	return false;
}

void ShaderMap::reload() {
	std::map<std::string, Shader*>::iterator it;

	for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
		Shader* tempShader;
		if (it->second->getShaderNames().size() == 1) {
			tempShader = new Shader(it->second->getShaderNames()[0]);
		}
		else if (it->second->getShaderNames().size() == 3) {
			tempShader = new Shader(it->second->getShaderNames()[0], it->second->getShaderNames()[1], it->second->getShaderNames()[2]);
		}
		else {
			tempShader = new Shader(it->second->getShaderNames()[0], it->second->getShaderNames()[1]);
		}
		if (tempShader->getValid()) {
			delete it->second;
			it->second = tempShader;

			logInfo("Recompile Shader Completed {0}", tempShader->getShaderNames().at(0));
		}
		else {
			delete tempShader;
		}
	}

	for (it = m_shaderMap.begin(); it != m_shaderMap.end(); it++) {
		logTrace("Shader ID: {0}", it->second->getShaderID());
	}
}

Shader* ShaderMap::useByName(std::string name) {

	if (existsWithName(name)) {
		m_shaderMap[name]->use();
		return m_shaderMap[name];
	}
	return nullptr;
}

void ShaderMap::destroy()
{
	cleanUp();
	delete m_shaderMapInstance;
	m_shaderMapInstance = nullptr;
}
