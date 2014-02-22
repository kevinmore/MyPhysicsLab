#include "DemoKeeper.h"


DemoKeeper::DemoKeeper(LabManager* labMan, Ogre::SceneManager* sm, hkpWorld* world)
{
	mLabMgr = labMan;
	mSceneMgr = sm;
	mWorld = world;
	resetAll();
}


DemoKeeper::~DemoKeeper(void)
{
}

void DemoKeeper::resetAll( void )
{
	Keyframe_demoRunning = false;
	binaryaction_demoRunning = false;
	mWorld->markForWrite();
	mWorld->setGravity(hkVector4(0.f,-9.81f,0.f));
	mWorld->unmarkForWrite();
}

bool DemoKeeper::demoLoop( const Ogre::FrameEvent& evt )
{
	mWorld->markForWrite();

	//
	//running demo loop accordingly
	//
	
	if (Keyframe_demoRunning)
	{
		hkVector4 pos;
		hkQuaternion rot;
		getKeyframePositionAndRotation(m_time + evt.timeSinceLastFrame, pos, rot);

		hkpKeyFrameUtility::applyHardKeyFrame(pos, rot, 1.0f /evt.timeSinceLastFrame, m_keyframedBody);
		m_time += evt.timeSinceLastFrame;
	}
	else if (binaryaction_demoRunning)
	{
		hkVector4 pos1 = m_boxRigidBody1->getPosition();
		hkVector4 pos2 = m_boxRigidBody2->getPosition();
		hkVector4 lenVec;
		lenVec.setSub4(pos1, pos2);

		manual_spring = mSceneMgr->createManualObject();
		manual_spring->setQueryFlags(0);
		manual_spring->begin( "BaseWhiteNoLighting",   Ogre::RenderOperation::OT_LINE_LIST);
		manual_spring->position(pos1(0),pos1(1),pos1(2));
		if (lenVec.length3() > m_springAction->getRestLength())
		{
			// Line is red if the spring is stretched.
			manual_spring->colour(1,0,0); 
		}
		else
		{
			// Line is blue is spring is at rest or compressed.
			manual_spring->colour(0,0,1); 
	
		}
		manual_spring->position(pos2(0),pos2(1),pos2(2));
		manual_spring->end();
		
		springNode->detachAllObjects();
		springNode->attachObject(manual_spring);
	}
		
	mWorld->unmarkForWrite();
	return true;
}

void DemoKeeper::createBrickWall( int height, int length, const hkVector4& position, hkReal gapWidth, hkVector4Parameter halfExtents )
{
	hkVector4 posx = position;
	hkVector4 boxSize(1,1,1);
	hkpBoxShape* box = new hkpBoxShape(boxSize,0);

	// do a raycast to place the wall
	// 	{
	// 		hkpWorldRayCastInput ray;
	// 		ray.m_from = posx;
	// 		ray.m_to = posx;
	// 
	// 		ray.m_from(1) += 20.0f;
	// 		ray.m_to(1)   -= 20.0f;
	// 
	// 		hkpWorldRayCastOutput result;
	// 		mWorld->markForWrite();
	// 		mWorld->castRay( ray, result );
	// 		
	// 		posx.setInterpolate4( ray.m_from, ray.m_to, result.m_hitFraction );
	// 		mWorld->unmarkForWrite();
	// 	}
	// move the start point
	posx(0) -= ( gapWidth + 2.0f * halfExtents(0) ) * length * 0.5f;
	posx(1) -= halfExtents(1) + box->getRadius();

	hkArray<hkpEntity*> entitiesToAdd;

	for ( int x = 0; x < length; x ++ )		// along the ground
	{
		hkVector4 pos = posx;
		for( int ii = 0; ii < height; ii++ )
		{
			pos(1) += (halfExtents(1) + box->getRadius())* 2.0f;

			hkpRigidBodyCinfo boxInfo;
			boxInfo.m_mass = 10.0f;
			hkMassProperties massProperties;
			hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, boxInfo.m_mass, massProperties);

			boxInfo.m_mass = massProperties.m_mass;
			boxInfo.m_centerOfMass = massProperties.m_centerOfMass;
			boxInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			boxInfo.m_solverDeactivation = boxInfo.SOLVER_DEACTIVATION_MEDIUM;
			boxInfo.m_shape = box;
			//boxInfo.m_qualityType = HK_COLLIDABLE_QUALITY_DEBRIS;
			boxInfo.m_restitution = 0.0f;

			boxInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;

			{
				boxInfo.m_position = pos;
				hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);



				//render it with Ogre
				Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
				boxNode->scale(2,2,2);
				Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
				mLabMgr->setColor(ent, Ogre::Vector3(0.5,0.4,0.6));
				HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, boxRigidBody,mWorld);
				boxNode->attachObject(ent);

				mLabMgr->registerEnity(boxNode, boxRigidBody);
				boxRigidBody->setQualityType(HK_COLLIDABLE_QUALITY_BULLET);
				
				mWorld->addEntity( boxRigidBody );
				boxRigidBody->removeReference();
			
			}

			pos(1) += (halfExtents(1) + box->getRadius())* 2.0f;
			pos(0) += halfExtents(0) * 0.6f;
			{
				boxInfo.m_position = pos;
				hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);


				//render it with Ogre
				Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
				boxNode->scale(2,2,2);
				Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
				mLabMgr->setColor(ent, Ogre::Vector3(0.5,0.4,0.6));
				HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, boxRigidBody,mWorld);
				boxNode->attachObject(ent);

				mLabMgr->registerEnity( boxNode, boxRigidBody);
				boxRigidBody->setQualityType(HK_COLLIDABLE_QUALITY_BULLET);
				
				mWorld->addEntity( boxRigidBody );
				boxRigidBody->removeReference();
				

			}
			pos(0) -= halfExtents(0) * 0.6f;
		}
		posx(0) += halfExtents(0)* 2.0f + gapWidth;
	}

}

void DemoKeeper::BrickWallDemo( void )
{
	mWorld->markForWrite();

	//
	// Create the walls
	//

	hkVector4 groundPos( 0.0f, 0.0f, 0.0f );
	hkVector4 boxSize( 1.0f, 1.0f, 1.0f);
	hkReal deltaZ = 25.0f;
	groundPos(2) = -deltaZ * 10 * 0.5f;

	for ( int y = 0; y < 4; y ++ )			// first wall
	{
		createBrickWall( 4, 8, groundPos, 0.2f, boxSize );
		groundPos(2) += deltaZ;
	}

	mWorld->unmarkForWrite();
}

void DemoKeeper::FrictionDemo( void )
{
	mWorld->markForWrite();
	// In this demo we have one fixed object and 10 dynamic objects, all of which are hkBoxShapes. But before we create
	// them we will outline the parameters used in this demo:

	//
	// Illustrative parameters for the demo
	//

	const int numObjects = 10;					// the number of moving boxes
	const hkReal objectStep = 1.2f;				// a notional 'width' of a moving box pair
	const hkReal initialFriction = 0.0f;
	const hkReal frictionRange = 1.0f;				// friction ranges from initialFriction to frictionRange
	const hkVector4 axis(0.0f,0.0f,1.0f);
	const hkReal angle = -HK_REAL_PI / 10.0f;
	const hkQuaternion boxOrientation(axis, angle);		// Orientation
	const hkReal fixedBoxHalfHeight = 1.0f;
	const hkReal movingBoxHalfHeight = 0.3f;
	const hkReal movingBoxInitialVerticalDisplacement = 0.25f;


	//
	// Derived parameters for the demo
	//

	const hkReal frictionStep = frictionRange / numObjects;	// friction ranges in steps of frictionStep
	const hkReal positionAlongZ = - (objectStep * numObjects) / 2.0f;
	const hkVector4 fixedBoxSize((objectStep * numObjects) / 2.0f + 2.0f, fixedBoxHalfHeight, (objectStep * numObjects) / 2.0f + 2.0f);
	const hkVector4 boxSize(0.5f, movingBoxHalfHeight, 0.5f);

	// Since we've rotated everything, the height above the fixed box is proportional to the 1/cosine of the angle we rotated by.
	const hkReal movingBoxCentreHeight = (fixedBoxHalfHeight + movingBoxHalfHeight + movingBoxInitialVerticalDisplacement) / hkMath::cos(angle);


	// As you can see these parameters are used to calculate the position, orientation and frictional values
	// that are assigned to each of the boxes. By scaling the frictional values between 0.0 and 0.9 you
	// can see a range of responses from the different boxes as they drop on the surface of the large sloped box.

	// The creation of the various rigid bodies in the demo is fairly straight forward and follows the usual scheme
	// of filling out the various members of the hkpRigidBodyCinfo structure, asking Havok to compute
	// the 'mass properties' for the body and finally adding it to the world. The only slight difference this time
	// is that each of the dynamic boxes receives a different value for 'm_friction':

	//
	// Box shapes
	//
	hkpBoxShape* fixedBoxShape = new hkpBoxShape( fixedBoxSize , 0 );
	hkpBoxShape* boxShape = new hkpBoxShape( boxSize , 0 );

	//
	// Physical parameters for the demo
	//
	hkReal fixedBoxFriction = 0.25f;
	hkReal boxMass = 1.0f;

	//
	// Create the fixed box
	//
	{
		// Position of the box
		hkVector4 boxPosition(0.0f, 5.0f, 0.0f);

		hkpRigidBodyCinfo info;
		{
			info.m_motionType = hkpMotion::MOTION_FIXED;
			info.m_friction = fixedBoxFriction;

			info.m_shape = fixedBoxShape;
			info.m_position = boxPosition;
			info.m_rotation = boxOrientation;
		}

		hkpRigidBody* box = new hkpRigidBody(info);
		mWorld->addEntity(box);
		box->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale and rotate
		boxNode->scale(((objectStep * numObjects) / 2.0f + 2.0f)*2,  fixedBoxHalfHeight*2,((objectStep * numObjects) / 2.0f + 2.0f)*2);
		Ogre::Quaternion q = HkOgre::Convert::hkQuatToOgre(boxOrientation);
		boxNode->rotate(q);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, box,mWorld);
		boxNode->attachObject(ent);
		
		//register to lab manager
		mLabMgr->registerEnity( boxNode, box);

	}

	//
	// Create the moving boxes
	//

	for(int i = 0; i < numObjects; i++)
	{
		
		// Compute the box inertia tensor
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(boxSize, boxMass, massProperties);

		// Set up the construction info parameters for the box
		hkpRigidBodyCinfo info;

		// Each box has a different friction ranging from 0.0 to 0.9
		info.m_friction = initialFriction + frictionStep * i;	

		info.m_shape = boxShape;
		info.m_mass  = boxMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_motionType = hkpMotion::MOTION_BOX_INERTIA;

		// Position of the box
		hkReal zpos = positionAlongZ + objectStep * i;

		info.m_position = hkVector4(0.0f, movingBoxCentreHeight+6.5, zpos);
		info.m_rotation = boxOrientation;

		// Create a box
		hkpRigidBody* box = new hkpRigidBody(info);
		mWorld->addEntity(box);
		box->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale
		boxNode->scale(0.5f*2, movingBoxHalfHeight*2, 0.5f*2);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.1356,0.3242,0.5403));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, box,mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, box);
		

	}

	//
	// Remove references from shapes
	//
	fixedBoxShape->removeReference();
	boxShape->removeReference();

	mWorld->unmarkForWrite();
}

void DemoKeeper::GravityChangeDemo( void )
{
	mWorld->markForWrite();


	// Illustrative parameters for the demo
	unsigned int numObjects = 10; // the number of box/sphere pairs
	hkReal objectStep = 1.2f; // a notional 'width' of a box/sphere pair


	// Derived parameters for the demo
	hkReal positionAlongX = - (objectStep * numObjects) / 2.0f;

	// Geometric parameters for the demo
	hkVector4 boxSize(0.5f, 0.1f, 0.5f);
	hkReal sphereRadius = 0.5f;
	hkpBoxShape* boxShape = new hkpBoxShape( boxSize , 0 );
	hkpSphereShape* sphereShape = new hkpSphereShape( sphereRadius );

	// Physical parameters for the demo
	hkReal sphereMass = 1.0f;

	// Create the objects
	for ( unsigned int i = 0; i <= numObjects; i++ )
	{
		{
			// Position of the box
			hkVector4 boxPosition(positionAlongX, 3.0f, 0.0f);

			// Set up the construction info parameters for the box
			hkpRigidBodyCinfo info;
			info.m_motionType = hkpMotion::MOTION_FIXED;
			info.m_restitution = 0.0f;
			info.m_shape = boxShape;
			info.m_position = boxPosition;

			// Create fixed box
			hkpRigidBody* box = new hkpRigidBody(info);
			mWorld->addEntity(box);
			box->removeReference();


			//render it with Ogre
			Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

			//scale
			boxNode->scale(0.5f, 0.1f, 0.5f);

			//display and sync
			Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
			mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
			HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, box,mWorld);
			boxNode->attachObject(ent);

			//register to lab manager
			mLabMgr->registerEnity( boxNode, box);
		}

		{

			// Compute the sphere inertia tensor
			hkMassProperties massProperties;
			hkpInertiaTensorComputer::computeSphereVolumeMassProperties(sphereRadius, sphereMass, massProperties);

			// Set up the construction info parameters for the sphere
			hkpRigidBodyCinfo info;
			info.m_restitution = 0.0f;
			info.m_mass = sphereMass;
			info.m_shape = sphereShape;
			info.m_inertiaTensor = massProperties.m_inertiaTensor;
			info.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
			info.m_position.set(positionAlongX, 8.0f, 0.0f);

			info.m_gravityFactor = -0.9f + 0.2f * i;

			// Create movable sphere
			hkpRigidBody* sphere = new hkpRigidBody(info);
			mWorld->addEntity(sphere);
			sphere->removeReference();


			//render it with Ogre
			Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

			//scale
			sphereNode->scale(0.5f, 0.5f, 0.5f);

			//display and sync
			Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defSphere.mesh");
			mLabMgr->setColor(ent, Ogre::Vector3(0.5538,0.3187,0.1275));
			HkOgre::Renderable* rend = new HkOgre::Renderable(sphereNode, sphere,mWorld);
			sphereNode->attachObject(ent);

			//register to lab manager
			mLabMgr->registerEnity( sphereNode, sphere);
		}

		positionAlongX += objectStep;
	}

	// Clean up
	boxShape->removeReference();
	sphereShape->removeReference();


	mWorld->unmarkForWrite();
}

void DemoKeeper::RestitutionDemo( void )
{
	mWorld->markForWrite();


	// Illustrative parameters for the demo
	unsigned int numObjects = 10; // the number of box/sphere pairs
	hkReal objectStep = 1.2f; // a notional 'width' of a box/sphere pair
	hkReal initialRestitution = 0.0f;
	hkReal restitutionRange = 1.0f; // sphere restitution ranges from initialRestitution to restitutionRange

	// Derived parameters for the demo
	hkReal restitutionStep = restitutionRange / numObjects; // restitution ranges in steps of restitutionStep
	hkReal positionAlongX = - (objectStep * numObjects) / 2.0f;

	// Geometric parameters for the demo
	hkVector4 boxSize(0.5f, 0.1f, 0.5f);
	hkReal sphereRadius = 0.5f;
	hkpBoxShape* boxShape = new hkpBoxShape( boxSize , 0 );
	hkpSphereShape* sphereShape = new hkpSphereShape( sphereRadius );

	// Physical parameters for the demo
	hkReal boxRestitution = 1.0f;
	hkReal sphereMass = 1.0f;

	// Create the objects
	for(unsigned int i = 0; i <= numObjects; i++)
	{
		{
			// Position of the box
			hkVector4 boxPosition(positionAlongX, 5.0f, 0.0f);

			// Set up the construction info parameters for the box
			hkpRigidBodyCinfo info;
			info.m_motionType = hkpMotion::MOTION_FIXED;

			//
			// NOTE: The current implementation of restitution uses a very simple approximation, and
			// may not produce physically accurate results. A more accurate version will 
			// implemented in later versions of the SDK.
			//
			info.m_restitution = boxRestitution;
			info.m_shape = boxShape;
			info.m_position = boxPosition;

			// Create fixed box
			hkpRigidBody* box = new hkpRigidBody(info);
			mWorld->addEntity(box);
			box->removeReference();

			//render it with Ogre
			Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

			//scale
			boxNode->scale(0.5f, 0.1f, 0.5f);

			//display and sync
			Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
			mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
			HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, box,mWorld);
			boxNode->attachObject(ent);

			//register to lab manager
			mLabMgr->registerEnity( boxNode, box);
		}

		{

			// Compute the sphere inertia tensor
			hkMassProperties massProperties;
			hkpInertiaTensorComputer::computeSphereVolumeMassProperties(sphereRadius, sphereMass, massProperties);

			// Set up the construction info parameters for the sphere
			hkpRigidBodyCinfo info;

			//
			// NOTE: The current implementation of restitution uses a very simple approximation, and
			// may not produce physically accurate results. A more accurate version will 
			// implemented in later versions of the SDK.
			//
			info.m_restitution = initialRestitution + restitutionStep * i;
			info.m_mass = sphereMass;
			info.m_shape = sphereShape;
			info.m_inertiaTensor = massProperties.m_inertiaTensor;
			info.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
			info.m_position.set(positionAlongX, 12.0f, 0.0f);

			// Create movable sphere
			hkpRigidBody* sphere = new hkpRigidBody(info);
			mWorld->addEntity(sphere);
			sphere->removeReference();


			//render it with Ogre
			Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

			//scale
			sphereNode->scale(0.5f, 0.5f, 0.5f);

			//display and sync
			Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defSphere.mesh");
			mLabMgr->setColor(ent, Ogre::Vector3(0.5538,0.3187,0.1275));
			HkOgre::Renderable* rend = new HkOgre::Renderable(sphereNode, sphere,mWorld);
			sphereNode->attachObject(ent);

			//register to lab manager
			mLabMgr->registerEnity( sphereNode, sphere);
		}

		positionAlongX += objectStep;
	}

	// Clean up
	boxShape->removeReference();
	sphereShape->removeReference();


	mWorld->unmarkForWrite();
}

void DemoKeeper::BinaryActionDemo( void )
{


	springNode=mSceneMgr->getRootSceneNode()->createChildSceneNode();

	mWorld->markForWrite();

	//
	// Create Rigid Body.
	//
	{
		/// Here we construct a simple cube with sides 2 units and mass 1.
		hkVector4 boxHalfExtents( 1.0f, 1.0f, 1.0f );  
		hkpBoxShape* geom = new hkpBoxShape( boxHalfExtents , 0 );

		const hkReal mass = 1.0f;
		hkVector4 pos( -2.0f, 5.0f, 0.0f );

		hkpRigidBodyCinfo boxInfo;
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(boxHalfExtents, mass, massProperties);
		boxInfo.m_mass = massProperties.m_mass;
		boxInfo.m_centerOfMass = massProperties.m_centerOfMass;
		boxInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
		boxInfo.m_shape = geom;
		boxInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
		boxInfo.m_position = pos;
		boxInfo.m_gravityFactor = 0;

		m_boxRigidBody1 = new hkpRigidBody(boxInfo);

		// As the rigid bodies now references our shape, we no longer need to.
		geom->removeReference();

		// and add it to the world.
		mWorld->addEntity( m_boxRigidBody1 );

		//render it with Ogre
		Ogre::SceneNode* boxNode1 = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale
		boxNode1->scale(2, 2, 2);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.5538,0.3187,0.1275));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode1, m_boxRigidBody1,mWorld);
		boxNode1->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode1, m_boxRigidBody1);
	}

	//
	// Create Rigid Body.
	//
	{
		// Here we construct a simple cube with sides 2 units and mass 1.
		hkVector4 boxHalfExtents( 1.0f, 1.0f, 1.0f );  
		hkpBoxShape* geom = new hkpBoxShape( boxHalfExtents , 0 );

		const hkReal mass = 1.0f;
		hkVector4 pos( 2.0f, 5.0f, 0.0f );

		hkpRigidBodyCinfo boxInfo;
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(boxHalfExtents, mass, massProperties);
		boxInfo.m_mass = massProperties.m_mass;
		boxInfo.m_centerOfMass = massProperties.m_centerOfMass;
		boxInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
		boxInfo.m_shape = geom;
		boxInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
		boxInfo.m_position = pos;
		boxInfo.m_gravityFactor = 0;

		m_boxRigidBody2 = new hkpRigidBody(boxInfo);

		// As the rigid bodies now references our shape, we no longer need to.
		geom->removeReference();

		// and add it to the world.
		mWorld->addEntity( m_boxRigidBody2 );


		//render it with Ogre
		Ogre::SceneNode* boxNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale
		boxNode2->scale(2, 2, 2);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.5538,0.3187,0.1275));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode2, m_boxRigidBody2,mWorld);
		boxNode2->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode2, m_boxRigidBody2);
	}

	//
	// Create the action and add it to the world.
	//
	{
		// hkpSpringAction applies forces to keep the two hkRigidBodies restLength apart
		// hkpSpringAction is defined in 'hkutilities/actions/spring/hkpSpringAction.h'.
		m_springAction = new hkpSpringAction(m_boxRigidBody1, m_boxRigidBody2);
		m_springAction->setPositionsInBodySpace( m_boxRigidBody1->getCenterOfMassLocal(), m_boxRigidBody2->getCenterOfMassLocal());
		m_springAction->setRestLength(6.0f);
		m_springAction->setDamping(0.3f);
		m_springAction->setStrength(10.0f);

		// Add springAction to the world. From now on springAction will automatically
		// be applied to its bodies during integration so long as the bodies are active.
		// If the bodies become inactive, antiGravityAction is not applied. If they
		// reactivate, springAction again starts applying to them.
		// springAction can also be removed using hkpWorld::removeAction(springAction)
		// after which it is no longer included in simulation.

		mWorld->addAction( m_springAction );
	}

	mWorld->unmarkForWrite();

	binaryaction_demoRunning = true;
}

void DemoKeeper::KeyframingDemo( void )
{
	mWorld->markForWrite();
	//
	// Set parameters for keyframes, and number of bodies
	//
	m_speed = 0.2f;
	m_radius = 5.0f;
	m_numBodies = 15;
	m_time = 0.0f;

	//
	// Create the rigid bodies.
	//
	createBodies();

	//
	// Create the ground.
	//
	createGround();

	//
	// Create the keyframed body.
	//
	{
		const hkVector4 halfExtents(3.0f, 1.75f, 0.3f);
		hkpBoxShape* shape = new hkpBoxShape(halfExtents, 0 );

		// Assign the rigid body properties
		hkpRigidBodyCinfo bodyInfo;
		bodyInfo.m_shape = shape;
		// By setting the motion type to hkpMotion::MOTION_KEYFRAMED we are essentially telling Havok that this
		// body has infinite mass. Thus neither applying forces nor impulses can change the velocity of the body and
		// as a result, the body is considered totally immovable during interactions (collisions) with other bodies.
		// This means that the user can force the body to follow a set of keyframes by directly setting the
		// velocity required to move to the next keyframe before each world step.
		// The body is guaranteed to reach the next keyframe, even if other bodies collide with it.
		// To help convert keyframes to velocities we have provided some useful methods in hkpKeyFrameUtility.
		// To create a keyframed object simply set the motion type as follows:

		bodyInfo.m_motionType = hkpMotion::MOTION_KEYFRAMED;

		{
			// Get inital keyframe.
			getKeyframePositionAndRotation(0.0f, bodyInfo.m_position, bodyInfo.m_rotation);
		}


		// Add our keyframed body.
		m_keyframedBody = new hkpRigidBody(bodyInfo);
		mWorld->addEntity(m_keyframedBody);
		m_keyframedBody->removeReference();

		shape->removeReference();


		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale
		boxNode->scale(6, 3.5, 0.6);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(rand()/(double)RAND_MAX,rand()/(double)RAND_MAX,rand()/(double)RAND_MAX));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, m_keyframedBody,mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, m_keyframedBody);
	}



	mWorld->unmarkForWrite();

	Keyframe_demoRunning = true;

}

void DemoKeeper::createBodies( void )
{
	// Each body needs at least one shape
	const hkVector4 halfExtents(1.0f, 0.75f, 1.0f);
	hkpShape* shape = new hkpBoxShape(halfExtents, 0 );

	// Compute the inertia tensor from the shape
	hkMassProperties massProperties;
	hkpInertiaTensorComputer::computeShapeVolumeMassProperties(shape, 5.0f, massProperties);

	// Assign the rigid body properties
	hkpRigidBodyCinfo bodyInfo;
	bodyInfo.m_mass = massProperties.m_mass;
	bodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
	bodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	bodyInfo.m_shape = shape;
	bodyInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;


	bodyInfo.m_friction = 0.1f;

	for(int i = 0; i < m_numBodies; i++)
	{
		bodyInfo.m_position.set( hkMath::cos(hkReal(i)) - 2.0f, i * 2.0f + 10, hkMath::sin(hkReal(i)));
		hkpRigidBody* body = new hkpRigidBody(bodyInfo);
		mWorld->addEntity(body);
		body->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale
		boxNode->scale(2, 1.5, 2);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(rand()/(double)RAND_MAX,rand()/(double)RAND_MAX,rand()/(double)RAND_MAX));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, body,mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, body);
	}	
	shape->removeReference();
}

void DemoKeeper::getKeyframePositionAndRotation( hkReal t, hkVector4& pos, hkQuaternion& rot )
{
	t *= 2 * HK_REAL_PI * m_speed;
	pos.set(m_radius * hkMath::cos(t), 10-2.0f, -m_radius * hkMath::sin(t));

	hkVector4 axis(0,1,0);
	rot.setAxisAngle(axis, t);
}

void DemoKeeper::createGround( void )
{
	const hkVector4 halfExtents(12.0f, 1.0f, 12.0f);
	hkpShape* groundShape = new hkpBoxShape(halfExtents, 0 );

	hkpRigidBodyCinfo bodyInfo;
	bodyInfo.m_mass = 0.0f;	
	bodyInfo.m_shape = groundShape;
	bodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	bodyInfo.m_position.set(0.0f, 10.0f, 0.0f);
	hkVector4 axis(1.0f, 0.0f, 0.0f);


	{
		axis.set(1,0,1);
		axis.normalize3();
		bodyInfo.m_rotation = hkQuaternion(axis, 0.5f);
		bodyInfo.m_position.set(10.0f, 10.0f, -10.0f);
		hkpRigidBody* groundBody = new hkpRigidBody(bodyInfo);

		mWorld->addEntity(groundBody);
		groundBody->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale and rotate
		boxNode->scale(24, 2, 24);
		Ogre::Quaternion q = HkOgre::Convert::hkQuatToOgre(bodyInfo.m_rotation);
		boxNode->rotate(q);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, groundBody, mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, groundBody);
	}

	{
		axis.set(-1,0,1);
		axis.normalize3();
		bodyInfo.m_rotation = hkQuaternion(axis, 0.5f);
		bodyInfo.m_position.set(10.0f, 10.0f, 10.0f);
		hkpRigidBody* groundBody = new hkpRigidBody(bodyInfo);

		mWorld->addEntity(groundBody);
		groundBody->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale and rotate
		boxNode->scale(24, 2, 24);
		Ogre::Quaternion q = HkOgre::Convert::hkQuatToOgre(bodyInfo.m_rotation);
		boxNode->rotate(q);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, groundBody, mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, groundBody);
	}

	{
		axis.set(1,0,-1);
		axis.normalize3();
		bodyInfo.m_rotation = hkQuaternion(axis, 0.5f);
		bodyInfo.m_position.set(-10.0f, 10.0f, -10.0f);
		hkpRigidBody* groundBody = new hkpRigidBody(bodyInfo);

		mWorld->addEntity(groundBody);
		groundBody->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale and rotate
		boxNode->scale(24, 2, 24);
		Ogre::Quaternion q = HkOgre::Convert::hkQuatToOgre(bodyInfo.m_rotation);
		boxNode->rotate(q);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, groundBody, mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, groundBody);
	}


	{
		axis.set(-1,0,-1);
		axis.normalize3();
		bodyInfo.m_rotation = hkQuaternion(axis, 0.5f);
		bodyInfo.m_position.set(-10.0f, 10.0f, 10.0f);
		hkpRigidBody* groundBody = new hkpRigidBody(bodyInfo);

		mWorld->addEntity(groundBody);
		groundBody->removeReference();

		//render it with Ogre
		Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale and rotate
		boxNode->scale(24, 2, 24);
		Ogre::Quaternion q = HkOgre::Convert::hkQuatToOgre(bodyInfo.m_rotation);
		boxNode->rotate(q);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.3047,0.3047,0.3047));
		HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, groundBody, mWorld);
		boxNode->attachObject(ent);

		//register to lab manager
		mLabMgr->registerEnity( boxNode, groundBody);
	}

	groundShape->removeReference();
}

void DemoKeeper::GravityActionDemo( void )
{
	mWorld->markForWrite();

	// Define world gravity as zero.
	hkVector4 gravity( 0.0f, 0.0f, 0.0f );
	mWorld->setGravity(gravity);

	//
	// Create the "planet" shape to orbit around.
	//
	hkpRigidBody* planet = HK_NULL;
	const hkVector4 gravityCenter(0.0f, 15.0f, 0.0f);
	{
		hkReal radius = 5.0f;
		hkpSphereShape* sphereShape = new hkpSphereShape(radius);

		hkpRigidBodyCinfo sphereInfo;

		sphereInfo.m_shape = sphereShape;
		sphereInfo.m_position.set(gravityCenter(0), gravityCenter(1), gravityCenter(2));
		sphereInfo.m_angularDamping = 0.0f;
		sphereInfo.m_linearDamping = 0.0f;
		sphereInfo.m_friction = 0.0f;

		// Planet is fixed.
		sphereInfo.m_motionType = hkpMotion::MOTION_FIXED;

		// Create a rigid body (using the template above).
		planet = new hkpRigidBody(sphereInfo);

		// Remove reference since the hkpRigidBody now "owns" the Shape.
		sphereShape->removeReference();

		// Add rigid body so we can see it, and remove reference since the hkpWorld now "owns" it.
		mWorld->addEntity(planet);


		//render it with Ogre
		Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		//scale
		sphereNode->scale(5.f, 5.f, 5.f);

		//display and sync
		Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defSphere.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.5538,0.3187,0.1275));
		HkOgre::Renderable* rend = new HkOgre::Renderable(sphereNode, planet, mWorld);
		sphereNode->attachObject(ent);
		
		//register to lab manager
		mLabMgr->registerEnity( sphereNode, planet);

	}

	// Limit number of polygons as Havok renderer is not perfect!
	const int numBodies = 100;

	//
	// Create the satellites.
	//
	{
		hkPseudoRandomGenerator generator(234);
		for(int i = 0; i < numBodies; i++)
		{
			hkVector4 halfExtents(0.25f, 0.25f, 0.25f);
			hkpBoxShape* satelliteShape = new hkpBoxShape(halfExtents, 0 );

			hkpRigidBodyCinfo satelliteInfo;

			satelliteInfo.m_shape = satelliteShape;
			satelliteInfo.m_position.set(   generator.getRandRange( 7.0f, 25.0f ),
				generator.getRandRange( 7.0f+15, 25.0f+15 ),
				generator.getRandRange( 7.0f, 25.0f ) );

			satelliteInfo.m_angularDamping = 0.0f;
			satelliteInfo.m_linearDamping = 0.0f;
			satelliteInfo.m_friction = 0.0f;

			satelliteInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;

			hkReal mass = 10.0f;
			hkMassProperties massProperties;
			hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, mass, massProperties);

			satelliteInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			satelliteInfo.m_centerOfMass = massProperties.m_centerOfMass;
			satelliteInfo.m_mass = massProperties.m_mass;			

			// Create a rigid body (using the template above).
			hkpRigidBody* satellite = new hkpRigidBody(satelliteInfo);

			// Give each hkpRigidBody an initial velocity so they orbit in the same direction.
			hkVector4 lv(0.0f, -5.0f, 0.0f);
			satellite->setLinearVelocity( lv );

			// Remove reference since the body now "owns" the Shape
			satelliteShape->removeReference();

			// Finally add body so we can see it, and remove reference since the world now "owns" it.
			mWorld->addEntity(satellite);

			const hkReal gravityConstant = 1000.0f;

			GravityAction* gravityAction = new GravityAction( satellite, gravityCenter, gravityConstant );


			mWorld->addAction( gravityAction );

			// After addAction() mWorld references antiGravityAction so the local reference can safely be removed.
			gravityAction->removeReference();

			satellite->removeReference();



			//render it with Ogre
			Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

			//scale
			boxNode->scale(0.5, 0.5, 0.5);

			//display and sync
			Ogre::Entity *ent = mSceneMgr->createEntity(Ogre::StringConverter::toString(mLabMgr->getEntityCount()),"defCube.mesh");
			mLabMgr->setColor(ent, Ogre::Vector3(rand()/(double)RAND_MAX,rand()/(double)RAND_MAX,rand()/(double)RAND_MAX));
			HkOgre::Renderable* rend = new HkOgre::Renderable(boxNode, satellite,mWorld);
			boxNode->attachObject(ent);

			//register to lab manager
			mLabMgr->registerEnity( boxNode, satellite);

		}
	}
	planet->removeReference();


	mWorld->unmarkForWrite();
}
