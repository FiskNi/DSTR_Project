#include <Pch/Pch.h>
#include <System/StateManager.h>
#include "DestructionTester.h"

#define SHOW_MEMORY_INFO false




DestructionTester::DestructionTester()
{
	// Assign
	m_player = nullptr;
	m_skybox = nullptr;
	
	m_camera = new Camera();
	m_player = new Player("Player", glm::vec3(0), m_camera);
	mu.printBoth("After physics and camera init:");
	
	m_renderer = Renderer::GetInstance();

	memTimer = 0.0f;
	fpsTimer = 0.0f;
	
	// Initialize
	Shader* basicTempShader = ShaderMap::GetInstance()->getShader(BASIC_FORWARD);
	basicTempShader->use();
	basicTempShader->setInt("albedoTexture", 0);
	basicTempShader->setInt("normalMap", 1);

	m_renderer->setupCamera(m_camera);
	mu.printBoth("After renderer:");

	weapon.position = m_camera->getCamPos();
	weapon.direction = m_camera->getCamFace();

	// Geneterate bullet objects / hitboxes
	//gContactAddedCallback = BulletCallback;

	// -50 for ground
	BulletPhysics::GetInstance()->getDynamicsWorld()->setGravity(btVector3(0, -50, 0));

	// Map
	LoadScene();

	// Timer
	m_debugFractureTimer.Allocate();
	









	
	mu.printBoth("End of play state init:");
}

DestructionTester::~DestructionTester()
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

void DestructionTester::Update(float dt)
{

	//ShowMemoryInfo();


	// DSTR project functions
	DebugButtons();

	if (m_runDebug)
		m_debugFractureTimer.time += dt;
	//DebugScript();
	// Fundamental updates
	if (m_player && m_camera)
	{
		m_player->Update(dt);
		m_camera->Update();
	}
	else if (m_camera)
	{
		m_camera->Update();
	}
	
	weapon.Update(dt);
	RayCastBreak();

	// Slow time
	//dt /= 30;


	BulletPhysics::GetInstance()->Update(dt);

	// Update game objects
	for (GameObject* object : m_objects)
	{
		object->Update(dt);
		object->UpdateModelMatrices();
	}
	
	// Draw debug bounding boxes
	//BulletPhysics::GetInstance()->getDynamicsWorld()->debugDrawWorld();
}

void DestructionTester::DebugScript()
{
	if (m_runDebug)
	{
		if (m_debugFractureTimer.CheckSize())
		{
			if (m_debugFractureTimer.CheckIteration())
			{
				
				// Reset destructible objects
				for (int i = (int)m_objects.size() - 1; i >= 0; i--)
				{
					if (m_objects[i]->getType() == DESTRUCTIBLE)
					{
						m_renderer->removeRenderObject(m_objects[i], STATIC);
						delete m_objects[i];
						m_objects.erase(m_objects.begin() + i);
					}
				}
				LoadDestructibles();

				int object = -1;

				for (int i = 0; i < m_objects.size(); i++)
				{
					if (m_objects[i]->GetName() == "Destructible_Script")
						object = i;
				}

				if (object == -1)
					return;


				DestructibleObject* dstrObj = nullptr;
				dstrObj = static_cast<DestructibleObject*>(m_objects[object]);

				// set point settings
				m_debugFractureGen.SetPointAmount(m_debugFractureTimer.currentSize);

				glm::vec2 hitPoint = glm::circularRand(2.0f);
				glm::vec3 hitDirection(0);

				auto fractureTime1 = std::chrono::steady_clock::now();
				m_debugFractureGen.DebugFracture(dstrObj, hitPoint, m_objects, &m_debugFractureTimer);
				auto fractureTime2 = std::chrono::steady_clock::now();

				auto computationTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(fractureTime2 - fractureTime1).count();
				float computationTime_milli = (float)computationTime_micro / 1000;

				// Add to debug
				m_debugFractureTimer.computationTime_micro.emplace_back(computationTime_micro);
				m_debugFractureTimer.computationTime_milli.emplace_back(computationTime_milli);


				//std::cout << "Current iteration: " << m_debugFractureTimer.currentIteration << std::endl;
				std::cout << "Current size: " << m_debugFractureTimer.currentSize << std::endl;
				std::cout << "Fracture time (ƒÊs): " << computationTime_micro << std::endl;
				std::cout << "Fracture time (ms): " << computationTime_milli << std::endl;

				// Increment iteration
				m_debugFractureTimer.currentIteration++;
					
				
			}
			else
			{
				m_debugFractureTimer.ComputeAverage();
				m_debugFractureTimer.ClearCurrent();
				m_debugFractureTimer.IterationAllocate();
				// Increase size
				m_debugFractureTimer.currentSize++;
				m_debugFractureTimer.currentSize++;
			}
		}
		else
		{
			m_debugFractureTimer.WriteResults();
			m_debugFractureTimer.ComputeTotal();
			m_debugFractureTimer.Reset();
			
			std::cout << std::endl;
			std::cout << "Fracure iterations complete" << std::endl;
			std::cout << "Total script execution time: " << m_debugFractureTimer.time << std::endl;
			std::cout << "Fracture time average (ƒÊs): " << m_debugFractureTimer.averageTotal_micro << std::endl;
			std::cout << "Fracture time average (ms): " << m_debugFractureTimer.averageTotal_milli << std::endl;

			m_runDebug = false;
		}
	}
}

void DestructionTester::RayCastBreak()
{
	if (m_camera)
	{
		btDiscreteDynamicsWorld* bt_world = BulletPhysics::GetInstance()->getDynamicsWorld();
		glm::vec3& position = weapon.position;
		glm::vec3& direction = weapon.direction;
		float length = 1000.0f;

		// Draw ray
		//if (weapon.cooldown > weapon.fireRate / 2)
		{
			btVector3 from = btVector3(position.x, position.y - 0.1f, position.z);
			btVector3 to = btVector3(position.x + direction.x * length, position.y + direction.y * length, position.z + direction.z * length);
			//bt_world->getDebugDrawer()->drawLine(from, to, btVector4(0.77, 0.07, 0.32, 1));
			bt_world->getDebugDrawer()->drawLine(from, to, btVector4(0.0, 0.847, 0.945, 1));
		}

		if (weapon.Shoot())
		{
			position = m_camera->getCamPos();
			direction = m_camera->getCamFace();

			direction += glm::sphericalRand(weapon.spread);

			//direction.x += ((float)(rand() / (1.0f + RAND_MAX) * 1.0f) - 0.5f) * weapon.spread;
			//direction.y += ((float)(rand() / (1.0f + RAND_MAX) * 1.0f) - 0.5f) * weapon.spread;

			// TODO: Possibly fix this mess
			btVector3 from;
			btVector3 to;
			btCollisionWorld::ClosestRayResultCallback hit(from, to);
			btCollisionWorld::AllHitsRayResultCallback allHits(from, to);
			m_player->RayCast(position, direction, hit, allHits);
			if (hit.hasHit())
			{
				GameObject* hitObject = static_cast<GameObject*>(hit.m_collisionObject->getUserPointer());
				if (hitObject && hitObject->getType() == DESTRUCTIBLE)
				{
					DestructibleObject* dstrObj = nullptr;
					dstrObj = static_cast<DestructibleObject*>(hitObject);

					if (dstrObj && !dstrObj->IsDestroyed())
					{
						// Get fracture generator
						m_fractureGen.seedRand();

						// Calculate hitpoint
						glm::vec3 hitPointWorld = glm::vec3(
							hit.m_hitPointWorld.getX(),
							hit.m_hitPointWorld.getY(),
							hit.m_hitPointWorld.getZ());
						glm::vec3 hitPoint = hitPointWorld - dstrObj->GetRigidTransform().position;
						// Adjust for object rotation
						hitPoint = hitPoint * dstrObj->GetRigidTransform().rotation;

						// Fracture object
						auto t1 = std::chrono::steady_clock::now();
						m_fractureGen.Fracture(dstrObj, glm::vec2(hitPoint.x, hitPoint.y), direction, m_objects);
						auto t2 = std::chrono::steady_clock::now();

						auto computationTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
						float computationTime_milli = (float)computationTime_micro / 1000;



						std::cout << "Fracture time: " << computationTime_micro << std::endl;
						std::cout << "Fracture time: " << computationTime_milli << std::endl;
						logInfo("Fracture time: " + std::to_string(computationTime_micro));
					}
				}
			}
		}
		else if (weapon.Pierce())
		{
			position = m_camera->getCamPos();
			direction = m_camera->getCamFace();


			direction += glm::sphericalRand(weapon.spread);

			//direction.x += ((float)(rand() / (1.0f + RAND_MAX) * 1.0f) - 0.5f) * weapon.spread;
			//direction.y += ((float)(rand() / (1.0f + RAND_MAX) * 1.0f) - 0.5f) * weapon.spread;

			// TODO: Possibly fix this mess
			btVector3 from;
			btVector3 to;
			btCollisionWorld::ClosestRayResultCallback hit(from, to);
			btCollisionWorld::AllHitsRayResultCallback allHits(from, to);
			m_player->RayCast(position, direction, hit, allHits);

			bool fractured = false;
			auto t1 = std::chrono::steady_clock::now();
			for (int i = 0; i < allHits.m_hitFractions.size(); i++)
			{
				GameObject* hitObject = static_cast<GameObject*>(allHits.m_collisionObjects[i]->getUserPointer());
				if (hitObject && hitObject->getType() == DESTRUCTIBLE)
				{
					fractured = true;

					DestructibleObject* dstrObj = nullptr;
					dstrObj = static_cast<DestructibleObject*>(hitObject);

					if (dstrObj && !dstrObj->IsDestroyed())
					{
						// Get fracture generator
						m_fractureGen.seedRand();

						// Calculate hitpoint
						glm::vec3 hitPointWorld = glm::vec3(
							allHits.m_hitPointWorld[i].getX(),
							allHits.m_hitPointWorld[i].getY(),
							allHits.m_hitPointWorld[i].getZ());
						glm::vec3 hitPoint = hitPointWorld - dstrObj->GetRigidTransform().position;
						// Adjust for object rotation
						hitPoint = hitPoint * dstrObj->GetRigidTransform().rotation;

						// Fracture object
						m_fractureGen.Fracture(dstrObj, glm::vec2(hitPoint.x, hitPoint.y), direction, m_objects);
					}
				}
			}

			if (fractured)
			{
				auto t2 = std::chrono::steady_clock::now();

				auto computationTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
				float computationTime_milli = (float)computationTime_micro / 1000;
				std::cout << "Fracture time: " << computationTime_micro << std::endl;
				std::cout << "Fracture time: " << computationTime_milli << std::endl;
				logInfo("Fracture time: " + std::to_string(computationTime_micro));
			}



		}

	}
}

void DestructionTester::DebugButtons()
{
	// Run debug script
	if (Input::isKeyPressed(GLFW_KEY_F1) && m_runDebug == false)
	{
		for (int i = (int)m_objects.size() - 1; i >= 0; i--)
		{
			if (m_objects[i]->getType() == DESTRUCTIBLE)
			{
				m_renderer->removeRenderObject(m_objects[i], STATIC);
				delete m_objects[i];
				m_objects.erase(m_objects.begin() + i);
			}
		}
		LoadDestructibles();

		m_debugFractureTimer.Reset();
		m_runDebug = true;
	}
	if (Input::isKeyPressed(GLFW_KEY_F2))
	{
		std::cout << std::endl << "Preset 1 Loaded" << std::endl;

		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->GetName() == "GroundPlane")
			{
				m_objects[i]->SetShouldRender(false);
			}
		}


		m_fractureGen.SetRadiusSize(8.0f);
		std::cout << "Circle Radius set to " << m_fractureGen.GetRadiusSize() << std::endl;

		m_fractureGen.SetPointAmount(100);
		std::cout << "100 (Default) Fractured meshes set" << std::endl;


		m_fractureGen.SetForceStrength(50.0f);
		std::cout << "Low physics (Force: 50)" << std::endl;

		BulletPhysics::GetInstance()->getDynamicsWorld()->setGravity(btVector3(0, 0, 0));
		std::cout << "Gravity Disabled " << std::endl;


	}
	if (Input::isKeyPressed(GLFW_KEY_F3))
	{
		std::cout << std::endl << "Preset 2 Loaded" << std::endl;

		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->GetName() == "GroundPlane")
			{
				m_objects[i]->SetShouldRender(true);
			}
		}

		m_fractureGen.SetRadiusSize(8.0f);
		std::cout << "Circle Radius set to " << m_fractureGen.GetRadiusSize() << std::endl;

		m_fractureGen.SetPointAmount(100);
		std::cout << "100 (Default) Fractured meshes set" << std::endl;

		m_fractureGen.SetForceStrength(200.0f);
		std::cout << "Low physics (Force: 50)" << std::endl;

		BulletPhysics::GetInstance()->getDynamicsWorld()->setGravity(btVector3(0, -50, 0));
		std::cout << "Gravity Enabled " << std::endl;
	}



	if (Input::isKeyPressed(GLFW_KEY_1))
	{
		m_fractureGen.SetPointAmount(100);
		std::cout << "(Default) 100 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_2))
	{
		m_fractureGen.SetPointAmount(20);
		std::cout << "20 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_3))
	{
		m_fractureGen.SetPointAmount(30);
		std::cout << "30 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_4))
	{
		m_fractureGen.SetPointAmount(40);
		std::cout << "40 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_5))
	{
		m_fractureGen.SetPointAmount(80);
		std::cout << "80 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_6))
	{
		m_fractureGen.SetPointAmount(200);
		std::cout << "200 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_7))
	{
		m_fractureGen.SetPointAmount(350);
		std::cout << "350 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_8))
	{
		m_fractureGen.SetPointAmount(500);
		std::cout << "500 Fractured meshes set" << std::endl;
	}
	if (Input::isKeyPressed(GLFW_KEY_9))
	{
		m_fractureGen.SetPointAmount(1000);
		std::cout << "1000 Fractured meshes set" << std::endl;
	}


	if (Input::isKeyPressed(GLFW_KEY_KP_7))
	{
		m_fractureGen.SetRadiusSize(m_fractureGen.GetRadiusSize() + 0.1f);
		std::cout << "Circle Radius set to " << m_fractureGen.GetRadiusSize() << std::endl;
	}

	if (Input::isKeyPressed(GLFW_KEY_KP_4))
	{
		m_fractureGen.SetRadiusSize(m_fractureGen.GetRadiusSize() - 0.1f);
		std::cout << "Circle Radius set to " << m_fractureGen.GetRadiusSize() << std::endl;
	}

	if (Input::isKeyPressed(GLFW_KEY_KP_8))
	{
		m_fractureGen.SetRadiusSize(m_fractureGen.GetRadiusSize() + 1.0f);
		std::cout << "Circle Radius set to " << m_fractureGen.GetRadiusSize() << std::endl;
	}

	if (Input::isKeyPressed(GLFW_KEY_KP_5))
	{
		m_fractureGen.SetRadiusSize(m_fractureGen.GetRadiusSize() - 1.0f);
		std::cout << "Circle Radius set to " << m_fractureGen.GetRadiusSize() << std::endl;
	}


	if (Input::isKeyPressed(GLFW_KEY_G))
	{
		// -50 for ground
		if (BulletPhysics::GetInstance()->getDynamicsWorld()->getGravity().getY() < -1.0)
		{
			BulletPhysics::GetInstance()->getDynamicsWorld()->setGravity(btVector3(0, 0, 0));
			std::cout << "Gravity Disabled " << std::endl;
		}
		else
		{
			BulletPhysics::GetInstance()->getDynamicsWorld()->setGravity(btVector3(0, -50, 0));
			std::cout << "Gravity Enabled " << std::endl;
		}
	}


	if (Input::isKeyPressed(GLFW_KEY_E))
	{
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->GetName() == "GroundPlane")
			{
				m_objects[i]->SetShouldRender(!m_objects[i]->GetShouldRender());
				
				//m_renderer->removeRenderObject(m_objects[i], STATIC);
				//delete m_objects[i];
				//m_objects.erase(m_objects.begin() + i);
			}
		}
	}

	if (Input::isKeyPressed(GLFW_KEY_H))
	{
		m_fractureGen.SetCirclePattern(!m_fractureGen.GetCirclePattern());
		std::cout << "CirclePattern: " << m_fractureGen.GetCirclePattern() << std::endl;
	}

	if (Input::isKeyPressed(GLFW_KEY_B))
	{
		m_fractureGen.SetBulletHoles(!m_fractureGen.GetBulletHoles());
		std::cout << "Bulletholes: " << m_fractureGen.GetBulletHoles() << std::endl;
	}

	if (Input::isKeyPressed(GLFW_KEY_F))
	{
		if (m_fractureGen.GetForceStrength() == 18.0f)
		{
			m_fractureGen.SetForceStrength(100.0f);
			std::cout << "100 Force | Default physics" << std::endl;
		}
		else if (m_fractureGen.GetForceStrength() == 100.0f)
		{
			m_fractureGen.SetForceStrength(200.0f);
			std::cout << "200 Force | High physics" << std::endl;
		}
		else if (m_fractureGen.GetForceStrength() == 200.0f)
		{
			m_fractureGen.SetForceStrength(300.0f);
			std::cout << "300 Force | Very High physics" << std::endl;
		}
		else if (m_fractureGen.GetForceStrength() == 300.0f)
		{
			m_fractureGen.SetForceStrength(10.0f);
			std::cout << "10 Force | Minimal physics" << std::endl;
		}
		else if(m_fractureGen.GetForceStrength() == 10.0f)
		{
			m_fractureGen.SetForceStrength(50.0f);
			std::cout << "50 Force | Small physics" << std::endl;
		}
		else
		{
			m_fractureGen.SetForceStrength(100.0f);
			std::cout << "Default physics (Force: 100)" << std::endl;
		}
	}

	if (Input::isKeyPressed(GLFW_KEY_T))
	{
		for (int i = (int)m_objects.size() - 1; i >= 0; i--)
		{
			if (m_objects[i]->getType() == DESTRUCTIBLE)
			{
				m_renderer->removeRenderObject(m_objects[i], STATIC);
				delete m_objects[i];
				m_objects.erase(m_objects.begin() + i);
			}
		}
		LoadDestructibles();

		int object = -1;
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->GetName() == "Destructible_Script")
				object = i;
		}
		if (object == -1)
			return;

		DestructibleObject* dstrObj = nullptr;
		dstrObj = static_cast<DestructibleObject*>(m_objects[object]);

		auto fractureTime1 = std::chrono::steady_clock::now();
		m_fractureGen.DebugFracture(dstrObj, glm::vec2(0), m_objects, &m_debugFractureTimer);
		auto fractureTime2 = std::chrono::steady_clock::now();
		
		auto computationTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(fractureTime2 - fractureTime1).count();
		float computationTime_milli = (float)computationTime_micro / 1000;
		std::cout << "Current size: " << m_fractureGen.GetPointAmount() << std::endl;
		std::cout << "Fracture time (ƒÊs): " << computationTime_micro << std::endl;
		std::cout << "Fracture time (ms): " << computationTime_milli << std::endl;
	}





	// Manual fracture
	// Hardcoded for object in vector
	//if (Input::isKeyPressed(GLFW_KEY_M))
	//{
	//	if (m_objects[1]->getType() == DESTRUCTIBLE)
	//	{
	//		DestructibleObject* dstrObj = nullptr;
	//		dstrObj = static_cast<DestructibleObject*>(m_objects[1]);
	//
	//		// Get fracture generator
	//		FractureGenerator* debugFractureGen = dstrObj->GetFractureGenerator();
	//
	//		glm::vec2 hitPoint(0);
	//		glm::vec3 hitDirection(0);
	//
	//		debugFractureGen->DebugFracture(dstrObj, hitPoint, m_objects, m_debugFractureTimer);
	//	}
	//}

	// Reset DSTR objects
	if (Input::isKeyPressed(GLFW_KEY_R))
	{
		for (int i = (int)m_objects.size() - 1; i >= 0; i--)
		{
			if (m_objects[i]->getType() == DESTRUCTIBLE)
			{
				m_renderer->removeRenderObject(m_objects[i], STATIC);
				delete m_objects[i];
				m_objects.erase(m_objects.begin() + i);
			}
		}

		LoadDestructibles();
	}
}

void DestructionTester::Render()
{
	Renderer::GetInstance()->Render();
}

void DestructionTester::LoadSkybox()
{
	m_skybox = new SkyBox();
	m_skybox->prepareBuffers();
	mu.printBoth("After Skybox:");
	m_renderer->submitSkybox(m_skybox);
}

void DestructionTester::LoadScene()
{
	// Skybox
	LoadSkybox();
	DebugCreatePlane();
	LoadDestructibles();

	// Lights
	LoadLights();
	mu.printBoth("After point lights:");
}

void DestructionTester::LoadLights()
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

	// Quick debug values
	float baseStrength = 60.0f;
	float att1 = 0.07f;
	float att2 = 0.017;

	// Light Middle
	//m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), baseStrength / 10));
	//m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	//// Light Right Back
	//m_pointlights.emplace_back(new Pointlight(glm::vec3(40.0f, 30.0f, 30.0f), glm::vec3(1.0, 0.0, 1.0), baseStrength * 3));
	//m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	//// Light Right Forward
	//m_pointlights.emplace_back(new Pointlight(glm::vec3(50.0f, 30.0f, -50.0f), glm::vec3(0.0, 1.0, 1.0), baseStrength));
	//m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	//// Light Left Back
	//m_pointlights.emplace_back(new Pointlight(glm::vec3(-40.0f, 30.0f, 30.0f), glm::vec3(0.0, 0.0, 1.0), baseStrength * 3));
	//m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	//// Light Left Forward 
	//m_pointlights.emplace_back(new Pointlight(glm::vec3(-50.0f, 30.0f, -50.0f), glm::vec3(0.0, 1.0, 1.0), baseStrength));
	//m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Middle
	m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Right Back
	m_pointlights.emplace_back(new Pointlight(glm::vec3(80.0f, 50.0f, 80.0f), glm::vec3(1.0, 0.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Right Forward
	m_pointlights.emplace_back(new Pointlight(glm::vec3(80.0f, 50.0f, -80.0f), glm::vec3(0.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Left Back
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-80.0f, 50.0f, 80.0f), glm::vec3(0.0, 0.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Left Forward 
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-80.0f, 50.0f, -80.0f), glm::vec3(0.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	baseStrength = 20.0f;
	att1 = 0.022f;
	att2 = 0.0019f;
	// Light Middle
	m_pointlights.emplace_back(new Pointlight(glm::vec3(200.0f, 80.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	m_pointlights.emplace_back(new Pointlight(glm::vec3(-200.0f, 80.0f, 0.0f), glm::vec3(1.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 80.0f, 200.0f), glm::vec3(1.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	m_pointlights.emplace_back(new Pointlight(glm::vec3(0.0f, 80.0f, -200.0f), glm::vec3(1.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	baseStrength = 20.0f;
	att1 = 0.022f;
	att2 = 0.0019f;
	// Light Right Back
	m_pointlights.emplace_back(new Pointlight(glm::vec3(180.0f, 130.0f, 180.0f), glm::vec3(1.0, 0.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Right Forward
	m_pointlights.emplace_back(new Pointlight(glm::vec3(180.0f, 130.0f, -180.0f), glm::vec3(0.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Left Back
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-180.0f, 130.0f, 180.0f), glm::vec3(0.0, 0.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));

	// Light Left Forward
	m_pointlights.emplace_back(new Pointlight(glm::vec3(-180.0f, 130.0f, -180.0f), glm::vec3(0.0, 1.0, 1.0), baseStrength));
	m_pointlights.back()->setAttenuationAndRadius(glm::vec4(1.0f, att1, att2, 1000.0f));





	// Submit lights
	for (Pointlight* p : m_pointlights)
	{
		m_renderer->submit(p, RENDER_TYPE::POINTLIGHT_SOURCE);
	}
}

// Might change these Pepega constructors later if feeling cute
void DestructionTester::LoadDestructibles()
{
	//LoadShowcaseDestructibles();
	LoadDefaultDestructables();

	/*
	//BGLoader meshLoader; // The file loader

	m_dstr_1 = DstrGenerator();
	m_dstr_2 = DstrGenerator();

	// Temporary variables to move later ---
	// Debug Destructibles
	int breakPoints = 10;
	float breakRadius = 3.0f;

	float gravityOnImpact = 30.0f;
	float timeToChange = 2.5f;
	float gravityAfterTime = 30.0f;
	
	//float gravityOnImpact = 0.0f;
	//float timeToChange = 2.0f;
	//float gravityAfterTime = 30.0f; // Stop-fall effect

	//float gravityOnImpact = -1.0f;
	//float timeToChange = 2.5f;
	//float gravityAfterTime = -8.0f; // Hover-up effect

	// Temporary variables to move later --
	m_dstr_1.setBreakSettings(0, breakPoints, breakRadius, gravityOnImpact);
	m_dstr_2.setBreakSettings(1, breakPoints, 6.0f, gravityOnImpact);

	meshLoader.LoadMesh(MESHPATH + "Debug_DSTR.mesh");
	for (int i = 0; i < (int)meshLoader.GetMeshCount(); i++)
	{
		m_objects.emplace_back(new DestructibleObject(
			&m_dstr_1,
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
			&m_dstr_2,
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
	*/
	
	//// Back object
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_99");
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(0.0f, 20.0f, 50));
	//m_renderer->submit(m_objects.back(), STATIC);



}
void DestructionTester::LoadDefaultDestructables()
{
	// Front object
	float placementY = 30.0f;

	m_objects.emplace_back(new DestructibleObject(&m_debugFractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_Script", 0.1f);
	m_objects.back()->SetName("Destructible_Script");
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(0.0f, placementY, 50));
	m_renderer->submit(m_objects.back(), STATIC);


	float placementZ = -25;
	placementZ = static_cast<DestructibleObject*>(m_objects.back())->getScale();
	placementZ += placementZ + 35;
	placementZ *= -2;

	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_1", 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(0.0f, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementZ = static_cast<DestructibleObject*>(m_objects.back())->getScale();
	placementZ += placementZ + 35;
	placementZ *= -3;

	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_2", 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(0.0f, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementZ = static_cast<DestructibleObject*>(m_objects.back())->getScale();
	placementZ += placementZ + 35;
	placementZ *= -4;

	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_3", 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(0.0f, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementZ = static_cast<DestructibleObject*>(m_objects.back())->getScale();
	placementZ += placementZ + 35;
	placementZ *= -5;

	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_4", 0.25f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(0.0f, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);



	float placementX = 0;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_5", 0.1f);
	m_objects.back()->BulletMakeStatic();

	placementX = static_cast<DestructibleObject*>(m_objects.back())->GetMax().x;
	placementX += placementX + 30;
	placementX *= -1;

	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, -25.0f));
	m_renderer->submit(m_objects.back(), STATIC);


	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_6", 1.0f);
	m_objects.back()->BulletMakeStatic();
	placementX = static_cast<DestructibleObject*>(m_objects.back())->GetMax().x;
	placementX += placementX + 30;
	placementX *= -2;
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, -25.0f));
	m_renderer->submit(m_objects.back(), STATIC);

	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_7", 1.5f);
	m_objects.back()->BulletMakeStatic();
	placementX = static_cast<DestructibleObject*>(m_objects.back())->GetMax().x;
	placementX += placementX + 30;
	placementX *= -3;
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, -25.0f));
	m_renderer->submit(m_objects.back(), STATIC);




	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_10", 0.1f);
	Transform transform = m_objects.back()->GetObjectWorldTransform();
	glm::quat myquaternion = glm::quat(glm::vec3(0, glm::radians(-90.0f), 0));
	transform.rotation = transform.rotation * myquaternion;
	m_objects.back()->SetTransform(transform);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(100.0f, placementY, 0.0f));
	m_renderer->submit(m_objects.back(), STATIC);







	//
	placementX = 250.0f;
	placementZ = -150.0f;

	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);
	placementZ -= 35;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);


	//
	placementY = 90.0f;
	placementX = 0.0f;
	placementZ = -110.0f;
	float rot = 0.0f;

	// Middle up
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.3f);
	transform = m_objects.back()->GetObjectWorldTransform();
	myquaternion = glm::quat(glm::vec3(glm::radians(25.0f), glm::radians(rot), 0));
	transform.rotation = transform.rotation * myquaternion;
	m_objects.back()->SetTransform(transform);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	//// Left up
	placementX += 80.0f;
	placementZ += 20.0f;
	rot += -20.0f;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.3f);
	transform = m_objects.back()->GetObjectWorldTransform();
	myquaternion = glm::quat(glm::vec3(glm::radians(25.0f), glm::radians(rot), 0));
	transform.rotation = transform.rotation * myquaternion;
	m_objects.back()->SetTransform(transform);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementX += 80.0f;
	placementZ += 20.0f;
	rot += -20.0f;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.3f);
	transform = m_objects.back()->GetObjectWorldTransform();
	myquaternion = glm::quat(glm::vec3(glm::radians(25.0f), glm::radians(rot), 0));
	transform.rotation = transform.rotation * myquaternion;
	m_objects.back()->SetTransform(transform);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);


	//// Right up
	placementX = -80.0f;
	placementZ = -110.0f;
	placementZ += 20.0f;
	rot = 20.0f;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.3f);
	transform = m_objects.back()->GetObjectWorldTransform();
	myquaternion = glm::quat(glm::vec3(glm::radians(25.0f), glm::radians(rot), 0));
	transform.rotation = transform.rotation * myquaternion;
	m_objects.back()->SetTransform(transform);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementX -= 80.0f;
	placementZ += 20.0f;
	rot += 20.0f;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.3f);
	transform = m_objects.back()->GetObjectWorldTransform();
	myquaternion = glm::quat(glm::vec3(glm::radians(25.0f), glm::radians(rot), 0));
	transform.rotation = transform.rotation * myquaternion;
	m_objects.back()->SetTransform(transform);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);





	placementY = 50.0f;
	placementX = 0.0f;
	placementZ = 300;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic2("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.3f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementY = 50.0f;
	placementX = 100.0f;
	placementZ = 300;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic3("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 5.0f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);

	placementY = 85.0f;
	placementX = -100.0f;
	placementZ = 300;
	m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic4("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 2.0f);
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);


}
void DestructionTester::LoadShowcaseDestructibles()
{

	// Front object
	float placementY = 30.0f;
	float placementZ = -35;
	float placementX = 0;

	float addX = 120.0f;

	m_objects.emplace_back(new DestructibleObject(&m_debugFractureGen));
	static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_Script", 0.3f);
	m_objects.back()->SetName("Destructible_Script");
	m_objects.back()->BulletMakeStatic();
	m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	m_renderer->submit(m_objects.back(), STATIC);


	//placementX += addX;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	//m_renderer->submit(m_objects.back(), STATIC);


	//placementX += addX;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.25f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	//m_renderer->submit(m_objects.back(), STATIC);


	//placementX += addX;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, -25.0f));
	//m_renderer->submit(m_objects.back(), STATIC);

	//placementX += addX;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 1.0f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, -25.0f));
	//m_renderer->submit(m_objects.back(), STATIC);

	//placementX += addX;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 1.5f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, -25.0f));
	//m_renderer->submit(m_objects.back(), STATIC);


	////
	//placementX = 250.0f;
	//placementZ = -150.0f;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	//m_renderer->submit(m_objects.back(), STATIC);

	////
	//placementX = 350.0f;
	//placementZ = -150.0f;
	//m_objects.emplace_back(new DestructibleObject(&m_fractureGen));
	//static_cast<DestructibleObject*>(m_objects.back())->LoadBasic("Destructible_" + std::to_string(placementZ) + std::to_string(placementX), 0.1f);
	//m_objects.back()->BulletMakeStatic();
	//m_objects.back()->SetBodyWorldPosition(glm::vec3(placementX, placementY, placementZ));
	//m_renderer->submit(m_objects.back(), STATIC);

}

void DestructionTester::ShowMemoryInfo()
{
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

		std::cout << memTex << std::endl;
	}

	//fpsTimer += DeltaTime;
	//if (fpsTimer >= 1.0f)
	//{
	//	fpsTimer = 0.0f;
	//	logInfo("fps: " + std::to_string(Framerate));
	//}
}

void DestructionTester::DebugCreatePlane()
{
	std::vector<Vertex> vertices;
	std::vector<Face> faces;

	vertices.resize(4);
	vertices[0].position = glm::vec3(-400.0f, 0.0f, 400.0f);
	vertices[1].position = glm::vec3(-400.0f, 0.0f, -400.0f);
	vertices[2].position = glm::vec3(400.0f, 0.0f, -400.0f);
	vertices[3].position = glm::vec3(400.0f, 0.0f, 400.0f);

	vertices[0].Normals = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[1].Normals = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[2].Normals = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices[3].Normals = glm::vec3(0.0f, 1.0f, 0.0f);
	
	faces.resize(2);
	faces[0].indices[0] = 2;
	faces[0].indices[1] = 1;
	faces[0].indices[2] = 0;

	faces[1].indices[0] = 0;
	faces[1].indices[1] = 3;
	faces[1].indices[2] = 2;

	// Init object
	m_objects.emplace_back(new MapObject());
	m_objects.back()->InitMesh(vertices, faces, "GroundPlane");
	m_objects.back()->SetName("GroundPlane");

	// Init physics
	m_objects.back()->BulletMakeStatic();

	// Init renderer
	m_renderer->submit(m_objects.back(), STATIC);
}

bool DestructionTester::BulletCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	std::string name;
	std::string name2;

	// If more collision cases are added these need to be filtered and sorted better
	// Avoid casting wrong types, it will crash
	Player* ply = nullptr;
	GameObject* sp1 = nullptr;
	GameObject* sp2 = nullptr;
	if (obj1->getCollisionObject()->getUserIndex() == 545)
	{
		ply = static_cast<Player*>(obj1->getCollisionObject()->getUserPointer());
		sp2 = static_cast<GameObject*>(obj2->getCollisionObject()->getUserPointer());
		name = ply->getName();
	}
	else if (obj2->getCollisionObject()->getUserIndex() == 545)
	{
		sp1 = static_cast<GameObject*>(obj1->getCollisionObject()->getUserPointer());
		ply = static_cast<Player*>(obj2->getCollisionObject()->getUserPointer());
		name2 = ply->getName();
	}
	else
	{
		sp1 = static_cast<GameObject*>(obj1->getCollisionObject()->getUserPointer());
		sp2 = static_cast<GameObject*>(obj2->getCollisionObject()->getUserPointer());

		if (!sp1 || !sp2)
			return false;
	}

	DestructibleObject* dstrObj = nullptr;
	Player* playerObj = nullptr;

	btVector3 hitpoint;

	if (sp1)
	{
		switch (sp1->getType())
		{
		case (DESTRUCTIBLE):
			dstrObj = static_cast<DestructibleObject*>(sp1);
			hitpoint = cp.m_localPointA;
			break;

		}
	}

	if (sp2)
	{
		switch (sp2->getType())
		{
		case (DESTRUCTIBLE):
		{
			dstrObj = static_cast<DestructibleObject*>(sp2);
			hitpoint = cp.m_localPointB;
			break;
		}


		//if (dstrObj && spellObj)
		//{
		//	if (!dstrObj->is_destroyed())
		//	{
		//		DstrGenerator* m_dstr = dstrObj->getDstr();
		//		int seed = m_dstr->seedRand();

		//		m_dstr->Destroy(dstrObj, glm::vec2(hitpoint.getX(), hitpoint.getY()), spellObj->getDirection());

		//		// TODO: Fix this
		//		if (spellObj->getRigidBody() != nullptr)
		//		{
		//			float rndX = rand() % 2000 + 1 - 1000; rndX /= 1000;
		//			float rndY = rand() % 2000 + 1 - 1000; rndY /= 1000;
		//			float rndZ = rand() % 2000 + 1 - 1000; rndZ /= 1000;
		//			spellObj->getRigidBody()->setLinearVelocity(btVector3(rndX, rndY, rndZ) * 35);
		//		}

		//	}
		//}

		if (dstrObj)
		{
			if (obj1->getCollisionObject()->getCollisionShape()->getName() == "CapsuleZ")
				playerObj = static_cast<Player*>(obj1->getCollisionObject()->getUserPointer());
			else if (obj2->getCollisionObject()->getCollisionShape()->getName() == "CapsuleZ")
				playerObj = static_cast<Player*>(obj2->getCollisionObject()->getUserPointer());
			if (!playerObj)
				return false;

			//apply a force from the player to the object
			for (size_t i = 0; i < dstrObj->getMeshesCount(); i++)
			{
				btVector3 btRigPos = dstrObj->getRigidBody(i)->getCenterOfMassPosition();
				glm::vec3 glmPlayerPos = playerObj->getPlayerPos();
				btVector3 playerPos = btVector3(glmPlayerPos.x, glmPlayerPos.y, glmPlayerPos.z);

				btVector3 dir = btRigPos - playerPos;
				dir.normalize();
				dstrObj->getRigidBody(i)->applyCentralImpulse(dir * 5);
			}
		}

		// Switch end
		}
	}


	return false;
}
