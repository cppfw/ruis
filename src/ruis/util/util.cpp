/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "util.hpp"

#include <utki/config.hpp>
#include <utki/debug.hpp>

#include "../context.hpp"
#include "../widget/widget.hpp"

using namespace std::string_literals;

using namespace ruis;

ruis::vec2 ruis::make_vec2(
	tml::forest::const_iterator begin, //
	tml::forest::const_iterator end
)
{
	ruis::vec2 ret;

	unsigned n = 0;
	real v = 0;
	for (auto i = begin; n != 2 && i != end; ++n, ++i) {
		v = real(i->value.to_float());
		ret[n] = v;
	}

	for (; n != 2; ++n) {
		ret[n] = v;
	}

	return ret;
}

ruis::rect ruis::make_rect(const tml::forest& desc)
{
	using std::min;
	vec2 p = make_vec2(
		desc.begin(), //
		desc.end()
	);
	vec2 d = make_vec2(
		utki::next(
			desc.begin(), //
			min(size_t(2), desc.size())
		),
		desc.end()
	);
	return {p, d};
}

ruis::sides<real> ruis::make_sides(const tml::forest& desc)
{
	using std::min;
	vec2 p = make_vec2(
		desc.begin(), //
		desc.end()
	);
	vec2 d = make_vec2(
		utki::next(
			desc.begin(), //
			min(size_t(2), desc.size())
		),
		desc.end()
	);
	return {
		p.x(), //
		p.y(),
		d.x(),
		d.y()
	};
}

const tml::leaf& ruis::get_property_value(const tml::tree& p)
{
	if (p.children.size() != 1) {
		throw std::invalid_argument("get_property_value(): property has no value");
	}
	return p.children.front().value;
}

bool ruis::is_all_undefined(const sides<styled<length>>& s)
{
	auto i = std::find_if(
		s.begin(), //
		s.end(),
		[](const auto& e) {
			return !e.get().is_undefined();
		}
	);
	return i == s.end();
}
