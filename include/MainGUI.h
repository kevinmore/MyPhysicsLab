#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "RenderBox.h"
#include "PanelViewWindow.h"
#include "PanelDirector.h"
#include "PanelDynamic.h"
#include "PanelStatic.h"
#include "ColourPanel.h"
#include "MessageBox.h"
#include <map>

// Havok and HkOgre includes
#include <hkOgreCooker.h>
#include <hkOgreRenderable.h>

#pragma once
class MainGUI
{
public:
	MainGUI(Ogre::Viewport* vp, Ogre::SceneManager* mSceneMgr, Ogre::RenderWindow* mWindow);
	~MainGUI(void);

	//Ogre
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::Viewport* vp;

	//Window
	unsigned int mHeight, mWidth;

	//GUI components
	MyGUI::Gui* mGuiSystem;
	MyGUI::OgrePlatform* mGUIPlatform;
	MyGUI::VectorWidgetPtr menuPtr;
	MyGUI::VectorWidgetPtr splashPtr;
	MyGUI::VectorWidgetPtr wizardPtr;
	MyGUI::VectorWidgetPtr statusPtr;
	MyGUI::VectorWidgetPtr curvePtr;
	MyGUI::VectorWidgetPtr settingPtr;

	//GUI
	void createGUI(size_t index);
	void destroyGUI(void);

	void createMenu(void);
	void destroyMenu(void);

	//render box
	bool followGUICam;
	std::map<MyGUI::Window*, wraps::RenderBox*> createRenderBox(Ogre::Camera* cam, std::string name);
	void createSplashscreen(void);
	void destroySplashscreen(void);
	void createWizard(void);
	void destroyWizard(void);
	
	//loop controlled
	bool mPause;
	

	//GUI callback
	void notifyMenuItemSelected(MyGUI::Widget* _sender);
	void notifyWindowPressed(MyGUI::Window* _widget, const std::string& _name);
	void addMenuListener(void);
	void notifyWizard( MyGUI::Widget* _sender );
	void notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index);
	void notifyStatus(MyGUI::Widget* _sender);
	void notifyCurve(MyGUI::Widget* _sender);
	void notifySetting(MyGUI::Widget* _sender);
	void notifyRenderBox(MyGUI::Widget* _sender);
	void notifyMessageBoxResultLoad(MyGUI::Message* _sender, MyGUI::MessageBoxStyle _result);

	//menu item operations
	bool flymode;
	bool Command_New;
	bool Command_Clear;
	bool Command_FileSave;
	bool Command_Scenario1;
	bool Command_Scenario2;
	bool Command_Scenario3;
	bool Command_Scenario4;
	bool Command_Scenario5;
	bool Command_Scenario6;
	bool Command_Scenario7;
	bool Command_RigidBody;
	bool Command_ShapeOK;
	bool Command_ResetCamera;
	bool Command_MainCameraFreeMode;
	bool Command_MainCameraPlaneMode;
	bool Command_MiniCameraSphericMode;
	bool Command_MiniCameraChaseMode;
	bool Command_SettingOK;
	bool Command_ColorOK;
	int Command_SlowMotion;
	size_t Theme_Choice;
	bool Command_SwitchTheme;

	//wizard operations
	bool isWizardInit;
	bool Wizard_Preview_Sphere;
	bool Wizard_Preview_Cube;
	bool PropertyOK;
	std::map<std::string, float> Shape_Property_Map;//the map to store shape properties
	std::map<std::string, float> getShapeProperty(void);
	void clearShapeProperty(void);
	void makeShapePrototype(std::string property);
	std::map<MyGUI::Window*, wraps::RenderBox*> wizard_renderBox;

	//color panel
	void notifyColourAccept(ColourPanel* _sender);
	ColourPanel* mColourPanel;
	MyGUI::Colour previewColor;
	
	//status window
	bool Status_Recording;
	MyGUI::Window* createStatusWindow(void);
	void destroyStatusWindow(void);
	void updateStatusWindow(std::map<std::string, float> map);

	//curve generator
	MyGUI::Window* CGWindow;
	MyGUI::Window* createCurveGenerator(void);
	std::string readCurveAxis(std::string name);

	//spline window
	void createSplineWindow(std::string h_axis, std::vector<float> h,std::string v_axis, std::vector<float> v);
	bool Command_GenerateCurve;
	std::string Horizontal_Axis, Vertical_Axis;

	//data window
	MyGUI::Window* mDataWindow;
	PanelViewWindow* mView;
	PanelDirector* mPanelDirector;
	PanelDynamic* mPanelDynamic_X;
	PanelDynamic* mPanelDynamic_Y;
	void createDataWindow(std::string h_axis, std::vector<float> h,std::string v_axis, std::vector<float> v);
	void destroyDataWindow(void);
	void notifyChangePanels(int _key, size_t _value);

	//setting window
	void createSettingWindow(void);
	void resetSettings(void);
	hkpWorldCinfo parseSettingWindow(void);
	bool settingDataOK;
	int dataFreq;
	float mWorldSize;

	//help window
	void createHelpWindow(void);
	void createAboutWindow(void);

	//message box
	MyGUI::Message* msg_quit;
};

