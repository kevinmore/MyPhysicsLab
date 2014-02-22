/*
-----------------------------------------------------------------------------
Filename:    MyPhysicsLab.h
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _ 
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
	  |___/                 |_|   |_|                                 
	  Ogre 1.8.x Application Wizard for VC10 (May 2012)
	  https://bitbucket.org/jacmoe/ogreappwizards
-----------------------------------------------------------------------------
*/
#ifndef __MyPhysicsLab_h_
#define __MyPhysicsLab_h_

#include "BaseApplication.h"
#include "LabManager.h"

//data keeper
#include "DataKeeper.h"

//demo keeper
#include "DemoKeeper.h"

//file manager
#include "FileManager.h"

//CameraControlSystem includes
#include <CCSCameraControlSystem.h>
#include "CCSBasicCameraModes.h"
#include "CCSOrbitalCameraMode.h"
#include "CCSSphericCameraMode.h"



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif


class MyPhysicsLab : public BaseApplication
{
public:
	MyPhysicsLab(void);
	virtual ~MyPhysicsLab(void);
	
protected:

	virtual void createScene(void);
	void destroyScene(void);
	void createPhysicalGround(float size);
	void createBase(void);
	void createLight(void);
	void resetCamera(void);
	void setFlymode(const Ogre::FrameEvent& evt, bool flag);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool gameLoop(const Ogre::FrameEvent& evt);

	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );

	//Status Window Timer
	Ogre::Timer* mTimer;
	int mFrameCount;
	int mGameLoopCount;

	//System Settings
	hkpWorldCinfo mWorldInfo;
	hkpWorld* rebuiltWorld(void);
	int data_freq;
	bool mLoadFromSetting;

	void throwStuff(void);

	void welcomeScene(void);
	void createWizardSpace(void);
	void GUIeventHandle(void);

	void gotoWizard(void);
	Ogre::SceneNode* worldAxisNode;

	//CameraControlSystem
	CCS::CameraControlSystem* mCameraCS;
	CCS::CameraControlSystem* miniCameraCS;
	Ogre::Camera* ccsCam;
	Ogre::Camera* miniCam;
	CCS::ChaseCameraMode* camMode_Chase;
	CCS::SphericCameraMode* camMode_Spheric;
	CCS::PlaneBindedCameraMode* camMode_Plane;
	OgreBites::SdkCameraMan* ccsCameraMan;
	std::string mainCamMode;
	std::string miniCamMode;
	void initCCS(void);
	
	//gui widgets
	MyGUI::Window* labRenderWindow;
	wraps::RenderBox* labRenderBox;
	void initRenderBox(void);
	MyGUI::Window* labStatusWindow;
	MyGUI::ComboBox* labComboBox;
	void initGUIWidgets(void);
	void hideGUI(void);

	//lab manager
	LabManager* mLabMgr;
	bool islabMgrInit;

	//analyzed target
	hkpRigidBody* targetRididBody;
	std::vector<hkpRigidBody*> targetsVector;
	void notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index);

	//data keeper
	DataKeeper* mDataKeeper;

	//demo keeper
	DemoKeeper* mDemoKeeper;

	//file manager
	FileManager* mFileMgr;

	//wizard camera
	Ogre::Camera* wizardCamera;
	OgreBites::SdkCameraMan* wizardCameraMan; 
	Ogre::SceneNode *wizardCamNode;
	Ogre::SceneNode *altarNode;
	Ogre::SceneNode *shapeNode;

	//pick entity
	int illusionCount;
	bool mQuitRayCast;
	bool checkHover(Ogre::SceneNode* entNode);
	std::vector<Ogre::SceneNode*> hoverFocusMap;//this map stores the illusion node
	std::map<Ogre::SceneNode*, Ogre::SceneNode*> clickFocusMap;//this map stores the illusion node and the axis node
	void castClickBehaviour(Ogre::SceneNode* entNode);
	void ClickFocus(MyGUI::IntPoint mousePos, const OIS::MouseEvent &arg);
	void HoverFocus(MyGUI::IntPoint mousePos, const OIS::MouseEvent &arg);
	bool PickEntity(Ogre::RaySceneQuery* mRaySceneQuery, 
		Ogre::Ray &ray, 
		Ogre::Entity **result, 
		Ogre::Vector3 &hitpoint, 
		bool excludeInVisible,
		Ogre::uint32 mask,
		const Ogre::String& excludeobject, 
		Ogre::Real max_distance);

	void GetMeshInformationEx(const Ogre::MeshPtr mesh,
		size_t &vertex_count,
		Ogre::Vector3* &vertices,
		size_t &index_count,
		unsigned long* &indices,
		const Ogre::Vector3 &position,
		const Ogre::Quaternion &orient,
		const Ogre::Vector3 &scale);
};

#endif // #ifndef __MyPhysicsLab_h_