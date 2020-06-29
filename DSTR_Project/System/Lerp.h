#ifndef _CUSTOM_LERP_H
#define _CUSTOM_LERP_H

static glm::vec3 CustomLerp(const glm::vec3& a, const glm::vec3& b, float t)
{
	glm::vec3 v;
	v.x = (1 - t) * a.x + t * b.x;
	v.y = (1 - t) * a.y + t * b.y;
	v.z = (1 - t) * a.z + t * b.z;

	return v;

}


#endif
