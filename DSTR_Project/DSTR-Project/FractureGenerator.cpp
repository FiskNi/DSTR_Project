#include <Pch/Pch.h>
//#include "FractureGenerator.h"



FractureGenerator::FractureGenerator()
{
	m_seed = 0;
	m_maxFratureDepth = 20;

	// Fracture settings
	m_voronoiType = VoronoiType::FORTUNESWEEP;
	m_circlePattern = true;
	m_bulletHoles = false;
	m_pointCount = 100;
	m_circleRadius = 8.0f;
	m_forceAll_mod = 200.0f;
}

FractureGenerator::~FractureGenerator()
{
}

bool FractureGenerator::DebugFracture(DestructibleObject* object, glm::vec2 hitPosition, glm::vec3 hitDirection)
{
	if (object->IsDestroyed())
		return false;

	// Get object data
	const std::string& meshName = object->GetMesh()->getName();
	const float& scale = object->getScale();
	const std::vector<glm::vec2>& hitPolygon = object->GetPolygon();
	int pointAmount = (int)hitPolygon.size();
	int mi = 1; // The first mesh (mi == 0) is the initial mesh to destroy

	if (pointAmount >= 3)
	{
		if (m_voronoiType == VoronoiType::FORTUNESWEEP)
		{
			// Points gather
			int numpoints = 30;
			jcv_point j_points[30];

			// Randomize points and move to hitlocation
			for (int i = 0; i < numpoints; i++)
			{
				j_points[i].x = hitPosition.x;
				j_points[i].y = hitPosition.y;

				j_points[i].x += (float)(rand() / (1.0f + RAND_MAX) * 1.0f) - 0.5f;
				j_points[i].y += (float)(rand() / (1.0f + RAND_MAX) * 1.0f) - 0.5f;
			}


			// Clip polygon
			jcv_clipping_polygon jcv_clippingPolygon;
			jcv_clippingPolygon.num_points = pointAmount;
			jcv_clippingPolygon.points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)pointAmount);
			// Polygon recived from object
			for (int i = 0; i < (size_t)jcv_clippingPolygon.num_points; i++)
			{
				jcv_clippingPolygon.points[i].x = hitPolygon[i].x;
				jcv_clippingPolygon.points[i].y = hitPolygon[i].y;
			}

			jcv_clipper jcv_polygonclipper;
			jcv_polygonclipper.test_fn = jcv_clip_polygon_test_point;
			jcv_polygonclipper.clip_fn = jcv_clip_polygon_clip_edge;
			jcv_polygonclipper.fill_fn = jcv_clip_polygon_fill_gaps;
			jcv_polygonclipper.ctx = &jcv_clippingPolygon;
			jcv_clipper* clipper = &jcv_polygonclipper;


			// Generate voronoi diagram
			jcv_diagram voronoiDiagram;
			memset(&voronoiDiagram, 0, sizeof(jcv_diagram));
			jcv_diagram_generate(numpoints, j_points, 0, clipper, &voronoiDiagram);

			// Sites and polygons
			const jcv_site* sites = jcv_diagram_get_sites(&voronoiDiagram);
			// For each site
			for (int i = 0; i < voronoiDiagram.numsites; i++)
			{
				std::vector<glm::vec2> siteVertices;
				jcv_graphedge* j_graphedge = sites[i].edges;
				// For each vertex in site
				while (j_graphedge)
				{
					// Get first vertex of edge
					siteVertices.emplace_back(glm::vec2(j_graphedge->pos[0].x, j_graphedge->pos[0].y));
					// Next edge in site
					j_graphedge = j_graphedge->next;
				}


				// Generate new mesh
				Mesh* newMesh = nullptr;
				if (MeshFromSite(newMesh, scale, siteVertices))
				{
					// Create mesh
					newMesh->nameMesh(meshName + "_" + std::to_string(i));
					glm::vec3 center = glm::vec3(newMesh->CenterPivot());
					newMesh->setUpBuffers();
					object->InitMesh(newMesh);

					// Fix placement
					glm::vec2 sitePosition = glm::vec2(sites[i].p.x, sites[i].p.y);
					Transform newTransform = object->GetRigidTransform(0);
					newTransform.position += center;
					// Debug placement
					newTransform.position += glm::vec3(sitePosition * 0.2, 0.0f) - (glm::vec3(hitPosition, 0.0f) * 0.2f);
					object->SetMeshOffsetTransform(newTransform, mi);

					// Physics
					//object->BulletCreateDynamic(CollisionObject::box, 100.0f * scale, mi, true);
					//ApplyPhysicsProjectile(object, hitPosition, center, newTransform, hitDirection, mi);

					// Increment mesh index
					mi++;
				}
			}

			// Update original object
			// If it has a body
			object->SetBodyWorldPosition(glm::vec3(999.0f), 0);
			// General mesh move
			object->SetMeshOffsetPosition(glm::vec3(999.0f), 0);
			// Set destroyed to not repeat
			object->SetDestroyed(true);


			// jcv free diagram memory
			jcv_diagram_free(&voronoiDiagram);
			return true;
		}
		else if (m_voronoiType == VoronoiType::DELAUNAY)
		{

		}
		else
		{
			return false;
		}



	}


	// Failed to fracture
	return false;
}

bool FractureGenerator::DebugFracture(DestructibleObject* object, const glm::vec2 hitPosition, std::vector<GameObject*>& objectList, DebugFractureTimer* debugFractureTimer)
{
	auto t_begin = std::chrono::steady_clock::now();
	debugFractureTimer->gpuTime_micro.emplace_back(0);
	debugFractureTimer->gpuTime_milli.emplace_back(0);


	const int fractureDepth = object->GetFractureDepth();
	if (object->IsDestroyed() || fractureDepth > m_maxFratureDepth)
		return false;
	//float pointRadius = (max.x + max.y);
	const float pointDiamater = (m_circleRadius * 1.7) / (1.0f + fractureDepth);		// Reduce diameter based on fracture depth
	const int numpoints = m_pointCount / (1 + fractureDepth * 5);	// Reduce amount of points based on fracture depth
	jcv_point* j_points = (jcv_point*)malloc(sizeof(jcv_point) * numpoints);

	//m_circlePattern = true;
	RandomizePoints(numpoints, pointDiamater, j_points, hitPosition);


	// Get object data
	const std::vector<glm::vec2>& hitPolygon = object->GetPolygon();
	const int vertexAmount = (int)hitPolygon.size();
	const std::string& meshName = object->GetMesh()->getName();
	const float& scale = object->getScale();
	const glm::vec2 min = object->GetMin() * 1.05f;
	const glm::vec2 max = object->GetMax() * 1.05f;
	


	if (vertexAmount >= 3)
	{
		if (m_voronoiType == VoronoiType::FORTUNESWEEP)
		{
			

			// Clip polygon
			jcv_clipping_polygon jcv_clippingPolygon;
			jcv_clippingPolygon.num_points = vertexAmount;
			jcv_clippingPolygon.points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)vertexAmount);
			// Polygon recived from object
			for (int i = 0; i < (size_t)jcv_clippingPolygon.num_points; i++)
			{
				jcv_clippingPolygon.points[i].x = hitPolygon[i].x;
				jcv_clippingPolygon.points[i].y = hitPolygon[i].y;
			}

			jcv_clipper jcv_polygonclipper;
			jcv_polygonclipper.test_fn = jcv_clip_polygon_test_point;
			jcv_polygonclipper.clip_fn = jcv_clip_polygon_clip_edge;
			jcv_polygonclipper.fill_fn = jcv_clip_polygon_fill_gaps;
			jcv_polygonclipper.ctx = &jcv_clippingPolygon;
			jcv_clipper* clipper = &jcv_polygonclipper;


			jcv_rect rect;
			rect.min.x = min.x;
			rect.min.y = min.y;

			rect.max.x = max.x;
			rect.max.y = max.y;


			// Generate voronoi diagram
			jcv_diagram voronoiDiagram;
			memset(&voronoiDiagram, 0, sizeof(jcv_diagram));
			jcv_diagram_generate(numpoints, j_points, &rect, clipper, &voronoiDiagram);
			//jcv_diagram_generate(numpoints, j_points, 0, 0, &voronoiDiagram);

			

			// Sites and polygons
			const jcv_site* sites = jcv_diagram_get_sites(&voronoiDiagram);
			
			// If it fails
			if (voronoiDiagram.numsites == 0)
			{
				logError("No sites");
				for (int i = 0; i < numpoints; i++)
				{
					j_points[i].x = (float)(rand() / (1.0f + RAND_MAX) * pointDiamater) - (pointDiamater / 2);
					j_points[i].y = (float)(rand() / (1.0f + RAND_MAX) * pointDiamater) - (pointDiamater / 2);

					j_points[i].x += hitPosition.x * 0.5;
					j_points[i].y += hitPosition.y * 0.5;
				}


				// Clip polygon
				jcv_clippingPolygon.num_points = vertexAmount;
				free(jcv_clippingPolygon.points);
				jcv_clippingPolygon.points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)vertexAmount);
				// Polygon recived from object
				for (int i = 0; i < (size_t)jcv_clippingPolygon.num_points; i++)
				{
					jcv_clippingPolygon.points[i].x = hitPolygon[i].x;
					jcv_clippingPolygon.points[i].y = hitPolygon[i].y;
				}
				jcv_polygonclipper.test_fn = jcv_clip_polygon_test_point;
				jcv_polygonclipper.clip_fn = jcv_clip_polygon_clip_edge;
				jcv_polygonclipper.fill_fn = jcv_clip_polygon_fill_gaps;
				jcv_polygonclipper.ctx = &jcv_clippingPolygon;
				clipper = &jcv_polygonclipper;


				// Generate voronoi diagram
				jcv_diagram_free(&voronoiDiagram);
				memset(&voronoiDiagram, 0, sizeof(jcv_diagram));
				jcv_diagram_generate(numpoints, j_points, 0, clipper, &voronoiDiagram);

				sites = jcv_diagram_get_sites(&voronoiDiagram);
				if (voronoiDiagram.numsites == 0)
				{
					logError("No sites attempt 2");
					return false;
				}
			}

			// Timer
			auto t_diagram = std::chrono::steady_clock::now();
			auto diagramTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(t_diagram - t_begin).count();
			float diagramTime_milli = (float)diagramTime_micro / 1000;
			// Add to debug
			debugFractureTimer->diagramTime_micro.emplace_back(diagramTime_micro);
			debugFractureTimer->diagramTime_milli.emplace_back(diagramTime_milli);


			// For each site
			objectList.reserve(voronoiDiagram.numsites);
			for (int i = 0; i < voronoiDiagram.numsites; ++i)
			{
				std::vector<glm::vec2> siteVertices;
				const jcv_graphedge* j_graphedge = sites[i].edges;
				// For each vertex in site
				while (j_graphedge)
				{
					// Get first vertex of edge
					float edgeX = j_graphedge->pos[0].x;
					float edgeY = j_graphedge->pos[0].y;
					glm::vec2 edgePos = glm::vec2(edgeX, edgeY);

					if (siteVertices.size() == 0)
					{
						siteVertices.emplace_back(edgePos);
					}
					else if(glm::length(edgePos - siteVertices[0]) > 0.0001f)
					{
						if (glm::length(edgePos - siteVertices.back()) > 0.0001f)
						{
							siteVertices.emplace_back(edgePos);
						}
					}
					else
					{
						break;
					}
					

					// Next edge in site
					j_graphedge = j_graphedge->next;
				}


				glm::vec2 min = siteVertices[0];
				glm::vec2 max = siteVertices[0];
				for (size_t i = 0; i < siteVertices.size(); i++)
				{
					min.x = fminf(siteVertices[i].x, min.x);
					min.y = fminf(siteVertices[i].y, min.y);

					max.x = fmaxf(siteVertices[i].x, max.x);
					max.y = fmaxf(siteVertices[i].y, max.y);
				}

				glm::vec2 s_center = glm::vec2((min + max) * 0.5f);
				for (size_t i = 0; i < siteVertices.size(); i++)
				{
					siteVertices[i] -= s_center;
				}

				Mesh* newMesh = nullptr;
				if (MeshFromSite(newMesh, scale, siteVertices))
				{

					DestructibleObject* newObject = new DestructibleObject(this);
					newObject->SetPolygon(siteVertices);
					newObject->SetScale(scale);
					newObject->SetFractureDepth(fractureDepth + 1);

					Transform newTransform = object->GetRigidTransform(0);

					// Create mesh
					newMesh->nameMesh(meshName + "_" + std::to_string(i));
					glm::vec3 center = glm::vec3(s_center, 0.0f) * glm::inverse(newTransform.rotation);

					// GPU Buffer
					auto t_bufferStart = std::chrono::steady_clock::now();
					newMesh->setUpBuffers();
					auto t_bufferEnd = std::chrono::steady_clock::now();
					auto gTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(t_bufferEnd - t_bufferStart).count();
					float gTime_milli = (float)gTime_micro / 1000;
					debugFractureTimer->gpuTime_micro.back() += gTime_micro;
					debugFractureTimer->gpuTime_milli.back() += gTime_milli;
					
					
					newObject->InitMesh(newMesh);

					// Fix placement
					glm::vec3 sitePosition = glm::vec3(sites[i].p.x, sites[i].p.y, 0.0f);
					newTransform.position += center;

					// Debug placement and scale
					//float debugOffset = 1.2f;
					//newTransform.position += (sitePosition * debugOffset - (glm::vec3(hitPosition, 0.0f) * debugOffset)) * glm::inverse(newTransform.rotation);
					newTransform.scale *= 0.97f;
					
					newObject->SetTransform(newTransform);

					
					// Add object to list
					objectList.push_back(newObject);
					Renderer::GetInstance()->submit(objectList.back(), STATIC);

				}
			}

			// Update original object
			// If it has a body
			object->SetBodyWorldPosition(glm::vec3(999.0f), 0); 
			// General mesh move
			object->SetMeshOffsetPosition(glm::vec3(999.0f), 0);
			// Set destroyed to not repeat
			object->SetDestroyed(true);


			// jcv free diagram memory
			jcv_diagram_free(&voronoiDiagram);
			
			//free(clipper);
			free(jcv_clippingPolygon.points);
			free(j_points);

			auto tMeshes2 = std::chrono::steady_clock::now();
			auto meshesTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(tMeshes2 - t_begin).count();
			float meshesTime_milli = (float)meshesTime_micro / 1000;
			// Add to debug
			debugFractureTimer->meshesTime_micro.emplace_back(meshesTime_micro);
			debugFractureTimer->meshesTime_milli.emplace_back(meshesTime_milli);

			return true;
		}
		else if (m_voronoiType == VoronoiType::DELAUNAY)
		{
			logError("No Delaunay implementation yet");
			free(j_points);
			return false;
		}
		else
		{
			logError("Unknown Voronoi type");
			free(j_points);
			return false;
		}
	}
	else
	{
		logError("Polygon count < 3");
	}

	logError("Failed to fracture");
	free(j_points);
	return false;
}

bool FractureGenerator::Fracture(DestructibleObject* object, const glm::vec2 hitPosition, const glm::vec3 hitDirection, std::vector<GameObject*>& objectList)
{
	const int fractureDepth = object->GetFractureDepth();
	if (object->IsDestroyed() || fractureDepth > m_maxFratureDepth)
		return false;

	// Get object data
	const std::vector<glm::vec2>& hitPolygon = object->GetPolygon();
	const int vertexAmount = (int)hitPolygon.size();
	const std::string& meshName = object->GetMesh()->getName();
	const float& scale = object->getScale();
	const glm::vec2 min = object->GetMin() * 1.15f;
	const glm::vec2 max = object->GetMax() * 1.15f;

	//float pointRadius = (max.x + max.y);
	//m_circlePattern = true;
	const float pointDiamater	= m_circleRadius;	//	/ (1 + fractureDepth);		// Reduce diameter based on fracture depth
	const int numpoints			= m_pointCount;		//	/ (1 + fractureDepth);				// Reduce amount of points based on fracture depth

	if (vertexAmount >= 3)
	{
		if (m_voronoiType == VoronoiType::FORTUNESWEEP)
		{
			jcv_point* j_points = (jcv_point*)malloc(sizeof(jcv_point) * numpoints);
			RandomizePoints(numpoints, pointDiamater, j_points, hitPosition);

			// Clip polygon
			jcv_clipping_polygon jcv_clippingPolygon;
			jcv_clippingPolygon.num_points = vertexAmount;
			jcv_clippingPolygon.points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)vertexAmount);
			// Polygon recived from object
			for (int i = 0; i < (size_t)jcv_clippingPolygon.num_points; i++)
			{
				jcv_clippingPolygon.points[i].x = hitPolygon[i].x;
				jcv_clippingPolygon.points[i].y = hitPolygon[i].y;
			}

			jcv_clipper jcv_polygonclipper;
			jcv_polygonclipper.test_fn = jcv_clip_polygon_test_point;
			jcv_polygonclipper.clip_fn = jcv_clip_polygon_clip_edge;
			jcv_polygonclipper.fill_fn = jcv_clip_polygon_fill_gaps;
			jcv_polygonclipper.ctx = &jcv_clippingPolygon;
			jcv_clipper* clipper = &jcv_polygonclipper;


			jcv_rect rect;
			rect.min.x = min.x;
			rect.min.y = min.y;

			rect.max.x = max.x;
			rect.max.y = max.y;


			// Generate voronoi diagram
			jcv_diagram voronoiDiagram;
			memset(&voronoiDiagram, 0, sizeof(jcv_diagram));
			jcv_diagram_generate(numpoints, j_points, &rect, clipper, &voronoiDiagram);
			//jcv_diagram_generate(numpoints, j_points, 0, 0, &voronoiDiagram);

			// Sites and polygons
			const jcv_site* sites = jcv_diagram_get_sites(&voronoiDiagram);

			// If it fails
			if (voronoiDiagram.numsites == 0)
			{
				logError("No sites");
				RandomizePoints(numpoints, pointDiamater, j_points, hitPosition);
				/*for (int i = 0; i < numpoints; i++)
				{
					j_points[i].x = (float)(rand() / (1.0f + RAND_MAX) * pointDiamater) - (pointDiamater / 2);
					j_points[i].y = (float)(rand() / (1.0f + RAND_MAX) * pointDiamater) - (pointDiamater / 2);

					j_points[i].x += hitPosition.x * 0.5;
					j_points[i].y += hitPosition.y * 0.5;
				}*/

				// Clip polygon
				jcv_clippingPolygon.num_points = vertexAmount;
				free(jcv_clippingPolygon.points);
				jcv_clippingPolygon.points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)vertexAmount);
				// Polygon recived from object
				for (int i = 0; i < (size_t)jcv_clippingPolygon.num_points; i++)
				{
					jcv_clippingPolygon.points[i].x = hitPolygon[i].x;
					jcv_clippingPolygon.points[i].y = hitPolygon[i].y;
				}
				jcv_polygonclipper.test_fn = jcv_clip_polygon_test_point;
				jcv_polygonclipper.clip_fn = jcv_clip_polygon_clip_edge;
				jcv_polygonclipper.fill_fn = jcv_clip_polygon_fill_gaps;
				jcv_polygonclipper.ctx = &jcv_clippingPolygon;
				clipper = &jcv_polygonclipper;

				// Generate voronoi diagram
				jcv_diagram_free(&voronoiDiagram);
				memset(&voronoiDiagram, 0, sizeof(jcv_diagram));
				jcv_diagram_generate(numpoints, j_points, 0, clipper, &voronoiDiagram);

				sites = jcv_diagram_get_sites(&voronoiDiagram);
				if (voronoiDiagram.numsites == 0)
				{
					logError("No sites attempt 2");
					return false;
				}
			}


			// For each site
			for (int i = 0; i < voronoiDiagram.numsites; ++i)
			{
				std::vector<glm::vec2> siteVertices;
				const jcv_graphedge* j_graphedge = sites[i].edges;
				// For each vertex in site
				while (j_graphedge)
				{
					// Get first vertex of edge
					float edgeX = j_graphedge->pos[0].x;
					float edgeY = j_graphedge->pos[0].y;
					glm::vec2 edgePos = glm::vec2(edgeX, edgeY);

					if (siteVertices.size() == 0)
					{
						siteVertices.emplace_back(edgePos);
					}
					else if (glm::length(edgePos - siteVertices[0]) > 0.0001f)
					{
						if (glm::length(edgePos - siteVertices.back()) > 0.0001f)
						{
							siteVertices.emplace_back(edgePos);
						}
					}
					else
					{
						break;
					}


					// Next edge in site
					j_graphedge = j_graphedge->next;
				}


				glm::vec2 min = siteVertices[0];
				glm::vec2 max = siteVertices[0];
				for (size_t i = 0; i < siteVertices.size(); i++)
				{
					min.x = fminf(siteVertices[i].x, min.x);
					min.y = fminf(siteVertices[i].y, min.y);

					max.x = fmaxf(siteVertices[i].x, max.x);
					max.y = fmaxf(siteVertices[i].y, max.y);
				}

				glm::vec2 s_center = glm::vec2((min + max) * 0.5f);
				for (size_t i = 0; i < siteVertices.size(); i++)
				{
					siteVertices[i] -= s_center;
				}

				Mesh* newMesh = nullptr;
				if (MeshFromSite(newMesh, scale, siteVertices))
				{

					DestructibleObject* newObject = new DestructibleObject(this);
					newObject->SetPolygon(siteVertices);
					newObject->SetScale(scale);
					newObject->SetFractureDepth(fractureDepth + 1);

					Transform newTransform = object->GetRigidTransform(0);

					// Create mesh
					newMesh->nameMesh(meshName + "_" + std::to_string(i));
					glm::vec3 center = glm::vec3(s_center, 0.0f) * glm::inverse(newTransform.rotation);
					//glm::vec3(newMesh->CenterPivot());

					newMesh->setUpBuffers();
					newObject->InitMesh(newMesh);

					// Fix placement
					glm::vec3 sitePosition = glm::vec3(sites[i].p.x, sites[i].p.y, 0.0f);
					newTransform.position += center;

					// Debug placement and scale
					float debugOffset = 0.08f;
					//newTransform.position += (sitePosition * debugOffset - (glm::vec3(hitPosition, 0.0f) * debugOffset)) * glm::inverse(newTransform.rotation);
					//newTransform.position += (sitePosition * 2.2f);
					//newTransform.scale *= 0.92f;

					newObject->SetTransform(newTransform);

					// Physics
					if (m_bulletHoles)
					{
						float distance = glm::length(glm::vec3(hitPosition, 0.0f) - center);
						if (distance < (m_circleRadius * 1.1))
						{					
							newObject->BulletCreateDynamic(CollisionObject::box, 100.0f * scale, 0, true);

							// Disable collision
							newObject->getRigidBody()->setCollisionFlags(newObject->getRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

							// Inherit forces
							btRigidBody* body = object->getRigidBody();
							btRigidBody* body2 = newObject->getRigidBody();
							body2->setLinearVelocity(body->getLinearVelocity());
							body2->setAngularVelocity(body->getAngularVelocity());

							// Apply new forces
							//ApplyPhysicsFracture(newObject, hitPosition, center, hitDirection);
							float oldForce = m_forceAll_mod;

							if (scale > 0.5f)
							{
								m_forceAll_mod *= 10.0f;
								m_forceAll_mod *= std::powf(1.2f, scale);
							}

							ApplyPhysicsFracture(newObject, hitPosition, glm::vec3(s_center, 0.0f), hitDirection);
							m_forceAll_mod = oldForce;
						}
						else
						{
							newObject->BulletMakeStatic();
						}
					}
					else
					{
						newObject->BulletCreateDynamic(CollisionObject::box, 100.0f * scale, 0, true);

						// Inherit forces
						btRigidBody* body = object->getRigidBody();
						btRigidBody* body2 = newObject->getRigidBody();
						body2->setLinearVelocity(body->getLinearVelocity());
						body2->setAngularVelocity(body->getAngularVelocity());

						// Apply new forces
						//ApplyPhysicsFracture(newObject, hitPosition, center, hitDirection);
						float oldForce = m_forceAll_mod;

						if (scale > 0.3f)
						{
							m_forceAll_mod *= 10.0f;
							m_forceAll_mod *= std::powf(1.2f, scale);
						}

						ApplyPhysicsFracture(newObject, hitPosition, glm::vec3(s_center, 0.0f), hitDirection);
						m_forceAll_mod = oldForce;
					}






					// Add object to list
					objectList.push_back(newObject);
					Renderer::GetInstance()->submit(objectList.back(), STATIC);

				}
			}

			// Update original object
			// If it has a body
			object->SetBodyWorldPosition(glm::vec3(999.0f), 0);
			// General mesh move
			object->SetMeshOffsetPosition(glm::vec3(999.0f), 0);
			// Set destroyed to not repeat
			object->SetDestroyed(true);


			// jcv free diagram memory
			jcv_diagram_free(&voronoiDiagram);

			//free(clipper);
			free(jcv_clippingPolygon.points);
			free(j_points);


			return true;
		}
		else if (m_voronoiType == VoronoiType::DELAUNAY)
		{
			logError("No Delaunay implementation yet");
			return false;
		}
		else
		{
			logError("Unknown Voronoi type");
			return false;
		}
	}
	else
	{
		logError("Polygon count < 3");
	}

	logError("Failed to fracture");
	return false;
}

void FractureGenerator::RandomizePoints(const int& numpoints, const float& pointRadius, jcv_point* j_points, const glm::vec2& hitPosition)
{
	// Randomize points and move to hitlocation

	for (int i = 0; i < numpoints; i++)
	{
		if (m_circlePattern)
		{
			//if (i > numpoints / 2)
			//{
			//	float radius = pointRadius;
			//	radius /= 5;

			//	float range = 0.01f;
			//	float rnd = 1.0f + (float)(rand() / (1.0f + RAND_MAX) * range) - (range / 2);
			//	radius *= rnd; // glm::sqrt(rnd);

			//	rnd = (float)(rand() / (1.0f + RAND_MAX));

			//	float angle = 2.0f * glm::pi<float>() * rnd;

			//	j_points[i].x = radius * glm::cos(angle);
			//	j_points[i].y = radius * glm::sin(angle);
			//}
			//else
			{
				float radius = pointRadius;
				
				float range = 1.0f;
				//float rnd = 1.0f + (float)(rand() / (1.0f + RAND_MAX) * range) - (range / 2);
				//radius *= rnd; 
				float rnd = (float)(rand() / (1.0f + RAND_MAX));
				radius *= glm::sqrt(rnd);
				
				rnd = (float)(rand() / (1.0f + RAND_MAX));
				
				float angle = 2.0f * glm::pi<float>() * rnd;
				
				j_points[i].x = radius * glm::cos(angle);
				j_points[i].y = radius * glm::sin(angle);

			}
		}
		else
		{
			// TODO: Keep this
			j_points[i].x = (float)(rand() / (1.0f + RAND_MAX) * pointRadius) - (pointRadius / 2);
			j_points[i].y = (float)(rand() / (1.0f + RAND_MAX) * pointRadius) - (pointRadius / 2);
		}

		// Move to impact location
		j_points[i].x += hitPosition.x;
		j_points[i].y += hitPosition.y;

		j_points[0].x = hitPosition.x;
		j_points[0].y = hitPosition.y;
	}
}

void FractureGenerator::SetPointAmount(int amount)
{
	m_pointCount = amount;
}

void FractureGenerator::SetRadiusSize(float radius)
{
	m_circleRadius = radius;
}

void FractureGenerator::SetForceStrength(float str)
{
	m_forceAll_mod = str;
}

void FractureGenerator::SetBulletHoles(bool arg)
{
	m_bulletHoles = arg;
}

void FractureGenerator::SetCirclePattern(bool arg)
{
	m_circlePattern = arg;
}

bool FractureGenerator::MeshFromSite(Mesh*& mesh, const float& scale, const std::vector<glm::vec2>& siteVertices)
{
	int vertexCount = siteVertices.size();
	if (vertexCount >= 3)
	{
		if (mesh)
		{
			mesh->Destroy();
			delete mesh;
		}
		mesh = new Mesh();


		std::vector<Vertex> newVertices;
		std::vector<Face> newFaces;
		glm::vec3 normal;

		newVertices.resize(6 * (size_t)vertexCount);
		newFaces.resize(4 * (size_t)vertexCount - 4);

		int vi = 0;		// Vertex index
		int uvi = 0;	// UV index
		int ni = 0;		// Normal index
		int fi = 0;		// Face index

		// 0 - Bottom Left
		// 1 - Bottom Right
		// 2 - Top Right
		// 3 - Top Left
		//float uvmin_u = uv[0].x;
		//float uvmax_u = uv[2].x;
		//
		//float uvmin_v = uv[0].y;
		//float uvmax_v = uv[2].y;
		// TODO: Check and confirm this -> UV.u is inversed

		// Front
		for (int i = 0; i < vertexCount; i++)
		{
			newVertices[vi++].position = glm::vec3(siteVertices[i].x, siteVertices[i].y, scale);

			//newVertices[uvi++].UV = glm::vec2(
			//	(uvmax_u - uvmin_u) * (siteVertices[i].x - polygon[0].x) / (polygon[2].x - polygon[0].x) + uvmin_u,
			//	(uvmax_v - uvmin_v) * (siteVertices[i].y - polygon[0].y) / (polygon[2].y - polygon[0].y) + uvmin_v
			//);

			newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
		}

		// Back
		for (int i = 0; i < vertexCount; i++)
		{
			newVertices[vi++].position = glm::vec3(siteVertices[i].x, siteVertices[i].y, -scale);

			//newVertices[uvi++].UV = glm::vec2(
			//	(uvmax_u - uvmin_u) * (siteVertices[i].x - polygon[0].x) / (polygon[2].x - polygon[0].x) + uvmin_u,
			//	(uvmax_v - uvmin_v) * (siteVertices[i].y - polygon[0].y) / (polygon[2].y - polygon[0].y) + uvmin_v
			//);

			newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
		}

		// Sides
		for (int i = 0; i < vertexCount; i++)
		{
			int iNext = i == vertexCount - 1 ? 0 : i + 1;

			newVertices[vi++].position = glm::vec3(siteVertices[i].x, siteVertices[i].y, scale);
			newVertices[vi++].position = glm::vec3(siteVertices[i].x, siteVertices[i].y, -scale);

			newVertices[vi++].position = glm::vec3(siteVertices[iNext].x, siteVertices[iNext].y, -scale);
			newVertices[vi++].position = glm::vec3(siteVertices[iNext].x, siteVertices[iNext].y, scale);

			normal = glm::normalize(glm::cross(glm::vec3(siteVertices[iNext] - siteVertices[i], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

			newVertices[ni++].Normals = normal;
			newVertices[ni++].Normals = normal;
			newVertices[ni++].Normals = normal;
			newVertices[ni++].Normals = normal;
		}

		for (int vert = 2; vert < vertexCount; vert++)
		{
			newFaces[fi].indices[0] = 0;
			newFaces[fi].indices[1] = vert - 1;
			newFaces[fi].indices[2] = vert;
			fi++;
		}

		for (int vert = 2; vert < vertexCount; vert++)
		{
			newFaces[fi].indices[0] = vertexCount;
			newFaces[fi].indices[1] = vertexCount + vert;
			newFaces[fi].indices[2] = vertexCount + vert - 1;
			fi++;
		}

		for (int vert = 0; vert < vertexCount; vert++)
		{
			int si = 2 * vertexCount + 4 * vert;

			newFaces[fi].indices[0] = si;
			newFaces[fi].indices[1] = si + 1;
			newFaces[fi].indices[2] = si + 2;
			fi++;

			newFaces[fi].indices[0] = si;
			newFaces[fi].indices[1] = si + 2;
			newFaces[fi].indices[2] = si + 3;
			fi++;
		}




		// Apply the vertices and faces to the mesh
		mesh->setUpMesh(newVertices, newFaces);

		return true;
	}
	else
	{
		return false;
	}


	return false;
}

void FractureGenerator::ApplyPhysicsProjectile(DestructibleObject* object, const glm::vec2& hitPosition, const glm::vec3& center, const glm::vec3& hitDirection, const int& mi)
{
	// Best with radius 3


	const Transform& newTransform = object->GetRigidTransform(0);
	float forceAll_mod = m_forceAll_mod;
	float forceAway_str = 1;
	float forceHit_str = 30;
	float distance = 1.0f + glm::length(center - glm::vec3(hitPosition, 0.0f));

	float distanceMod = 1.0;
	if (distanceMod > 0)
	{
		//forceHit_str *= 1 /  ((1 - distanceMod) * 0.0f + distanceMod * 1.0f);// glm::lerp(0.0f, 1.0f, distanceMod);
		forceHit_str *= 1 / std::powf(10.0f + distance, 1.0);
		forceAway_str *= 1 / std::powf(distance, 1.2);
	}

	//forceHit_str = std::powf(forceHit_str, 2);

	glm::vec3 forceAway_dir = glm::normalize(center - glm::vec3(hitPosition, 0.0f)) * glm::inverse(newTransform.rotation);
	forceAway_dir *= forceAway_str;

	// Randomize force
	float randRange = 0.3f;
	float forceRand = 1 + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);

	//float spinForce_mod = 0.2f * (forceAll_mod / 4);
	float spinForce_mod = 0.4f * (forceAll_mod / 4) * (std::powf(distance, 1.2));
	glm::vec3 spinVector = glm::normalize(glm::cross(center - glm::vec3(hitPosition, 0.0f), -hitDirection));
	glm::vec3 forceSpin = spinVector * spinForce_mod * forceRand;
	
	//float dirRnd = (float)(rand() / (1.0f + RAND_MAX) * spinForce) - (spinForce / 2);

	glm::vec3 force = (forceAway_dir + (hitDirection * forceHit_str)) * forceAll_mod * forceRand;
	btRigidBody* body = object->getRigidBody(mi);
	if (body)
	{
		//body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
		body->setLinearVelocity(btVector3(force.x * 0.2, force.y * 0.2, force.z * 0.2));
		//body->setAngularVelocity(body->getAngularVelocity() + btVector3(forceSpin.x, forceSpin.y, forceSpin.z));
		body->applyTorqueImpulse(btVector3(forceSpin.x, forceSpin.y, forceSpin.z));
	}
}
void FractureGenerator::ApplyPhysicsExplosion(DestructibleObject* object, const glm::vec2& hitPosition, const glm::vec3& center, const glm::vec3& hitDirection, const int& mi)
{
	float forceAll_mod = m_forceAll_mod;
	float forceAway_str = 1;
	float forceHit_str = 15;

	float distanceMod = 1.0 + std::powf(glm::length(center - glm::vec3(hitPosition, 0.0f)), 1.6f);
	if (distanceMod > 0)
	{
		//forceHit_str *= 1 /  ((1 - distanceMod) * 0.0f + distanceMod * 1.0f);// glm::lerp(0.0f, 1.0f, distanceMod);
		forceHit_str *= 1 / distanceMod;
		forceAway_str *= std::powf(distanceMod, 1.4f);
	}

	glm::vec3 forceAway_dir = glm::normalize(center - glm::vec3(hitPosition, 0.0f));
	forceAway_dir *= forceAway_str;

	// Randomize force
	float randRange = 0.3f;
	float forceRand = 1 + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);

	//float spinForce_mod = 0.2f * (forceAll_mod / 4);
	float spinForce_mod = 1.55f * (forceAll_mod / 4);
	glm::vec3 spinVector = glm::normalize(glm::cross(center - glm::vec3(hitPosition, 0.0f), -hitDirection));
	glm::vec3 forceSpin = spinVector * spinForce_mod * forceRand;

	spinVector.x = (2 * (float)rand() / (float)RAND_MAX - 1) * forceAll_mod;
	spinVector.y = (2 * (float)rand() / (float)RAND_MAX - 1) * forceAll_mod;
	spinVector.z = (2 * (float)rand() / (float)RAND_MAX - 1) * forceAll_mod;


	//float dirRnd = (float)(rand() / (1.0f + RAND_MAX) * spinForce) - (spinForce / 2);

	glm::vec3 force = (forceAway_dir + (hitDirection * forceHit_str)) * forceAll_mod * forceRand;
	btRigidBody* body = object->getRigidBody(mi);
	if (body)
	{
		body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
		//body->setLinearVelocity(btVector3(force.x * 0.2, force.y * 0.2, force.z * 0.2));
		//body->setAngularVelocity(body->getAngularVelocity() + btVector3(forceSpin.x, forceSpin.y, forceSpin.z));
		body->applyTorqueImpulse(btVector3(spinVector.x, spinVector.y, spinVector.z));
	}
}

void FractureGenerator::ApplyPhysicsFracture(DestructibleObject* object, const glm::vec2& hitPosition, const glm::vec3& center, const glm::vec3& hitDirection, const int& mi)
{
	const Transform& newTransform = object->GetRigidTransform(0);
	float forceAll_mod = 10.0;
	float forceAway_str = 1.0;
	float forceHit_str = 10;
	float distance = 1.0f + glm::length(center - glm::vec3(hitPosition, 0.0f));

	float distanceMod = 1.0;
	if (distanceMod > 0)
	{
		//forceHit_str *= 1 /  ((1 - distanceMod) * 0.0f + distanceMod * 1.0f);// glm::lerp(0.0f, 1.0f, distanceMod);
		//forceHit_str *= 1 / std::powf(distance / 8, 1.4f);
		//forceAway_str += std::powf(distance / 8, 1.5f);

		forceAway_str *= std::fminf((1.0f + (distance / 12)), 10.0f);

		forceHit_str /= distance;

	}

	glm::vec3 forceAway_dir = glm::normalize(center - glm::vec3(hitPosition, 0.0f)) * glm::inverse(newTransform.rotation);
	forceAway_dir *= forceAway_str;


	//float dirRnd = (float)(rand() / (1.0f + RAND_MAX) * spinForce) - (spinForce / 2);
	//float spinForce_mod = 0.2f * (forceAll_mod / 4);
	// Randomize force
	float randRange = 0.5f;
	float forceRand = 1.0f + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);

	glm::vec3 spinVector = glm::normalize(glm::cross(center - glm::vec3(hitPosition, 0.0f), -hitDirection));
	spinVector.x = (2 * (float)rand() / (float)RAND_MAX - 1) * 1.0f; // * (distance / 2);
	spinVector.y = (2 * (float)rand() / (float)RAND_MAX - 1) * 1.0f; // * (distance / 2);
	spinVector.z = (2 * (float)rand() / (float)RAND_MAX - 1) * 1.0f; // * (distance / 2);
	spinVector.x *= 1.0f + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);
	spinVector.y *= 1.0f + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);
	spinVector.z *= 1.0f + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);

	glm::vec3 forceSpin = spinVector * (m_forceAll_mod / 40);
	forceSpin *= (distance * 5);

	glm::vec3 force = (forceAway_dir + (hitDirection * forceHit_str)) * m_forceAll_mod;
	force.x *= 1 + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);
	force.y *= 1 + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);
	force.z *= 1 + (float)(rand() / (1.0f + RAND_MAX) * randRange) - (randRange / 2);

	btRigidBody* body = object->getRigidBody(mi);
	if (body)
	{	
		body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
		//body->setLinearVelocity(btVector3(force.x * 0.2, force.y * 0.2, force.z * 0.2));
		//body->setAngularVelocity(body->getAngularVelocity() + btVector3(forceSpin.x, forceSpin.y, forceSpin.z));
		//body->setAngularVelocity(btVector3(forceSpin.x, forceSpin.y, forceSpin.z));
		//body->applyTorqueImpulse(btVector3(spinVector.x, spinVector.y, spinVector.z));
		body->applyTorqueImpulse(btVector3(forceSpin.x, forceSpin.y, forceSpin.z));
	}
}

const unsigned int FractureGenerator::seedRand(int seed)
{
	if (seed == -1)
	{
		if (m_seed == int(time(NULL)))
		{
			m_seed = int(time(NULL));
			srand(m_seed);
		}
		else
		{
			return m_seed;
		}
	}
	else
	{
		srand(seed);
		return seed;
	}

	return m_seed;
}





