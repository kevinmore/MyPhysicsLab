
#ifndef __PANEL_DIRECTOR_H__
#define __PANEL_DIRECTOR_H__

#include <MyGUI.h>
#include "PanelView/BasePanelViewItem.h"

	enum EventInfo
	{
		EVENT_SHOW_VERTICAL,
		EVENT_SHOW_HORIZONTAL,
	};

	typedef MyGUI::delegates::CDelegate2<int, size_t> EventHandle_EventDirector;

	class PanelDirector :
		public wraps::BasePanelViewItem
	{
	public:
		PanelDirector(std::vector<float> a, std::vector<float> b);

		virtual void initialise();
		virtual void shutdown();

		EventHandle_EventDirector eventChangePanels;

	private:
		void notifyMouseButtonClick(MyGUI::Widget* _sender);
		void notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index);

	private:
		MyGUI::Button* mCheckShowVertical;
		MyGUI::Button* mCheckShowHorizontal;
		MyGUI::TextBox* Max_H;
		MyGUI::TextBox* Min_H;
		MyGUI::TextBox* Max_V; 
		MyGUI::TextBox* Min_V;
		MyGUI::TextBox* Mean_V;
		MyGUI::TextBox* Mean_H;
		MyGUI::TextBox* Data_Total;
		std::vector<float> h, v;
	};



#endif // __PANEL_DIRECTOR_H__
