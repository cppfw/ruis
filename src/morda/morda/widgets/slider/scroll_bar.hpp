#pragma once

#include <functional>

#include "../widget.hpp"

#include "../group/pile.hpp"

#include "../base/oriented_widget.hpp"
#include "../base/fraction_band_widget.hpp"

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
	scroll_bar(std::shared_ptr<morda::context> c, const treeml::forest& desc, bool vertical);

	void on_fraction_change()override;

	void on_band_change()override;

public:

	virtual ~scroll_bar()noexcept{}

private:
	void lay_out()override;
};

class vertical_scroll_bar : public scroll_bar{
public:
	vertical_scroll_bar(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			scroll_bar(this->context, desc, true)
	{}

	vertical_scroll_bar(const vertical_scroll_bar&) = delete;
	vertical_scroll_bar& operator=(const vertical_scroll_bar&) = delete;
};

class horizontal_scroll_bar : public scroll_bar{
public:
	horizontal_scroll_bar(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			scroll_bar(this->context, desc, false)
	{}

	horizontal_scroll_bar(const horizontal_scroll_bar&) = delete;
	horizontal_scroll_bar& operator=(const horizontal_scroll_bar&) = delete;
};

}
