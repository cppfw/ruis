#pragma once

#include <functional>

#include "../widget.hpp"

#include "../group/Pile.hpp"

#include "../base/OrientedWidget.hpp"

#include "FractionBandWidget.hpp"

//disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif


namespace morda{


class ScrollBar :
		public FractionBandWidget,
		protected OrientedWidget,
		private Pile //users do not need to know that it is a container
{
	ScrollBar(const ScrollBar&) = delete;
	ScrollBar& operator=(const ScrollBar&) = delete;

	Widget& handle;

	bool isGrabbed = false;
	float clickPoint;

protected:
	ScrollBar(const puu::forest& desc, bool vertical);
	ScrollBar(const stob::Node* chain, bool vertical) : ScrollBar(stob_to_puu(chain), vertical){}

	void onFractionChange() override;

	void onBandSizeChanged() override;

public:

	virtual ~ScrollBar()noexcept{}

private:
	void lay_out() override;
};



class VScrollBar : public ScrollBar{
public:
	VScrollBar(const puu::forest& desc) :
			widget(desc),
			ScrollBar(desc, true)
	{}
	VScrollBar(const stob::Node* chain) : VScrollBar(stob_to_puu(chain)){}

	VScrollBar(const VScrollBar&) = delete;
	VScrollBar& operator=(const VScrollBar&) = delete;
};


class HScrollBar : public ScrollBar{
public:
	HScrollBar(const puu::forest& desc) :
			widget(desc),
			ScrollBar(desc, false)
	{}
	HScrollBar(const stob::Node* chain) : HScrollBar(stob_to_puu(chain)){}

	HScrollBar(const HScrollBar&) = delete;
	HScrollBar& operator=(const HScrollBar&) = delete;
};

}
