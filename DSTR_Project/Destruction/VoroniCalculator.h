#ifndef _VORONICALCULATOR_h
#define _VORONICALCULATOR_h
#include <Pch/Pch.h>
#include <Destruction/DelaunayGenerator.h>
#include <Destruction/PointCompare.h>

class VoroniCalculator
{
private:


public:
	VoroniCalculator();
	~VoroniCalculator();

	VoroniDiagram CalculateDiagram(std::vector<glm::vec2> inputVertices);
	void CalculateDiagram(std::vector<glm::vec2> inputVertices, VoroniDiagram &diagram);

	int NonSharedPoint(std::vector<int> tris, int ti0, int ti1);
	bool SharesEdge(std::vector<int> tris, int ti0, int ti1);


private:
	DelaunayGenerator triangulator;
	PointCompare compare;
	std::vector<PointTriangle> pts;

	Geometry geometry;

};

#endif

