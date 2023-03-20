/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <functional>

#include "../../widget.hpp"

#include "../../util/oriented.hpp"

#include "../base/fraction_band_widget.hpp"

// disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif

namespace morda{

class scroll_bar :
		public fraction_band_widget,
		protected oriented,
		private container // users do not need to know that it is a container
{
	scroll_bar(const scroll_bar&) = delete;
	scroll_bar& operator=(const scroll_bar&) = delete;

	widget& handle;

	bool isGrabbed = false;
	float clickPoint;

protected:
	scroll_bar(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical);

	void on_fraction_change()override;

	void on_band_change()override;

public:

	virtual ~scroll_bar()noexcept{}

private:
	void on_lay_out()override;
};

class vertical_scroll_bar : public scroll_bar{
public:
	vertical_scroll_bar(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
			widget(c, desc),
			scroll_bar(this->context, desc, true)
	{}

	vertical_scroll_bar(const vertical_scroll_bar&) = delete;
	vertical_scroll_bar& operator=(const vertical_scroll_bar&) = delete;
};

class horizontal_scroll_bar : public scroll_bar{
public:
	horizontal_scroll_bar(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
			widget(c, desc),
			scroll_bar(this->context, desc, false)
	{}

	horizontal_scroll_bar(const horizontal_scroll_bar&) = delete;
	horizontal_scroll_bar& operator=(const horizontal_scroll_bar&) = delete;
};

}
