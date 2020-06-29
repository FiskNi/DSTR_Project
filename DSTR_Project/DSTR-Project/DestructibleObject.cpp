#include <Pch/Pch.h>
#include "DestructibleObject.h"

DestructibleObject::DestructibleObject(DstrGenerator* dstr)
{
	dstrRef = dstr;
	m_type = DESTRUCTIBLE;
	m_scale = 0;
	m_lifetime = 0.0f;
}

DestructibleObject::DestructibleObject(FractureGenerator* fractureGen)
{
	fractureGenRef = fractureGen;
	m_type = DESTRUCTIBLE;
	m_scale = 0;
	m_lifetime = 0.0f;
}

DestructibleObject::~DestructibleObject()
{
	for (int i = 0; i < (int)m_meshes.size(); i++)
	{
		if (m_meshes[i].body)
		{
			RemoveBody(i);
		}
		m_meshes[i].mesh->Destroy();
		MeshMap::GetInstance()->removeMesh(m_meshes[i].mesh->getName());
	}
}

void DestructibleObject::Update(float dt)
{
	m_lifetime += dt;

	if (m_lifetime > 50000.0f)
	{
		Renderer* renderer = Renderer::GetInstance();
		renderer->removeRenderObject(this, STATIC);
	}

	// Enable collision
	//if (m_lifetime > 2.5f && m_lifetime < 3.0f)
	//{
	//	if (m_meshes[0].body)
	//	{
	//		m_meshes[0].body->setCollisionFlags(m_meshes[0].body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	//	}
	//}



	UpdateBulletRigids();
}

void DestructibleObject::SetPolygon(std::vector<glm::vec2> vertices)
{
	m_polygonFace = vertices;
	CalculateMinMax();
}

void DestructibleObject::SetScale(float scale)
{
	m_scale = scale;
}

void DestructibleObject::CalculateMinMax()
{
	min = m_polygonFace[0];
	max = m_polygonFace[0];
	for (size_t i = 0; i < m_polygonFace.size(); i++)
	{
		min.x = fminf(m_polygonFace[i].x, min.x);
		min.y = fminf(m_polygonFace[i].y, min.y);

		max.x = fmaxf(m_polygonFace[i].x, max.x);
		max.y = fmaxf(m_polygonFace[i].y, max.y);
	}
}


void DestructibleObject::loadDestructible(std::string fileName, float size)
{
	m_polygonFace.resize(4);
	m_polygonUV.resize(4);

	BGLoader meshLoader;	// The file loader
	meshLoader.LoadMesh(MESHPATH + fileName);
	if (meshLoader.GetVertexCount() < 4)
	{
		logWarning("DSTR: Invalid destructible mesh: {0}", fileName);
		return;
	}

	FindVertices(meshLoader.GetVertices());
	m_scale = size;
	meshFromPolygon(meshLoader.GetMeshName());
	
	// Load material
	Material newMaterial = meshLoader.GetMaterial();
	std::string materialName = newMaterial.name;
	if (!MaterialMap::GetInstance()->existsWithName(materialName)) 	// This creates the material if it does not exist (by name)
	{
		if (meshLoader.GetAlbedo() != "-1")
		{
			std::string albedoFile = TEXTUREPATH + meshLoader.GetAlbedo();
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load and generate the texture
			int width, height, nrChannels;
			unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, NULL);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				newMaterial.texture = true;
				newMaterial.textureID.push_back(texture);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
		}
		else
		{
			newMaterial.texture = false;
		}

		SetMaterial(MaterialMap::GetInstance()->createMaterial(materialName, newMaterial));
		logTrace("Material created: {0}", materialName);
	}
	else
	{
		SetMaterial(MaterialMap::GetInstance()->getMaterial(materialName));
	}

	SetTransform(meshLoader.GetTransform());
	meshLoader.Unload();
}

void DestructibleObject::loadDestructible(std::vector<Vertex> vertices_in, std::string name,
	Material newMaterial_in, std::string albedo_in, Transform transform, float size)
{
	m_polygonFace.resize(4);
	m_polygonUV.resize(4);

	FindVertices(vertices_in);
	m_scale = size;
	meshFromPolygon(name);

	// Load material
	Material newMaterial = newMaterial_in;
	std::string materialName = newMaterial_in.name;
	if (!MaterialMap::GetInstance()->existsWithName(materialName)) 	// This creates the material if it does not exist (by name)
	{
		if (albedo_in != "-1")
		{
			std::string albedoFile = TEXTUREPATH + albedo_in;
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load and generate the texture
			int width, height, nrChannels;
			unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, NULL);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				newMaterial.texture = true;
				newMaterial.textureID.push_back(texture);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);

		}
		else
		{
			newMaterial.texture = false;
		}

		SetMaterial(MaterialMap::GetInstance()->createMaterial(materialName, newMaterial));
		logTrace("Material created: {0}", materialName);
	}
	else
	{
		SetMaterial(MaterialMap::GetInstance()->getMaterial(materialName));
	}


	SetTransform(transform);
}

void DestructibleObject::LoadBasic(std::string name, float size)
{
	
	m_polygonFace.resize(4);
	m_polygonUV.resize(4);

	float sizeX = 27;
	float sizeY = 18;

	m_polygonFace[0] = glm::vec2(-sizeX, -sizeY);
	m_polygonFace[1] = glm::vec2(sizeX, -sizeY);
	m_polygonFace[2] = glm::vec2(sizeX, sizeY);
	m_polygonFace[3] = glm::vec2(-sizeX, sizeY);

	/*m_polygonFace.resize(12);
	m_polygonUV.resize(12);
	m_polygonFace[0] = glm::vec2(-30.0f, -19.0f);
	m_polygonFace[1] = glm::vec2(-15.0f, -21.0f);
	m_polygonFace[2] = glm::vec2(0.0f, -22.0f);
	m_polygonFace[3] = glm::vec2(15.0f, -21.0f);
	m_polygonFace[4] = glm::vec2(30.0f, -19.0f);
	m_polygonFace[5] = glm::vec2(35.0f, 0.0f);
	m_polygonFace[6] = glm::vec2(30.0f, 19.0f);
	m_polygonFace[7] = glm::vec2(15.0f, 21.0f);
	m_polygonFace[8] = glm::vec2(0.0f, 22.0f);
	m_polygonFace[9] = glm::vec2(-15.0f, 21.0f);
	m_polygonFace[10] = glm::vec2(-30.0f, 19.0f);
	m_polygonFace[11] = glm::vec2(-35.0f, 0.0f);*/

	m_scale = size;

	int count = 0;
	count = m_polygonFace.size();

	meshFromPolygon(name);
	CalculateMinMax();
}

void DestructibleObject::LoadBasic2(std::string name, float size)
{

	m_polygonFace.resize(4);
	m_polygonUV.resize(4);

	float sizeX = 27;
	float sizeY = 18;

	m_polygonFace.resize(12);
	m_polygonUV.resize(12);
	m_polygonFace[0] = glm::vec2(-30.0f, -19.0f);
	m_polygonFace[1] = glm::vec2(-15.0f, -21.0f);
	m_polygonFace[2] = glm::vec2(0.0f, -22.0f);
	m_polygonFace[3] = glm::vec2(15.0f, -21.0f);
	m_polygonFace[4] = glm::vec2(30.0f, -19.0f);
	m_polygonFace[5] = glm::vec2(35.0f, 0.0f);
	m_polygonFace[6] = glm::vec2(30.0f, 19.0f);
	m_polygonFace[7] = glm::vec2(15.0f, 21.0f);
	m_polygonFace[8] = glm::vec2(0.0f, 22.0f);
	m_polygonFace[9] = glm::vec2(-15.0f, 21.0f);
	m_polygonFace[10] = glm::vec2(-30.0f, 19.0f);
	m_polygonFace[11] = glm::vec2(-35.0f, 0.0f);

	m_scale = size;

	int count = 0;
	count = m_polygonFace.size();

	meshFromPolygon(name);
	CalculateMinMax();
}


void DestructibleObject::LoadBasic3(std::string name, float size)
{

	m_polygonFace.resize(4);
	m_polygonUV.resize(4);

	float sizeX = 20;
	float sizeY = 20;

	m_polygonFace[0] = glm::vec2(-sizeX, -sizeY);
	m_polygonFace[1] = glm::vec2(sizeX, -sizeY);
	m_polygonFace[2] = glm::vec2(sizeX, sizeY);
	m_polygonFace[3] = glm::vec2(-sizeX, sizeY);

	m_scale = size;

	int count = 0;
	count = m_polygonFace.size();

	meshFromPolygon(name);
	CalculateMinMax();
}

void DestructibleObject::LoadBasic4(std::string name, float size)
{

	m_polygonFace.resize(4);
	m_polygonUV.resize(4);

	float sizeX = 2;
	float sizeY = 80;

	m_polygonFace[0] = glm::vec2(-sizeX, -sizeY);
	m_polygonFace[1] = glm::vec2(sizeX, -sizeY);
	m_polygonFace[2] = glm::vec2(sizeX, sizeY);
	m_polygonFace[3] = glm::vec2(-sizeX, sizeY);

	m_scale = size;

	int count = 0;
	count = m_polygonFace.size();

	meshFromPolygon(name);
	CalculateMinMax();
}

void DestructibleObject::LoadDefined(std::string name, std::vector<glm::vec2> polygon)
{
	m_polygonFace = polygon;
	if (polygon.size() < 4)
	{
		logWarning("DSTR: Invalid destructible mesh: {0}", name);
		return;
	}
	m_scale = 0.1f;

	meshFromPolygon(name);
}

void DestructibleObject::FindVertices(const std::vector<Vertex>& vertices)
{
	glm::vec3 normal = vertices[0].Normals;

	// Order of vertices
	int bL = 0;		// Bottom Left
	int bR = 1;		// Bottom Right
	int tR = 2;		// Top Right
	int tL = 3;		// Top Left

	enum NormalDirection
	{
		undefined,
		Xpositive, Ypositive, Zpositive,
		Xnegative, Ynegative, Znegative
	};
	NormalDirection ndir = undefined;

	int x = glm::normalize(normal).x;
	int y = glm::normalize(normal).y;
	int z = glm::normalize(normal).z;
	if (x == 1 && y == 0 && z == 0)
		ndir = Xpositive;
	else if (x == 0 && y == 1 && z == 0)
		ndir = Ypositive;
	else if (x == 0 && y == 0 && z == 1)
		ndir = Zpositive;
	else if (x == -1 && y == 0 && z == 0)
		ndir = Xnegative;
	else if (x == 0 && y == -1 && z == 0)
		ndir = Ynegative;
	else if (x == 0 && y == 0 && z == -1)
		ndir = Znegative;

	switch (ndir)
	{
	case (Xpositive):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.z > 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.z < 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.z > 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	case (Ypositive):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.z > 0 && vertices[i].position.x < 0)
				bL = i;
			if (vertices[i].position.z > 0 && vertices[i].position.x > 0)
				bR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.x > 0)
				tR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.x < 0)
				tL = i;
		}
		break;
	case (Zpositive):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.x < 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.x > 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.x > 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.x < 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	case (Xnegative):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.z < 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.z > 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.z > 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.z < 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	case (Ynegative):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.x < 0 && vertices[i].position.z < 0)
				bL = i;
			if (vertices[i].position.x > 0 && vertices[i].position.z < 0)
				bR = i;
			if (vertices[i].position.x > 0 && vertices[i].position.z > 0)
				tR = i;
			if (vertices[i].position.x < 0 && vertices[i].position.z > 0)
				tL = i;
		}
		break;
	case (Znegative):
		for (size_t i = 0; i < 4; i++)
		{
			if (vertices[i].position.x > 0 && vertices[i].position.y < 0)
				bL = i;
			if (vertices[i].position.x < 0 && vertices[i].position.y < 0)
				bR = i;
			if (vertices[i].position.x < 0 && vertices[i].position.y > 0)
				tR = i;
			if (vertices[i].position.x > 0 && vertices[i].position.y > 0)
				tL = i;
		}
		break;
	default:
		logWarning("DSTR: Error finding vertices of destructible mesh, using default");
		break;
	}


	// Counter clockwise order
	m_polygonFace[0] = vertices[bL].position;	// Bottom Left
	m_polygonFace[1] = vertices[bR].position;	// Bottom Right
	m_polygonFace[2] = vertices[tR].position;	// Top Right
	m_polygonFace[3] = vertices[tL].position;	// Top Left

	m_polygonUV[0] = vertices[bL].UV;
	m_polygonUV[1] = vertices[bR].UV;
	m_polygonUV[2] = vertices[tR].UV;
	m_polygonUV[3] = vertices[tL].UV;
}

void DestructibleObject::meshFromPolygon(std::string name)
{
	int count = 0;
	glm::vec3 normal = glm::vec3();

	count = m_polygonFace.size();

	std::vector<Vertex> newVertices;
	newVertices.resize(6 * (size_t)count);
	std::vector<Face> newFace;
	newFace.resize(4 * (size_t)count - 4);

	int vi = 0;
	int uvi = 0;
	int ni = 0;
	int ti = 0;

	// Front
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, m_scale);
		newVertices[uvi++].UV = glm::vec2(m_polygonUV[i].x, m_polygonUV[i].y);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	// Back
	for (int i = 0; i < count; i++)
	{
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -m_scale);
		newVertices[uvi++].UV = glm::vec2(m_polygonUV[i].x, m_polygonUV[i].y);
		newVertices[ni++].Normals = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	// Sides
	for (int i = 0; i < count; i++)
	{
		//int iNext = i == count - 1 ? 0 : i + 1;
		int iNext = 0;
		if (i == count - 1)
			iNext = 0;
		else
			iNext = i + 1;

		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, m_scale);
		glm::vec3 test1 = newVertices[vi].position;
		newVertices[vi++].position = glm::vec3(m_polygonFace[i].x, m_polygonFace[i].y, -m_scale);

		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, -m_scale);
		newVertices[vi++].position = glm::vec3(m_polygonFace[iNext].x, m_polygonFace[iNext].y, m_scale);



		normal = glm::normalize(glm::cross(glm::vec3(m_polygonFace[iNext] - m_polygonFace[i], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		newVertices[ni++].Normals = normal;
		newVertices[ni++].Normals = normal;
		newVertices[ni++].Normals = normal;
		newVertices[ni++].Normals = normal;
	}

	// Faces
	for (int vert = 2; vert < count; vert++)
	{
		newFace[ti].indices[0] = 0;
		newFace[ti].indices[1] = vert - 1;
		newFace[ti].indices[2] = vert;
		ti++;
	}

	for (int vert = 2; vert < count; vert++)
	{
		newFace[ti].indices[0] = count;
		newFace[ti].indices[1] = count + vert;
		newFace[ti].indices[2] = count + vert - 1;
		ti++;
	}

	for (int vert = 0; vert < count; vert++)
	{
		int si = 2 * count + 4 * vert;

		newFace[ti].indices[0] = si;
		newFace[ti].indices[1] = si + 1;
		newFace[ti].indices[2] = si + 2;
		ti++;

		newFace[ti].indices[0] = si;
		newFace[ti].indices[1] = si + 2;
		newFace[ti].indices[2] = si + 3;
		ti++;
	}

	InitMesh(newVertices, newFace, name);

}


bool DestructibleObject::MeshFromSite(const float& scale, const std::vector<glm::vec2>& siteVertices)
{
	int vertexCount = siteVertices.size();
	if (vertexCount > 0)
	{
		Mesh* mesh = new Mesh();


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

		int si = 0;
		for (int vert = 0; vert < vertexCount; vert++)
		{
			si = 2 * vertexCount + 4 * vert;

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