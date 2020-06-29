 #include "Pch/Pch.h"
#include "Player.h"

Player::Player(std::string name, glm::vec3 playerPosition, Camera *camera)
{
	// References
	m_playerCamera = camera;

	// Often moving values 
	m_playerPosition = playerPosition;
	m_name = name;
	m_directionVector = glm::vec3(0, 0, 0);
	m_moveDir = glm::vec3(0.0f);
	m_moveSpeed = 20.0f;


	from = btVector3(m_playerPosition.x, m_playerPosition.y, m_playerPosition.z);
	to = btVector3(m_playerPosition.x + 1.0f, m_playerPosition.y, m_playerPosition.z);
}

Player::~Player()
{
	
}

void Player::Update(float deltaTime)
{
	if (m_playerCamera->isCameraActive()) 
	{																		// IMPORTANT; DOING THESE WRONG WILL CAUSE INPUT LAG
		//Move(deltaTime);
		m_playerCamera->Update();											// Update this first so that subsequent uses are synced
		m_directionVector = glm::normalize(m_playerCamera->getCamFace());	// Update this first so that subsequent uses are synced
	}
}

void Player::Move(float deltaTime)
{
	m_moveDir = glm::vec3(0.0f);
	m_oldMoveDir = glm::vec3(0.0f);

	glm::vec3 lookDirection = m_directionVector;
	lookDirection.y = 0.0f;
	glm::vec3 lookRightVector = m_playerCamera->getCamRight();

	// Movement
	if (Input::isKeyHeldDown(GLFW_KEY_A))
		m_moveDir -= lookRightVector;
	if (Input::isKeyHeldDown(GLFW_KEY_D))
		m_moveDir += lookRightVector;
	if (Input::isKeyHeldDown(GLFW_KEY_W))
		m_moveDir += lookDirection;
	if (Input::isKeyHeldDown(GLFW_KEY_S))
		m_moveDir -= lookDirection;
	if (Input::isKeyHeldDown(GLFW_KEY_SPACE))
		m_moveDir.y += 1.0f;
	if (Input::isKeyHeldDown(GLFW_KEY_C) || Input::isKeyHeldDown(GLFW_KEY_LEFT_CONTROL))
		m_moveDir.y -= 1.0f;

	// Make sure moving is a constant speed
	if (glm::length(m_moveDir) >= 0.0001f)
		m_moveDir = glm::normalize(m_moveDir);

	m_playerPosition += m_moveDir * m_moveSpeed * deltaTime;

	// Set cameraPos
	m_cameraPosition = m_playerPosition;

	m_playerCamera->setCameraPos(m_cameraPosition);
}

bool Player::RayCast(glm::vec3 position, glm::vec3 direction, btCollisionWorld::ClosestRayResultCallback& closestResult, btCollisionWorld::AllHitsRayResultCallback& allResults)
{
	from = btVector3(position.x, position.y - 0.1f, position.z);
	direction *= 1000.0f;
	to = btVector3(position.x + direction.x, position.y + direction.y, position.z + direction.z);
	
	if (btDiscreteDynamicsWorld* bt_world = BulletPhysics::GetInstance()->getDynamicsWorld())
	{
		//bt_world->getDebugDrawer()->drawLine(from, to, btVector4(0, 0, 1, 1));

		// First hit
		closestResult = btCollisionWorld::ClosestRayResultCallback(from, to);
		bt_world->rayTest(from, to, closestResult);
		if (closestResult.hasHit())
		{
			btVector3 p = from.lerp(to, closestResult.m_closestHitFraction);
			//bt_world->getDebugDrawer()->drawLine(p, p + closestResults.m_hitNormalWorld, btVector4(0, 1, 0, 1));
		}

		// All hits
		allResults = btCollisionWorld::AllHitsRayResultCallback(from, to);
		bt_world->rayTest(from, to, allResults);
		for (int i = 0; i < allResults.m_hitFractions.size(); i++)
		{
			btVector3 p = from.lerp(to, allResults.m_hitFractions[i]);
			//if (p != from.lerp(to, closestResults.m_closestHitFraction))
			//	bt_world->getDebugDrawer()->drawLine(p, p + allResults.m_hitNormalWorld[i], btVector4(1, 0, 0, 1));
		}

		if (closestResult.hasHit())
			return true;
	}
	return false;
}

void Player::setPlayerPos(glm::vec3 pos)
{
	m_playerPosition = pos;
}

const glm::vec3& Player::getPlayerPos() const
{
	return m_playerPosition;
}

Camera* Player::getCamera()
{
	return m_playerCamera;
}

const std::string& Player::getName() const
{
	return m_name;
}
