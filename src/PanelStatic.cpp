
#include "Precompiled.h"
#include "PanelStatic.h"


	PanelStatic::PanelStatic() : BasePanelViewItem("PanelStatic.layout")
	{
	}

	void PanelStatic::initialise()
	{
		mPanelCell->setCaption("Static panel");
	}

	void PanelStatic::shutdown()
	{
	}


