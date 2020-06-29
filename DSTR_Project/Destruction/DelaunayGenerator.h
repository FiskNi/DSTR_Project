#ifndef _DELAUNAYGENERATOR_h
#define _DELAUNAYGENERATOR_h
#include <Pch/Pch.h>


class DelaunayGenerator
{
public:
	DelaunayGenerator();
	~DelaunayGenerator();

	void Triangulate(std::vector<glm::vec2> points, DelaunayTriangulation& triangulation);
	bool Higher(int index_0, int index_1);
	void BoyerWatson();
	void GenerateResults(DelaunayTriangulation& triangulation);

	int FindTriangleNode(int index);
	bool PointInTriangle(int pointIndex, int triangleIndex);
	bool ToTheLeft(int pointIndex, int leftIndex_0, int leftIndex_1);

	int LeafWithEdge(int triangleIndex, int edge_0, int edge_1);
	bool LegalEdge(int k, int l, int i, int j);
	void LegalizeEdge(int index_0, int index_1, int pointIndex, int edge_0, int edge_1);

private:
	std::vector<glm::vec2> m_vertices;
	std::vector<TriangleNode> m_triangles;

	int highest;

	Geometry m_geometry;


};

#endif
