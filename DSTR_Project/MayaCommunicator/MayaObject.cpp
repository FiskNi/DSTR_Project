#include "Pch/Pch.h"
#include "MayaObject.h"

MayaObject::MayaObject(std::string name) : GameObject(name)
{
}

MayaObject::~MayaObject()
{
	for (int i = 0; i < (int)m_meshes.size(); i++)
	{
		m_meshes[i].mesh->Destroy();
		delete m_meshes[i].mesh;
		m_meshes[i].mesh = nullptr;
	}
}

void MayaObject::Update(float dt)
{
}

void MayaObject::InitMesh(Mesh *mesh)
{
	m_meshes.emplace_back();						
	m_meshes.back().material = MaterialMap::GetInstance()->GetFirst();
	m_meshes.back().mesh = mesh;


	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
}
