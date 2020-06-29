#include <Pch/Pch.h>
#include "Mesh.h"

Mesh::Mesh()
{
	m_name = {};
	m_filePath = {};
	m_materialName = {};
	m_skeleton = {};
	m_indexInFile = 0;

	m_vertexCount = 0;
	m_faceCount = 0;
}

Mesh::~Mesh()
{
	
}

void Mesh::Destroy()
{	
	glDeleteVertexArrays(1, &m_vertexBuffer.vao);
	glDeleteBuffers(1, &m_vertexBuffer.vbo);
	glDeleteBuffers(1, &m_vertexBuffer.ibo);
	
}

void Mesh::setUpMesh(std::vector<Vertex> vertices, std::vector<Face> faces)
{
	m_vertexCount = (int)vertices.size();
	m_faceCount = (int)faces.size();

	m_vertices = vertices;
	m_faces = faces;
}

void Mesh::loadMesh(std::string fileName)
{
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + fileName);

	saveFilePath(tempLoader.GetFileName(), 0);
	nameMesh(tempLoader.GetMeshName());
	setUpMesh(tempLoader.GetVertices(), tempLoader.GetFaces());
}

void Mesh::nameMesh(std::string name)
{
	m_name = name;
}

glm::vec3 Mesh::CenterPivot()
{
	glm::vec3 min = m_vertices[0].position;
	glm::vec3 max = m_vertices[0].position;
	for (size_t i = 0; i < m_vertices.size(); i++)
	{
		min.x = fminf(m_vertices[i].position.x, min.x);
		min.y = fminf(m_vertices[i].position.y, min.y);
		min.z = fminf(m_vertices[i].position.z, min.z);

		max.x = fmaxf(m_vertices[i].position.x, max.x);
		max.y = fmaxf(m_vertices[i].position.y, max.y);
		max.z = fmaxf(m_vertices[i].position.z, max.z);
	}

	glm::vec3 center = glm::vec3((min + max) * 0.5f);
	for (size_t i = 0; i < m_vertices.size(); i++)
	{
		m_vertices[i].position -= center;
	}

	return center;
}

void Mesh::saveFilePath(std::string name, int index)
{
	m_filePath = name;
	m_indexInFile = index;
}

void Mesh::setUpBuffers()
{
	glGenVertexArrays(1, &m_vertexBuffer.vao);
	glGenBuffers(1, &m_vertexBuffer.vbo);
	glGenBuffers(1, &m_vertexBuffer.ibo);

	glBindVertexArray(m_vertexBuffer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(int) * 3, &m_faces[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normals));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
	//vertex tangent for normal mapping
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	//glBindVertexArray(0);


	m_vertexBuffer.nrOfFaces = static_cast<int>(m_faces.size());
}

void Mesh::SetMaterial(std::string matName)
{
	m_materialName = matName;
}

const std::string& Mesh::getMaterial() const
{
	return m_materialName;
}

Buffers Mesh::getBuffers() const
{
	return m_vertexBuffer;
}
