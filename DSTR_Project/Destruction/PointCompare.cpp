#include <Pch/Pch.h>
#include "PointCompare.h"

PointCompare::PointCompare()
{
}

bool PointCompare::Compare(const PointTriangle& pt0, const PointTriangle& pt1)
{
	if (pt0.point < pt1.point)
	{
		return true;
	}
	else if (pt0.point > pt1.point)
	{
		return false;
	}
	else
	{
		return CompareAngles(pt0, pt1);
	}

}

bool PointCompare::CompareAngles(const PointTriangle& pt0, const PointTriangle& pt1)
{
	glm::vec2 rp = m_verts[pt0.point];
	
	glm::vec2 p0 = Centroid(pt0) - rp;
	glm::vec2 p1 = Centroid(pt1) - rp;

	bool q0 = ((p0.y < 0) || ((p0.y == 0) && (p0.x < 0)));
	bool q1 = ((p1.y < 0) || ((p1.y == 0) && (p1.y < 0)));

	if (q0 == q1)
	{
		// p0 and p1 are within 180 degrees of each other, so just
		// use cross product to find out if pt1 is to the left of p0.
		float cp = p0.x * p1.y - p0.y * p1.x;

		if (cp > 0)
		{
			return true;
		}
		else if (cp < 0)
		{
			return false;
		}
	}
	else
	{
		// if q0 != q1, q1 is true, then p0 is in quadrants 1 or 2,
		// and p1 is in quadrants 3 or 4. Hence, pt0 < pt1. If q1
		// is not true, vice versa.
		return q1 ? true : false;
	}
	
	
}

glm::vec2 PointCompare::Centroid(const PointTriangle& pt)
{
	int ti = pt.triangle;
	return geomtry.TriangleCentroid(m_verts[m_tris[ti]], m_verts[m_tris[ti + 1]], m_verts[m_tris[ti + 2]]);
}

void PointCompare::SetTris(std::vector<int> tris)
{
	m_tris = tris;
}

void PointCompare::SetVerts(std::vector<glm::vec2> verts)
{
	m_verts = verts;
}

void PointCompare::ClearTris()
{
	m_tris.clear();
	m_tris.shrink_to_fit();

}

void PointCompare::ClearVerts()
{
	m_verts.clear();
	m_verts.shrink_to_fit();
}
