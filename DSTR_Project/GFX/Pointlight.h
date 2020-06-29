#ifndef _POINTLIGHT_h
#define _POINTLIGHT_h
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

class Pointlight : public GameObject {
public: 
	Pointlight();
	Pointlight(const glm::vec3& position, const glm::vec3& color, const float& strength);


	virtual ~Pointlight();

	void setAttenuationAndRadius(const glm::vec4 attenAndRadius);
	void Update(float dt);

	void setLightStrength(const float& strength);
	const float& getStrength() const;
	const glm::vec4& getAttenuationAndRadius() const;
	const glm::vec3& getColor() const;
private: 
	glm::vec3 m_color;
	float m_strength; //Light strength
	glm::vec4 m_attenAndRadius; //First 3 dims are for the attenuation, final 4th is for radius
};


#endif