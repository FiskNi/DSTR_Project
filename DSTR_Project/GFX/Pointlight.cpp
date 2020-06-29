#include "Pch/Pch.h"


Pointlight::Pointlight() : GameObject() 
{
	SetObjectWorldPosition(glm::vec3(0));
	m_color = glm::vec3(1);
}

Pointlight::Pointlight(const glm::vec3& position, const glm::vec3& color, const float& strength) : GameObject("POINTLIGHT")
{
	SetObjectWorldPosition(position);
	m_color = color; 
	m_type = OBJECT_TYPE::POINTLIGHT;
	m_strength = strength;
	setAttenuationAndRadius(glm::vec4(1.0f, 0.09f, 0.032f, 10)); //Default Attenation values
}

Pointlight::~Pointlight()
{
}

void Pointlight::setAttenuationAndRadius(const glm::vec4 attenAndRadius)
{
	m_attenAndRadius = attenAndRadius;
}

void Pointlight::Update(float dt) {

}

void Pointlight::setLightStrength(const float& strength)
{
	m_strength = strength;
}

const float& Pointlight::getStrength() const
{
	return m_strength;
}

const glm::vec4& Pointlight::getAttenuationAndRadius() const
{
	return m_attenAndRadius;
}

const glm::vec3& Pointlight::getColor() const
{
	return m_color;
}
