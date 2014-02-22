
#ifndef __PANEL_VIEW_WINDOW_H__
#define __PANEL_VIEW_WINDOW_H__
//This is the basic panel
#include "PanelView.h"
#include "BaseLayout/BaseLayout.h"

	class PanelViewWindow :
		public wraps::BaseLayout
	{
	public:
		PanelViewWindow() :
			BaseLayout("PanelView.layout"),
			mPanelView(nullptr)
		{
			mPrefix = assignBase(mPanelView, "scroll_View");
		}

		PanelView* getPanelView()
		{
			return mPanelView;
		}
		std::string getPrefix()
		{
			return mPrefix;
		}

	private:
		PanelView* mPanelView;
		std::string mPrefix;
	};



#endif // __PANEL_VIEW_WINDOW_H__
