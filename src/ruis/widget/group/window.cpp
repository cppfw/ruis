/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "window.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"
#include "../label/margins.hpp"
#include "../label/nine_patch.hpp"

using namespace std::string_literals;
using namespace ruis::length_literals;

using namespace ruis;

namespace {

namespace m = ruis::make;

utki::shared_ref<container> make_top_row(utki::shared_ref<context> c)
{
	// clang-format off
	return m::container(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::min}
			},
			.container_params = {
				.layout = ruis::layout::row
			}
		},
		{
			m::mouse_proxy(c,
				{
					.widget_params = {
						.id = "ruis_lt_proxy"s
					}
				}
			),
			m::mouse_proxy(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::fill},
						.weight = 1
					},
					.widget_params = {
						.id = "ruis_t_proxy"s
					}
				}
			),
			m::mouse_proxy(c,
				{
					.widget_params = {
						.id = "ruis_rt_proxy"s
					}
				}
			)
		}
	);
	// clang-format on
}

utki::shared_ref<container> make_bottom_row(utki::shared_ref<context> c)
{
	// clang-format off
	return m::container(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::min}
			},
			.container_params = {
				.layout = ruis::layout::row
			}
		},
		{
			m::mouse_proxy(c,
				{
					.widget_params = {
						.id = "ruis_lb_proxy"s
					}
				}
			),
			m::mouse_proxy(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::fill},
						.weight = 1
					},
					.widget_params = {
						.id = "ruis_b_proxy"s
					}
				}
			),
			m::mouse_proxy(c,
				{
					.widget_params = {
						.id = "ruis_rb_proxy"s
					}
				}
			)
		}
	);
	// clang-format on
}

utki::shared_ref<container> make_caption(utki::shared_ref<context> c)
{
	// clang-format off
	return m::container(c,
		{
			.layout_params = {
				.dims = {ruis::dim::max, ruis::dim::min}
			},
			.widget_params = {
				.clip = true
			},
			.container_params = {
				.layout = ruis::layout::pile
			}
		},
		{
			m::mouse_proxy(c,
				{
					.layout_params = {
						.dims = {ruis::dim::max, ruis::dim::max}
					},
					.widget_params = {
						.id = "ruis_caption_proxy"s
					}
				}
			),
			m::rectangle(c,
				{
					.layout_params = {
						.dims = {ruis::dim::max, ruis::dim::max}
					},
					.widget_params = {
						.id = "ruis_window_title_bg"s
					}
				}
			),
			m::container(c,
				{
					.layout_params = {
						.dims = {ruis::dim::max, ruis::dim::max}
					},
					.container_params = {
						.layout = ruis::layout::row
					}
				},
				{
					m::margins(c,
						{
							.layout_params = {
								.dims = {ruis::dim::fill, ruis::dim::min},
								.weight = 1
							},
							.container_params = {
								.layout = layout::pile
							},
							.frame_params = {
								.borders = {3_pp, 2_pp, 0_pp, 2_pp}
							}
						},
						{
							m::text(c,
								{
									.layout_params = {
										.dims = {ruis::dim::min, ruis::dim::min},
										.align = {ruis::align::front, ruis::align::center}
									},
									.widget_params = {
										.id = "ruis_title"s
									}
								},
								{}
							)
						}
					)
				}
			)
		}
	);
	// clang-format on
}

utki::shared_ref<container> make_middle(utki::shared_ref<context> c, container::parameters container_params)
{
	// clang-format off
	return m::container(c,
		{
			.layout_params = {
				.dims = {ruis::dim::max, ruis::dim::max},
				.weight = 1
			},
			.container_params = {
				.layout = ruis::layout::column
			}
		},
		{
			make_caption(c),
			m::container(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::fill},
						.weight = 1
					},
					.widget_params = {
						.id = "ruis_content"s,
						.clip = true
					},
					.container_params = std::move(container_params)
				},
				{}
			)
		}
	);
	// clang-format on
}

utki::shared_ref<container> make_middle_row(utki::shared_ref<context> c, container::parameters container_params)
{
	// clang-format off
	return m::container(c,
		{
			.layout_params = {
				.dims = {ruis::dim::max, ruis::dim::max},
				.weight = 1
			},
			.container_params = {
				.layout = ruis::layout::row
			}
		},
		{
			m::mouse_proxy(c,
				{
					.layout_params = {
						.dims = {ruis::dim::min, ruis::dim::fill}
					},
					.widget_params = {
						.id = "ruis_l_proxy"s
					}
				}
			),
			make_middle(c, std::move(container_params)),
			m::mouse_proxy(c,
				{
					.layout_params = {
						.dims = {ruis::dim::min, ruis::dim::fill}
					},
					.widget_params = {
						.id = "ruis_r_proxy"s
					}
				}
			)
		}
	);
	// clang-format on
}

std::vector<utki::shared_ref<widget>> make_children(utki::shared_ref<context> c, container::parameters container_params)
{
	// clang-format off
	return {
		m::container(c,
			{
				.layout_params = {
					.dims = {ruis::dim::max, ruis::dim::max}
				},
				.container_params = {
					.layout = ruis::layout::column
				}
			},
			{
				make_top_row(c),
				make_middle_row(c, std::move(container_params)),
				make_bottom_row(c)
			}
		)
	};
	// clang-format on
}
} // namespace

void ruis::window::set_background(utki::shared_ref<widget> w)
{
	ASSERT(this->children().size() == 1 || this->children().size() == 2)
	if (this->children().size() == 2) {
		this->erase(this->children().begin());
	}

	ASSERT(this->children().size() == 1)

	this->insert(w, this->children().begin());
}

window::window(
	utki::shared_ref<ruis::context> c,
	all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children
) :
	widget( //
		std::move(c),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	container( //
		this->context,
		{.container_params = {.layout = ruis::layout::pile}},
		make_children(this->context, std::move(params.container_params))
	)
{
	this->setup_widgets();

	using namespace ruis::length_literals;
	constexpr static const auto default_border_size_pp = 5_pp;
	for (auto& b : params.borders) {
		if (b.is_undefined()) {
			b = default_border_size_pp;
		}
	}

	this->set_title(params.title);

	if (params.background) {
		this->set_background(utki::make_shared_from(*params.background));
	} else {
		this->set_background(
			ruis::make::nine_patch(
				this->context,
				{//
				 .layout_params =
					 {
						 .dims = {ruis::dim::fill, ruis::dim::fill} //
					 },
				 .container_params =
					 {
						 .layout = layout::pile //
					 },
				 .nine_patch_params =
					 {
						 .nine_patch = this->context.get().loader.load<ruis::res::nine_patch>("ruis_npt_window_bg") //
					 }
				},
				{}
			)
		);
	}

	// TODO: caption colors

	this->set_borders(params.borders);

	// this should go after initializing borders
	this->empty_min_dim = this->measure(vector2(-1));

	this->content_area->push_back(children);
}

ruis::window::window(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	container(
		this->context,
		{.container_params = {.layout = ruis::layout::pile}},
		make_children(this->context, {.layout = ruis::layout::pile})
	)
{
	this->setup_widgets();

	constexpr static const auto default_border_size_pp = length::make_pp(5);
	sides<length> borders(default_border_size_pp);

	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "title") {
			this->set_title(get_property_value(p).string);
		} else if (p.value == "look") {
			for (const auto& pp : p.children) {
				if (!is_property(pp)) {
					continue;
				}

				if (pp.value == "title_color_active") {
					this->title_bg_color_topmost = get_property_value(pp).to_uint32();
				} else if (pp.value == "title_color_inactive") {
					this->title_bg_color_non_topmost = get_property_value(pp).to_uint32();
				} else if (pp.value == "background") {
					this->set_background(this->context.get().inflater.inflate(pp.children));
				} else if (pp.value == "left") {
					borders.left() = parse_dimension_value(get_property_value(pp), this->context.get().units);
				} else if (pp.value == "top") {
					borders.top() = parse_dimension_value(get_property_value(pp), this->context.get().units);
				} else if (pp.value == "right") {
					borders.right() = parse_dimension_value(get_property_value(pp), this->context.get().units);
				} else if (pp.value == "bottom") {
					borders.bottom() = parse_dimension_value(get_property_value(pp), this->context.get().units);
				}
			}
		}
	}
	this->set_borders(borders);

	// this should go after initializing borders
	this->empty_min_dim = this->measure(vector2(-1));

	this->content_area->push_back_inflate(desc);
}

void ruis::window::setup_widgets()
{
	// TODO: use get_widget_as()
	this->content_area = this->try_get_widget_as<container>("ruis_content");
	ASSERT(this->content_area)

	this->title = this->try_get_widget_as<text>("ruis_title");
	ASSERT(this->title)

	this->title_bg = this->try_get_widget_as<rectangle>("ruis_window_title_bg");
	ASSERT(this->title_bg);

	std::function<decltype(mouse_proxy::mouse_button_handler)(cursor_iter&)> make_mouse_button_handler =
		[this](cursor_iter& iter) {
			return decltype(mouse_proxy::mouse_button_handler)([this,
																&iter](mouse_proxy& mp, const mouse_button_event& e) {
				if (e.button != mouse_button::left) {
					return false;
				}

				this->mouse_captured = e.is_down;

				if (e.is_down) {
					this->capture_point = e.pos;
				} else {
					if (!mp.is_hovered()) {
						this->context.get().cursor_manager.pop(iter);
					}
				}
				return true;
			});
		};

	std::function<decltype(ruis::mouse_proxy::hovered_change_handler)(ruis::mouse_cursor, cursor_iter&)>
		make_hovered_change_handler = [this](ruis::mouse_cursor cursor, cursor_iter& iter) {
			return [this, cursor, &iter](mouse_proxy& mp, unsigned pointer_id) {
				// LOG("hover = " << mp.is_hovered() << std::endl)
				// LOG("this->mouse_captured = " << this->mouse_captured << std::endl)
				if (this->mouse_captured) {
					return;
				}
				if (mp.is_hovered()) {
					iter = this->context.get().cursor_manager.push(cursor);
				} else {
					this->context.get().cursor_manager.pop(iter);
				}
			};
		};

	{
		auto caption = this->try_get_widget_as<mouse_proxy>("ruis_caption_proxy");
		ASSERT(caption)

		caption->mouse_button_handler = make_mouse_button_handler(this->caption_cursor_iter);

		caption->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				this->move_by(e.pos - this->capture_point);
				return true;
			}
			return false;
		};
		caption->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::grab, this->caption_cursor_iter);
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_lt_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->lt_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::min;
				ruis::vector2 d = e.pos - this->capture_point;
				d = min(d, this->rect().d - this->empty_min_dim); // clamp top
				this->move_by(d);
				this->resize_by(-d);
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::top_left_corner, this->lt_border_cursor_iter);
		this->lt_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_lb_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->lb_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::min;
				using std::max;
				ruis::vector2 d = e.pos - this->capture_point;
				d.x() = min(d.x(), this->rect().d.x() - this->empty_min_dim.x()); // clamp top
				d.y() = max(d.y(), -(this->rect().d.y() - this->empty_min_dim.y())); // clamp bottom
				this->move_by(ruis::vector2(d.x(), 0));
				this->resize_by(ruis::vector2(-d.x(), d.y()));
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::bottom_left_corner, this->lb_border_cursor_iter);
		this->lb_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_rt_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->rt_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::min;
				using std::max;
				ruis::vector2 d = e.pos - this->capture_point;
				d.x() = max(d.x(), -(this->rect().d.x() - this->empty_min_dim.x())); // clamp bottom
				d.y() = min(d.y(), this->rect().d.y() - this->empty_min_dim.y()); // clamp top
				this->move_by(ruis::vector2(0, d.y()));
				this->resize_by(ruis::vector2(d.x(), -d.y()));
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::top_right_corner, this->rt_border_cursor_iter);
		this->rt_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_rb_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->rb_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::max;
				ruis::vector2 d = e.pos - this->capture_point;
				d = max(d, -(this->rect().d - this->empty_min_dim)); // clamp bottom
				this->resize_by(d);
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::bottom_right_corner, this->rb_border_cursor_iter);
		this->rb_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_l_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->l_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::min;
				ruis::vector2 d = e.pos - this->capture_point;
				d.x() = min(d.x(), this->rect().d.x() - this->empty_min_dim.x()); // clamp top
				this->move_by(ruis::vector2(d.x(), 0));
				this->resize_by(ruis::vector2(-d.x(), 0));
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::left_side, this->l_border_cursor_iter);
		this->l_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_r_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->r_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::max;
				ruis::vector2 d = e.pos - this->capture_point;
				d.x() = max(d.x(), -(this->rect().d.x() - this->empty_min_dim.x())); // clamp bottom
				this->resize_by(ruis::vector2(d.x(), 0));
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::right_side, this->r_border_cursor_iter);
		this->r_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_t_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->t_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::min;
				ruis::vector2 d = e.pos - this->capture_point;
				d.y() = min(d.y(), this->rect().d.y() - this->empty_min_dim.y()); // clamp top
				this->move_by(ruis::vector2(0, d.y()));
				this->resize_by(ruis::vector2(0, -d.y()));
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::top_side, this->t_border_cursor_iter);
		this->t_border = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("ruis_b_proxy");
		ASSERT(w)
		w->mouse_button_handler = make_mouse_button_handler(this->b_border_cursor_iter);
		w->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) {
			if (this->mouse_captured) {
				using std::max;
				ruis::vector2 d = e.pos - this->capture_point;
				d.y() = max(d.y(), -(this->rect().d.y() - this->empty_min_dim.y())); // clamp bottom
				this->resize_by(ruis::vector2(0, d.y()));
			}
			return false;
		};
		w->hovered_change_handler =
			make_hovered_change_handler(ruis::mouse_cursor::bottom_side, this->b_border_cursor_iter);
		this->b_border = w;
	}
}

void ruis::window::set_title(const std::string& str)
{
	this->title->set_text(utki::to_utf32(str));
}

void ruis::window::set_title(std::u32string str)
{
	this->title->set_text(str);
}

void ruis::window::set_borders(sides<length> borders)
{
	this->l_border->get_layout_params().dims.x() = borders.left();
	this->t_border->get_layout_params().dims.y() = borders.top();
	this->r_border->get_layout_params().dims.x() = borders.right();
	this->b_border->get_layout_params().dims.y() = borders.bottom();

	this->lb_border->get_layout_params().dims.x() = borders.left();
	this->lb_border->get_layout_params().dims.y() = borders.bottom();

	this->rb_border->get_layout_params().dims.x() = borders.right();
	this->rb_border->get_layout_params().dims.y() = borders.bottom();

	this->lt_border->get_layout_params().dims.x() = borders.left();
	this->lt_border->get_layout_params().dims.y() = borders.top();

	this->rt_border->get_layout_params().dims.x() = borders.right();
	this->rt_border->get_layout_params().dims.y() = borders.top();
}

bool ruis::window::on_mouse_button(const mouse_button_event& e)
{
	if (e.is_down && !this->is_topmost()) {
		this->context.get().post_to_ui_thread([this]() {
			this->move_to_top();
			this->focus();
		});
	}

	this->container::on_mouse_button(e);

	return true;
}

bool ruis::window::on_mouse_move(const mouse_move_event& e)
{
	this->container::on_mouse_move(e);
	return true;
}

void window::move_to_top()
{
	if (!this->parent()) {
		return;
	}

	ASSERT(this->parent()->children().size() != 0)

	if (this->parent()->children().size() == 1) {
		return;
	}

	auto& prev_topmost = this->parent()->children().rbegin()->get();
	if (&prev_topmost == this) {
		return; // already topmost
	}

	container* p = this->parent();

	p->change_child_z_position(p->find(*this), p->children().end());

	this->update_topmost();

	if (auto pt = dynamic_cast<window*>(&prev_topmost)) {
		pt->update_topmost();
	}
}

bool window::is_topmost() const noexcept
{
	if (!this->parent()) {
		return false;
	}

	ASSERT(this->parent()->children().size() != 0)

	return &this->parent()->children().back().get() == this;
}

void window::update_topmost()
{
	this->title_bg->set_color(this->is_topmost() ? this->title_bg_color_topmost : this->title_bg_color_non_topmost);
}

void window::on_lay_out()
{
	this->update_topmost();
	this->container::on_lay_out();
}
