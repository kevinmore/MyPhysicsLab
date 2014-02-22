
#ifndef __PANEL_VIEW_H__
#define __PANEL_VIEW_H__

#include "PanelView/BasePanelView.h"
#include "PanelViewCell.h"

	class PanelView :
		public wraps::BasePanelView<PanelViewCell>
	{
	public:
		PanelView(MyGUI::Widget* _parent) :
			wraps::BasePanelView<PanelViewCell>("", _parent)
		{
		}
	};


#endif // __PANEL_VIEW_H__
