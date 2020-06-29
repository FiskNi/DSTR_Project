#ifndef _POINTCOMPARE_h
#define _POINTCOMPARE_h
#include <Pch/Pch.h>


class PointCompare
{
public:

	PointCompare();
	bool Compare(const PointTriangle& pt0, const PointTriangle& pt1);
	bool CompareAngles(const PointTriangle& pt0, const PointTriangle& pt1);
	glm::vec2 Centroid(const PointTriangle& pt);

	void SetTris(std::vector<int> tris);
	void SetVerts(std::vector<glm::vec2> verts);

	void ClearTris();
	void ClearVerts();



private:
	std::vector<int> m_tris;
	std::vector<glm::vec2> m_verts;

	Geometry geomtry;

};


#endif