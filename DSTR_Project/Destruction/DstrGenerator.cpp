#include <Pch/Pch.h>
#include <Destruction/DstrGenerator.h>

DstrGenerator::DstrGenerator()
{
	m_seed = 0;

	m_settings.breakPoints = 16;
	m_settings.breakAreaRadius = 1.8f;
	m_settings.timeSinceLastDestruction = 0.0f;
	m_settings.initGravity = btVector3(0.0f, -50.0f, 0.0f);

	initPoints();
}

DstrGenerator::~DstrGenerator()
{
}

void DstrGenerator::initPoints(glm::vec2 position)
{
	m_randomPoints.resize(m_settings.breakPoints);

	float rnd = 0;
	float offset = m_settings.breakAreaRadius;
	float angle = 0;

	seedRand(); // Important for the networking to get consistent breakage

	if (m_settings.breakPoints < 0.1f)
	{
		m_settings.breakAreaRadius = 0.1f;
		logWarning("Not enough precision for break radius less than 0.1, clamping");
	}

	switch (m_dstType)
	{

	// Creates points randomly
	case 0:
		// First point centered
		m_randomPoints[0] = position;
		for (int i = 1; i < m_randomPoints.size(); i++)
		{
			m_randomPoints[i] = position;
			offset = m_settings.breakAreaRadius;

			if (i < m_randomPoints.size() / 2)
			{
				randomizeCircle(rnd, offset, angle, m_randomPoints[i]);
			}
			else if (i < m_randomPoints.size())
			{
				offset *= 1.5;	// Increase radius slighly
				offset += (0.05f * m_settings.breakAreaRadius * glm::sqrt(rnd));
				randomizeCircle(rnd, offset, angle, m_randomPoints[i]);
			}
		}
		break;

	case 1:
		for (int i = 0; i < m_randomPoints.size(); i++)
		{
			m_randomPoints[i] = position;
			offset = m_settings.breakAreaRadius;

			randomizeCircle(rnd, offset, angle, m_randomPoints[i]);
		}
		break;

		
	default:
		break;

	}
	
}

void DstrGenerator::randomizeCircle(float& rnd, float& offset, float& angle, glm::vec2& point)
{
	rnd = rand() % 100 + 10;
	rnd /= 100;

	offset *= glm::sqrt(rnd);

	rnd = rand() % 100 + 10;
	rnd /= 100;

	angle = 2.0f * glm::pi<float>() * rnd;

	point.x += offset * glm::cos(angle);
	point.y += offset * glm::sin(angle);
}

void DstrGenerator::setBreakSettings(int type, float breakPoints, float breakAreaRadius, float gravity)
{
	m_dstType = type;
	m_settings.breakPoints = breakPoints;
	m_settings.breakAreaRadius = breakAreaRadius;
	m_settings.initGravity = btVector3(0.0f, -gravity, 0.0f);
}

void DstrGenerator::setBreakPoints(float breakPoints)
{
	m_settings.breakPoints = breakPoints;
}

void DstrGenerator::setBreakRadius(float breakAreaRadius)
{
	m_settings.breakAreaRadius = breakAreaRadius;
}

void DstrGenerator::setBreakGravity(float gravity)
{
	m_settings.initGravity = btVector3(0.0f, -gravity, 0.0f);
}

void DstrGenerator::setBreakType(int type)
{
	m_dstType = type;
}

void DstrGenerator::offsetPoints(glm::vec2 position)
{
	for (int i = 0; i < m_randomPoints.size(); i++)
	{
		m_randomPoints[i] += position;
	}
}

void DstrGenerator::Update()
{
	m_settings.timeSinceLastDestruction += DeltaTime;
}

void DstrGenerator::Destroy(DestructibleObject* object, glm::vec2 hitPosition, glm::vec3 hitDirection)
{
	if (object->IsDestroyed())
		return;	

	Clear();
	initPoints(hitPosition);

	m_diagram = m_voroniCalc.CalculateDiagram(m_randomPoints);
	m_diagram.sites = m_diagram.triangulation.vertices;

	const std::vector<glm::vec2> polygon = object->GetPolygon();
	const std::vector<glm::vec2> uv = object->getUv();
	const float& scale = object->getScale();

	// Resets for each
	count = 0;
	vi = 0;
	uvi = 0;
	ni = 0;
	ti = 0;

	// Counts each
	mi = 1; // The first mesh (mi == 0) is the initial mesh to destroy

	// - Vertex order format
	// 0 - Bottom Left
	// 1 - Bottom Right
	// 2 - Top Right
	// 3 - Top Left
	glm::vec3 normal = glm::vec3();
	for (int i = 0; i < m_randomPoints.size(); i++)
	{
		m_clipper.ClipSite(m_diagram, polygon, i, m_clipped);

		if (m_clipped.size() > 0)
		{
			MeshFromClipped(scale, polygon, uv,  normal);

			glm::vec3 min = m_newVertices[0].position;
			glm::vec3 max = m_newVertices[0].position;
			for (size_t i = 0; i < m_newVertices.size(); i++)
			{
				min.x = fminf(m_newVertices[i].position.x, min.x);
				min.y = fminf(m_newVertices[i].position.y, min.y);
				min.z = fminf(m_newVertices[i].position.z, min.z);

				max.x = fmaxf(m_newVertices[i].position.x, max.x);
				max.y = fmaxf(m_newVertices[i].position.y, max.y);
				max.z = fmaxf(m_newVertices[i].position.z, max.z);
			}

			glm::vec3 center = glm::vec3((min + max) * 0.5f);
			for (size_t i = 0; i < m_newVertices.size(); i++)
			{
				m_newVertices[i].position -= center;
			}

			object->InitMesh(m_newVertices, m_newFace, object->GetMesh()->getName() + "_" + std::to_string(i));

			Transform newTransform = object->GetRigidTransform(0);
			newTransform.position += center * glm::inverse(newTransform.rotation);
			object->SetMeshOffsetTransform(newTransform, mi);

			ApplyDebugPlacement(newTransform, i, object);
			//object->BulletMakeStatic();
			//object->BulletCreateDynamic(CollisionObject::box, 100.0f * scale, mi, true);
			//ApplyPhysicsProjectile(object, scale, i, hitPosition, newTransform, hitDirection);

			mi++;
		}
	}

	object->SetMaterial(nullptr, -1);
	object->setLifetime(0.0f);
	object->SetBodyWorldPosition(glm::vec3(999.0f), 0);
	object->SetDestroyed(true);
	//object->SetBodyActive(false);
}

void DstrGenerator::ApplyDebugPlacement(Transform& newTransform, int i, DestructibleObject* object)
{
	// DEBUG PLACEMENT
	newTransform.position += glm::vec3(
		m_diagram.sites[i].x * 0.2f,
		m_diagram.sites[i].y * 0.2f,
		0.0f);
	object->SetMeshOffsetTransform(newTransform, mi);
}

void DstrGenerator::ApplyPhysicsProjectile(DestructibleObject* object, const float& scale, int i, glm::vec2& hitPosition, Transform& newTransform, glm::vec3& hitDirection)
{
	// Values for destroyed object
	// TODO: Move thiss
	float force_awayDir = 100;
	float force_hitDir = 600;

	glm::vec3 forceDir = glm::vec3((m_diagram.sites[i] - hitPosition), 0.0f) * glm::inverse(newTransform.rotation);
	forceDir *= force_awayDir;
	glm::vec3 force = forceDir + (hitDirection * force_hitDir);

	float forceRand = rand() % 400;
	forceRand /= 1000;
	force *= 0.8f + forceRand;

	float dirRndX = 2 * (double)rand() / (double)RAND_MAX - 1;
	float dirRndY = 2 * (double)rand() / (double)RAND_MAX - 1;
	float dirRndZ = 2 * (double)rand() / (double)RAND_MAX - 1;

	btRigidBody* body = object->getRigidBody(mi);
	if (body)
	{
		body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
		body->applyTorque(btVector3(force.x * dirRndX, force.y * dirRndY, force.z * dirRndZ) * 15);
	}
}

void DstrGenerator::MeshFromClipped(const float& scale, const std::vector<glm::vec2>& polygon, const std::vector<glm::vec2>& uv, glm::vec3& normal)
{
	count = m_clipped.size();

	m_newVertices.clear();
	m_newVertices.resize(6 * count);

	m_newFace.clear();
	m_newFace.resize(4 * count - 4);

	vi = 0;
	uvi = 0;
	ni = 0;
	ti = 0;

	// 0 - Bottom Left
	// 1 - Bottom Right
	// 2 - Top Right
	// 3 - Top Left
	float uvmin_u = uv[0].x;
	float uvmax_u = uv[2].x;

	float uvmin_v = uv[0].y;
	float uvmax_v = uv[2].y;


	// TODO: Check and confirm this -> UV.u is inversed
	// Front
	for (int i = 0; i < count; i++)
	{
		m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, 0);
		m_newVertices[uvi++].UV = glm::vec2(
			(uvmax_u - uvmin_u) * (m_clipped[i].x - polygon[0].x) / (polygon[2].x - polygon[0].x) + uvmin_u,
			(uvmax_v - uvmin_v) * (m_clipped[i].y - polygon[0].y) / (polygon[2].y - polygon[0].y) + uvmin_v
		);

		m_newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	// Back
	for (int i = 0; i < count; i++)
	{
		m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, -scale);
		m_newVertices[uvi++].UV = glm::vec2(
			(uvmax_u - uvmin_u) * (m_clipped[i].x - polygon[0].x) / (polygon[2].x - polygon[0].x) + uvmin_u,
			(uvmax_v - uvmin_v) * (m_clipped[i].y - polygon[0].y) / (polygon[2].y - polygon[0].y) + uvmin_v
		);

		m_newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	// Sides
	for (int i = 0; i < count; i++)
	{
		int iNext = i == count - 1 ? 0 : i + 1;

		m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, 0);
		m_newVertices[vi++].position = glm::vec3(m_clipped[i].x, m_clipped[i].y, -scale);
		m_newVertices[vi++].position = glm::vec3(m_clipped[iNext].x, m_clipped[iNext].y, -scale);
		m_newVertices[vi++].position = glm::vec3(m_clipped[iNext].x, m_clipped[iNext].y, 0);

		normal = glm::normalize(glm::cross(glm::vec3(m_clipped[iNext] - m_clipped[i], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		m_newVertices[ni++].Normals = normal;
		m_newVertices[ni++].Normals = normal;
		m_newVertices[ni++].Normals = normal;
		m_newVertices[ni++].Normals = normal;
	}

	for (int vert = 2; vert < count; vert++)
	{
		m_newFace[ti].indices[0] = 0;
		m_newFace[ti].indices[1] = vert - 1;
		m_newFace[ti].indices[2] = vert;
		ti++;
	}

	for (int vert = 2; vert < count; vert++)
	{
		m_newFace[ti].indices[0] = count;
		m_newFace[ti].indices[1] = count + vert;
		m_newFace[ti].indices[2] = count + vert - 1;
		ti++;
	}

	int si = 0;
	for (int vert = 0; vert < count; vert++)
	{
		si = 2 * count + 4 * vert;

		m_newFace[ti].indices[0] = si;
		m_newFace[ti].indices[1] = si + 1;
		m_newFace[ti].indices[2] = si + 2;
		ti++;

		m_newFace[ti].indices[0] = si;
		m_newFace[ti].indices[1] = si + 2;
		m_newFace[ti].indices[2] = si + 3;
		ti++;
	}
}

const unsigned int DstrGenerator::seedRand(int seed)
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

void DstrGenerator::Clear()
{
	m_diagram.Clear();

	m_clipped.clear();
	m_clipped.shrink_to_fit();

	m_newVertices.clear();
	m_newVertices.shrink_to_fit();
	m_newFace.clear();
	m_newFace.shrink_to_fit();
}


