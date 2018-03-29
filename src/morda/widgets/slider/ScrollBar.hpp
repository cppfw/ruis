#pragma once

#include <functional>

#include "../Widget.hpp"

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
	ScrollBar(const stob::Node* chain, bool vertical);

	void onFractionChange() override;

	void onBandSizeChanged() override;
	
public:

	virtual ~ScrollBar()noexcept{}
	
private:
	void layOut() override;
};



class VScrollBar : public ScrollBar{
public:
	VScrollBar(const stob::Node* chain) : 
			Widget(chain),
			ScrollBar(chain, true)
	{}
	
	VScrollBar(const VScrollBar&) = delete;
	VScrollBar& operator=(const VScrollBar&) = delete;
};


class HScrollBar : public ScrollBar{
public:
	HScrollBar(const stob::Node* chain) : 
			Widget(chain),
			ScrollBar(chain, false)
	{}
	
	HScrollBar(const HScrollBar&) = delete;
	HScrollBar& operator=(const HScrollBar&) = delete;
};

}
