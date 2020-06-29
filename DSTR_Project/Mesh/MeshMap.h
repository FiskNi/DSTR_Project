#ifndef _MESHMAP_H
#define _MESHMAP_H
#include <Pch/Pch.h>
#include <Mesh/MeshFormat.h>
#include <Mesh/Mesh.h>

class MeshMap {

public:
	static MeshMap* GetInstance();
	void cleanUp();
	bool existsWithName(std::string name);
	Mesh* GetMesh(std::string name);
	Mesh* createMesh(std::string name, Mesh mesh);
	Mesh* AddMesh(std::string name, Mesh* mesh);
	void removeMesh(std::string name);
	void destroy();

private:
	MeshMap();
	static MeshMap* m_meshMapInstance;
	std::map<std::string, Mesh*> m_meshMap;

};


#endif