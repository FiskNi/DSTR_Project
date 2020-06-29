#ifndef _TRIANGLENODE_h
#define _TRIANGLENODE_h
#include <Pch/Pch.h>

// A single node in the triangle tree.
// All parameters are indexes.
// negative numbers means it doesn't exist or isn't assigned
struct TriangleNode
{
	int m_vertex_0;
	int m_vertex_1;
	int m_vertex_2;
	
	// Child triangle
	int m_C0;
	int m_C1;
	int m_C2;
	
	// Adjacent triangle (vertex_1 & vertex_2 as an edge).
	int m_A0;
	int m_A1;
	int m_A2;

	TriangleNode(int vertex_0, int vertex_1, int vertex_2)
	{
		m_vertex_0 = vertex_0;
		m_vertex_1 = vertex_1;
		m_vertex_2 = vertex_2;

		m_C0 = -1;
		m_C1 = -1;
		m_C2 = -1;

		m_A0 = -1;
		m_A1 = -1;
		m_A2 = -1;
	}

	bool Is_Leaf() { return m_C0 < 0 && m_C1 < 0 && m_C2 < 0; }
	bool Is_Inner() { return m_vertex_0 >= 0 && m_vertex_1 >= 0 && m_vertex_2 >= 0; }

	bool Has_Edge(int edge_0, int edge_1)
	{ 
		if (edge_0 == m_vertex_0)
			return edge_1 == m_vertex_1 || edge_1 == m_vertex_2;
		else if(edge_0 == m_vertex_1)
			return edge_1 == m_vertex_0 || edge_1 == m_vertex_2;
		else if (edge_0 == m_vertex_2)
			return edge_1 == m_vertex_0 || edge_1 == m_vertex_1;

		return false; 
	}

	int GetLastPoint(int vertex_0, int vertex_1)
	{
		if (vertex_0 == m_vertex_0)
		{
			if (vertex_1 == m_vertex_1) return m_vertex_2;
			if (vertex_1 == m_vertex_2) return m_vertex_1;
		}
		if (vertex_0 == m_vertex_1)
		{
			if (vertex_1 == m_vertex_0) return m_vertex_2;
			if (vertex_1 == m_vertex_2) return m_vertex_0;
		}
		if (vertex_0 == m_vertex_2)
		{
			if (vertex_1 == m_vertex_0) return m_vertex_1;
			if (vertex_1 == m_vertex_1) return m_vertex_0;
		}
		else
		{
			logTrace("DSTR: Error");
		}
	}

	int GetOpposite(int vertex)
	{
		if (vertex == m_vertex_0) return m_A0;
		if (vertex == m_vertex_1) return m_A1;
		if (vertex == m_vertex_2) return m_A2;

		logTrace("DSTR: Error");
	}





};


#endif

