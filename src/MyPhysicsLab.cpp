/*
-----------------------------------------------------------------------------
Filename:    MyPhysicsLab.cpp
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

#include "MyPhysicsLab.h"

//-------------------------------------------------------------------------------------
MyPhysicsLab::MyPhysicsLab(void)
{
	labRenderWindow = nullptr;
	labStatusWindow = nullptr;

	targetRididBody = nullptr;
	mTimer = new Ogre::Timer();
	mTimer->reset();

	illusionCount = 0;
	mFrameCount = 0;
	mGameLoopCount = 0;

	//data capture frequency
	data_freq = 25;
	mLoadFromSetting = false;
}
//-------------------------------------------------------------------------------------
MyPhysicsLab::~MyPhysicsLab(void)
{
	
}

//-------------------------------------------------------------------------------------
void MyPhysicsLab::createScene(void)
{
		
	createLight();
	


	//init helpers
	mLabMgr = new LabManager(mSceneMgr,mWorld);
	mDataKeeper = new DataKeeper();
	mDemoKeeper = new DemoKeeper(mLabMgr,mSceneMgr,mWorld);
	mFileMgr = new FileManager();

	//init world axis node
	worldAxisNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("worldAxisNode");
	worldAxisNode->setPosition(0,0,0);

	

	//init essential GUI widgets
	initGUIWidgets();

	initCCS();

	welcomeScene();
	createWizardSpace();
}



void MyPhysicsLab::createPhysicalGround( float size )
{
	hkReal x,y,z;
	x = size/2; y = 5.f; z = size/2;
	hkVector4 baseSize(x,y,z);
	hkpBoxShape* groundShape = new hkpBoxShape(baseSize,0);
	hkpRigidBodyCinfo ci;
	ci.m_shape = groundShape;
	ci.m_motionType = hkpMotion::MOTION_FIXED;
	ci.m_position = hkVector4( 0.0f, -5.f, 0.0f );
	ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
	ci.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(31);
	hkpRigidBody* groundBody = new hkpRigidBody(ci);
	mWorld->markForWrite();
	// Add the ground geometry to the physical world.
	mWorld->addEntity(groundBody)->removeReference();
	// Remove the reference to our physical
	// shape because it's unneeded.
	groundShape->removeReference();
	mWorld->unmarkForWrite();
}

hkpWorld* MyPhysicsLab::rebuiltWorld( void )
{
	mWorldInfo.m_contactPointGeneration = hkpWorldCinfo::CONTACT_POINT_ACCEPT_ALWAYS;

	// Set the simulation type of the world to multi-threaded.
	mWorldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

	// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
	mWorldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY;//hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;

	hkpWorld* hkpworld = new hkpWorld(mWorldInfo);

	hkpworld->m_wantDeactivation = false;

	hkpworld->markForWrite();

	hkpAgentRegisterUtil::registerAllAgents( hkpworld->getCollisionDispatcher() );

	hkpworld->registerWithJobQueue( mJobQueue );
	hkpworld->unmarkForWrite();

	return hkpworld;
}

void MyPhysicsLab::destroyScene(void)
{	
	//clear all data
	mDataKeeper->clearData();
	labComboBox->removeAllItems();
	targetRididBody = nullptr;
	targetsVector.clear();

	//stop demo loop
	mDemoKeeper->resetAll();

	//destroy lights
	mSceneMgr->destroyAllLights();

	mSceneMgr->destroyAllEntities();
	mLabMgr->removeAllHavokEntities();
	
	mWorld->markForWrite();
	mWorld->removeReference();
	//if the user has not used the setting panel
	//create the world in the default way
	if (!mLoadFromSetting)
	{
		mWorld = createhkpWorld();
	}
	else
	{
		mWorld = rebuiltWorld();
	}

	resetCamera();
	//set up 1 light
	createLight();
}


void MyPhysicsLab::createLight()
{
	//set light color and shadow type
	mSceneMgr->setAmbientLight(Ogre::ColourValue::White);
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	
	//light1
	Ogre::Light* light = mSceneMgr->createLight("Light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(-1, -1, -1);
	
}
void MyPhysicsLab::welcomeScene( void )
{
	//hide the render box window as soon as possible :P
	labRenderWindow->setVisible(false);
	//additional lights
	//light2
	Ogre::Light* light;
	light = mSceneMgr->createLight("Light2");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(-1, -1, 1);

	//light3
	light = mSceneMgr->createLight("Light3");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(1, -1, 1);

	//light4
	light = mSceneMgr->createLight("Light4");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(1, -1, -1);


	mCamera->setPosition(0, 250, 360);
	mCamera->lookAt(0, 120, 0);
	mSceneMgr->setSkyBoxEnabled(false);
	mainGUI->flymode = true;
	createPhysicalGround(mainGUI->mWorldSize);

	//create the blue floor
	Ogre :: Plane plane (Ogre :: Vector3 :: UNIT_Y , 0) ;
	Ogre :: MeshManager :: getSingleton( ) . createPlane( "FloorPlane" , Ogre :: ResourceGroupManager:: DEFAULT_RESOURCE_GROUP_NAME, 
		plane , 800 ,800 , 1 , 1 , true, 1, 1, 1, Ogre :: Vector3 :: UNIT_Z );
	Ogre :: Entity * FloorPlane= mSceneMgr-> createEntity( "GroundEntity", "FloorPlane") ; 
	mSceneMgr -> getRootSceneNode ( ) ->createChildSceneNode ( ) ->attachObject (FloorPlane ) ;
	FloorPlane -> setMaterialName ( "Ground" ) ; 
	FloorPlane->setCastShadows(false);

	// add angle model
	Ogre::Entity *angle_ent = mSceneMgr->createEntity("angle.mesh");
	angle_ent->setQueryFlags(0);
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(angle_ent);

	


//  	hkpBvTreeShape* angleShpape = HkOgre::Cooker::processOgreMesh(angle_ent);
// 
// 	hkpRigidBodyCinfo ci;
// 	ci.m_shape = angleShpape;
// 	ci.m_motionType = hkpMotion::MOTION_FIXED;
// 	ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
// 	hkpRigidBody* angleBody = new hkpRigidBody(ci);
// 	mWorld->markForWrite();
// 	mWorld->addEntity(angleBody)->removeReference();
// 	angleShpape->removeReference();
// 	mWorld->unmarkForWrite();

	
}

void MyPhysicsLab::createBase()
{
	mSceneMgr->setSkyBoxEnabled(true);
	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

	float size = mainGUI->mWorldSize;
	
	Ogre :: Plane plane (Ogre :: Vector3 :: UNIT_Y , 0) ;
	Ogre::String materialName = "BasePlane_"+MyGUI::utility::toString(mTimer->getMicroseconds());
	Ogre :: MeshManager :: getSingleton( ) . createPlane(  materialName, Ogre :: ResourceGroupManager:: DEFAULT_RESOURCE_GROUP_NAME, 
		plane , size ,size , int(size/50)+1 , int(size/50)+1 , true, 1, int(size/50)+1, int(size/50)+1, Ogre :: Vector3 :: UNIT_Z );
	Ogre :: Entity * FloorPlane= mSceneMgr-> createEntity( "GroundEntity", materialName) ; 
	mSceneMgr -> getRootSceneNode ( ) ->createChildSceneNode ( ) ->attachObject (FloorPlane ) ;
	FloorPlane -> setMaterialName ( "Examples/HavokFloor" ) ; 
	FloorPlane->setCastShadows(false);

	// Create the collision shape and rigid body of our level.
	createPhysicalGround(size);

}

bool MyPhysicsLab::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	bool mouseOnWidget = MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));

	if(!mouseOnWidget)
	{
		//if the user clicks left button, and it's the main camera then focus on the entity
		if (id == OIS::MB_Left && vp->getCamera()==mCamera)
		{
			MyGUI::IntPoint mousePos = MyGUI::InputManager::getInstance().getMousePosition();
			ClickFocus(mousePos, arg);
		}

	}
	
	

	return BaseApplication::mousePressed(arg, id);
}

bool MyPhysicsLab::mouseMoved( const OIS::MouseEvent &arg )
{

	bool mouseOnWidget = MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);

	MyGUI::IntPoint mousePos = MyGUI::InputManager::getInstance().getMousePosition();

	if (!mouseOnWidget)
	{
		if (vp->getCamera()==mCamera)
			HoverFocus(mousePos, arg);
	}
	


	return BaseApplication::mouseMoved(arg);
}

bool MyPhysicsLab::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return BaseApplication::mouseReleased(arg, id);
}

bool MyPhysicsLab::keyPressed( const OIS::KeyEvent &arg )
{
	

	if(MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key), arg.text))
		return true;

	
	if (arg.key == OIS::KC_1) throwStuff();

	//toggle pause and resume
	if (arg.key == OIS::KC_SPACE) 
	{
		if (mainGUI->mPause)
		{
			mainGUI->mPause = false;
		}
		else 
		{	
			mainGUI->mPause = true;
		}
			
	}
	if (arg.key == OIS::KC_ESCAPE && !mainGUI->isWizardInit) 
	{
		resetCamera();//if the user is not in the wizard, reset the camera
		mainGUI->Status_Recording = false;//stop recording
	}	

	return BaseApplication::keyPressed(arg);
}

bool MyPhysicsLab::keyReleased( const OIS::KeyEvent &arg )
{
	MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
	
	return BaseApplication::keyReleased(arg);;
}

bool MyPhysicsLab::gameLoop( const Ogre::FrameEvent& evt )
{
	//if the game is paused, skip
	if(mainGUI->mPause) return true;
	
	//if the game is in slow motion, skip 20 frames
	if (mainGUI->Command_SlowMotion%2==1 && ++mGameLoopCount%20!=0) return true;

	//update the target rigid body stats in the status window
	if (targetRididBody != nullptr && mainGUI->Status_Recording)
	{
		//capture the data every 25 frames as default
		if (++mFrameCount%data_freq == 0)
		{
			mainGUI->updateStatusWindow(mLabMgr->parseRigidBody(targetRididBody, mWorld, mTimer->getMilliseconds(), mDataKeeper));
		}
	}

	// Update the Havok world if initialized
	if (isHavokInitialized)
	{
		if (evt.timeSinceLastFrame > 0)
			mWorld->stepMultithreaded( mJobQueue, mThreadPool, evt.timeSinceLastFrame);//1 / 60.0f );
		// Clear accumulated timer data in this thread and all slave threads
		hkMonitorStream::getInstance().reset();
		mThreadPool->clearTimerData();
	}

	//handle force and torque
	mLabMgr->mgrLoop(evt);
	mDemoKeeper->demoLoop(evt);

}

bool MyPhysicsLab::frameRenderingQueued( const Ogre::FrameEvent& evt )
{	
	GUIeventHandle();
	mCameraCS->update(evt.timeSinceLastFrame);
	miniCameraCS->update(evt.timeSinceLastFrame);

	setFlymode(evt, mainGUI->flymode);
	wizardCamNode->yaw(Ogre::Radian(Ogre::Degree(evt.timeSinceLastFrame * 10)));

	//make sure the cameras are above the ground
	Ogre::Vector3 camPos = mCamera->getPosition();
	if (camPos.y<0.5)
	{
		mCamera->setPosition(camPos.x, 0.5, camPos.z);
	}

	


	gameLoop(evt);

	return BaseApplication::frameRenderingQueued(evt);
}

void MyPhysicsLab::resetCamera( void )
{

	mCamera->setPosition(0,16,20);
	mCamera->lookAt(0,5,0);
	vp->setCamera(mCamera);
}

void MyPhysicsLab::setFlymode(  const Ogre::FrameEvent& evt, bool flag )
{

	if (flag)
	{
		if(!isFlying)
		{
			mCamera->setPosition(0, 250, 360);
			mCamera->lookAt(0, 120, 0);
		}
		//make the camera rotate around the center
		mCamNode->yaw(Ogre::Radian(Ogre::Degree(evt.timeSinceLastFrame * 10)));
		isFlying = true;
	} 
	else
	{
		mCamNode->yaw(Ogre::Radian(Ogre::Degree(0)));
		isFlying = false;
	}
}

void MyPhysicsLab::throwStuff(void)
{	
	

	hkReal radius = 1.f;

	hkpRigidBodyCinfo info;
	hkMassProperties massProperties;
	info.m_mass = 100.0f;
	info.m_centerOfMass  = massProperties.m_centerOfMass;
	info.m_inertiaTensor = massProperties.m_inertiaTensor;
	info.m_shape = new hkpSphereShape( radius );
	info.m_position = HkOgre::Convert::ogreVec3ToHk(mCamera->getDerivedPosition());

	info.m_motionType  = hkpMotion::MOTION_SPHERE_INERTIA;
	info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	hkpInertiaTensorComputer::setShapeVolumeMassProperties(info.m_shape, info.m_mass,info);

	hkpRigidBody* sphereRigidBody = new hkpRigidBody( info );

	
	Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity *ent = mSceneMgr->createEntity("defSphere.mesh");
	mLabMgr->setColor(ent, Ogre::Vector3(0.3021,0.3308,0.3671));
	sphereNode->attachObject(ent);
	
	HkOgre::Renderable* rend = new HkOgre::Renderable(sphereNode, sphereRigidBody, mWorld);

	mLabMgr->registerEnity(sphereNode, sphereRigidBody);


	sphereRigidBody->setLinearVelocity(HkOgre::Convert::ogreVec3ToHk(mCamera->getRealDirection() * 80));
	
	mWorld->markForWrite();
	mWorld->addEntity(sphereRigidBody)->removeReference();
	info.m_shape->removeReference();
	
	mWorld->unmarkForWrite();

}


void MyPhysicsLab::GUIeventHandle( void )
{
	

	if(mainGUI->Command_New)
	{
		vp->setCamera(mCamera);
		//	mWindow->setFullscreen(true, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		mainGUI->flymode = false;
		mainGUI->Command_New = false;

		destroyScene();
		createBase();
		
	}
	else if (mainGUI->Command_Clear)
	{
		vp->setCamera(mCamera);
		mainGUI->Command_Clear = false;
		destroyScene();
		welcomeScene();

		
	}
	else if (mainGUI->Command_FileSave)
	{
		mainGUI->Command_FileSave = false;


	}
	else if (mainGUI->followGUICam)
	{
		mainGUI->followGUICam = false;
		
		mCamera->setPosition(miniCameraCS->getCameraPosition());

		vp->setCamera(mCamera);

	}
	else if (mainGUI->Command_SettingOK)
	{
		mainGUI->Command_SettingOK = false;
		//parse the user input and rebuilt the world
		mWorldInfo = mainGUI->parseSettingWindow();
		
		//from now on, create new world according to the settings
		mLoadFromSetting = true;

		//get data capture frequency
		data_freq = mainGUI->dataFreq;

		mainGUI->Command_New = false;
		mainGUI->flymode = false;
		destroyScene();
		welcomeScene();
	}
	else if (mainGUI->Command_RigidBody)
	{
		mainGUI->Command_RigidBody = false;
	
		if (!mainGUI->isWizardInit)
		{
			mainGUI->createWizard();
			vp->setCamera(wizardCamera);
			gotoWizard();
		}
		
	}
	else if (mainGUI->Command_ColorOK)
	{
		mainGUI->Command_ColorOK = false;

		//get the entity
		Ogre::Entity *ent = static_cast<Ogre::Entity*>(shapeNode->getAttachedObject(0));
		MyGUI::Colour c = mainGUI->previewColor;
		mLabMgr->setColor(ent, Ogre::Vector3(c.red, c.green, c.blue));
	}
	else if (mainGUI->Wizard_Preview_Sphere)
	{
		mainGUI->Wizard_Preview_Sphere = false;

		//create a sphere to preview
		Ogre::Entity *ent = mSceneMgr->createEntity("defSphere.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.5538,0.3187,0.1275));
		mainGUI->previewColor.set(0.5538,0.3187,0.1275);
	
		shapeNode->detachAllObjects();
		shapeNode->attachObject(ent);

	}
	else if (mainGUI->Wizard_Preview_Cube)
	{
		mainGUI->Wizard_Preview_Cube = false;

		//create a cube to preview
		Ogre::Entity *ent = mSceneMgr->createEntity("defCube.mesh");
		mLabMgr->setColor(ent, Ogre::Vector3(0.2,0.4,0.7));
		mainGUI->previewColor.set(0.2,0.4,0.7);
		shapeNode->detachAllObjects();
		shapeNode->attachObject(ent);

	}
	else if(mainGUI->Command_ShapeOK)
	{
		mainGUI->Command_ShapeOK = false;
	
		//get the shape property and create the rigid body
		Ogre::SceneNode* entNode = mLabMgr->createRigidBody(mainGUI->getShapeProperty());
		//cast click behaviour on this rigid body if it's not fixed
		if(entNode->getAttachedObject(0)->getQueryFlags()!=0)
			castClickBehaviour(entNode);
		//then clear the map
		mainGUI->clearShapeProperty();
		
	}
	else if (mainGUI->Command_ResetCamera)
	{
		mainGUI->Command_ResetCamera = false;
		resetCamera();
	}
	else if (mainGUI->Command_MainCameraFreeMode)
	{
		mainGUI->Command_MainCameraFreeMode = false;

		vp->setCamera(mCamera);
		mainCamMode = "Free";
	}
	else if (mainGUI->Command_MainCameraPlaneMode)
	{
		mainGUI->Command_MainCameraPlaneMode = false;
		mainCamMode = "Plane";
	}
	else if (mainGUI->Command_MiniCameraChaseMode)
	{
		mainGUI->Command_MiniCameraChaseMode = false;
		miniCamMode = "Chase";
	}
	else if (mainGUI->Command_MiniCameraSphericMode)
	{
		mainGUI->Command_MiniCameraSphericMode = false;
		miniCamMode = "Spheric";
	}
	else if (mainGUI->Command_GenerateCurve)
	{
		mainGUI->Command_GenerateCurve = false;
		
		//get data, create the data window && curve
		std::vector<float> x = mDataKeeper->getData(mainGUI->Horizontal_Axis);
		std::vector<float> y = mDataKeeper->getData(mainGUI->Vertical_Axis);
		mainGUI->createDataWindow(mainGUI->Horizontal_Axis, x, mainGUI->Vertical_Axis, y);
		mainGUI->createSplineWindow(mainGUI->Horizontal_Axis, x, mainGUI->Vertical_Axis, y);

	}
	else if (mainGUI->Command_Scenario1)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario1 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;

		//Brick Wall Demo
		mDemoKeeper->BrickWallDemo();
	}
	else if (mainGUI->Command_Scenario2)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario2 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;

		//Friction Demo
		mDemoKeeper->FrictionDemo();
	}
	else if (mainGUI->Command_Scenario3)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario3 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;
	
		//Gravity Change Demo
		mDemoKeeper->GravityChangeDemo();
	}
	else if (mainGUI->Command_Scenario4)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario4 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;

	
		//Restitution Change Demo
		mDemoKeeper->RestitutionDemo();
	}
	else if (mainGUI->Command_Scenario5)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario5 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;

		//Binary Action Demo
		mDemoKeeper->BinaryActionDemo();
	}
	else if (mainGUI->Command_Scenario6)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario6 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;

		//Keyframing Demo
		mDemoKeeper->KeyframingDemo();		
	}
	else if (mainGUI->Command_Scenario7)
	{
		mainGUI->flymode = false;
		mainGUI->Command_Scenario7 = false;
		mSceneMgr->setSkyBoxEnabled(true);
		destroyScene();
		createBase();

		mainGUI->Command_New = false;
		mainGUI->flymode = false;

		//Gravity Action Demo
		mDemoKeeper->GravityActionDemo();
	}
	else if (mainGUI->Command_SwitchTheme)
	{
		//accept command
		mainGUI->Command_SwitchTheme = false;

		//destroy gui
		mainGUI->Status_Recording=false;
		MyGUI::Gui::getInstance().destroyAllChildWidget();

		//create gui
		mainGUI->createGUI(mainGUI->Theme_Choice);
		initGUIWidgets();
		
	}
}

void MyPhysicsLab::createWizardSpace( void )
{
	// Create the camera
	wizardCamera = mSceneMgr->createCamera("WizardCam");
	wizardCamera->setNearClipDistance(0.1);
	// Alter the camera aspect ratio to match the viewport
	wizardCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	wizardCameraMan = new OgreBites::SdkCameraMan(wizardCamera);   // create a wizard camera controller
	wizardCameraMan->setTopSpeed(5);

	// Populate the camera container
	wizardCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("WizardCamNode", Ogre::Vector3::ZERO);
	wizardCamNode->attachObject(wizardCamera);

	//init the node to display the shape
	shapeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	shapeNode->scale(2,2,2);
	shapeNode->setPosition(0, -190, 0);

	//init the node to display the alter
	altarNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	altarNode->setPosition(0,-200,0);

}

void MyPhysicsLab::gotoWizard( void )
{
	//hide the mini camera render window
	hideGUI();
	//clean up
	shapeNode->detachAllObjects();
	//load the altar mesh :D
	Ogre::Entity *altar_ent = mSceneMgr->createEntity("Altar.mesh");
	altarNode->detachAllObjects();
	altarNode->attachObject(altar_ent);
	//set camera
	wizardCamera->setPosition(15,-185,15);
	wizardCamera->lookAt(0,-192,0);
	
}

bool MyPhysicsLab::PickEntity( Ogre::RaySceneQuery* mRaySceneQuery, Ogre::Ray &ray, Ogre::Entity **result, 
	Ogre::Vector3 &hitpoint, bool excludeInVisible,Ogre::uint32 mask, const Ogre::String& excludeobject, Ogre::Real max_distance )
{
	mRaySceneQuery->setRay(ray);
	mRaySceneQuery->setQueryMask(mask);
	mRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
	mRaySceneQuery->setSortByDistance(true);

	if (mRaySceneQuery->execute().size() <= 0) return (false);

	// at this point we have raycast to a series of different objects bounding boxes.
	// we need to test these different objects to see which is the first polygon hit.
	// there are some minor optimizations (distance based) that mean we wont have to
	// check all of the objects most of the time, but the worst case scenario is that
	// we need to test every triangle of every object.
	Ogre::Real closest_distance = max_distance;
	Ogre::Vector3 closest_result;
	Ogre::RaySceneQueryResult &query_result = mRaySceneQuery->getLastResults();
	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		// stop checking if we have found a raycast hit that is closer
		// than all remaining entities
		if ((closest_distance >= 0.0f) && (closest_distance < query_result[qr_idx].distance))
		{
			break;
		}

		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL) && (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
		{
			// get the entity to check
			Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

			if(excludeInVisible)
				if (!pentity->getVisible())
					continue;
			if(pentity->getName() == excludeobject) 
				continue;

			// mesh data to retrieve
			size_t vertex_count;
			size_t index_count;
			Ogre::Vector3 *vertices;
			unsigned long *indices;

			// get the mesh information
			GetMeshInformationEx(pentity->getMesh(), vertex_count, vertices, index_count, indices,
				pentity->getParentNode()->_getDerivedPosition(),
				pentity->getParentNode()->_getDerivedOrientation(),
				pentity->getParentNode()->_getDerivedScale());

			//maybe there is a bug in GetMeshInformationEx(),when mesh is a line or a circle, the vertex_count is not multiple of 3
			//            if (index_count%3 != 0)
			//            {
			//                index_count-=index_count%3;
			//            }

			// test for hitting individual triangles on the mesh
			bool new_closest_found = false;
			for (int i = 0; i < static_cast<int>(index_count); i += 3) 
			{

				// check for a hit against this triangle
				std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
					vertices[indices[i+1]], vertices[indices[i+2]], true, true);

				// if it was a hit check if its the closest
				if (hit.first)
				{
					if ((closest_distance < 0.0f) || (hit.second < closest_distance))
					{
						// this is the closest so far, save it off
						closest_distance = hit.second;
						new_closest_found = true;
					}
				}
			}

			// free the verticies and indicies memory
			delete[] vertices;
			delete[] indices;

			// if we found a new closest raycast for this object, update the
			// closest_result before moving on to the next object.
			if (new_closest_found)
			{
				closest_result = ray.getPoint(closest_distance);
				(*result) = pentity;
			}
		}
	}

	// return the result
	if (closest_distance != max_distance)
	{
		hitpoint = closest_result;
		return true;
	}
	else
	{
		// raycast failed
		return false;
	}
}

void MyPhysicsLab::GetMeshInformationEx( const Ogre::MeshPtr mesh, size_t &vertex_count, Ogre::Vector3* &vertices, size_t &index_count, unsigned long* &indices, const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale )
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	indices = new unsigned long[index_count];

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
			}
		}

		ibuf->unlock();
		current_offset = next_offset;
	}
}

void MyPhysicsLab::castClickBehaviour(Ogre::SceneNode* entNode)
{
	
	//get the target rigid body
	targetRididBody = mLabMgr->getsyncMap()[entNode];
	
	//start recording
	mainGUI->Status_Recording = true;

	//clear all the data in the data keeper
	mDataKeeper->clearData();

	//reset the timer
	mTimer->reset();

	Ogre::SceneNode* axisNode;
	Ogre::SceneNode* illNode;
	//prepare the axis object
	Ogre::Entity* axisObj = mSceneMgr->createEntity("axes.mesh");
	axisObj->setRenderQueueGroup(Ogre::RENDER_QUEUE_9);
	axisObj->setMaterialName("Core/NodeMaterial");
	axisObj->setQueryFlags(0);

	//remove its hover effect first
	entNode->removeAllChildren();

	Ogre::Entity* ent = static_cast<Ogre::Entity*>(entNode->getAttachedObject(0));
	Ogre::Real scaleFator = ent->getBoundingRadius()/axisObj->getBoundingRadius();
	axisNode = entNode->createChildSceneNode();
	axisNode->scale(scaleFator+0.1,scaleFator+0.1,scaleFator+0.1);//make sure the axis object fit the entity
	axisNode->attachObject(axisObj);

	//set click focus effect
	ent->setRenderQueueGroup(Ogre::RENDER_QUEUE_7);
	Ogre::Entity* illusion = ent->clone("illusion_"+Ogre::StringConverter::toString(illusionCount));
	illusion->setMaterialName("cg/no_depth_check_glow"); 
	illusion->setRenderQueueGroup (Ogre::RENDER_QUEUE_6);
	illusion->setQueryFlags(0);//IMPORTANT!!! IT TROUBLED ME FOR 3 DAYS THAT I SHOULD NEVER FORGOT TO ADD THIS LINE!!!
	illNode = entNode->createChildSceneNode();
	illNode->attachObject(illusion);


	//remove the previous object and add the illusion object to the map
	//if there is an illusion in the map
	if (clickFocusMap.size()>0)
	{
		std::map<Ogre::SceneNode*,Ogre::SceneNode*>::iterator it=clickFocusMap.begin();
		(it->first)->detachAllObjects();
		(it->second)->detachAllObjects();
		clickFocusMap.clear();
	}
	clickFocusMap[illNode] = axisNode;
	illusionCount++;


	//stop the previous camera mode first
	if (miniCameraCS->getCurrentCameraMode())
	{
		miniCameraCS->getCurrentCameraMode()->stop();
	}
	if (mCameraCS->getCurrentCameraMode())
	{
		mCameraCS->getCurrentCameraMode()->stop();
	}
	

	miniCameraCS->setCameraTarget(entNode);
	//set camera mode accordingly
	if (miniCamMode=="Chase")
	{
		//apply chase camera mode
		//the distance should be relative to the entity size
		Ogre::Vector3 relativePos = Ogre::Vector3(ent->getBoundingBox().getSize());
		camMode_Chase->setCameraRelativePosition((1+relativePos)*2);
		miniCameraCS->setCurrentCameraMode(camMode_Chase);

		
	} 
	else if(miniCamMode=="Spheric")
	{
		miniCameraCS->setCurrentCameraMode(camMode_Spheric);
	}


	//show the render box window to display the camera
	labRenderWindow->setVisibleSmooth(true);
	labRenderBox->setViewport(miniCam);

	if (mainCamMode=="Plane")
	{
		//set main camera to be plane mode
		mCameraCS->setCameraTarget(entNode);
		mCameraCS->setCurrentCameraMode(camMode_Plane);		
		vp->setCamera(ccsCam);
	}

	//check if there are nullptr in targets vector
	std::vector<hkpRigidBody*>::iterator t_it;
	for (t_it=targetsVector.begin();t_it!=targetsVector.end();)
	{
		if (*t_it == nullptr)
		{
			t_it= targetsVector.erase(t_it);
		}
		else
			t_it++;
	}

	//re assemble the combo box
	labComboBox->removeAllItems();
	for (int i=0;i<targetsVector.size();i++)
	{
		labComboBox->addItem("Object "+MyGUI::utility::toString(i+1));
	}
	
	//add this object into the combo box
// 	time_t now;
// 	time(&now);
// 	std::string local = ctime(&now);
// 	local = local.substr(0, local.size()-1);//get rid of the \n char
	

	//pop up the status window
	labStatusWindow->showSmooth();

	//check if it's in the vector to avoid duplicate selection
	bool exist = false;
	for (int i=0;i<targetsVector.size();i++)
	{
		//if the rigid body does not exist anymore, erase the pair
		if (targetsVector.at(i) == targetRididBody)
		{
			exist = true; 
			break;
		}
	}

	//push the target rigid body into the vector and the combobox if it's the first time
	if (!exist) 
	{
		targetsVector.push_back(targetRididBody);
		labComboBox->addItem("Object "+MyGUI::utility::toString(targetsVector.size()));
		labComboBox->setIndexSelected(labComboBox->getItemCount()-1);//select the current line
	}


}

void MyPhysicsLab::ClickFocus( MyGUI::IntPoint mousePos, const OIS::MouseEvent &arg )
{
	//avoid picking any object if we are in the shape wizard
	if(mainGUI->isWizardInit) return;

	//pick an entity
	Ogre::Entity* rayResult=NULL;
	Ogre::Vector3 hitPoint;
	Ogre::SceneNode* entNode;
	
	//do the raycast
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.left/float(arg.state.width),mousePos.top/float(arg.state.height));
	Ogre::RaySceneQuery* rayQuery=mSceneMgr->createRayQuery(Ogre::Ray());
	if (PickEntity(rayQuery,mouseRay,&rayResult,hitPoint,true,~0,"GroundEntity",5000))//don't query the ground entity or mask of 0
	{
		entNode = rayResult->getParentSceneNode(); //find an entity

		//cast effect
		castClickBehaviour(entNode);
	}
		
}

void MyPhysicsLab::HoverFocus( MyGUI::IntPoint mousePos, const OIS::MouseEvent &arg )
{
	//do nothing if we are in the shape wizard
	if(mainGUI->isWizardInit) return;

	//pick an entity
	Ogre::Entity* rayResult=NULL;
	Ogre::Vector3 hitPoint;
	Ogre::SceneNode* entNode;
	Ogre::SceneNode* illNode;
	//do the raycast
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.left/float(arg.state.width),mousePos.top/float(arg.state.height));
	Ogre::RaySceneQuery* rayQuery=mSceneMgr->createRayQuery(Ogre::Ray());
	if (PickEntity(rayQuery,mouseRay,&rayResult,hitPoint,true,~0,"GroundEntity",5000))//don't query the ground entity or mask of 0
	{
		entNode = rayResult->getParentSceneNode(); //find an entity
		
		//if the entity has been clicked or already set hover focus effect, skip this
		if(checkHover(entNode)) return;

		//add the hover effect
		Ogre::Entity* ent = static_cast<Ogre::Entity*>(entNode->getAttachedObject(0));

		ent->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
		Ogre::Entity* illusion = ent->clone("illusion_"+Ogre::StringConverter::toString(illusionCount));
		illusion->setMaterialName("cg/glow"); 
		illusion->setRenderQueueGroup (Ogre::RENDER_QUEUE_7);
		illusion->setQueryFlags(0);//IMPORTANT!!! IT TROUBLED ME FOR 3 DAYS THAT I SHOULD NEVER FORGOT TO ADD THIS LINE!!!
		illNode = entNode->createChildSceneNode();
		illNode->attachObject(illusion);

		//remove the previous object and add the illusion object to the map
		//if there is an illusion in the map
		if (hoverFocusMap.size()>0)
		{
			(hoverFocusMap.at(0))->detachAllObjects();
			hoverFocusMap.clear();
		}

		hoverFocusMap.push_back(illNode);

		illusionCount++;

	}

}

bool MyPhysicsLab::checkHover( Ogre::SceneNode* entNode )
{  
	bool exist = false;
	if (clickFocusMap.size()>0)
	{
		std::map<Ogre::SceneNode*,Ogre::SceneNode*>::iterator it = clickFocusMap.begin();
		Ogre::SceneNode* node = it->first->getParentSceneNode();
		if (node==entNode)
		{
			exist = true;
		}	
	}

	if (hoverFocusMap.size()>0)
	{
		if (hoverFocusMap.at(0)->getParentSceneNode()==entNode)
			exist = true;
	}
	return exist;
}

void MyPhysicsLab::initCCS( void )
{
	//init menu commands
	mainCamMode = "Free";
	miniCamMode = "Chase";

	//init 2 additional cameras
	ccsCam = mSceneMgr->createCamera("ccsCam");
	ccsCam->setNearClipDistance(0.1);
	ccsCam->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	miniCam = mSceneMgr->createCamera("miniCam");
	miniCam->setNearClipDistance(0.1);
	miniCam->setAspectRatio(
		Ogre::Real(labRenderWindow->getWidth()) / Ogre::Real(labRenderWindow->getHeight()));
	
	mCameraCS = new CCS::CameraControlSystem(mSceneMgr,"CameraControlSystem",ccsCam);
	miniCameraCS = new CCS::CameraControlSystem(mSceneMgr,"miniCameraControlSystem",miniCam);

	// Create an instance of the "chase" camera mode
	camMode_Chase = new CCS::ChaseCameraMode(miniCameraCS,Ogre::Vector3::ZERO);
	camMode_Chase->setFixedYawAxis(true,Ogre::Vector3::UNIT_Y);

	// Create an instance of the "spheric" camera mode
	camMode_Spheric = new CCS::SphericCameraMode(mCameraCS,Ogre::Vector3(5,5,5),10);
	camMode_Spheric->setAutoResetTime(1);

	camMode_Plane = new CCS::PlaneBindedCameraMode(mCameraCS,Ogre::Plane(Ogre :: Vector3 :: UNIT_X , 0));

	// Register the instance in the camera control system
	miniCameraCS->registerCameraMode("SphericMode",camMode_Spheric);
	miniCameraCS->registerCameraMode("ChaseMode",camMode_Chase);

	mCameraCS->registerCameraMode("SphericMode",camMode_Spheric);
	mCameraCS->registerCameraMode("PlaneMode",camMode_Plane);



}

void MyPhysicsLab::initRenderBox( void )
{

	//init render box window and hide it
	std::map<MyGUI::Window*, wraps::RenderBox*> lab_renderBox_map
		= mainGUI->createRenderBox(nullptr,"WindowCSX");

	std::map<MyGUI::Window*, wraps::RenderBox*>::iterator it ;
	for (it=lab_renderBox_map.begin();it!=lab_renderBox_map.end();it++)
	{
		labRenderWindow = it->first;
		labRenderBox = it->second;
	}

	labRenderWindow->setCaption("Mini Camera View");
	labRenderWindow->setVisible(false);

}

void MyPhysicsLab::notifyComboAccept( MyGUI::ComboBox* _sender, size_t _index )
{
	//cast click behaviour from the combo box
	std::map<Ogre::SceneNode*, hkpRigidBody*> sycnMap = mLabMgr->getsyncMap();
	std::map<Ogre::SceneNode*, hkpRigidBody*>::iterator it_sync;
	for (it_sync = sycnMap.begin();it_sync!=sycnMap.end();it_sync++)
	{
		if (it_sync->second == targetsVector.at(_index))
		{
			castClickBehaviour(it_sync->first);
		}
	}

}

void MyPhysicsLab::hideGUI( void )
{
	if(labStatusWindow!=nullptr) labStatusWindow->hideSmooth();
	if(labRenderWindow!=nullptr) labRenderWindow->hideSmooth();
}

//init essential GUI widgets
void MyPhysicsLab::initGUIWidgets( void )
{
	initRenderBox();
	labStatusWindow = mainGUI->createStatusWindow();
	labComboBox = MyGUI::Gui::getInstance().findWidget<MyGUI::ComboBox>("Status_Object");
	labComboBox->eventComboAccept += newDelegate(this, &MyPhysicsLab::notifyComboAccept);
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
	int main(int argc, char *argv[])
#endif
	{
		// Create application object
		MyPhysicsLab app;

		try {
			app.go();
		} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif
