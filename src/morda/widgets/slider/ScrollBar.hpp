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
	ScrollBar(const std::shared_ptr<morda::context>& c, const puu::forest& desc, bool vertical);

	void onFractionChange() override;

	void onBandSizeChanged() override;

public:

	virtual ~ScrollBar()noexcept{}

private:
	void lay_out() override;
};



class VScrollBar : public ScrollBar{
public:
	VScrollBar(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
			widget(c, desc),
			ScrollBar(c, desc, true)
	{}

	VScrollBar(const VScrollBar&) = delete;
	VScrollBar& operator=(const VScrollBar&) = delete;
};


class HScrollBar : public ScrollBar{
public:
	HScrollBar(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
			widget(c, desc),
			ScrollBar(c, desc, false)
	{}

	HScrollBar(const HScrollBar&) = delete;
	HScrollBar& operator=(const HScrollBar&) = delete;
};

}
