#include "LabManager.h"


LabManager::LabManager(Ogre::SceneManager* sm, hkpWorld* world):
allEntities(NULL),
sessionEntities(NULL),
mSceneMgr(sm),
mWorld(world)
{
	
}


LabManager::~LabManager(void)
{
}


std::map<Ogre::SceneNode*, hkpRigidBody*> LabManager::getsyncMap( void )
{
	return syncMap;
}




int LabManager::getEntityCount(void)
{
	return sessionEntities.getSize();
}
void LabManager::registerEnity(Ogre::SceneNode* node, hkpRigidBody* body)
{
	mWorld->markForWrite();
	allEntities.pushBack(body);
	sessionEntities.pushBack(body);
	syncMap[node] = body;

	//if the rigid body is fixed, set it's entity query flag to 0
	if (body->getMotionType() == hkpMotion::MOTION_FIXED || body->getMotionType() == hkpMotion::MOTION_KEYFRAMED)
	{
		node->getAttachedObject(0)->setQueryFlags(0);
	}
	else
	{
		body->setMaxLinearVelocity(10000);
		body->setMaxAngularVelocity(10000);
	}
	mWorld->unmarkForWrite();
}
void LabManager::removeAllHavokEntities()
{

	sessionEntities.clearAndDeallocate();
	syncMap.clear();
}


void LabManager::setColor( Ogre::Entity* ent ,Ogre::Vector3 v )
{
	Ogre::MaterialPtr m_pMat = ent->getSubEntity(0)->getMaterial()->clone("newMat");
	m_pMat->setAmbient(v.x,v.y,v.z);
	m_pMat->setDiffuse(v.x,v.y,v.z, 1);
	ent->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->setSpecular(0.072,0.072,0.072, 1);
	ent->setMaterial(m_pMat);
}
Ogre::SceneNode* LabManager::createRigidBody( std::map<std::string, float> Shape_Property_Map )
{
	mWorld->markForWrite();

	hkpRigidBodyCinfo Info;

	hkReal radius, length, height, width;
	hkReal p_x,p_y,p_z;//position
	hkReal f_x,f_y,f_z;//force
	hkReal t_x,t_y,t_z;//torque
	hkReal lv_x,lv_y,lv_z;//linear velocity
	hkReal av_x,av_y,av_z;//angular velocity
	hkReal li_x,li_y,li_z;//linear impulse
	hkReal ai_x,ai_y,ai_z;//angular impulse

	float shapeType = -1;

	std::map<std::string, float>::iterator it;
	for (it = Shape_Property_Map.begin();it != Shape_Property_Map.end();it++)
	{
		//physical part
		//define the motion first
		if (it->first == "Motion_Selector")
		{
			int index = (int)it->second;

			if (index == 0) Info.m_motionType = hkpMotion::MOTION_DYNAMIC;
			else if (index == 1) Info.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
			else if (index == 2) Info.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
			else if (index == 3) Info.m_motionType = hkpMotion::MOTION_THIN_BOX_INERTIA;
			else if (index == 4) Info.m_motionType = hkpMotion::MOTION_KEYFRAMED;
			else if (index == 5) Info.m_motionType = hkpMotion::MOTION_FIXED;
				
		}
		else if (it->first == "Shape_Mass")
		{
			Info.m_mass = it->second;
		}
		
		else if (it->first == "Shape_Friction")
		{
			Info.m_friction = it->second;
		}
		else if (it->first == "Shape_Gravity")
		{
			Info.m_gravityFactor = it->second;
		}
		else if (it->first == "Shape_Rest")
		{
			Info.m_restitution = it->second;
		}
		//shape type
		else if (it->first == "Shape_Selector")
		{
			shapeType = it->second;
		}
		//shape size
		else if (it->first == "Shape_Radius")
		{
			radius = it->second;
		}
		else if (it->first == "Shape_Length")
		{
			length = it->second;
		}
		else if (it->first == "Shape_Height")
		{
			height = it->second;
		}
		else if (it->first == "Shape_Width")
		{
			width = it->second;
		}
		//spatial part
		//position
		else if (it->first == "p_x")
		{
			p_x = it->second;
		}
		else if (it->first == "p_y")
		{
			p_y = it->second;
		}
		else if (it->first == "p_z")
		{
			p_z = it->second;
		}
		//force
		else if (it->first == "f_x")
		{
			f_x = it->second;
		}
		else if (it->first == "f_y")
		{
			f_y = it->second;
		}
		else if (it->first == "f_z")
		{
			f_z = it->second;
		}
		//torque
		else if (it->first == "t_x")
		{
			t_x = it->second;
		}
		else if (it->first == "t_y")
		{
			t_y = it->second;
		}
		else if (it->first == "t_z")
		{
			t_z = it->second;
		}
		//linear velocity
		else if (it->first == "lv_x")
		{
			lv_x = it->second;
		}
		else if (it->first == "lv_y")
		{
			lv_y = it->second;
		}
		else if (it->first == "lv_z")
		{
			lv_z = it->second;
		}
		//angular velocity
		else if (it->first == "av_x")
		{
			av_x = it->second;
		}
		else if (it->first == "av_y")
		{
			av_y = it->second;
		}
		else if (it->first == "av_z")
		{
			av_z = it->second;
		}
		//linear impulse
		else if (it->first == "li_x")
		{
			li_x = it->second;
		}
		else if (it->first == "li_y")
		{
			li_y = it->second;
		}
		else if (it->first == "li_z")
		{
			li_z = it->second;
		}
		//angular impulse
		else if (it->first == "ai_x")
		{
			ai_x = it->second;
		}
		else if (it->first == "ai_y")
		{
			ai_y = it->second;
		}
		else if (it->first == "ai_z")
		{
			ai_z = it->second;
		}
		
	}//end the map iteration

	//create the rigid body
	if (shapeType==0)
	{
		//create a sphere
		
		Info.m_shape = new hkpSphereShape( radius );
		Info.m_position = hkVector4(p_x,p_y,p_z);

		// Compute the sphere inertia tensor
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, Info.m_mass, massProperties);
		Info.m_inertiaTensor = massProperties.m_inertiaTensor;


		hkpRigidBody* sphereRigidBody = new hkpRigidBody( Info );

		

		//apply spatial properties
		sphereRigidBody->setLinearVelocity(hkVector4(lv_x,lv_y,lv_z));
		sphereRigidBody->setAngularVelocity(hkVector4(av_x,av_y,av_z));
		sphereRigidBody->applyLinearImpulse(hkVector4(li_x,li_y,li_z));
		sphereRigidBody->applyAngularImpulse(hkVector4(ai_x,ai_y,ai_z));

		//at least one of the vector element is greater than 0
		if ((f_x*f_x + f_y*f_y + f_z*f_z > 0) && Info.m_motionType!=hkpMotion::MOTION_FIXED)
		{
			ForceHandler* fh = new ForceHandler(sphereRigidBody,hkVector4(f_x,f_y,f_z));
			ForceHandlerVec.push_back(fh);
		}
		if ((t_x*t_x + t_y*t_y + t_z*t_z > 0) && Info.m_motionType!=hkpMotion::MOTION_FIXED)
		{
			TorqueHandler* th = new TorqueHandler(sphereRigidBody, hkVector4(t_x,t_y,t_z));
			TorqueHandlerVec.push_back(th);
		}
	
		//display
		Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		Ogre::Entity *ent = mSceneMgr->createEntity("defSphere.mesh");
		
		setColor(ent, Ogre::Vector3(Shape_Property_Map["Red"],Shape_Property_Map["Green"],Shape_Property_Map["Blue"]));
		sphereNode->scale(radius,radius,radius);//sync the size
		sphereNode->setPosition(Ogre::Vector3(p_x,p_y,p_z));
		sphereNode->attachObject(ent);
		HkOgre::Renderable* rend = new HkOgre::Renderable(sphereNode, sphereRigidBody, mWorld);

		registerEnity(sphereNode, sphereRigidBody);//register to lab manager

		
		mWorld->addEntity(sphereRigidBody)->removeReference();
		Info.m_shape->removeReference();
		mWorld->unmarkForWrite();

		return sphereNode;

	}
	else if (shapeType==1)
	{
		//create a cube

		hkVector4 cubeSize(length/2, height/2, width/2);
		Info.m_shape = new hkpBoxShape( cubeSize, 0 );
		Info.m_position = hkVector4(p_x,p_y,p_z);

		// Compute the box inertia tensor
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(cubeSize, Info.m_mass, massProperties);
		Info.m_inertiaTensor = massProperties.m_inertiaTensor;
	
		hkpRigidBody* cubeRigidBody = new hkpRigidBody( Info );


		//apply spatial properties
		cubeRigidBody->setLinearVelocity(hkVector4(lv_x,lv_y,lv_z));
		cubeRigidBody->setAngularVelocity(hkVector4(av_x,av_y,av_z));
		cubeRigidBody->applyLinearImpulse(hkVector4(li_x,li_y,li_z));
		cubeRigidBody->applyAngularImpulse(hkVector4(ai_x,ai_y,ai_z));

		//at least one of the vector element is greater than 0
		if ((f_x*f_x + f_y*f_y + f_z*f_z > 0) && Info.m_motionType!=hkpMotion::MOTION_FIXED)
		{
			ForceHandler* fh = new ForceHandler(cubeRigidBody,hkVector4(f_x,f_y,f_z));
			ForceHandlerVec.push_back(fh);
		}
		if ((t_x*t_x + t_y*t_y + t_z*t_z > 0) && Info.m_motionType!=hkpMotion::MOTION_FIXED)
		{
			TorqueHandler* th = new TorqueHandler(cubeRigidBody, hkVector4(t_x,t_y,t_z));
			TorqueHandlerVec.push_back(th);
		}
		

		//display
		Ogre::SceneNode* cubeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		Ogre::Entity *ent = mSceneMgr->createEntity("defCube.mesh");
		//if the entity is fixed, set it's query flag to be 0
		
		setColor(ent, Ogre::Vector3(Shape_Property_Map["Red"],Shape_Property_Map["Green"],Shape_Property_Map["Blue"]));
		cubeNode->scale(length,height,width);//sync the size
		cubeNode->setPosition(Ogre::Vector3(p_x,p_y,p_z));
		cubeNode->attachObject(ent);
		HkOgre::Renderable* rend = new HkOgre::Renderable(cubeNode, cubeRigidBody, mWorld);

		registerEnity(cubeNode, cubeRigidBody); //register to lab manager

		
		mWorld->addEntity(cubeRigidBody)->removeReference();
		Info.m_shape->removeReference();
		mWorld->unmarkForWrite();

		

		return cubeNode;
	}

	
}



std::map<std::string, float> LabManager::parseRigidBody( hkpRigidBody* rb ,hkpWorld* world, unsigned long mTimer, DataKeeper* mDataKeeper)
{
	std::map<std::string, float> map;
	hkVector4 v;
	
	//
	//Parse the rigid body and store the data into the data keeper
	//

	//get the time
	map["Time"] = (float)mTimer/1000;
	mDataKeeper->setData("Time", (float)mTimer/1000);

	//lock the world
	world->markForRead();

	/************************************************************************/
	/*MotionType && Mass                                                    */
	/************************************************************************/
	map["MotionType"] = rb->getMotionType();
	map["Mass"] = rb->getMass();

	/************************************************************************/
	/*LinearDamping && AngularDamping                                       */
	/************************************************************************/
	map["LinearDamping"] = rb->getLinearDamping();
	map["AngularDamping"] = rb->getAngularDamping();

	/************************************************************************/
	/*Gravity                                                               */
	/************************************************************************/
	v = world->getGravity();

	map["Gravity"] = abs(rb->getMass()*v(1)*rb->getGravityFactor());

	/************************************************************************/
	/*Friction Factor                                                       */
	/************************************************************************/
	map["Friction"] = rb->getFriction();

	/************************************************************************/
	/*Restitution Factor                                                    */
	/************************************************************************/
	map["Restitution"] = rb->getRestitution();

	/************************************************************************/
	/*CenterOfMassLocal                                                     */
	/************************************************************************/
	v = rb->getCenterOfMassLocal();
	map["CenterOfMassLocal_X"] = v(0);
	map["CenterOfMassLocal_Y"] = v(1);
	map["CenterOfMassLocal_Z"] = v(2);
	
	/************************************************************************/
	/*CenterOfMassInWorld                                                   */
	/************************************************************************/
	v = rb->getCenterOfMassInWorld();
	map["CenterOfMassInWorld_X"] = v(0); mDataKeeper->setData("CenterOfMassInWorld_X", v(0));
	map["CenterOfMassInWorld_Y"] = v(1); mDataKeeper->setData("CenterOfMassInWorld_Y", v(1));
	map["CenterOfMassInWorld_Z"] = v(2); mDataKeeper->setData("CenterOfMassInWorld_Z", v(2));

	/************************************************************************/
	/*LinearVelocity && KineticEnergy && LinearImpulse                      */
	/************************************************************************/
	v = rb->getLinearVelocity();
	map["LinearVelocity_X"] = v(0); mDataKeeper->setData("LinearVelocity_X", v(0));
	map["LinearVelocity_Y"] = v(1); mDataKeeper->setData("LinearVelocity_Y", v(1));
	map["LinearVelocity_Z"] = v(2); mDataKeeper->setData("LinearVelocity_Z", v(2));
	map["ResultantLinearVelocity"] = sqrt(v(0)*v(0) + v(1)*v(1) + v(2)*v(2));
	mDataKeeper->setData("ResultantLinearVelocity", map["ResultantLinearVelocity"]);

	hkReal ke = rb->getMass()*(v(0)*v(0) + v(1)*v(1) + v(2)*v(2));
	map["KineticEnergy"] = ke;
	mDataKeeper->setData("KineticEnergy", ke);

	map["LinearImpulse_X"] = rb->getMass()*v(0); mDataKeeper->setData("LinearImpulse_X", v(0));
	map["LinearImpulse_Y"] = rb->getMass()*v(1); mDataKeeper->setData("LinearImpulse_Y", v(1));
	map["LinearImpulse_Z"] = rb->getMass()*v(2); mDataKeeper->setData("LinearImpulse_Z", v(2));
	map["ResultantLinearImpulse"] = rb->getMass()*map["ResultantLinearVelocity"];
	mDataKeeper->setData("ResultantLinearImpulse", map["ResultantLinearImpulse"]);
	
	/************************************************************************/
	/*AngularVelocity                                                       */
	/************************************************************************/
	v = rb->getAngularVelocity();
	map["AngularVelocity_X"] = v(0); mDataKeeper->setData("AngularVelocity_X", v(0));
	map["AngularVelocity_Y"] = v(1); mDataKeeper->setData("AngularVelocity_Y", v(1));
	map["AngularVelocity_Z"] = v(2); mDataKeeper->setData("AngularVelocity_Z", v(2));
	map["ResultantAngularVelocity"] = sqrt(v(0)*v(0) + v(1)*v(1) + v(2)*v(2));
	mDataKeeper->setData("ResultantAngularVelocity", map["ResultantAngularVelocity"]);

	/************************************************************************/
	/*GravPotentialEnergy                                                   */
	/************************************************************************/
	hkReal mg = map["Gravity"];
	hkReal h = rb->getCenterOfMassInWorld()(1);
	hkReal p = abs(mg*h);
	map["GravPotentialEnergy"] = p;
	mDataKeeper->setData("GravPotentialEnergy", p);

	/************************************************************************/
	/*Total Energy                                                          */
	/************************************************************************/
	map["TotalEnergy"] = ke + p;
	mDataKeeper->setData("TotalEnergy", ke + p);

	//unlock the world
	world->unmarkForRead();

	return map;
}



bool LabManager::mgrLoop( const Ogre::FrameEvent& evt)
{
	
	//erase dead bodies
	std::vector<ForceHandler*>::iterator fit;
	for (fit=ForceHandlerVec.begin();fit!=ForceHandlerVec.end();)
	{
		if ((*fit)->mBody->getMotionType()==hkpMotion::MOTION_FIXED || *fit==nullptr)
		{
			fit = ForceHandlerVec.erase(fit);
		}
		else
			fit++;
		if(ForceHandlerVec.empty()) break;
	}
	std::vector<TorqueHandler*>::iterator tit;
	for (tit=TorqueHandlerVec.begin();tit!=TorqueHandlerVec.end();)
	{
		if ((*tit)->mBody->getMotionType()==hkpMotion::MOTION_FIXED || *tit==nullptr)
		{
			tit = TorqueHandlerVec.erase(tit);
		}
		else
			tit++;
		if(TorqueHandlerVec.empty()) break;
	}

	for (int i=0; i<ForceHandlerVec.size();i++)
	{
		ForceHandlerVec.at(i)->mBody->markForWrite();
		ForceHandlerVec.at(i)->apply(evt.timeSinceLastFrame);
		ForceHandlerVec.at(i)->mBody->unmarkForWrite();
	}

	for (int i=0; i<TorqueHandlerVec.size();i++)
	{
		TorqueHandlerVec.at(i)->mBody->markForWrite();
		TorqueHandlerVec.at(i)->apply(evt.timeSinceLastFrame);
		TorqueHandlerVec.at(i)->mBody->markForWrite();
	}

	return true;
}
