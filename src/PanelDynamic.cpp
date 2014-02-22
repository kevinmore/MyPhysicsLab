
#include "Precompiled.h"
#include "PanelDynamic.h"


	PanelDynamic::PanelDynamic(std::string caption, std::vector<float> v) : BasePanelViewItem("")
	{
		mCaption = caption;
		mVector = v;
	}

	void PanelDynamic::initialise()
	{
		mPanelCell->setCaption(mCaption);

		const int height = 24;
		const int height_step = 26;
		const int width = 55;
		const int width_step = 3;
		int height_current = 0;
		for (size_t pos = 0; pos < mVector.size(); ++pos)
		{
			MyGUI::TextBox* text = mWidgetClient->createWidget<MyGUI::TextBox>("TextBox", MyGUI::IntCoord(width_step, height_current, width, height), MyGUI::Align::Left | MyGUI::Align::Top);
			text->setTextAlign(MyGUI::Align::Right | MyGUI::Align::VCenter);
			text->setCaption(MyGUI::utility::toString("Slot ", pos + 1, " : "));
			mItemsText.push_back(text);

			MyGUI::EditBox* edit = mWidgetClient->createWidget<MyGUI::EditBox>("Edit", MyGUI::IntCoord(width_step + width_step + width, height_current, mWidgetClient->getWidth() - (width_step + width_step + width_step + width), height), MyGUI::Align::HStretch | MyGUI::Align::Top);
			edit->setCaption(MyGUI::utility::toString(mVector.at(pos)));
			mItemsEdit.push_back(edit);

			height_current += height_step;
		}

		mPanelCell->setClientHeight(height_current, false);
	}

	void PanelDynamic::shutdown()
	{
		mItemsText.clear();
		mItemsEdit.clear();
	}

	void PanelDynamic::setVisibleCount(size_t _count)
	{
		const int height_step = 26;
		int height_current = 0;
		for (size_t pos = 0; pos < mVector.size(); ++pos)
		{
			if (pos < _count)
			{
				mItemsText[pos]->setVisible(true);
				mItemsEdit[pos]->setVisible(true);
				height_current += height_step;
			}
			else
			{
				mItemsText[pos]->setVisible(false);
				mItemsEdit[pos]->setVisible(false);
			}
		}
		mPanelCell->setClientHeight(height_current, true);
	}

