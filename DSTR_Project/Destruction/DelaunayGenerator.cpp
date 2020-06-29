#include "Pch/Pch.h"
#include "DelaunayGenerator.h"

DelaunayGenerator::DelaunayGenerator()
{
	highest = 0;
}

DelaunayGenerator::~DelaunayGenerator()
{
}

void DelaunayGenerator::Triangulate(std::vector<glm::vec2> points, DelaunayTriangulation& triangulation)
{
	if ((int)points.size() < 3)
	{
		//TODO: No implementation for less than 3 points
		return;
	}

	m_vertices.clear();
	m_vertices.shrink_to_fit();
	m_triangles.clear();
	m_triangles.shrink_to_fit();

	m_vertices = points;

	highest = 0;
	for (int i = 0; i < (int)m_vertices.size(); i++)
	{
		if (Higher(highest, i))
		{
			highest = i;
		}
	}

	m_triangles.push_back(TriangleNode(-2, -1, highest));
	BoyerWatson();
	GenerateResults(triangulation);

}

bool DelaunayGenerator::Higher(int index_0, int index_1)
{
	if (index_0 == -2)
	{
		return false;
	}
	else if (index_0 == -1)
	{
		return true;
	}
	else if (index_1 == -2)
	{
		return true;
	}
	else if (index_1 == -1)
	{
		return false;
	}
	else {
		glm::vec2 vertex_0 = m_vertices[index_0];
		glm::vec2 vertex_1 = m_vertices[index_1];

		if (vertex_0.y < vertex_1.y)
		{
			return true;
		}
		else if (vertex_0.y > vertex_1.y)
		{
			return false;
		}
		else
		{
			return vertex_0.x < vertex_1.x;
		}
	}


}

void DelaunayGenerator::BoyerWatson()
{
	for (int i = 0; i < m_vertices.size(); i++)
	{
		int pointIndex = i;
		if (pointIndex == highest)
			continue;

		int triangleIndex = FindTriangleNode(pointIndex);

		TriangleNode& triangleNode = m_triangles[triangleIndex];

		float point_0 = triangleNode.m_vertex_0;
		float point_1 = triangleNode.m_vertex_1;
		float point_2 = triangleNode.m_vertex_2;

		int newTindex_0 = m_triangles.size();
		int newTindex_1 = newTindex_0 + 1;
		int newTindex_2 = newTindex_0 + 2;

		TriangleNode newTriangle_0(pointIndex, point_0, point_1);
		TriangleNode newTriangle_1(pointIndex, point_1, point_2);
		TriangleNode newTriangle_2(pointIndex, point_2, point_0);

		newTriangle_0.m_A0 = triangleNode.m_A2;
		newTriangle_1.m_A0 = triangleNode.m_A0;
		newTriangle_2.m_A0 = triangleNode.m_A1;

		newTriangle_0.m_A1 = newTindex_1;
		newTriangle_1.m_A1 = newTindex_2;
		newTriangle_2.m_A1 = newTindex_0;

		newTriangle_0.m_A2 = newTindex_2;
		newTriangle_1.m_A2 = newTindex_0;
		newTriangle_2.m_A2 = newTindex_1;


		triangleNode.m_C0 = newTindex_0;
		triangleNode.m_C1 = newTindex_1;
		triangleNode.m_C2 = newTindex_2;

		m_triangles[triangleIndex] = triangleNode;

		m_triangles.push_back(newTriangle_0);
		m_triangles.push_back(newTriangle_1);
		m_triangles.push_back(newTriangle_2);

		if (newTriangle_0.m_A0 != -1) LegalizeEdge(newTindex_0, newTriangle_0.m_A0, pointIndex, point_0, point_1);
		if (newTriangle_1.m_A0 != -1) LegalizeEdge(newTindex_1, newTriangle_1.m_A0, pointIndex, point_1, point_2);
		if (newTriangle_2.m_A0 != -1) LegalizeEdge(newTindex_2, newTriangle_2.m_A0, pointIndex, point_2, point_0);

	}
}

void DelaunayGenerator::GenerateResults(DelaunayTriangulation& triangulation)
{
	triangulation.Clear();

	for (int i = 0; i < (int)m_vertices.size(); i++)
	{
		triangulation.vertices.push_back(m_vertices[i]);
	}

	for (int i = 1; i < (int)m_triangles.size(); i++)
	{
		if (m_triangles[i].Is_Leaf() && m_triangles[i].Is_Inner())
		{
			triangulation.faces.push_back(m_triangles[i].m_vertex_0);
			triangulation.faces.push_back(m_triangles[i].m_vertex_1);
			triangulation.faces.push_back(m_triangles[i].m_vertex_2);
			
		}
	}

}

int DelaunayGenerator::FindTriangleNode(int pointIndex)
{
	int current = 0;
	while (!m_triangles[current].Is_Leaf())
	{
		TriangleNode& node = m_triangles[current];
		if (node.m_C0 >= 0 && PointInTriangle(pointIndex, node.m_C0))
		{
			current = node.m_C0;
		}
		else if (node.m_C1 >= 0 && PointInTriangle(pointIndex, node.m_C1))
		{
			current = node.m_C1;
		}
		else
		{
			//assert(node.m_C2 >= 0 && PointInTriangle(pointIndex, node.m_C2));
			current = node.m_C2;
		}

		if (current == -1)
			return 0;
	}

	return current;
}

bool DelaunayGenerator::PointInTriangle(int pointIndex, int triangleIndex)
{
	TriangleNode& node = m_triangles[triangleIndex];
	return ToTheLeft(pointIndex, node.m_vertex_0, node.m_vertex_1)
		&& ToTheLeft(pointIndex, node.m_vertex_1, node.m_vertex_2)
		&& ToTheLeft(pointIndex, node.m_vertex_2, node.m_vertex_0);
}

bool DelaunayGenerator::ToTheLeft(int pointIndex, int edge_0, int edge_1)
{
	if (edge_0 == -2)
	{
		return Higher(edge_1, pointIndex);
	}
	else if (edge_0 == -1)
	{
		return Higher(pointIndex, edge_1);
	}
	else if (edge_1 == -2)
	{
		return Higher(pointIndex, edge_0);
	}
	else if (edge_1 == -1)
	{
		return Higher(edge_0, pointIndex);
	}
	else
	{
		//assert(edge_0 >= 0);
		//assert(edge_1 >= 0);

		return m_geometry.ToTheLeft(m_vertices[pointIndex], m_vertices[edge_0], m_vertices[edge_1]);
	}

}


int DelaunayGenerator::LeafWithEdge(int triangleIndex, int edge_0, int edge_1)
{
	//assert(m_triangles[triangleIndex].Has_Edge(edge_0, edge_1));

	while (!m_triangles[triangleIndex].Is_Leaf())
	{
		TriangleNode& tNode = m_triangles[triangleIndex];

		if (tNode.m_C0 != -1 && m_triangles[tNode.m_C0].Has_Edge(edge_0, edge_1))
		{
			triangleIndex = tNode.m_C0;
		}
		else if (tNode.m_C1 != -1 && m_triangles[tNode.m_C1].Has_Edge(edge_0, edge_1))
		{
			triangleIndex = tNode.m_C1;
		}
		else if (tNode.m_C2 != -1 && m_triangles[tNode.m_C2].Has_Edge(edge_0, edge_1))
		{
			triangleIndex = tNode.m_C2;
		}
	}

	return triangleIndex;
}

bool DelaunayGenerator::LegalEdge(int k, int l, int i, int j)
{
	//assert(k != highest && k >= 0);

	float lMagic = l < 0;
	float iMagic = i < 0;
	float jMagic = j < 0;

	//assert(!(iMagic && jMagic));

	if (lMagic)
	{
		return true;
	}
	else if (iMagic)
	{
		//assert(!jMagic);

		glm::vec2 p = m_vertices[l];
		glm::vec2 l0 = m_vertices[k];
		glm::vec2 l1 = m_vertices[j];

		return m_geometry.ToTheLeft(p, l0, l1);
	}
	else if (jMagic)
	{
		//assert(!iMagic);

		glm::vec2 p = m_vertices[l];
		glm::vec2 l0 = m_vertices[k];
		glm::vec2 l1 = m_vertices[i];

		return !m_geometry.ToTheLeft(p, l0, l1);
	}
	else
	{
		//assert(k >= 0 && l >= 0 && i >= 0 && j >= 0);

		glm::vec2 p = m_vertices[l];
		glm::vec2 c0 = m_vertices[k];
		glm::vec2 c1 = m_vertices[i];
		glm::vec2 c2 = m_vertices[j];

		//assert(m_geometry.ToTheLeft(c2, c0, c1));
		//assert(m_geometry.ToTheLeft(c2, c1, p));

		return !m_geometry.InsideCircumcircle(p, c0, c1, c2);
	}


	return false;
}

void DelaunayGenerator::LegalizeEdge(int index_0, int index_1, int pointIndex, int edge_0, int edge_1)
{
	index_1 = LeafWithEdge(index_1, edge_0, edge_1);

	TriangleNode triangle_0 = m_triangles[index_0];
	TriangleNode triangle_1 = m_triangles[index_1];
	int index_q = triangle_1.GetLastPoint(edge_0, edge_1);


	//assert(triangle_0.Has_Edge(edge_0, edge_1));
	//assert(triangle_1.Has_Edge(edge_0, edge_1));
	//assert(triangle_0.Is_Leaf());
	//assert(triangle_1.Is_Leaf());
	//assert(triangle_0.m_vertex_0 == pointIndex || triangle_0.m_vertex_1 == pointIndex || triangle_0.m_vertex_2 == pointIndex);
	//assert(triangle_1.m_vertex_0 == index_q || triangle_1.m_vertex_1 == index_q || triangle_1.m_vertex_2 == index_q);

	if (!LegalEdge(pointIndex, index_q, edge_0, edge_1))
	{
		int index_2 = m_triangles.size();
		int index_3 = index_2 + 1;

		TriangleNode triangle_2(pointIndex, edge_0, index_q);
		TriangleNode triangle_3(pointIndex, index_q, edge_1);

		triangle_2.m_A0 = triangle_1.GetOpposite(edge_1);
		triangle_2.m_A1 = index_3;
		triangle_2.m_A2 = triangle_0.GetOpposite(edge_1);

		triangle_3.m_A0 = triangle_1.GetOpposite(edge_0);
		triangle_3.m_A1 = triangle_0.GetOpposite(edge_0);
		triangle_3.m_A2 = index_2;

		m_triangles.push_back(triangle_2);
		m_triangles.push_back(triangle_3);

		TriangleNode newTriangle_0 = m_triangles[index_0];
		TriangleNode newTriangle_1 = m_triangles[index_1];

		newTriangle_0.m_C0 = index_2;
		newTriangle_0.m_C1 = index_3;

		newTriangle_1.m_C0 = index_2;
		newTriangle_1.m_C1 = index_3;

		m_triangles[index_0] = newTriangle_0;
		m_triangles[index_1] = newTriangle_1;

		if (triangle_2.m_A0 != -1) LegalizeEdge(index_2, triangle_2.m_A0, pointIndex, edge_0, index_q);
		if (triangle_3.m_A0 != -1) LegalizeEdge(index_3, triangle_3.m_A0, pointIndex, index_q, edge_1);

	}
}
