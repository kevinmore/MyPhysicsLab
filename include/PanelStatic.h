
#ifndef __PANEL_STATIC_H__
#define __PANEL_STATIC_H__

#include <MyGUI.h>
#include "PanelView/BasePanelViewItem.h"


	class PanelStatic :
		public wraps::BasePanelViewItem
	{
	public:
		PanelStatic();

		virtual void initialise();
		virtual void shutdown();
	};



#endif // __PANEL_STATIC_H__
