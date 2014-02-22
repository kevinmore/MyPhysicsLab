//The GUI Class

#include "MainGUI.h"



MainGUI::MainGUI(Ogre::Viewport* vp, Ogre::SceneManager* mSceneMgr, Ogre::RenderWindow* mWindow):
vp(vp), mSceneMgr(mSceneMgr), mWindow(mWindow), mView(nullptr)
{

	flymode = true;
	Command_New = false;
	Command_Clear = false;
	Command_FileSave = false;
	Command_Scenario1 = false;
	Command_Scenario2 = false;
	Command_Scenario3 = false;
	Command_Scenario4 = false;
	Command_Scenario5 = false;
	Command_Scenario6 = false;
	Command_Scenario7 = false;
	isWizardInit = false;
	Command_RigidBody = false;
	Wizard_Preview_Sphere = false;
	Wizard_Preview_Cube = false;
	Command_ShapeOK = false;
	Command_ResetCamera = false;
	Command_MainCameraFreeMode = false;
	Command_MainCameraPlaneMode = false;
	Command_MiniCameraSphericMode = false;
	Command_MiniCameraChaseMode = false;
	Command_GenerateCurve = false;
	mPause = false;
	Command_SettingOK = false;
	followGUICam = false;
	Command_ColorOK = false;
	Command_SlowMotion = 0;
	mWorldSize = 5000.f;

	//get window size
	mWidth = mWindow->getWidth();
	mHeight = mWindow->getHeight();

	//Init MyGUI
	mGUIPlatform = new MyGUI::OgrePlatform();
	mGUIPlatform->initialise(mWindow, mSceneMgr);
	mGuiSystem = new MyGUI::Gui();
	mGuiSystem->initialise();

	//default theme: black orange
	Command_SwitchTheme = false;
	Theme_Choice = 0;
	createGUI(Theme_Choice);
	mGuiSystem->findWidget<MyGUI::Widget>("Command_Theme0")->setEnabled(false);

}


MainGUI::~MainGUI(void)
{

}


void MainGUI::createGUI(size_t index)
{
	
	//load extra resource
	MyGUI::ResourceManager::getInstance().load("FrameworkSkin.xml");
	MyGUI::ResourceManager::getInstance().load("SelectionSkins.xml");
	MyGUI::ResourceManager::getInstance().load("MessageBoxResources.xml");

	if (index == 0)
	{
		//black orange
		MyGUI::ResourceManager::getInstance().load("MyGUI_BlackOrangeTheme.xml");
	}
	else if (index == 1)
	{
		//black blue
		MyGUI::ResourceManager::getInstance().load("MyGUI_BlackBlueTheme.xml");
	}
	else if (index == 2)
	{
		//white blue
		MyGUI::ResourceManager::getInstance().load("MyGUI_BlueWhiteTheme.xml");
	}
	
	// load menu
	createMenu();

	//create the curve generator
	CGWindow = createCurveGenerator();

	//color panel pointer
	mColourPanel = nullptr;
	
}

void MainGUI::destroyGUI()
{
	mGuiSystem->shutdown();
	delete mGuiSystem;

	mGUIPlatform->shutdown();
	delete mGUIPlatform;
}

void MainGUI::createMenu( void )
{
	
	menuPtr = MyGUI::LayoutManager::getInstance().loadLayout("MyPhysicsLab_Menu.layout");
	addMenuListener();

	//disable current theme menu item
	mGuiSystem->findWidget<MyGUI::Widget>("Command_Theme"+MyGUI::utility::toString(Theme_Choice))->setEnabled(false);
}


// set callback
void MainGUI::addMenuListener( void )
{

	MyGUI::Widget* widget;
	
	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Pause"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Play"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_SlowMotion"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Quit"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_New"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Clear"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_FileSave"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario1"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario2"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario3"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario4"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario5"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario6"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Scenario7"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);
		
	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Solid"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Wireframe"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Points"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_ResetCamera"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_ShowWorldAxis"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_HideWorldAxis"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_MainCameraFreeMode"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_MainCameraPlaneMode"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_MiniCameraSphericMode"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_MiniCameraChaseMode"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_SystemSetting"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Window"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_FullScreen"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Theme0"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Theme1"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Theme2"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_Units"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_About"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);

	if(widget = mGuiSystem->findWidget<MyGUI::Widget>("Command_ScreenShot"))
		widget-> eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyMenuItemSelected);
}


void MainGUI::notifyMenuItemSelected( MyGUI::Widget* _sender )
{
	std::string name = _sender->getName();
	if (name == "Command_Quit")
	{
		msg_quit = MyGUI::Message::createMessageBox("Message", "Quit","Leaving so soon?", MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
		msg_quit->eventMessageBoxResult += MyGUI::newDelegate(this, &MainGUI::notifyMessageBoxResultLoad);
	} 
	else if(name == "Command_Pause")
	{
		mPause = true;
	}
	else if(name == "Command_Play")
	{
		mPause = false;
	}
	else if(name == "Command_SlowMotion")
	{
		Command_SlowMotion++;
	}
	else if(name == "Command_FileSave")
	{
		Command_FileSave = true;
	}
	else if(name == "Command_EnableFlyMode")
	{
		flymode = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(true);
	}
	else if(name == "Command_DisableFlyMode")
	{
		flymode = false;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_New")
	{
		Command_New = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Clear")
	{
		Command_Clear = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(true);
	}
	else if(name == "Command_Scenario1")
	{
		Command_Scenario1 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Scenario2")
	{
		Command_Scenario2 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Scenario3")
	{
		Command_Scenario3 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Scenario4")
	{
		Command_Scenario4 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Scenario5")
	{
		Command_Scenario5 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Scenario6")
	{
		Command_Scenario6 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_Scenario7")
	{
		Command_Scenario7 = true;
		mGuiSystem->findWidget<MyGUI::Widget>("Command_RigidBody")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_EnableFlyMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_DisableFlyMode")->setEnabled(false);
	}
	else if(name == "Command_RigidBody")
	{
		Command_RigidBody = true;
		
	}
	else if(name == "Command_Solid")
	{
		vp->getCamera()->setPolygonMode(Ogre::PM_SOLID);

		mGuiSystem->findWidget<MyGUI::Widget>("Command_Solid")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_Wireframe")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_Points")->setEnabled(true);
	}
	else if(name == "Command_Wireframe")
	{
		vp->getCamera()->setPolygonMode(Ogre::PM_WIREFRAME);

		mGuiSystem->findWidget<MyGUI::Widget>("Command_Solid")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_Wireframe")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_Points")->setEnabled(true);
	}
	else if(name == "Command_Points")
	{
		vp->getCamera()->setPolygonMode(Ogre::PM_POINTS);

		mGuiSystem->findWidget<MyGUI::Widget>("Command_Solid")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_Wireframe")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_Points")->setEnabled(false);
	}
	else if(name == "Command_ResetCamera")
	{
		Command_ResetCamera = true;
	}
	else if(name == "Command_ShowWorldAxis")
	{
		//prepare the axis object
		Ogre::Entity* axisObj = mSceneMgr->createEntity("axes.mesh");
		axisObj->setRenderQueueGroup(Ogre::RENDER_QUEUE_9);
		axisObj->setQueryFlags(0);
		mSceneMgr->getSceneNode("worldAxisNode")->attachObject(axisObj);
		
		mGuiSystem->findWidget<MyGUI::Widget>("Command_ShowWorldAxis")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_HideWorldAxis")->setEnabled(true);
	}
	else if(name == "Command_HideWorldAxis")
	{
		mSceneMgr->getSceneNode("worldAxisNode")->detachAllObjects();

		mGuiSystem->findWidget<MyGUI::Widget>("Command_ShowWorldAxis")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_HideWorldAxis")->setEnabled(false);
	}
	else if(name == "Command_MainCameraFreeMode")
	{
		Command_MainCameraFreeMode = true;
		Command_MainCameraPlaneMode = false;

		mGuiSystem->findWidget<MyGUI::Widget>("Command_MainCameraPlaneMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_MainCameraFreeMode")->setEnabled(false);
	}
	else if(name == "Command_MainCameraPlaneMode")
	{
		Command_MainCameraFreeMode = false;
		Command_MainCameraPlaneMode = true;

		mGuiSystem->findWidget<MyGUI::Widget>("Command_MainCameraPlaneMode")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_MainCameraFreeMode")->setEnabled(true);
	}
	else if(name == "Command_MiniCameraSphericMode")
	{
		Command_MiniCameraSphericMode = true;
		Command_MiniCameraChaseMode = false;

		mGuiSystem->findWidget<MyGUI::Widget>("Command_MiniCameraChaseMode")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_MiniCameraSphericMode")->setEnabled(false);
	}
	else if(name == "Command_MiniCameraChaseMode")
	{
		Command_MiniCameraSphericMode = false;
		Command_MiniCameraChaseMode = true;

		mGuiSystem->findWidget<MyGUI::Widget>("Command_MiniCameraChaseMode")->setEnabled(false);
		mGuiSystem->findWidget<MyGUI::Widget>("Command_MiniCameraSphericMode")->setEnabled(true);
	}
	else if(name == "Command_SystemSetting")
	{
		//if the setting window has not been created
		if(settingPtr.size()==0)
		{
			createSettingWindow();
		}
		mGuiSystem->findWidget<MyGUI::Window>("Setting_Window")->showSmooth();
	}
	else if(name == "Command_Window")
	{
		mWindow->setFullscreen(false, mWidth, mHeight);
	}
	else if(name == "Command_FullScreen")
	{
		mWindow->setFullscreen(true, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
	else if(name == "Command_Theme0")
	{
		Theme_Choice = 0;
		Command_SwitchTheme = true;
	}
	else if(name == "Command_Theme1")
	{
		Theme_Choice = 1;
		Command_SwitchTheme = true;
	}
	else if(name == "Command_Theme2")
	{
		Theme_Choice = 2;
		Command_SwitchTheme = true;
	}
	else if(name == "Command_Units")
	{
		createHelpWindow();
	}
	else if(name == "Command_About")
	{
		createAboutWindow();
	}
	else if(name == "Command_ScreenShot")
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	
}

void MainGUI::createWizard( void )
{
	//pause the game
	mPause = true;

	mSceneMgr->setSkyBoxEnabled(true);
	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

	wizardPtr = MyGUI::LayoutManager::getInstance().loadLayout("ShapeWizard.layout");
	MyGUI::LayerManager::getInstancePtr()->resizeView(MyGUI::RenderManager::getInstancePtr()->getViewSize());

	MyGUI::Window* window;
	window = mGuiSystem->findWidget<MyGUI::Window>("Shape_Wizard");
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);

	//hide the menu
	MyGUI::Widget* widget;
	widget = mGuiSystem->findWidget<MyGUI::Widget>("Menu");
	widget->setVisible(false);
	
	//register event listener
	//ok button
	MyGUI::Widget* button = mGuiSystem->findWidget<MyGUI::Widget>("Shape_OK");
	button->eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyWizard);

	//color picker button
	button = mGuiSystem->findWidget<MyGUI::Widget>("Shape_Color");
	button->eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyWizard);

	//combo box
	MyGUI::ComboBox* combo;
	combo = mGuiSystem->findWidget<MyGUI::ComboBox>("Shape_Selector");
	combo->eventComboAccept += MyGUI::newDelegate(this, &MainGUI::notifyComboAccept);
	combo = mGuiSystem->findWidget<MyGUI::ComboBox>("Motion_Selector");
	combo->eventComboAccept += MyGUI::newDelegate(this, &MainGUI::notifyComboAccept);
	combo = mGuiSystem->findWidget<MyGUI::ComboBox>("PolygonMode_Selector");
	combo->eventComboAccept += MyGUI::newDelegate(this, &MainGUI::notifyComboAccept);


	isWizardInit = true;
	
	//create a render box
	wizard_renderBox = createRenderBox(mSceneMgr->getCamera("PlayerCam"),"WindowCS");
}

MyGUI::Window* MainGUI::createStatusWindow( void )
{
	Status_Recording = true;

	statusPtr = MyGUI::LayoutManager::getInstance().loadLayout("StatusWindow.layout");

	MyGUI::Window* window;
	window = mGuiSystem->findWidget<MyGUI::Window>("StatusWindow");
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);
	window->setVisible(false);

	//set position
	const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();
	window->setPosition(size.width - window->getWidth(), 26);

	//add listener
	MyGUI::Button* button = mGuiSystem->findWidget<MyGUI::Button>("Status_Stop");
	button->eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyStatus);

	return window;
}

MyGUI::Window* MainGUI::createCurveGenerator( void )
{
	curvePtr = MyGUI::LayoutManager::getInstance().loadLayout("CurveGenerator.layout");
	MyGUI::Window* window;
	window = mGuiSystem->findWidget<MyGUI::Window>("CurveGenerator");
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);

	//combo box
	MyGUI::ComboBox* cb = mGuiSystem->findWidget<MyGUI::ComboBox>("Horizontal_Axis");
	cb->setIndexSelected(0);
	cb = mGuiSystem->findWidget<MyGUI::ComboBox>("Vertical_Axis");
	cb->setIndexSelected(0);

	//ok button
	MyGUI::Widget* okButton = mGuiSystem->findWidget<MyGUI::Widget>("Curve_OK");
	okButton->eventMouseButtonClick += MyGUI::newDelegate(this, &MainGUI::notifyCurve);

	return window;
}

void MainGUI::createSplineWindow(std::string x, std::vector<float> h, std::string y, std::vector<float> v )
{
	const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();
	
	//create the window
	MyGUI::Window* window = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("WindowCX", MyGUI::IntCoord((size.width - 585)/2, (size.height - 430)/2, 585, 430), MyGUI::Align::Center, "Overlapped");
	window->setVisible(false);
	window->showSmooth();
	window->setCaption("H: "+x+"         V: "+y);
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);

	//create the empty panel
	MyGUI::Widget* emptyPanel = window->createWidget<MyGUI::Widget>("PanelSkin", MyGUI::IntCoord(0,0,window->getWidth(), window->getHeight()-30), MyGUI::Align::Stretch);

	//create the client widget
	MyGUI::Widget* mClient = emptyPanel->createWidget<MyGUI::Widget>("PanelEmpty", MyGUI::IntCoord(MyGUI::IntPoint(),emptyPanel->getSize()), MyGUI::Align::Stretch);

	//load coordinates plane image
	MyGUI::ImageBox* plane = mClient->createWidget<MyGUI::ImageBox>("ImageBox",MyGUI::IntCoord(MyGUI::IntPoint(-2,-1), mClient->getSize()), MyGUI::Align::Stretch);
	plane->setImageTexture("plane.png");

	// create widget with skin that contain specific sub skin - PolygonalSkin
	MyGUI::Widget* widget = mClient->createWidget<MyGUI::Widget>("PolygonalSkin", MyGUI::IntCoord(MyGUI::IntPoint(), mClient->getSize()), MyGUI::Align::Stretch);
	widget->setColour(MyGUI::Colour::Red);


	// get main subskin
	MyGUI::ISubWidget* main = widget->getSubWidgetMain();
	MyGUI::PolygonalSkin* mPolygonalSkin = main->castType<MyGUI::PolygonalSkin>();


	//get the width and the height of the panel
	//make sure the line can be seen, that's why -5 pixels is here
	int width = mPolygonalSkin->getWidth() - 5;
	int height = mPolygonalSkin->getHeight() - 5;
	

	//find the min and max values of both
	float min_h = h.at(0); float max_h = h.at(0);
	float min_v = v.at(0); float max_v = v.at(0);

	for (unsigned int i=0; i <h.size(); i++)
	{
		if (h.at(i) < min_h) min_h = h.at(i);
		if (h.at(i) > max_h) max_h = h.at(i);
	}
	for (unsigned int i=0; i <v.size(); i++)
	{
		if (v.at(i) < min_v) min_v = v.at(i);
		if (v.at(i) > max_v) max_v = v.at(i);
	}

// 
// 	do a linear transform to stretch the values to fit the window
//  it's different according to the min value
// 	
	if (min_h>=0)
	{
		float kh = width/(max_h-min_h);
		for (int i=0; i <h.size(); i++)
		{
			h.at(i) = kh*h.at(i) - kh*min_h;
		}
	}
	else
	{
		float kh = (width*2)/(max_h-min_h);
		for (int i=0; i <h.size(); i++)
		{
			h.at(i) = kh*h.at(i) - kh*max_h + width;
		}
	}
	
	if (min_v>=0)
	{
		float kv = height/(max_v-min_v);
		for (int i=0; i <v.size(); i++)
		{
			v.at(i) = kv*v.at(i) - kv*min_v;
		}
	} 
	else
	{
		float kv = (height*2)/(max_v-min_v);
		for (int i=0; i <v.size(); i++)
		{
			v.at(i) = kv*v.at(i) - kv*max_v + height;
		}
	}


	//parse the h and v vectors into line points
	std::vector<MyGUI::FloatPoint> mLinePoints;
	//those 2 vectors are of the same size
	for (int i=0; i <h.size(); i++)
	{
		float hPoint, vPoint;
		//left and top distance
		if (min_h>=0)
		{
			hPoint = h.at(i)+width/2;
		}
		else
		{
			hPoint = 0;
		}
		if (min_v>=0)
		{
			vPoint = height/2 - v.at(i);
		} 
		else
		{
			vPoint = height - v.at(i);
		}
		mLinePoints.push_back(MyGUI::FloatPoint(hPoint, vPoint));
	}

	//
	//set PolygonalSkin properties and draw the line
	// 
	mPolygonalSkin->setWidth(5.0f);
	mPolygonalSkin->setPoints(mLinePoints);

}

void MainGUI::notifyWizard( MyGUI::Widget* _sender )
{
	std::string name = _sender->getName();
	if (name == "Shape_Color")
	{
		//create the color panel
		mColourPanel = new ColourPanel();
		mColourPanel->eventColourAccept = MyGUI::newDelegate(this, &MainGUI::notifyColourAccept);
		
		//color picker button
		mGuiSystem->findWidget<MyGUI::Widget>("Shape_Color")->setEnabled(false);

	}
	else if (name == "Shape_OK")
	{
		//get the shape and motion first
		//get the values in the edit boxes
		MyGUI::ComboBox* cb;
		float value_float;

		if (cb = mGuiSystem->findWidget<MyGUI::ComboBox>("Shape_Selector"))
		{
			Shape_Property_Map["Shape_Selector"] = cb->getIndexSelected();
		}
		if (cb = mGuiSystem->findWidget<MyGUI::ComboBox>("Motion_Selector"))
		{
			Shape_Property_Map["Motion_Selector"] = cb->getIndexSelected();
		}


		//
		//make prototype
		//

		PropertyOK = true;
		//physical part
		makeShapePrototype("Shape_Mass");
		makeShapePrototype("Shape_Length");
		makeShapePrototype("Shape_Height");
		makeShapePrototype("Shape_Width");
		makeShapePrototype("Shape_Radius");
		makeShapePrototype("Shape_Friction");
		makeShapePrototype("Shape_Gravity");
		makeShapePrototype("Shape_Rest");
		//spatial part
		//position
		makeShapePrototype("p_x");
		makeShapePrototype("p_y");
		makeShapePrototype("p_z");
		//force
		makeShapePrototype("f_x");
		makeShapePrototype("f_y");
		makeShapePrototype("f_z");
		//torque
		makeShapePrototype("t_x");
		makeShapePrototype("t_y");
		makeShapePrototype("t_z");
		//linear velocity
		makeShapePrototype("lv_x");
		makeShapePrototype("lv_y");
		makeShapePrototype("lv_z");
		//angular velocity
		makeShapePrototype("av_x");
		makeShapePrototype("av_y");
		makeShapePrototype("av_z");
		//linear impulse
		makeShapePrototype("li_x");
		makeShapePrototype("li_y");
		makeShapePrototype("li_z");
		//angular impulse
		makeShapePrototype("ai_x");
		makeShapePrototype("ai_y");
		makeShapePrototype("ai_z");


		//rendering part
		Shape_Property_Map["Red"] = previewColor.red;
		Shape_Property_Map["Green"] = previewColor.green;
		Shape_Property_Map["Blue"] = previewColor.blue;


		//if the input is all ok, tell the main class
		if(PropertyOK)
		{
			Command_ShapeOK = true;
			destroyWizard();
		}
		else //pop up the message box
		{
			MyGUI::Message::createMessageBox("Message", "Invalid Data","You have typed invalid data, please check again.", MyGUI::MessageBoxStyle::Ok | MyGUI::MessageBoxStyle::IconError);
		}
		
		
	} 
}

void MainGUI::notifyComboAccept( MyGUI::ComboBox* _sender, size_t _index )
{
	std::string name = _sender->getName();

	if (name == "Shape_Selector")
	{
		//enable the motion selector
		mGuiSystem->findWidget<MyGUI::Widget>("Motion_Selector")->setEnabled(true);

		//create a sphere
		if (_index == 0)
		{
			Wizard_Preview_Sphere = true;

			//enable the radius editbox only
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Radius")->setEnabled(true);
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Length")->setEnabled(false);
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Height")->setEnabled(false);
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Width")->setEnabled(false);
		} 
		//create a cube
		else if(_index == 1)
		{
			Wizard_Preview_Cube = true;

			//disable the radius editbox only
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Radius")->setEnabled(false);
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Length")->setEnabled(true);
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Height")->setEnabled(true);
			mGuiSystem->findWidget<MyGUI::Widget>("Shape_Width")->setEnabled(true);
		}
	}
	else if (name == "Motion_Selector")
	{
		//enable the tab control panel and the ok button
		mGuiSystem->findWidget<MyGUI::Widget>("Shape_Tab_Control")->setEnabled(true);
		mGuiSystem->findWidget<MyGUI::Widget>("Shape_OK")->setEnabled(true);
	}
	else if (name == "PolygonMode_Selector")
	{
		

		if (_index == 0)
		{
			mSceneMgr->getCamera("WizardCam")->setPolygonMode(Ogre::PM_SOLID);
		}
		else if(_index == 1)
		{
			mSceneMgr->getCamera("WizardCam")->setPolygonMode(Ogre::PM_WIREFRAME);
		}
		else if(_index == 2)
		{
			mSceneMgr->getCamera("WizardCam")->setPolygonMode(Ogre::PM_POINTS);
		}
	}

	
}

std::map<MyGUI::Window*, wraps::RenderBox*> MainGUI::createRenderBox(Ogre::Camera* cam, std::string skin)
{
	
	const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();


	MyGUI::Window* window = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>(skin, MyGUI::IntCoord(size.width - 360, size.height - 270, 360, 270), MyGUI::Align::Right|MyGUI::Align::Bottom, "Overlapped","RenderBox");
	window->setCaption("Camera View");
	window->setMinSize(MyGUI::IntSize(100, 100));
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);
	MyGUI::Canvas* canvas = window->createWidget<MyGUI::Canvas>("Canvas", MyGUI::IntCoord(0, 0, window->getClientCoord().width, window->getClientCoord().height), MyGUI::Align::Stretch, "RenderCanvas");
	canvas->setPointer("hand");
	

	if (skin=="WindowCSX")
	{
		//only add listener with the mini camera view
		canvas->eventMouseButtonClick += newDelegate(this, &MainGUI::notifyRenderBox);
	} 

	wraps::RenderBox* gRenderBox = new wraps::RenderBox();
	gRenderBox->setCanvas(canvas);
	gRenderBox->setViewport(cam);
	gRenderBox->setBackgroundColour(MyGUI::Colour::Black);

	std::map<MyGUI::Window*, wraps::RenderBox*> map;
	map[window] = gRenderBox;
	return map;

	
}

void MainGUI::notifyWindowPressed( MyGUI::Window* _widget, const std::string& _name )
{
	MyGUI::Window* window = _widget->castType<MyGUI::Window>(); 
	if (_name == "close") 
		{
			std::string widget_name = _widget->getName();
			//just hide those windows, do not destroy them
			if (widget_name=="RenderBox"||widget_name=="StatusWindow"||widget_name=="CurveGenerator")
			{
				window->hideSmooth();
			}
			else if (widget_name=="Shape_Wizard")
			{
				destroyWizard();
			}
			else if (widget_name=="Setting_Window")
			{
				window->hideSmooth();
			}
			else if (window == mDataWindow)
			{
				mDataWindow->hideSmooth();
			}
			else
				window->destroySmooth(); 
	
		}
	else if (_name == "minimized") { 
		// hide window and show button in your task bar 
	} 
	else if (_name == "maximized") { 
		// maximized window 
	} 

}

void MainGUI::createSplashscreen( void )
{
	splashPtr = MyGUI::LayoutManager::getInstance().loadLayout("Splash.layout");
}

void MainGUI::destroySplashscreen( void )
{
	if (&splashPtr)
		MyGUI::LayoutManager::getInstance().unloadLayout(splashPtr);
	
}

void MainGUI::makeShapePrototype( std::string property )
{
	//get the values in the edit boxes
	MyGUI::EditBox* eb;
	std::string value_string;
	float value_float;

	if (eb = mGuiSystem->findWidget<MyGUI::EditBox>(property))
	{
		value_string = eb->getOnlyText();
		if (value_string!="")
		{	
			value_float = MyGUI::utility::parseFloat(value_string);
			Shape_Property_Map[property] = value_float;
		}

		//check if the data is valid
		if (property=="Shape_Mass"||property=="Shape_Radius"||property=="Shape_Length"||
			property=="Shape_Height"||property=="Shape_Width")
		{
			if(value_float<=0)
				PropertyOK = false;
		}
		if (property=="Shape_Friction"||property=="Shape_Rest")
		{
			if(value_float<0)
				PropertyOK = false;
		}
	}
}

std::map<std::string, float> MainGUI::getShapeProperty( void )
{
	return Shape_Property_Map;
}

void MainGUI::clearShapeProperty( void )
{
	Shape_Property_Map.clear();
}

void MainGUI::updateStatusWindow( std::map<std::string, float> map)
{
	//parse the motion type
	int m_motionType = map["MotionType"];
	std::string motionType_str;

	switch(m_motionType)
	{
		case 1:
			motionType_str = "DYNAMIC";
			break;
		case 2:
			motionType_str = "SPHERE_INERTIA";
			break;
		case 3:
			motionType_str = "BOX_INERTIA";
			break;
		case 4:
			motionType_str = "KEYFRAMED";
			break;
		case 5:
			motionType_str = "FIXED";
			break;
		case 6:
			motionType_str = "THIN_BOX_INERTIA";
			break;
		case 7:
			motionType_str = "CHARACTER";
			break;
		case 8:
			motionType_str = "MAX_ID";
			break;
		default:
			motionType_str = "INVALID";
			break;
	}

	MyGUI::TextBox* t;

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_MotionType");
	t->setCaption("Motion Type: " + motionType_str);

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_Mass");
	t->setCaption("Mass: " + MyGUI::utility::toString(map["Mass"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_LinearDamping");
	t->setCaption("Linear Damping: " + MyGUI::utility::toString(map["LinearDamping"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_AngularDamping");
	t->setCaption("Angular Damping: " + MyGUI::utility::toString(map["AngularDamping"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_Gravity");
	t->setCaption("Gravity: " + MyGUI::utility::toString(map["Gravity"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_Friction");
	t->setCaption("Friction Factor: " + MyGUI::utility::toString(map["Friction"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_Restitution");
	t->setCaption("Restitution Factor: " + MyGUI::utility::toString(map["Restitution"]));
	

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_CenterOfMassLocal");
	t->setCaption("Center Of Mass Local: ( " + 
		MyGUI::utility::toString(map["CenterOfMassLocal_X"]) + ", "+
		MyGUI::utility::toString(map["CenterOfMassLocal_Y"]) + ", "+
		MyGUI::utility::toString(map["CenterOfMassLocal_Z"]) + " )");



	/************************************************************************/
	/* Dynamic part                                                         */
	/************************************************************************/

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_Timer");
	t->setCaption("Timer: " + MyGUI::utility::toString(map["Time"]) + " seconds" );

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_CenterOfMassInWorld");
	t->setCaption("Center Of Mass In World: ( " + 
		MyGUI::utility::toString(map["CenterOfMassInWorld_X"]) + ", "+
		MyGUI::utility::toString(map["CenterOfMassInWorld_Y"]) + ", "+
		MyGUI::utility::toString(map["CenterOfMassInWorld_Z"]) + " )");

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_LinearVelocity");
	t->setCaption("Linear Velocity: ( " + 
		MyGUI::utility::toString(map["LinearVelocity_X"]) + ", "+
		MyGUI::utility::toString(map["LinearVelocity_Y"]) + ", "+
		MyGUI::utility::toString(map["LinearVelocity_Z"]) + " )");

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_ResultantLinearVelocity");
	t->setCaption("Resultant Linear Velocity: " + MyGUI::utility::toString(map["ResultantLinearVelocity"]));


	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_LinearImpulse");
	t->setCaption("Linear Impulse: ( " + 
		MyGUI::utility::toString(map["LinearImpulse_X"]) + ", "+
		MyGUI::utility::toString(map["LinearImpulse_Y"]) + ", "+
		MyGUI::utility::toString(map["LinearImpulse_Z"]) + " )");

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_ResultantLinearImpulse");
	t->setCaption("Resultant Linear Impulse: " + MyGUI::utility::toString(map["ResultantLinearImpulse"]));



	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_AngularVelocity");
	t->setCaption("Angular Velocity: ( " + 
		MyGUI::utility::toString(map["AngularVelocity_X"]) + ", "+
		MyGUI::utility::toString(map["AngularVelocity_Y"]) + ", "+
		MyGUI::utility::toString(map["AngularVelocity_Z"]) + " )");

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_ResultantAngularVelocity");
	t->setCaption("Resultant Angular Velocity: " + MyGUI::utility::toString(map["ResultantAngularVelocity"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_KineticEnergy");
	t->setCaption("Kinetic Energy: " + MyGUI::utility::toString(map["KineticEnergy"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_GravPotentialEnergy");
	t->setCaption("Gravity Potential Energy: " + MyGUI::utility::toString(map["GravPotentialEnergy"]));

	t = mGuiSystem->findWidget<MyGUI::TextBox>("Status_TotalEnergy");
	t->setCaption("Total Energy: " + MyGUI::utility::toString(map["TotalEnergy"]));


	

}

void MainGUI::notifyStatus( MyGUI::Widget* _sender )
{
	std::string name = _sender->getName();
	if (name == "Status_Stop")
	{
		//stop recording or updating entity status
		Status_Recording = false;

		//pop up the curve generator
		if (!CGWindow->isVisible())
			CGWindow->showSmooth();
			
	}
}

void MainGUI::notifyCurve( MyGUI::Widget* _sender )
{
	std::string name = _sender->getName();
	if (name == "Curve_OK")
	{
		//notice to create a SplineWindow
		Command_GenerateCurve = true;
		//read user option
		Horizontal_Axis = readCurveAxis("Horizontal_Axis");
		Vertical_Axis = readCurveAxis("Vertical_Axis");
	}
}

void MainGUI::destroyStatusWindow( void )
{
	MyGUI::LayoutManager::getInstance().unloadLayout(statusPtr);
}



void MainGUI::destroyWizard( void )
{
	MyGUI::Window* window;
	if(window = mGuiSystem->findWidget<MyGUI::Window>("Shape_Wizard"))
	{
		window->destroySmooth();
		std::map<MyGUI::Window*, wraps::RenderBox*>::iterator it;
		for (it=wizard_renderBox.begin();it!=wizard_renderBox.end();it++)
		{
			it->first->destroySmooth();
		}
		wizard_renderBox.clear();
	}
		

	//show the menu
	MyGUI::Widget* widget;
	widget = mGuiSystem->findWidget<MyGUI::Widget>("Menu");
	widget->setVisible(true);


	isWizardInit = false;
	Ogre::Camera* mCamera = mSceneMgr->getCamera("PlayerCam");
	mCamera->setPosition(10,10,10);
	mCamera->lookAt(0,5,0);
	vp->setCamera(mCamera);

	//resume the game
	mPause = false;
}

void MainGUI::destroyMenu( void )
{
	if (&menuPtr)
		MyGUI::LayoutManager::getInstance().unloadLayout(menuPtr);
}

std::string MainGUI::readCurveAxis( std::string name )
{
	//read axis selected
	MyGUI::ComboBox* cb = mGuiSystem->findWidget<MyGUI::ComboBox>(name);
	
	int i = cb->getIndexSelected();
	if (i==0) return "Time";
	else if (i==1) return "CenterOfMassInWorld_X";
	else if (i==2) return "CenterOfMassInWorld_Y";
	else if (i==3) return "CenterOfMassInWorld_Z";
	else if (i==4) return "LinearVelocity_X";
	else if (i==5) return "LinearVelocity_Y";
	else if (i==6) return "LinearVelocity_Z";
	else if (i==7) return "ResultantLinearVelocity";
	else if (i==8) return "KineticEnergy";
	else if (i==9) return "LinearImpulse_X";
	else if (i==10) return "LinearImpulse_Y";
	else if (i==11) return "LinearImpulse_Z";
	else if (i==12) return "ResultantLinearImpulse";
	else if (i==13) return "AngularVelocity_X";
	else if (i==14) return "AngularVelocity_Y";
	else if (i==15) return "AngularVelocity_Z";
	else if (i==16) return "ResultantAngularVelocity";
	else if (i==17) return "GravPotentialEnergy";
	else if (i==18) return "TotalEnergy";

	else return "";
}

void MainGUI::createDataWindow( std::string h_axis, std::vector<float> h,std::string v_axis, std::vector<float> v )
{
	//destroy the previous one first
	if (mView != nullptr) 
		destroyDataWindow();

	//create the data window
	mView = new PanelViewWindow();
	mDataWindow= mGuiSystem->findWidget<MyGUI::Window>(mView->getPrefix()+"_Main");
	mDataWindow->eventWindowButtonPressed += newDelegate(this, &MainGUI::notifyWindowPressed);
	mDataWindow->showSmooth();

	//create the panels
	mPanelDirector = new PanelDirector(h, v);
	mPanelDynamic_X = new PanelDynamic(h_axis, h);
	mPanelDynamic_Y = new PanelDynamic(v_axis, v);

	mPanelDirector->eventChangePanels = MyGUI::newDelegate(this, &MainGUI::notifyChangePanels);
	mView->getPanelView()->addItem(mPanelDirector);
	mView->getPanelView()->addItem(mPanelDynamic_X);
	mView->getPanelView()->addItem(mPanelDynamic_Y);


}

void MainGUI::notifyChangePanels( int _key, size_t _value )
{
	if (_key == EVENT_SHOW_VERTICAL)
	{
		mPanelDynamic_Y->setVisible(_value != 0);
	}
	else if (_key == EVENT_SHOW_HORIZONTAL)
	{
		mPanelDynamic_X->setVisible(_value != 0);
	}

}

void MainGUI::destroyDataWindow( void )
{
	mView->getPanelView()->removeAllItems();

	delete mView;
	mView = nullptr;
	delete mPanelDirector;
	mPanelDirector = nullptr;
	delete mPanelDynamic_X;
	mPanelDynamic_X = nullptr;
	delete mPanelDynamic_Y;
	mPanelDynamic_Y = nullptr;

}

void MainGUI::createSettingWindow( void )
{
	//load layout
	settingPtr = MyGUI::LayoutManager::getInstance().loadLayout("SettingsWindow.layout");
	MyGUI::Window* window = mGuiSystem->findWidget<MyGUI::Window>("Setting_Window");
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);
	window->setVisible(false);

	//set position
	const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();
	window->setPosition((size.width-window->getWidth())/2, (size.height-window->getHeight())/2);

	//init combo box
	mGuiSystem->findWidget<MyGUI::ComboBox>("Setting_SolverType")->setIndexSelected(4);

	//ok button
	mGuiSystem->findWidget<MyGUI::Button>("Setting_OK")->eventMouseButtonClick
		+= newDelegate(this, &MainGUI::notifySetting);

	//reset button
	mGuiSystem->findWidget<MyGUI::Button>("Setting_Reset")->eventMouseButtonClick
		+= newDelegate(this, &MainGUI::notifySetting);
}

void MainGUI::resetSettings( void )
{
	//init combo box
	mGuiSystem->findWidget<MyGUI::ComboBox>("Setting_SolverType")->setIndexSelected(4);

	//edit boxes
	mGuiSystem->findWidget<MyGUI::EditBox>("Setting_WorldSize")->setCaption("5000.0");
	mGuiSystem->findWidget<MyGUI::EditBox>("Setting_GravX")->setCaption("0.0");
	mGuiSystem->findWidget<MyGUI::EditBox>("Setting_GravY")->setCaption("-9.81");
	mGuiSystem->findWidget<MyGUI::EditBox>("Setting_GravZ")->setCaption("0.0");
	mGuiSystem->findWidget<MyGUI::EditBox>("Setting_ColTolerance")->setCaption("0.05");
	mGuiSystem->findWidget<MyGUI::EditBox>("Setting_DataFreq")->setCaption("25");

}



void MainGUI::notifySetting( MyGUI::Widget* _sender )
{
	std::string name = _sender->getName();
	if (name == "Setting_Reset")
	{
		//reset the settings
		resetSettings();
	}
	else if (name == "Setting_OK")
	{
		//check if the data is valid
		parseSettingWindow();

		if (settingDataOK)
		{
			//hide the window and notice the main class
			mGuiSystem->findWidget<MyGUI::Window>("Setting_Window")->hideSmooth();
			Command_SettingOK = true;
		}
		else
		{
			MyGUI::Message::createMessageBox("Message", "Invalid Data","You have typed invalid data, please check again.", MyGUI::MessageBoxStyle::Ok | MyGUI::MessageBoxStyle::IconError);
		}

		
	}
	
}

hkpWorldCinfo MainGUI::parseSettingWindow( void )
{
	settingDataOK = true;

	hkpWorldCinfo info;
	
	// check if the data capture frequency is valid
	float f = MyGUI::utility::parseInt(
		MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("Setting_DataFreq")->getOnlyText());
	if (f>0)
	{
		dataFreq = f;
	}
	else
	{
		settingDataOK = false;
	}

	//world size
	mWorldSize = MyGUI::utility::parseFloat(
		mGuiSystem->findWidget<MyGUI::EditBox>("Setting_WorldSize")->getOnlyText());

	if (mWorldSize>0)
	{
		info.setBroadPhaseWorldSize(mWorldSize);
	}
	else
	{
		settingDataOK = false;
	}


	//gravity setting
	float gx = MyGUI::utility::parseFloat(mGuiSystem->findWidget<MyGUI::EditBox>("Setting_GravX")->getOnlyText());
	float gy = MyGUI::utility::parseFloat(mGuiSystem->findWidget<MyGUI::EditBox>("Setting_GravY")->getOnlyText());
	float gz = MyGUI::utility::parseFloat(mGuiSystem->findWidget<MyGUI::EditBox>("Setting_GravZ")->getOnlyText());
	info.m_gravity.set(gx,gy,gz);

	//Collusion Tolerance
	float ct = MyGUI::utility::parseFloat(
		mGuiSystem->findWidget<MyGUI::EditBox>("Setting_ColTolerance")->getOnlyText());

	if (ct>0)
	{
		info.m_collisionTolerance = ct;
	}
	else
	{
		settingDataOK = false;
	}
	

	//solver type
	int solver = mGuiSystem->findWidget<MyGUI::ComboBox>("Setting_SolverType")->getIndexSelected();
	if (solver == 0)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_2ITERS_SOFT);
	} 
	else if (solver == 1)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_2ITERS_MEDIUM);
	}
	else if (solver == 2)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_2ITERS_HARD);
	}
	else if (solver == 3)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_SOFT);
	}
	else if (solver == 4)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);
	}
	else if (solver == 5)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_HARD);
	}
	else if (solver == 6)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_SOFT);
	}
	else if (solver == 7)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM);
	}
	else if (solver == 8)
	{
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_HARD);
	}




	return info;

}

void MainGUI::createHelpWindow( void )
{
	const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();

	//create the window
	MyGUI::Window* window = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("WindowCX", MyGUI::IntCoord((size.width - 900)/2, (size.height - 340)/2, 900, 340), MyGUI::Align::Center, "Overlapped");
	window->setVisible(false);
	window->showSmooth();
	window->setCaption("Units and Term in this project");
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);

	//show the units image
	MyGUI::ImageBox* img = window->createWidget<MyGUI::ImageBox>("ImageBox", MyGUI::IntCoord(0,0,window->getWidth(), window->getHeight()-30), MyGUI::Align::Stretch);
	img->setImageTexture("Units_Terms.png");
}

void MainGUI::createAboutWindow( void )
{
	const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();

	//create the window
	MyGUI::Window* window = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("WindowCSX", MyGUI::IntCoord((size.width - 1366)/2, (size.height - 646)/2, 1366, 646), MyGUI::Align::Center, "Overlapped");
	window->setVisible(false);
	window->showSmooth();
	window->setCaption("This is all about MyPhysicsLab v0.1");
	window->setMinSize(140,80);
	window->eventWindowButtonPressed += MyGUI::newDelegate(this, &MainGUI::notifyWindowPressed);
	
	//show the units image
	MyGUI::ImageBox* img = window->createWidget<MyGUI::ImageBox>("ImageBox", MyGUI::IntCoord(0,0,window->getWidth(), window->getHeight()-36), MyGUI::Align::Stretch);
	img->setImageTexture("About.png");
}

void MainGUI::notifyRenderBox( MyGUI::Widget* _sender )
{
	followGUICam = true;
}

void MainGUI::notifyColourAccept( ColourPanel* _sender )
{
	//color picker button
	MyGUI::Button* but = mGuiSystem->findWidget<MyGUI::Button>("Shape_Color",false);
	if (but!=nullptr) but->setEnabled(true);


	//return the color
	if(mColourPanel->colorPicked)
	{
		previewColor = mColourPanel->getColour();
		but->setColour(previewColor);
		Command_ColorOK = true;
	}
		

	mColourPanel->~ColourPanel();
	
}

void MainGUI::notifyMessageBoxResultLoad( MyGUI::Message* _sender, MyGUI::MessageBoxStyle _result )
{
	if (_sender == msg_quit)
	{
		if (_result == MyGUI::MessageBoxStyle::Yes )
		{
			mWindow->destroy();
		}
	}
}


