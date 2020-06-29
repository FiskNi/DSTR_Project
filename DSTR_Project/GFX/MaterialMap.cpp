#include "Pch/Pch.h"
#include "MaterialMap.h"

MaterialMap* MaterialMap::m_materialMapInstance = 0;

MaterialMap::MaterialMap() 
{
	createMaterial("N_Default");
}

MaterialMap* MaterialMap::GetInstance()
{
	if (m_materialMapInstance == 0) {
		m_materialMapInstance = new MaterialMap();
	}
	return m_materialMapInstance;
}

void MaterialMap::cleanUp()
{
	std::map<std::string, Material*>::iterator it;

	for (it = m_Material.begin(); it != m_Material.end(); it++) {
		delete it->second;
	}

	m_Material.clear();
}

bool MaterialMap::existsWithName(std::string name)
{
	if (m_Material.find(name) != m_Material.end()) {
		return true;
	}
	return false;
}


void MaterialMap::useByName(std::string name)
{
	//TODO: Remove?

	//if (existsWithName(name)) {
	//	m_Materials[name]->use();
	//}
}

Material* MaterialMap::getMaterial(std::string name)
{
	if (existsWithName(name))
	{
		return m_Material[name];
	}

	return nullptr;
}

Material* MaterialMap::GetFirst()
{
	std::map<std::string, Material*>::iterator it;
	it = m_Material.begin();
	if (it->second)
		return it->second;

	return nullptr;
}


Material* MaterialMap::createMaterial(std::string name, Material material)
{
	if (existsWithName(name))
	{
		logWarning("Material {0} already exists", name);
		return nullptr;
	}
	Material* newMaterial = new Material(name);
	*newMaterial = material;
	m_Material[name] = newMaterial;
	return newMaterial;
}

Material* MaterialMap::createMaterial(std::string name)
{
	if (existsWithName(name))
	{
		logWarning("Material {0} already exists", name);
		return nullptr;
	}
	Material* newMaterial = new Material(name);
	m_Material[name] = newMaterial;
	return newMaterial;
}




void MaterialMap::destroy()
{
	cleanUp();
	delete m_materialMapInstance;
	m_materialMapInstance = nullptr;
}
