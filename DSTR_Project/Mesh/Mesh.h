#ifndef _MESH_h
#define _MESH_h
#include <Pch/Pch.h>
#include <Loader/BGLoader.h>

class Mesh
{
private:
	std::string m_name;
	std::string m_filePath;
	std::string m_materialName;
	std::string m_skeleton;						//Left blank if none
	std::vector<std::string> m_animations;		//Left empty if none

	int m_indexInFile;
	
	int m_vertexCount;
	int m_faceCount;
	std::vector<Face> m_faces;
	std::vector<Vertex> m_vertices;
	Buffers m_vertexBuffer;

public:
	Mesh();
	~Mesh();
	void Destroy();

	void setUpMesh(std::vector<Vertex> vertices, std::vector<Face> faces);
	void loadMesh(std::string fileName);
	void nameMesh(std::string name);
	glm::vec3 CenterPivot();
	void saveFilePath(std::string name, int index);
	void setUpBuffers();
	void SetMaterial(std::string matName);	

	const std::string& getMaterial() const;

	// Returns mesh local position
	Buffers getBuffers() const;

	int getVertexCount() const { return m_vertexCount; }
	int getFaceCount() const { return m_faceCount; }
	
	const std::string& getName() { return m_name; }
	const std::vector<Face>& getFaces() { return m_faces; }
	const std::vector<Vertex>& getVertices() { return m_vertices; }
};

#endif