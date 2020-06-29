#pragma once
#include <Pch/Pch.h>

struct DelaunayTriangulation
{
	std::vector<glm::vec2> vertices;
	std::vector<int> faces;

	std::vector<Vertex> GetAsVertices()
	{
		std::vector<Vertex> result;
		result.resize(vertices.size());
		for (int i = 0; i < (int)vertices.size(); i++)
		{
			glm::vec3 pos = glm::vec3(vertices[i].x, vertices[i].y, 0.0f);
			result[i].position = pos;
		}

		return result;
	}

	std::vector<Face> GetAsFaces()
	{
		std::vector<Face> result;
		result.resize((int)faces.size() / 3);
		for (int i = 0; i < (int)faces.size(); i += 3)
		{
			result[i / 3].indices[0] = faces[i];
			result[i / 3].indices[1] = faces[i + 1];
			result[i / 3].indices[2] = faces[i + 2];
		}

		return result;
	}

	void Clear()
	{
		vertices.clear();
		vertices.shrink_to_fit();
		faces.clear();
		faces.shrink_to_fit();
	}

};

struct PointTriangle
{
	int point;
	int triangle;

	PointTriangle(int p, int t)
	{
		point = p;
		triangle = t;
	}
};

struct Edge
{
	enum EdgeType
	{
		Line,
		RayCCW,
		RayCW,
		Segment
	};

	EdgeType type;

	int site;
	int vertex_0;
	int vertex_1;

	glm::vec2 direction;

	Edge(EdgeType o_type, int o_site, int o_vertex_0, int o_vertex_1, glm::vec2 o_direction)
	{
		type = o_type;
		site = o_site;
		vertex_0 = o_vertex_0;
		vertex_1 = o_vertex_1;
		direction = o_direction;
	}

};

struct VoroniDiagram
{
	DelaunayTriangulation triangulation;

	std::vector<glm::vec2> sites;
	std::vector<glm::vec2> vertices;
	std::vector<Edge> edges;

	std::vector<int> firstEdgeBySite;

	VoroniDiagram()
	{
		sites = triangulation.vertices;
	}

	void Clear()
	{
		triangulation.Clear();

		sites.clear();
		sites.shrink_to_fit();
		vertices.clear();
		vertices.shrink_to_fit();
		edges.clear();
		edges.shrink_to_fit();
		firstEdgeBySite.clear();
		firstEdgeBySite.shrink_to_fit();
	}


};