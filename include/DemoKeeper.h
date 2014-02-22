#ifndef __DemoKeeper_h_
#define __DemoKeeper_h_

// Havok and HkOgre includes
#include <hkOgreCooker.h>
#include <hkOgreRenderable.h>
#include "LabManager.h"

//demo class include
#include "GravityAction.h"
#pragma once
class DemoKeeper
{
//class members
private:
	LabManager* mLabMgr;
	Ogre::SceneManager* mSceneMgr;
	hkpWorld* mWorld;

public:
	DemoKeeper(LabManager* labMan, Ogre::SceneManager* sm, hkpWorld* world);
	~DemoKeeper(void);

	//demo loop
	virtual bool demoLoop(const Ogre::FrameEvent& evt);
	void resetAll( void );

	//
	//utility
	//
	//brick wall
	void createBrickWall( int height, int length, const hkVector4& position, hkReal gapWidth, hkVector4Parameter halfExtents );
	
	//keyframing
	hkpRigidBody*	m_keyframedBody;
	hkReal			m_speed;
	hkReal			m_radius;
	int				m_numBodies;
	hkReal			m_time;
	void createGround(void);
	void createBodies(void);
	void getKeyframePositionAndRotation(hkReal t,  hkVector4& pos, hkQuaternion& rot);
	bool Keyframe_demoRunning;

	//binary action
	hkpRigidBody* m_boxRigidBody1;
	hkpRigidBody* m_boxRigidBody2;
	hkpSpringAction* m_springAction;
	bool binaryaction_demoRunning;
	Ogre::ManualObject* manual_spring;
	Ogre::SceneNode* springNode;
	

	//demos functions
	void BrickWallDemo(void);
	void FrictionDemo(void);
	void GravityChangeDemo(void);
	void RestitutionDemo(void);
	void KeyframingDemo(void);
	void BinaryActionDemo(void);
	void GravityActionDemo(void);
	void FountainDemo(void);
};

#endif