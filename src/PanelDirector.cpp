
#include "Precompiled.h"
#include "PanelDirector.h"

	PanelDirector::PanelDirector(std::vector<float> a, std::vector<float> b) :
		BasePanelViewItem("PanelDirector.layout"),
		mCheckShowVertical(nullptr),
		mCheckShowHorizontal(nullptr),
		h(a),
		v(b)
	{
	}

	void PanelDirector::initialise()
	{
		mPanelCell->setCaption("Overview");

		assignWidget(mCheckShowVertical, "check_Vertical");
		assignWidget(mCheckShowHorizontal, "check_Horizontal");
		assignWidget(Max_H, "Max_H");
		assignWidget(Min_H, "Min_H");
		assignWidget(Mean_H, "Mean_H");
		assignWidget(Max_V, "Max_V");
		assignWidget(Min_V, "Min_V");
		assignWidget(Mean_V, "Mean_V");
		assignWidget(Data_Total, "Data_Total");

		mCheckShowVertical->eventMouseButtonClick += MyGUI::newDelegate(this, &PanelDirector::notifyMouseButtonClick);
		mCheckShowHorizontal->eventMouseButtonClick += MyGUI::newDelegate(this, &PanelDirector::notifyMouseButtonClick);


		//parse the vectors
		//find the min and max values of both
		float min_h = h.at(0); float max_h = h.at(0);
		float min_v = v.at(0); float max_v = v.at(0);
		float sum_h = 0; float sum_v = 0;
		for (unsigned int i=0; i < h.size(); i++)
		{
			sum_h += h.at(i);
			if (h.at(i) < min_h) min_h = h.at(i);
			if (h.at(i) > max_h) max_h = h.at(i);
		}
		for (unsigned int i=0; i < v.size(); i++)
		{
			sum_v += v.at(i);
			if (v.at(i) < min_v) min_v = v.at(i);
			if (v.at(i) > max_v) max_v = v.at(i);
		}

		//display them
		Data_Total->setCaption("Total Points: " + MyGUI::utility::toString(h.size()));
		Max_H->setCaption("Horizontal Max: " + MyGUI::utility::toString(max_h));
		Min_H->setCaption("Horizontal Min: " + MyGUI::utility::toString(min_h));
		Max_V->setCaption("Vertical Max: " + MyGUI::utility::toString(max_v));
		Min_V->setCaption("Vertical Min: " + MyGUI::utility::toString(min_v));
		Mean_H->setCaption("Horizontal Mean: " + MyGUI::utility::toString(sum_h/h.size()));
		Mean_V->setCaption("Vertical Mean: " + MyGUI::utility::toString(sum_v/v.size()));

	}

	void PanelDirector::shutdown()
	{
	}

	void PanelDirector::notifyMouseButtonClick(MyGUI::Widget* _sender)
	{
		if (_sender == mCheckShowVertical)
		{
			mCheckShowVertical->setStateSelected( ! mCheckShowVertical->getStateSelected());
			eventChangePanels(EVENT_SHOW_VERTICAL, (size_t)mCheckShowVertical->getStateSelected());
		}
		else if (_sender == mCheckShowHorizontal)
		{
			mCheckShowHorizontal->setStateSelected( ! mCheckShowHorizontal->getStateSelected());
			eventChangePanels(EVENT_SHOW_HORIZONTAL, (size_t)mCheckShowHorizontal->getStateSelected());
		}
	}


