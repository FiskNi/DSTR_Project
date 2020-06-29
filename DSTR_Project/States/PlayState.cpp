#include <Pch/Pch.h>
#include <System/StateManager.h>
#include "PlayState.h"
#define PLAYSECTION "PLAYSTATE"

#define SHOW_MEMORY_INFO false

void logVec3(glm::vec3 vector) {
	logTrace("Vector: ({0}, {1}, {2})", std::to_string(vector.x), std::to_string(vector.y), std::to_string(vector.z));
}

PlayState::PlayState()
{
	Shader* basicTempShader = ShaderMap::GetInstance()->getShader(BASIC_FORWARD);
	basicTempShader->use();
	basicTempShader->setInt("albedoTexture", 0);
	basicTempShader->setInt("normalMap", 1);	
	

	m_camera = new Camera();
	mu.printBoth("After physics and camera init:");

	Renderer* renderer = Renderer::GetInstance();
	renderer->setupCamera(m_camera);
	mu.printBoth("After renderer:");

	//// To get the height of the character at start due to bounding box calculations.
	//GameObject* AnimationMesh = new WorldObject("AnimationMesh");
	//AnimationMesh->loadMesh("NyCharacter.mesh");
	//delete AnimationMesh;
	//mu.printBoth("After animationMesh:");

	//m_player = new Player("Player", glm::vec3(0), m_camera);
	mu.printBoth("After player:");

	// Skybox
	LoadSkybox(renderer);

	// Map
	loadMap(renderer);

	mu.printBoth("End of play state init:");

}

PlayState::~PlayState()
{
	mu.printBoth("Before deleting playstate:");

	for (GameObject* object : m_objects)
		delete object;

	for (Pointlight* light : m_pointlights)
		if (light)
			delete light;

	m_pointlights.clear();
	m_objects.clear();

	if (m_skybox)
		delete m_skybox;
	if (m_player)
		delete m_player;
	if (m_camera)
		delete m_camera;

	BulletPhysics::GetInstance()->destroy();
	MeshMap::GetInstance()->cleanUp();

	mu.printBoth("Afer deleting playstate:");
}

void PlayState::LoadSkybox(Renderer* renderer)
{
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	mu.printBoth("After Skybox:");
	renderer->submitSkybox(m_skybox);
}

void PlayState::loadMap(Renderer* renderer)
{
	// Map objects
	switch (m_map)
	{
	case 0:
		// Insert objects here
		break;

	case 1:
		m_objects.push_back(new MapObject("Debug_Map"));
		m_objects[m_objects.size() - 1]->loadMesh("Debug_Map.mesh");
		break;

	default:
		break;
	}

	// Submit objects
	if (m_objects.size() > 0)
		renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
	mu.printBoth("After map objects:");

	// Collision 
	// TODO: Move to object constructor
	//for (GameObject* g : m_objects)
	//	g->BulletMakeStatic();
	mu.printBoth("After rigidbodies:");

	// Non-hitbox
	loadDecor(renderer);
	mu.printBoth("After decor objects:");

	// Lights
	loadLights(renderer);
	mu.printBoth("After point lights:");

	// Destuction
	loadDestructibles();
	mu.printBoth("After destructables:");
}

void PlayState::loadDecor(Renderer* renderer)
{
	switch (m_map)
	{
	case 0:
		// Load objects
		break;

	case 1:
		break;

	default:
		break;

	}

	// Submit objects
	if (m_objects.size() > 0)
		renderer->submit(m_objects[m_objects.size() - 1], RENDER_TYPE::STATIC);
	mu.printBoth("After decor:");
}

void PlayState::loadLights(Renderer* renderer)
{
	// Light attenuation chart
/*
	7		1.0,	0.7,		1.8
	13		1.0,	0.35,		0.44
	20		1.0,	0.22,		0.20
	32		1.0,	0.14,		0.07
	50		1.0,	0.09,		0.032
	65		1.0,	0.07,		0.017
	100		1.0,	0.045,		0.0075
	160		1.0,	0.027,		0.0028
	200		1.0,	0.022,		0.0019
	325		1.0,	0.014,		0.0007
	600		1.0,	0.007,		0.0002
	3250	1.0,	0.0014,		0.000007
*/

	switch (m_map)
	{
	case 0:
		break;

	case 1:
		// Light Middle
		m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), 40.0f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Right Back
		m_pointlights.emplace_back(new Pointlight(glm::vec3(50.0f, 20.0f, 50.0f), glm::vec3(1.0, 0.0, 1.0), 40.0f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Right Forward
		m_pointlights.emplace_back(new Pointlight(glm::vec3(50.0f, 20.0f, -50.0f), glm::vec3(0.0, 1.0, 1.0), 40.0f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Left Back
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-50.0f, 20.0f, 50.0f), glm::vec3(0.0, 0.0, 1.0), 40.0f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));

		// Light Left Forward
		m_pointlights.emplace_back(new Pointlight(glm::vec3(-50.0f, 20.0f, -50.0f), glm::vec3(0.0, 1.0, 1.0), 40.0f));
		m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, 0.07f, 0.017f, 100.0f));
		break;

	default:
		break;
	}

	// Submit lights
	for (Pointlight* p : m_pointlights)
	{
		renderer->submit(p, RENDER_TYPE::POINTLIGHT_SOURCE);
	}
}

// Might change these Pepega constructors later if feeling cute
void PlayState::loadDestructibles()
{
	Renderer* renderer = Renderer::GetInstance();
	BGLoader meshLoader; // The file loader
	
	m_dstr = DstrGenerator();
	m_dstr_alt1 = DstrGenerator();

	// Temporary variables to move later ---
	// Debug Destructibles
	int breakPoints = 16;
	float breakRadius = 1.1f;

	/*float gravityOnImpact = 0.0f;
	float timeToChange = 2.0f;
	float gravityAfterTime = 30.0f;*/ // Stop-fall effect

	float gravityOnImpact = -1.0f;
	float timeToChange = 2.5f;
	float gravityAfterTime = -8.0f; // Hover-up effect

	// Temporary variables to move later ---
	m_dstr.setBreakSettings(0, breakPoints, breakRadius, gravityOnImpact);
	m_dstr_alt1.setBreakSettings(1, breakPoints, 6.0f, gravityOnImpact);

	switch (m_map)
	{
	case 0:
		for (int i = (int)m_objects.size() - 1; i >= 0; i--)
		{
			if (m_objects[i]->getType() == DESTRUCTIBLE)
			{
				renderer->removeRenderObject(m_objects[i], STATIC);
				delete m_objects[i];
				m_objects.erase(m_objects.begin() + i);
			}
		}

		// Wall desctructibles
		meshLoader.LoadMesh(MESHPATH + "DSTRWalls.mesh");
		for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		{
			m_objects.emplace_back(new DestructibleObject(
				&m_dstr,
				m_objects.size(),
				gravityAfterTime,
				timeToChange
			));

			static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
				meshLoader.GetVertices(i),
				meshLoader.GetMeshName(i),
				meshLoader.GetMaterial(i),
				meshLoader.GetAlbedo(i),
				meshLoader.GetTransform(i),
				0.15f
			);

			m_objects.back()->BulletMakeStatic();
			Renderer::GetInstance()->submit(m_objects.back(), STATIC);
		}
		meshLoader.Unload();

		// Maze desctructibles
		meshLoader.LoadMesh(MESHPATH + "DSTRMaze.mesh");
		for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		{
			m_objects.emplace_back(new DestructibleObject(
				&m_dstr,
				m_objects.size(),
				gravityAfterTime,
				timeToChange
			));

			static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
				meshLoader.GetVertices(i),
				meshLoader.GetMeshName(i),
				meshLoader.GetMaterial(i),
				meshLoader.GetAlbedo(i),
				meshLoader.GetTransform(i),
				0.5f
			);

			m_objects.back()->BulletMakeStatic();
			Renderer::GetInstance()->submit(m_objects.back(), STATIC);
		}
		meshLoader.Unload();

		// Pillar destructibles
		meshLoader.LoadMesh(MESHPATH + "DSTRPillars.mesh");
		for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
		{
			m_objects.emplace_back(new DestructibleObject(
				&m_dstr_alt1,
				m_objects.size(),
				gravityAfterTime,
				timeToChange
			));

			static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
				meshLoader.GetVertices(i),
				meshLoader.GetMeshName(i),
				meshLoader.GetMaterial(i),
				meshLoader.GetAlbedo(i),
				meshLoader.GetTransform(i),
				1.6f
			);

			m_objects.back()->BulletMakeStatic();
			Renderer::GetInstance()->submit(m_objects.back(), STATIC);
		}
		meshLoader.Unload();
		break;

		case 1:
			// Debug Destructibles
			// Temporary variables to move later ---
			breakPoints = 20;
			breakRadius = 10.0f;

			gravityOnImpact = 30.0f;
			timeToChange = 2.5f;
			gravityAfterTime = 30.0f; 

			m_dstr.setBreakSettings(0, breakPoints, breakRadius, gravityOnImpact);

			meshLoader.LoadMesh(MESHPATH + "Debug_DSTR.mesh");
			for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
			{
				m_objects.emplace_back(new DestructibleObject(
					&m_dstr,
					m_objects.size(),
					gravityAfterTime,
					timeToChange));

				static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
					meshLoader.GetVertices(i),
					meshLoader.GetMeshName(i),
					meshLoader.GetMaterial(i),
					meshLoader.GetAlbedo(i),
					meshLoader.GetTransform(i),
					0.3f
				);

				m_objects.back()->BulletMakeStatic();
				Renderer::GetInstance()->submit(m_objects.back(), STATIC);
			}

			meshLoader.LoadMesh(MESHPATH + "Debug_DSTR_pillar.mesh");
			for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
			{
				m_objects.emplace_back(new DestructibleObject(
					&m_dstr,
					m_objects.size(),
					gravityAfterTime,
					timeToChange));

				static_cast<DestructibleObject*>(m_objects.back())->loadDestructible(
					meshLoader.GetVertices(i),
					meshLoader.GetMeshName(i),
					meshLoader.GetMaterial(i),
					meshLoader.GetAlbedo(i),
					meshLoader.GetTransform(i),
					1.5f
				);

				m_objects.back()->BulletMakeStatic();
				Renderer::GetInstance()->submit(m_objects.back(), STATIC);
			}
			meshLoader.Unload();

		default:
			break;
	}
}




static float memTimer = 0.0f;
static float fpsTimer = 0.0f;
void PlayState::Update(float dt)
{
	if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
	{
	}

	m_dstr.Update();
	m_dstr_alt1.Update();
	
	Update_isPlaying(dt);

	removeDeadObjects();

#if SHOW_MEMORY_INFO
	memTimer += DeltaTime;
	if (memTimer >= 1.0f) 
	{
		memTimer = 0.0f;
		mu.updateBoth();
		std::string memTex =
			"Ram: " + std::to_string(mu.getCurrentRamUsage()) +
			" | VRam: " + std::to_string(mu.getCurrentVramUsage()) +
			" | Highest Ram: " + std::to_string(mu.getHighestRamUsage()) +
			" | Highest VRam: " + std::to_string(mu.getHighestVramUsage());
		logInfo(memTex);
	}

	fpsTimer += DeltaTime;
	if (fpsTimer >= 1.0f) 
	{
		fpsTimer = 0.0f;
		logInfo("fps: " + std::to_string(Framerate));
	}
#endif





}

void PlayState::removeDeadObjects()
{
	Renderer* renderer = Renderer::GetInstance();
	for (int i = (int)m_objects.size() - 1; i >= 0; i--)
	{
		// Destructible case
		if (m_objects[i]->getType() == DESTRUCTIBLE)
		{
			DestructibleObject* obj = static_cast<DestructibleObject*>(m_objects[i]);
			if (obj->IsDestroyed() && obj->getLifetime() >= 20.0 )
			{
				renderer->removeRenderObject(m_objects[i], STATIC);
				// Keeping gameobjects for now, desync in indexing with server
				//delete m_objects[i];
				//m_objects.erase(m_objects.begin() + i);
			}
		}
	}
}

void PlayState::Update_isPlaying(const float& dt)
{
	if (m_player && m_camera)
	{
		m_player->Update(dt);
		m_camera->Update();
	}
	else if (m_camera)
	{
		m_camera->Update();
	}

	BulletPhysics::GetInstance()->Update(dt);

	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->Update(dt);
	}

}

void PlayState::Render()
{	
	Renderer::GetInstance()->Render();
}