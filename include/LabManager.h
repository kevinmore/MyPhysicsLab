#ifndef __LabManager_h_
#define __LabManager_h_



#pragma once

// Havok and HkOgre includes
#include <hkOgreCooker.h>
#include <hkOgreRenderable.h>

//data keeper
#include "DataKeeper.h"



class ForceHandler{
public:
	hkpRigidBody* mBody;
	hkVector4 mForce;
	ForceHandler(hkpRigidBody* b, hkVector4& f):
	mBody(b), mForce(f)
	{
	};
	void apply(hkReal deltaTime)
	{
		mBody->applyForce(deltaTime, mForce);
	};
};

class TorqueHandler{
public:
	hkpRigidBody* mBody;
	hkVector4 mTorque;
	TorqueHandler(hkpRigidBody* b, hkVector4& t):
	mBody(b), mTorque(t)
	{
	};
	void apply(hkReal deltaTime)
	{
		mBody->applyForce(deltaTime, mTorque);
	};
};




class LabManager
{

protected:
	

	Ogre::SceneManager* mSceneMgr;
	hkpWorld* mWorld;
	hkArray<hkpRigidBody*> allEntities;
	hkArray<hkpRigidBody*> sessionEntities;

public:
	LabManager(Ogre::SceneManager* mSceneMgr, hkpWorld* world);
	~LabManager(void);

	

	//rigid body recorder
	int getEntityCount(void);
	void registerEnity(Ogre::SceneNode* node, hkpRigidBody* body);
	std::map<Ogre::SceneNode*, hkpRigidBody*> syncMap;
	std::map<Ogre::SceneNode*, hkpRigidBody*> getsyncMap(void);
	void removeAllHavokEntities(void);
	
	//create rigid body
	Ogre::SceneNode* setupRidigBody(hkpRigidBody* rb);
	Ogre::SceneNode* createRigidBody(std::map<std::string, float> Shape_Property_Map);
	std::map<std::string, float> parseRigidBody(hkpRigidBody* rb, hkpWorld* world, unsigned long mTimer, DataKeeper* mDataKeeper);
	void syncRigidBody(hkpRigidBody* rb);

	//force and torque handler
	std::vector<ForceHandler*> ForceHandlerVec;
	std::vector<TorqueHandler*> TorqueHandlerVec;
	virtual bool mgrLoop(const Ogre::FrameEvent& evt);

	//other
	void setColor( Ogre::Entity* ent ,Ogre::Vector3 v );
	

};




#endif // #ifndef __LabManager_h_