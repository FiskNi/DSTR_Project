#ifndef _VORONICLIPPER_h
#define _VORONICLIPPER_h
#include <Pch/Pch.h>

class VoroniClipper
{
public:
	void ClipSite(VoroniDiagram diagram, const std::vector<glm::vec2>& polygon, int site, std::vector<glm::vec2>& clipped);



private:
	std::vector<glm::vec2> pointsIn;
	std::vector<glm::vec2> pointsOut;

	Geometry geometry;


};

#endif