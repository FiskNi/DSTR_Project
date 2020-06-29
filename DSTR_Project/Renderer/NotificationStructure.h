#ifndef _NOTIFICATION_STRUCTURE_H
#define _NOTIFICATION_STRUCTURE_H
#include <Pch/Pch.h>

struct NotificationText {
	float alphaColor;
	std::vector<std::pair<std::string, glm::vec3>> textParts;
	unsigned int width = 0;
	glm::vec3 scale = glm::vec3(1.0f);
	float lifeTimeInSeconds = 1.0f;
	bool useAlpha = true;
};


#endif
