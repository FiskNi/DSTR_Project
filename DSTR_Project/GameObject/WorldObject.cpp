  #include "Pch/Pch.h"
#include "WorldObject.h"

WorldObject::WorldObject()
	: GameObject()
{
	m_type = 0;
}

WorldObject::WorldObject(std::string name) 
	: GameObject(name)
{
	m_type = 0;
}

WorldObject::~WorldObject()
{

}

void WorldObject::Update(float dt)
{

}
