#include <Pch/Pch.h>
#include <Loader/BGLoader.h>
#include "GameObject.h"

GameObject::GameObject()
{
	m_objectName = "Empty";
	m_type = 0;
	m_shouldRender = true;
}

GameObject::GameObject(std::string objectName)
{
	m_objectName = objectName;
	m_type = 0;
	m_shouldRender = true;
}

GameObject::~GameObject()
{
	for (int i = 0; i < (int)m_meshes.size(); i++)
		RemoveBody(i);
}

void GameObject::loadMesh(std::string fileName)
{
	BGLoader tempLoader;	// The file loader
	tempLoader.LoadMesh(MESHPATH + fileName);

	for (int i = 0; i < tempLoader.GetMeshCount(); i++)
	{
		// Get mesh
		MeshObject tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
		std::string meshName = tempLoader.GetMeshName(i);
		tempMeshBox.transform = tempLoader.GetTransform(i);		// One way of getting the meshes transform

		if (!MeshMap::GetInstance()->existsWithName(meshName))	// This creates the mesh if it does not exist (by name)
		{
			Mesh tempMesh;
			tempMesh.saveFilePath(tempLoader.GetFileName(), i);
			tempMesh.nameMesh(tempLoader.GetMeshName(i));
			// Default mesh
			tempMesh.setUpMesh(tempLoader.GetVertices(i), tempLoader.GetFaces(i));
			tempMesh.setUpBuffers();
			tempLoader.GetLoaderVertices(0)[0].tangent;

			// other way of getting the meshes transform
			// Value that may or may not be needed depening on how we want the meshes default position to be
			// Needs more testing, this value is per global mesh, the MeshObject value is per GameObject mesh
			// tempMesh.SetTransform(tempLoader.GetTransform(id));

			
			tempMesh.SetMaterial(tempLoader.GetMaterial(i).name);
			//Get the mesh pointer so that we don't have to always search through the MeshMap, when rendering
			tempMeshBox.mesh = MeshMap::GetInstance()->createMesh(meshName, tempMesh); 
			logTrace("Mesh loaded: {0}, Expecting material: {1}", tempMesh.getName().c_str(), tempMesh.getMaterial());
		}
		else {
			tempMeshBox.mesh = MeshMap::GetInstance()->GetMesh(meshName);
		}

		// Get material
		Material tempMaterial = tempLoader.GetMaterial(i);
		std::string materialName = tempMaterial.name;
		if (!MaterialMap::GetInstance()->existsWithName(materialName)) 	// This creates the material if it does not exist (by name)
		{
			if (tempLoader.GetAlbedo(i) != "-1")
			{
				std::string albedoFile = TEXTUREPATH + tempLoader.GetAlbedo(i);
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
				unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);

					tempMaterial.texture = true;
					tempMaterial.textureID.push_back(texture);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);
			}
			else
			{
				tempMaterial.texture = false;
			}

			tempMaterial.normalMap = false;

			//Get the material pointer so that we don't have to always search through the MatMap, when rendering
			tempMeshBox.material = MaterialMap::GetInstance()->createMaterial(materialName, tempMaterial);
 			logTrace("Material created: {0}", materialName);
		}
		else {
			tempMeshBox.material = MaterialMap::GetInstance()->getMaterial(materialName);
		}

		m_meshes.push_back(tempMeshBox);						// This effectively adds the mesh to the gameobject
	}

	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
	for (size_t i = 0; i < m_modelMatrixes.size(); i++)
	{
		m_modelMatrixes[i] = glm::mat4(1.0);
	}

	tempLoader.Unload();
}

void GameObject::InitMesh(Mesh mesh)
{
	MeshObject tempMeshBox;											// Meshbox holds the mesh identity and local transform to GameObject
	m_meshes.push_back(tempMeshBox);								// This effectively adds the mesh to the gameobject
	if (!MeshMap::GetInstance()->existsWithName(mesh.getName()))	// This creates the mesh if it does not exist (by name)
	{
		//Add mesh
		MeshMap::GetInstance()->createMesh(mesh.getName(), mesh);
	}

	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
}

void GameObject::InitMesh(Mesh* mesh)
{
	m_meshes.emplace_back();
	m_meshes.back().material = MaterialMap::GetInstance()->GetFirst();
	m_meshes.back().mesh = mesh;

	if (!MeshMap::GetInstance()->existsWithName(mesh->getName()))	// This creates the mesh if it does not exist (by name)
	{
		//Add mesh
		MeshMap::GetInstance()->AddMesh(mesh->getName(), mesh);
	}

	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
}

void GameObject::InitMesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces, std::string name)
{
	if (name == "")
		name = m_objectName + "_mesh";


	MeshObject tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
	if (!MeshMap::GetInstance()->existsWithName(name))		// This creates the mesh if it does not exist (by name)
	{
		Mesh tempMesh;
		tempMesh.nameMesh(name);
		
		// Default mesh
		tempMesh.setUpMesh(vertices, faces);
		tempMesh.setUpBuffers();

		//Add mesh
		tempMeshBox.mesh = MeshMap::GetInstance()->createMesh(name, tempMesh);

	}
	else {
		tempMeshBox.mesh = MeshMap::GetInstance()->GetMesh(name);
	}
	m_meshes.push_back(tempMeshBox);
	//Allocate all of the model matrixes
	m_modelMatrixes.resize(m_meshes.size());
}

const bool& GameObject::GetShouldRender() const
{
	return m_shouldRender;
}

const glm::vec3 GameObject::GetLastPosition() const
{
	return m_lastPosition;
}

//Update each individual modelmatrix for the meshes
void GameObject::UpdateModelMatrices() 
{
	Transform transform;
	for (size_t i = 0; i < m_modelMatrixes.size(); i++)
	{
		m_modelMatrixes[i] = glm::mat4(1.0f);
		transform = GetMeshWorldTransform(i);

		m_modelMatrixes[i] = glm::translate(m_modelMatrixes.at(i), transform.position);
		m_modelMatrixes[i] *= glm::mat4_cast(transform.rotation);
		m_modelMatrixes[i] = glm::scale(m_modelMatrixes[i], transform.scale);
	}

	m_lastPosition = m_transform.position;
}

void GameObject::SetTransform(Transform transform)
{
	m_transform = transform;
}

void GameObject::SetTransform(glm::vec3 worldPosition, glm::quat worldRot, glm::vec3 worldScale)
{
	m_transform.position = worldPosition;
	m_transform.scale = worldScale;
	m_transform.rotation = worldRot;
}

void GameObject::SetMeshOffsetTransform(Transform transform, int meshIndex)
{
	m_meshes[meshIndex].transform = transform;
}

void GameObject::SetObjectWorldPosition(glm::vec3 worldPosition)
{
	m_transform.position = worldPosition;
}

void GameObject::SetObjectWorldRotation(glm::quat worldRotation)
{
	m_transform.rotation = worldRotation;
}

void GameObject::SetObjectWorldScale(glm::vec3 scale)
{
	m_transform.scale = scale;
}

void GameObject::SetMeshOffsetPosition(glm::vec3 position, int meshIndex)
{
	m_meshes[meshIndex].transform.position = position;
}

void GameObject::SetMeshOffsetRotation(glm::quat rotation, int meshIndex)
{
	m_meshes[meshIndex].transform.rotation = rotation;
}

void GameObject::SetBodyWorldPosition(glm::vec3 worldPosition, int meshIndex)
{
	if (m_meshes[meshIndex].body)
	{
		btTransform newTransform = m_meshes[meshIndex].body->getWorldTransform();
		newTransform.setOrigin(btVector3(worldPosition.x, worldPosition.y, worldPosition.z));
		m_meshes[meshIndex].body->setWorldTransform(newTransform);


		SetTransformFromRigid(meshIndex);
	}
}

void GameObject::RemoveBody(int meshIndex)
{
	if (m_meshes[meshIndex].body)
	{
		BulletPhysics::GetInstance()->removeObject(m_meshes[meshIndex].body);
		m_meshes[meshIndex].body = nullptr;
	}
}

void GameObject::SetShouldRender(bool condition)
{
	m_shouldRender = condition;
}

void GameObject::SetMaterial(Material* material, int meshIndex)
{
	if (meshIndex == -1)
	{
		// Special case to make all models use material of the first mesh
		for (int i = 0; i < (int)m_meshes.size(); i++)
		{
			if (m_meshes[i].mesh)
				m_meshes[i].material = m_meshes[0].material;
		}
	}
	else
	{
		if (m_meshes.size() >= meshIndex)
			m_meshes[meshIndex].material = material;
	}

}

Mesh* GameObject::GetMesh(const int& meshIndex)
{
	return m_meshes[meshIndex].mesh;
}

Material* GameObject::GetMeshMaterial(const int& meshIndex)
{
	return m_meshes[meshIndex].material;
}

const Transform GameObject::GetMeshWorldTransform(int meshIndex) const
{
	// Adds the inherited transforms together to get the world position of a mesh
	Transform world_transform;
	world_transform.position = m_transform.position + m_meshes[meshIndex].transform.position;
	world_transform.rotation = m_transform.rotation * m_meshes[meshIndex].transform.rotation;
	world_transform.scale = m_transform.scale * m_meshes[meshIndex].transform.scale;

	return world_transform;
}

const Transform GameObject::GetObjectWorldTransform() const
{
	return m_transform;
}

const Transform GameObject::GetLocalTransform(int meshIndex) const
{
	return m_meshes[meshIndex].transform;
}

const Transform GameObject::GetRigidTransform(int meshIndex) const
{
	if (m_meshes.size() == 0 || !m_meshes[meshIndex].body)
		return GetObjectWorldTransform();

	btVector3 rigidBodyPos = m_meshes[meshIndex].body->getWorldTransform().getOrigin();

	btTransform rigidBodyTransform = m_meshes[meshIndex].body->getWorldTransform();
	Transform newTransform;
	newTransform.position.x = rigidBodyTransform.getOrigin().getX();
	newTransform.position.y = rigidBodyTransform.getOrigin().getY();
	newTransform.position.z = rigidBodyTransform.getOrigin().getZ();

	newTransform.rotation.x = rigidBodyTransform.getRotation().getX();
	newTransform.rotation.y = rigidBodyTransform.getRotation().getY();
	newTransform.rotation.z = rigidBodyTransform.getRotation().getZ();
	newTransform.rotation.w = rigidBodyTransform.getRotation().getW();

	newTransform.scale = GetMeshWorldTransform(meshIndex).scale;

	return newTransform;
}

const glm::mat4& GameObject::GetMatrix(const int& i) const
{
	if (m_modelMatrixes.size() == 0) {
		return glm::mat4(1.0f);
	}
	//if we are trying to access a matrix beyond our count
	if (i > static_cast<int>(m_modelMatrixes.size())) {
		return glm::mat4(1.0f);
	}
	return m_modelMatrixes[i];
}

void GameObject::BindMaterialToShader(std::string shaderName, int meshIndex)
{
	ShaderMap::GetInstance()->getShader(shaderName)->SetMaterial(m_meshes[meshIndex].material);
}

void GameObject::BindMaterialToShader(Shader* shader, const int& meshIndex)
{
	shader->SetMaterial(m_meshes[meshIndex].material);
}

void GameObject::BulletMakeStatic()
{

	for (size_t i = 0; i < m_meshes.size(); i++)
	{


		const std::vector<Vertex>& vertices = m_meshes[i].mesh->getVertices();
		
		glm::vec3 min = vertices[0].position;
		glm::vec3 max = vertices[0].position;

		for (size_t i = 1; i < vertices.size(); i++)
		{
			min.x = fminf(vertices[i].position.x, min.x);
			min.y = fminf(vertices[i].position.y, min.y);
			min.z = fminf(vertices[i].position.z, min.z);

			max.x = fmaxf(vertices[i].position.x, max.x);
			max.y = fmaxf(vertices[i].position.y, max.y);
			max.z = fmaxf(vertices[i].position.z, max.z);
		}


		glm::vec3 center = glm::vec3((min + max) * 0.5f) + GetMeshWorldTransform(i).position;
		glm::vec3 halfSize = glm::vec3((max - min) * 0.5f) * GetMeshWorldTransform(i).scale;

		m_meshes[i].body = BulletPhysics::GetInstance()->createObject(
			box,
			0.0f,
			center,
			halfSize,
			GetMeshWorldTransform(i).rotation
		);

		m_meshes[i].body->setUserPointer(this);
		

		SetTransformFromRigid(i);
	}

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();
	UpdateBulletRigids();
}

void GameObject::BulletAddRigidBody(btRigidBody* body, int meshIndex)
{
	if (m_meshes.size() <= meshIndex)
	{
		MeshObject newBox;
		newBox.body = body;
		newBox.body->setUserPointer(this);

		m_meshes.emplace_back(newBox);
	}
	else
	{
		m_meshes[meshIndex].body = body;
		m_meshes[meshIndex].body->setUserPointer(this);
	}

}

void GameObject::BulletCreateDynamic(CollisionObject shape, float weight, int meshIndex, bool recenter)
{
	if (m_meshes.size() <= meshIndex)
		m_meshes.resize((size_t)meshIndex + 1);

	const std::vector<Vertex>& vertices = m_meshes[meshIndex].mesh->getVertices();

	glm::vec3 min = vertices[0].position;
	glm::vec3 max = vertices[0].position;
	for (size_t i = 1; i < vertices.size(); i++)
	{
		min.x = fminf(vertices[i].position.x, min.x);
		min.y = fminf(vertices[i].position.y, min.y);
		min.z = fminf(vertices[i].position.z, min.z);

		max.x = fmaxf(vertices[i].position.x, max.x);
		max.y = fmaxf(vertices[i].position.y, max.y);
		max.z = fmaxf(vertices[i].position.z, max.z);
	}

	glm::vec3 center = GetMeshWorldTransform(meshIndex).position;
	if (!recenter)
		glm::vec3 center = glm::vec3((min + max) * 0.5f) + GetMeshWorldTransform(meshIndex).position;

	glm::vec3 halfSize = glm::vec3((max - min) * 0.5f) * GetMeshWorldTransform(meshIndex).scale;
	m_meshes[meshIndex].body = BulletPhysics::GetInstance()->createObject(
		shape,
		weight,
		center,
		halfSize,
		GetMeshWorldTransform(meshIndex).rotation,
		true,
		0.0f,
		1.0f);

	m_meshes[meshIndex].body->setUserPointer(this);
	

	m_transform.position = glm::vec3(0.0f);
	m_transform.rotation = glm::quat();

	SetTransformFromRigid(meshIndex);
}

void GameObject::UpdateBulletRigids()
{
	for (int i = 0; i < (int)m_meshes.size(); i++)
	{
		if (!m_meshes[i].body)
			continue;

		if (!m_meshes[i].body->isActive())
			continue;

		SetTransformFromRigid(i);
	}
}

void GameObject::SetTransformFromRigid(int meshIndex)
{
	const btTransform& rigidBodyTransform = m_meshes[meshIndex].body->getWorldTransform();
	const btVector3& btPos = rigidBodyTransform.getOrigin();
	const btQuaternion& btRotation = rigidBodyTransform.getRotation();

	t_transform.position.x = btPos.getX();
	t_transform.position.y = btPos.getY();
	t_transform.position.z = btPos.getZ();

	t_transform.rotation.x = btRotation.getX();
	t_transform.rotation.y = btRotation.getY();
	t_transform.rotation.z = btRotation.getZ();
	t_transform.rotation.w = btRotation.getW();

	m_meshes[meshIndex].transform.position = t_transform.position;
	m_meshes[meshIndex].transform.rotation = t_transform.rotation;
}

void GameObject::SetTexture(const char* fileName)
{
	std::string albedoFile = TEXTUREPATH + fileName;
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
	unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		for (int i = 0; i < m_meshes.size(); i++)
		{
			m_meshes.at(i).material->texture = true;
			m_meshes.at(i).material->textureID.push_back(texture);
		}
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);			
}
