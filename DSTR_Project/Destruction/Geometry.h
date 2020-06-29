#ifndef _GEOMETRY_h
#define _GEOMETRY_h
#include <Pch/Pch.h>

class Geometry
{
public:
	// Is point p to the left of the line from l0 to l1?
	static bool ToTheLeft(glm::vec2 p, glm::vec2 l0, glm::vec2 l1)
	{
		return ((l1.x - l0.x) * (p.y - l0.y) - (l1.y - l0.y) * (p.x - l0.x)) >= 0;
	}

	// Is point p to the right of the line from l0 to l1?
	static bool ToTheRight(glm::vec2 p, glm::vec2 l0, glm::vec2 l1) 
	{
		return !ToTheLeft(p, l0, l1);
	}

	// Is point p inside the triangle formed by c0, c1 and c2 (assuming c1,
	// c2 and c3 are in CCW order)
	static bool PointInTriangle(glm::vec2 p, glm::vec2 c0, glm::vec2 c1, glm::vec2 c2) 
	{
		return ToTheLeft(p, c0, c1)
			&& ToTheLeft(p, c1, c2)
			&& ToTheLeft(p, c2, c0);
	}

	// Is point p inside the circumcircle formed by c0, c1 and c2?
	static bool InsideCircumcircle(glm::vec2 p, glm::vec2 c0, glm::vec2 c1, glm::vec2 c2) 
	{
		float ax = c0.x - p.x;
		float ay = c0.y - p.y;
		float bx = c1.x - p.x;
		float by = c1.y - p.y;
		float cx = c2.x - p.x;
		float cy = c2.y - p.y;

		return (
			(ax * ax + ay * ay) * (bx * cy - cx * by) -
			(bx * bx + by * by) * (ax * cy - cx * ay) +
			(cx * cx + cy * cy) * (ax * by - bx * ay)
			) > 0;
	}

	// Rotate vector v left 90 degrees
	static glm::vec2 RotateRightAngle(glm::vec2 v) 
	{
		float x = v.x;
		v.x = -v.y;
		v.y = x;

		return v;
	}

	// General line/line intersection method. Each line is defined by a
	// two vectors, a point on the line (p0 and p1 for the two lines) and a
	// direction vector (v0 and v1 for the two lines). The returned value
	// indicates whether the lines intersect. m0 and m1 are the
	// coefficients of how much you have to multiply the direction vectors
	// to get to the intersection. 
	//
	// In other words, if the intersection is located at X, then: 
	//
	//     X = p0 + m0 * v0
	//     X = p1 + m1 * v1
	//
	// By checking the m0/m1 values, you can check intersections for line
	// segments and rays.
	static bool LineLineIntersection(glm::vec2 p0, glm::vec2 v0, glm::vec2 p1, glm::vec2 v1, float &m0, float &m1) 
	{
		float det = (v0.x * v1.y - v0.y * v1.x);

		if (glm::abs(det) < 0.000001f) 
		{
			m0 = NAN;
			m1 = NAN;

			return false;
		}
		else 
		{
			m0 = ((p0.y - p1.y) * v1.x - (p0.x - p1.x) * v1.y) / det;

			if (glm::abs(v1.x) >= 0.000001f) 
			{
				m1 = (p0.x + m0 * v0.x - p1.x) / v1.x;
			}
			else 
			{
				m1 = (p0.y + m0 * v0.y - p1.y) / v1.y;
			}

			return true;
		}
	}

	// Returns the intersections of two lines. p0/p1 are points on the
	// line, v0/v1 are the direction vectors for the lines. 
	//
	// If there are no intersections, returns a NaN vector
	static glm::vec2 LineLineIntersection(glm::vec2 p0, glm::vec2 v0, glm::vec2 p1, glm::vec2 v1) 
	{
		float m0, m1;

		if (LineLineIntersection(p0, v0, p1, v1, m0, m1)) 
		{
			return p0 + m0 * v0;
		}
		else 
		{
			return glm::vec2(NAN, NAN);
		}
	}

	// Returns the center of the circumcircle defined by three points (c0,
	// c1 and c2) on its edge.
	static glm::vec2 CircumcircleCenter(glm::vec2 c0, glm::vec2 c1, glm::vec2 c2) 
	{
		glm::vec2 mp0 = 0.5f * (c0 + c1);
		glm::vec2 mp1 = 0.5f * (c1 + c2);

		glm::vec2 v0 = RotateRightAngle(c0 - c1);
		glm::vec2 v1 = RotateRightAngle(c1 - c2);

		float m0, m1;

		LineLineIntersection(mp0, v0, mp1, v1, m0, m1);

		return mp0 + m0 * v0;
	}

	// Returns the triangle centroid for triangle defined by points c0, c1
	// and c2. 
	static glm::vec2 TriangleCentroid(glm::vec2 c0, glm::vec2 c1, glm::vec2 c2) 
	{
		glm::vec2 val = (1.0f / 3.0f) * (c0 + c1 + c2);
		return val;
	}

	// Returns the signed area of a polygon. CCW polygons return a positive
	// area, CW polygons return a negative area.
	static float Area(std::vector<glm::vec2> polygon) 
	{
		float area = 0.0f;

		int count = polygon.size();

		for (int i = 0; i < count; i++) 
		{
			int j = (i == count - 1) ? 0 : (i + 1);

			glm::vec2 p0 = polygon[i];
			glm::vec2 p1 = polygon[j];

			area += p0.x * p1.y - p1.y * p1.x;
		}


		return 0.5f * area;
	}

};





#endif