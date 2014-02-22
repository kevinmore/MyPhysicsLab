
#ifndef __PANEL_DYNAMIC_H__
#define __PANEL_DYNAMIC_H__
#include <MyGUI.h>
#include "PanelView/BasePanelViewItem.h"

	class PanelDynamic :
		public wraps::BasePanelViewItem
	{
	public:
		PanelDynamic(std::string caption, std::vector<float> v);

		virtual void initialise();
		virtual void shutdown();

		void setVisibleCount(size_t _count);

	private:
		MyGUI::VectorWidgetPtr mItemsText;
		MyGUI::VectorWidgetPtr mItemsEdit;
		std::string mCaption;
		std::vector<float> mVector;
	};



#endif // __PANEL_DYNAMIC_H__
