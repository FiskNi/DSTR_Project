#include "Pch/Pch.h"
#include "VoroniCalculator.h"
#include <functional>


VoroniCalculator::VoroniCalculator()
{
}

VoroniCalculator::~VoroniCalculator()
{
}

VoroniDiagram VoroniCalculator::CalculateDiagram(std::vector<glm::vec2> inputVertices)
{
	VoroniDiagram diagram;
	CalculateDiagram(inputVertices, diagram);
	return diagram;
}

void VoroniCalculator::CalculateDiagram(std::vector<glm::vec2> inputVertices, VoroniDiagram& diagram)
{
	if ((int)inputVertices.size() < 3)
	{
		//TODO: no implementation for less than 3 vertices
		return;
	}

	diagram.Clear();
	DelaunayTriangulation& trig = diagram.triangulation;
	triangulator.Triangulate(inputVertices, trig);


	for (int i = 0; i < trig.faces.size(); i += 3)
	{
		glm::vec2 c0 = trig.vertices[trig.faces[i]];
		glm::vec2 c1 = trig.vertices[trig.faces[i + 1]];
		glm::vec2 c2 = trig.vertices[trig.faces[i + 2]];


		for (int j = 0; j < trig.vertices.size(); j++)
		{
			glm::vec2 p = trig.vertices[j];

			//assert(!geometry.InsideCircumcircle(p, c0, c1, c2));
		}
	}

	std::vector<glm::vec2>& verts = trig.vertices;
	std::vector<int>& faces = trig.faces;
	std::vector<glm::vec2>& centers = diagram.vertices;
	std::vector<Edge>& edges = diagram.edges;

	// Allocate momery
	pts.clear();
	pts.shrink_to_fit();
	if (faces.size() > pts.capacity())
		pts.reserve(faces.size());
	if (faces.size() > edges.capacity())
		edges.reserve(faces.size());

	for (int ti = 0; ti < (int)faces.size(); ti += 3)
	{
		glm::vec2 p0 = verts[faces[ti]];
		glm::vec2 p1 = verts[faces[ti + 1]];
		glm::vec2 p2 = verts[faces[ti + 2]];

		//assert(geometry.ToTheLeft(p2, p0, p1));

		centers.push_back(geometry.CircumcircleCenter(p0, p1, p2));
	}

	for (int ti = 0; ti < faces.size(); ti += 3)
	{
		pts.push_back(PointTriangle(faces[ti], ti));
		pts.push_back(PointTriangle(faces[ti + 1], ti));
		pts.push_back(PointTriangle(faces[ti + 2], ti));
	}

	compare.ClearTris();
	compare.ClearVerts();
	compare.SetTris(faces);
	compare.SetVerts(verts);
	std::stable_sort(pts.begin(), pts.end(), [this](const PointTriangle& a, const PointTriangle& b)->bool {return compare.Compare(a, b); }); // TODO: make sure this work
	
	for (int i = 0; i < (int)pts.size(); i++)
	{
		diagram.firstEdgeBySite.push_back((int)edges.size());

		int start = i;
		int end = -1;

		for (int j = i + 1; j < (int)pts.size(); j++)
		{
			if (pts[i].point != pts[j].point)
			{
				end = j - 1;
				break;
			}
		}

		if (end == -1)
		{
			end = (int)pts.size() - 1;
		}

		i = end;

		int count = end - start;

		//assert(count >= 0);

		for (int ptiCurr = start; ptiCurr <= end; ptiCurr++)
		{
			bool isEdge = true;

			int ptiNext = ptiCurr + 1;

			if (ptiNext > end)
				ptiNext = start;

			PointTriangle ptCurr = pts[ptiCurr];
			PointTriangle ptNext = pts[ptiNext];

			int tiCurr = ptCurr.triangle;
			int tiNext = ptNext.triangle;

			glm::vec2 p0 = verts[ptCurr.point];

			glm::vec2 v2nan = glm::vec2(NAN, NAN);

			if (count == 0)
			{
				isEdge = true;
			}
			else if (count == 1)
			{
				glm::vec2 cCurr = geometry.TriangleCentroid(verts[faces[tiCurr]], verts[faces[tiCurr + 1]], verts[faces[tiCurr + 2]]);
				glm::vec2 cNext = geometry.TriangleCentroid(verts[faces[tiNext]], verts[faces[tiNext + 1]], verts[faces[tiNext + 2]]);
				
				isEdge = geometry.ToTheLeft(cCurr, p0, cNext);
			}
			else
			{
				isEdge = !SharesEdge(faces, tiCurr, tiNext);
			}

			if (isEdge)
			{
				glm::vec2 v0, v1;
				
				if (ptCurr.point == faces[tiCurr])
				{
					v0 = verts[faces[tiCurr + 2]] - verts[faces[tiCurr + 0]];
				}
				else if (ptCurr.point == faces[tiCurr + 1])
				{
					v0 = verts[faces[tiCurr + 0]] - verts[faces[tiCurr + 1]];
				}
				else
				{
					//assert(ptCurr.point == faces[tiCurr + 2]);
					v0 = verts[faces[tiCurr + 1]] - verts[faces[tiCurr + 2]];
				}

				if (ptNext.point == faces[tiNext])
				{
					v1 = verts[faces[tiNext + 0]] - verts[faces[tiNext + 1]];
				} 
				else if (ptNext.point == faces[tiNext + 1])
				{
					v1 = verts[faces[tiNext + 1]] - verts[faces[tiNext + 2]];
				}
				else
				{
					//assert(ptNext.point == faces[tiNext + 2]);
					v1 = verts[faces[tiNext + 2]] - verts[faces[tiNext + 0]];
				}

				

				edges.push_back(Edge(
					Edge::EdgeType::RayCCW,
					ptCurr.point,
					tiCurr / 3,
					-1,
					geometry.RotateRightAngle(v0)
				));

				edges.push_back(Edge(
					Edge::EdgeType::RayCW,
					ptCurr.point,
					tiNext / 3,
					-1,
					geometry.RotateRightAngle(v1)
				));
			}
			else
			{
				edges.push_back(Edge(
					Edge::EdgeType::Segment,
					ptCurr.point,
					tiCurr / 3,
					tiNext / 3,
					v2nan
				));
			}
		}
	}
}

int VoroniCalculator::NonSharedPoint(std::vector<int> tris, int ti0, int ti1)
{
	//assert(SharesEdge(tris, ti0, ti1));

	int x0 = tris[ti0];
	int x1 = tris[ti0 + 1];
	int x2 = tris[ti0 + 2];

	int y0 = tris[ti1];
	int y1 = tris[ti1 + 1];
	int y2 = tris[ti1 + 2];

	if (x0 != y0 && x0 != y1 && x0 != y2) return x0;
	if (x1 != y0 && x1 != y1 && x1 != y2) return x1;
	if (x2 != y0 && x2 != y1 && x2 != y2) return x2;

	//assert(false);
	return -1;
}

bool VoroniCalculator::SharesEdge(std::vector<int> tris, int ti0, int ti1)
{
	int x0 = tris[ti0];
	int x1 = tris[ti0 + 1];
	int x2 = tris[ti0 + 2];

	int y0 = tris[ti1];
	int y1 = tris[ti1 + 1];
	int y2 = tris[ti1 + 2];

	int n = 0;

	if (x0 == y0 || x0 == y1 || x0 == y2) n++;
	if (x1 == y0 || x1 == y1 || x1 == y2) n++;
	if (x2 == y0 || x2 == y1 || x2 == y2) n++;

	//assert(n != 3);

	return n >= 2;
}

