#include "Pch/Pch.h"
#include "BulletPhysics.h"

enum {
	NormalObjects = 1,
	DestructableObjects
};

BulletPhysics* BulletPhysics::m_bpInstance = 0;

BulletPhysics::BulletPhysics(float gravity)
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, gravity, 0));
	m_ghostCallback = new btGhostPairCallback();

	m_dynamicsWorld->setDebugDrawer(&debugDrawer);
}

BulletPhysics::~BulletPhysics()
{
	m_dynamicsWorld->removeAction(m_character);
	//Remove the rigidbodies from the dynamics world and delete them
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject(obj);

		delete obj;
	}
	
	//Delete collsion shapes
	for (int i = 0; i < m_collisionShapes.size(); i++)
	{
		if (m_collisionShapes[i] == 0)
			continue;

		btCollisionShape* shape = m_collisionShapes[i];
		if (shape == nullptr)
			continue;

		m_collisionShapes[i] = 0;
		delete shape;
	}
	delete m_character;

	delete m_ghostCallback;
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_overlappingPairCache;
	delete m_dispatcher;
	delete m_collisionConfiguration;

	m_collisionShapes.clear();

}

BulletPhysics* BulletPhysics::GetInstance()
{
	if (m_bpInstance == 0) {
		m_bpInstance = new BulletPhysics(-19.82);
	}
	return m_bpInstance;
}

void BulletPhysics::destroy()
{
	delete m_bpInstance;
	m_bpInstance = nullptr;
}

btRigidBody* BulletPhysics::createObject(CollisionObject object, float inMass, glm::vec3 position, glm::vec3 extend, glm::quat rotation, bool destruction, float restitution, float friction)
{
	btCollisionShape* objectShape;
	switch (object)
	{
	case box:
		objectShape = new btBoxShape(btVector3(
			btScalar(extend.x),
			btScalar(extend.y),
			btScalar(extend.z)));
		break;

	case sphere:
		objectShape = new btSphereShape(btScalar(extend.x));
		break;

	case capsule:
		objectShape = new btCapsuleShape(extend.x, extend.y);
		break;

	default:
		break;
	}
	m_collisionShapes.push_back(objectShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	// if you want to rotate something 
	btQuaternion rot;
	rot.setX(rotation.x);
	rot.setY(rotation.y);
	rot.setZ(rotation.z);
	rot.setW(rotation.w);
	startTransform.setRotation(rot);
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));

	btScalar mass(inMass);
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(1.0f, 1.0f, 1.0f);
	//btVector3 localInertia(inLocalInertia.x, inLocalInertia.y, inLocalInertia.z);
	if (isDynamic)
		objectShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, objectShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//how much bounce and friction a object should have
	body->setRestitution(restitution);	
	body->setFriction(friction);
	body->setSpinningFriction(1.0f);

	//groups and mask is for collision, same mask and group to collide with that
	int myGoup = NormalObjects;
	int mask = NormalObjects | DestructableObjects | btBroadphaseProxy::CharacterFilter;

	if (destruction)
	{
		// Disable self collision here

		myGoup = DestructableObjects;
		mask = NormalObjects | btBroadphaseProxy::CharacterFilter;
		//mask = DestructableObjects;

		destructionobj(body);	
	}
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	m_dynamicsWorld->addRigidBody(body, myGoup, mask); //
	return body;
}

void BulletPhysics::setCharacterSize(glm::vec3 halfSize)
{
	m_boxSize = btVector3(halfSize.x, halfSize.y, halfSize.z);
}

btDiscreteDynamicsWorld* BulletPhysics::getDynamicsWorld() const
{
	return m_dynamicsWorld;
}

btVector3 BulletPhysics::getCharacterSize() const
{
	return m_boxSize;
}

btKinematicCharacterController* BulletPhysics::createCharacter(const glm::vec3& position)
{
	//create the character and add him to the dynamicsWorld
	btScalar capsuleX = m_boxSize.getX() * 0.8f;
	btScalar capsuleY = m_boxSize.getY() * 2.0f;
	btScalar capsuleZ = m_boxSize.getZ() * 0.9f;
	//height of capsule is	totalHeight = height + radius * 2
	//						height = totalHeight - radius * 2
	btScalar realY = (capsuleY) - (capsuleZ * 2.0f);
	m_playerShape = new btCapsuleShapeZ(capsuleZ, realY);

	btVector3 localInertia(1.0f, 1.0f, 1.0f);
	m_playerShape->calculateLocalInertia(1001.0f, localInertia);
	
	m_ghostObject = new btPairCachingGhostObject();
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));
	m_ghostObject->setWorldTransform(startTransform);

	m_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(m_ghostCallback);
	m_ghostObject->setCollisionShape(m_playerShape);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	m_character = new btKinematicCharacterController(m_ghostObject, m_playerShape, 1.0f, btVector3(0.0f, 1.0f, 0.0f));
	m_dynamicsWorld->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter | NormalObjects | DestructableObjects);


	m_character->setMaxSlope(btRadians(80.0));
	m_collisionShapes.push_back(m_playerShape);
	m_dynamicsWorld->addAction(m_character);
	m_character->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	m_character->setMaxPenetrationDepth(0.1f);

	return m_character;
}

void BulletPhysics::removeObject(btRigidBody* body)
{
	if (body)
	{
		if (body->getMotionState())
			delete body->getMotionState();
		m_collisionShapes.remove(body->getCollisionShape());

		if (body->getCollisionShape())
			delete body->getCollisionShape();
		m_dynamicsWorld->removeRigidBody(body);

		delete body;
		body = nullptr;
	}
}

void BulletPhysics::Update(float dt)
{
	//counter to make sure that the gravity starts after 60 frames
	if (m_counter > 15 && !m_setGravity)
	{
		if(m_character != nullptr)
			m_character->setGravity(btVector3(0.0f, -35.0f, 0.0f));
		m_setGravity = true;
	}
	if (!m_setGravity)
		m_counter++;

	// If it crashes here it's probably the callback function in playstate
	m_dynamicsWorld->stepSimulation(dt, 5, 1.0f/ 120.0f);
}

void BulletPhysics::destructionobj(btRigidBody* body)
{
	body->setRestitution(0.24f);
	body->setFriction(0.7f);
	body->setSpinningFriction(0.7f);
	body->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
	//body->setDamping(0.05f, 0.1f);	// Low (Space)
	body->setDamping(0.3f, 0.3f);	// High

}

