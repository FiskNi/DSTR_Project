#include <Pch/Pch.h>	
#include "VoroniClipper.h"

void VoroniClipper::ClipSite(VoroniDiagram diagram, const std::vector<glm::vec2>& polygon, int site, std::vector<glm::vec2>& clipped)
{
	pointsIn.clear();
	pointsIn.shrink_to_fit();
	pointsIn = polygon;

	int firstEdge, lastEdge = 0;
	
	if (site == (int)diagram.sites.size() - 1)
	{
		firstEdge = diagram.firstEdgeBySite[site];
		lastEdge = (int)diagram.edges.size() - 1;
	}
	else
	{
		firstEdge = diagram.firstEdgeBySite[site];
		lastEdge = diagram.firstEdgeBySite[site + 1] - 1;
	}

	for (int ei = firstEdge; ei <= lastEdge; ei++)
	{
		pointsOut.clear();
		pointsOut.shrink_to_fit();

		Edge edge = diagram.edges[ei];

		glm::vec2 lp, ld;

		if (edge.type == Edge::EdgeType::RayCCW || edge.type == Edge::EdgeType::RayCW)
		{
			lp = diagram.vertices[edge.vertex_0];
			ld = edge.direction;

			if (edge.type == Edge::EdgeType::RayCW)
			{
				ld *= -1;
			}
		}
		else if (edge.type == Edge::EdgeType::Segment)
		{
			glm::vec2 lp0 = diagram.vertices[edge.vertex_0];
			glm::vec2 lp1 = diagram.vertices[edge.vertex_1];

			lp = lp0;
			ld = lp1 - lp0;
		}
		else if (edge.type == Edge::EdgeType::Line)
		{
			// No Support for voroni halfplanes;
			return;
		}
		else
		{
			//assert(false);
			return;
		}

		for (int pi0 = 0; pi0 < (int)pointsIn.size(); pi0++)
		{
			int pi1 = pi0 == (int)pointsIn.size() - 1 ? 0 : pi0 + 1;

			glm::vec2 p0 = pointsIn[pi0];
			glm::vec2 p1 = pointsIn[pi1];

			bool p0Inside = geometry.ToTheLeft(p0, lp, lp + ld);
			bool p1Inside = geometry.ToTheLeft(p1, lp, lp + ld);

			if (p0Inside && p1Inside)
			{
				pointsOut.push_back(p1);
			}
			else if (!p0Inside && !p1Inside)
			{
				
			}
			else 
			{
				glm::vec2 intersection = geometry.LineLineIntersection(lp, glm::normalize(ld), p0, glm::normalize(p1 - p0));

				if (p0Inside)
				{
					pointsOut.push_back(intersection);
				}
				else if (p1Inside)
				{
					pointsOut.push_back(intersection);
					pointsOut.push_back(p1);
				}
				else
				{
					//assert(false);
				}
			}
		}

		std::vector<glm::vec2> temp = pointsIn;
		pointsIn = pointsOut;
		pointsOut = temp;
	}

	clipped.clear();
	clipped.shrink_to_fit();

	clipped = pointsIn;
}
