#pragma once

#include <functional>

#include "../widget.hpp"

#include "../group/pile.hpp"

#include "../base/oriented_widget.hpp"

#include "FractionBandWidget.hpp"

// disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif

namespace morda{

class ScrollBar :
		public FractionBandWidget,
		protected oriented_widget,
		private pile // users do not need to know that it is a container
{
	ScrollBar(const ScrollBar&) = delete;
	ScrollBar& operator=(const ScrollBar&) = delete;

	widget& handle;

	bool isGrabbed = false;
	float clickPoint;

protected:
	ScrollBar(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical);

	void on_fraction_change()override;

	void onBandSizeChanged()override;

public:

	virtual ~ScrollBar()noexcept{}

private:
	void lay_out()override;
};

class VScrollBar : public ScrollBar{
public:
	VScrollBar(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			ScrollBar(this->context, desc, true)
	{}

	VScrollBar(const VScrollBar&) = delete;
	VScrollBar& operator=(const VScrollBar&) = delete;
};

class HScrollBar : public ScrollBar{
public:
	HScrollBar(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			ScrollBar(this->context, desc, false)
	{}

	HScrollBar(const HScrollBar&) = delete;
	HScrollBar& operator=(const HScrollBar&) = delete;
};

}
