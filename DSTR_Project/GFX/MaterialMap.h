#ifndef _MATERIALMAP_h
#define _MATERIALMAP_h
#include <Pch/Pch.h>
#include <Mesh/MeshFormat.h>

class MaterialMap {

public:
	static MaterialMap* GetInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	void useByName(std::string name);
	Material* getMaterial(std::string name);
	Material* GetFirst();
	Material* createMaterial(std::string name, Material material);
	Material* createMaterial(std::string name);
	void destroy();

private:

	MaterialMap();
	static MaterialMap* m_materialMapInstance;
	std::map<std::string, Material*> m_Material;
};


#endif