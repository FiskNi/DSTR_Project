#include "Pch/Pch.h"
#include "MapObject.h"

MapObject::MapObject() : GameObject()
{
	m_type = 0;
}

MapObject::MapObject(std::string name) : GameObject(name)
{
	m_type = 0;
	MaterialMap* matMap = MaterialMap::GetInstance(); 
	/* Loop through all of the object and set the materials rim lighting value*/
	for (auto mesh : m_meshes) {
		//Set the rimlighting value
		//mesh.material->rimLighting = 1;
	}
}

MapObject::~MapObject()
{
}

void MapObject::Update(float dt)
{
}
