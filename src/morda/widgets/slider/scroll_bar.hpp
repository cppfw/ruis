#pragma once

#include <functional>

#include "../widget.hpp"

#include "../group/pile.hpp"

#include "../base/oriented_widget.hpp"

#include "fraction_band_widget.hpp"

// disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif

namespace morda{

class scroll_bar :
		public fraction_band_widget,
		protected oriented_widget,
		private pile // users do not need to know that it is a container
{
	scroll_bar(const scroll_bar&) = delete;
	scroll_bar& operator=(const scroll_bar&) = delete;

	widget& handle;

	bool isGrabbed = false;
	float clickPoint;

protected:
	scroll_bar(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical);

	void on_fraction_change()override;

	void on_band_changed()override;

public:

	virtual ~scroll_bar()noexcept{}

private:
	void lay_out()override;
};

class VScrollBar : public scroll_bar{
public:
	VScrollBar(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			scroll_bar(this->context, desc, true)
	{}

	VScrollBar(const VScrollBar&) = delete;
	VScrollBar& operator=(const VScrollBar&) = delete;
};

class HScrollBar : public scroll_bar{
public:
	HScrollBar(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			scroll_bar(this->context, desc, false)
	{}

	HScrollBar(const HScrollBar&) = delete;
	HScrollBar& operator=(const HScrollBar&) = delete;
};

}
