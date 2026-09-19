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

#include "scroll_area_page.hpp"

#include <ruis/res/tml.hpp>
#include <ruis/standard_widgets.hpp>
#include <ruis/style/style_sheet.hpp>
#include <ruis/widget/button/impl/rectangle_push_button.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <ruis/widget/group/touch/dialog.hpp>
#include <ruis/widget/group/touch/scroll_area.hpp>
#include <ruis/widget/input/labeled_text_field.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

#include "style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace {

void apply_theme(
	ruis::context& c, //
	ruis::theme theme
)
{
	auto style_res = c.loader().load<ruis::res::tml>(ruis::to_resource_id(theme));
	c.style().set(utki::make_shared<ruis::style_sheet>(style_res.get().forest));
}

class theme_selection_provider : public ruis::list_provider
{
	std::vector<std::u32string> items;

public:
	theme_selection_provider(const utki::shared_ref<ruis::context>& context) :
		ruis::list_provider(context),
		items{U"Dark"s, U"Light"s}
	{}

	size_t count() const noexcept override
	{
		return this->items.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		// clang-format off
		return m::text(this->context,
			{
				.params{
					.color{
						.normal = this->context.get().style().get_color_text()
					}
				}
			},
			this->items.at(index)
		);
		// clang-format on
	}

	utki::shared_ref<ruis::widget> get_highlighted_widget(size_t index) override
	{
		// clang-format off
		return m::text(this->context,
			{
				.params{
					.color{
						.normal = this->context.get().style().get_color_highlight()
					}
				}
			},
			this->items.at(index)
		);
		// clang-format on
	}
};

utki::shared_ref<ruis::touch::dialog> make_dialog(const utki::shared_ref<ruis::context>& c)
{
	// clang-format off
	auto text_field = m::labeled_text_field(
		c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::min}
			},
			.params{
				.label{
					.string = U"Name"s
				},
				.text_input{
					.specific{
						.hint = U"Enter your name"s
					}
				}
			}
		},
		ruis::string()
	);
	// clang-format on

	// clang-format off
	auto close_button = m::push_button(
		c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::min}
			}
		},
		{
			m::text(c, {}, U"Close"s)
		}
	);
	// clang-format on

	auto& close_button_ref = close_button.get();

	// clang-format off
	auto dialog = ruis::touch::make::dialog(
		c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		},
		{
			m::text(c, {}, U"Dialog"s),
			m::gap(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, c.get().style().get_len_gap_small().get()}
					}
				}
			),
			std::move(text_field),
			m::gap(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, c.get().style().get_len_gap_small().get()}
					}
				}
			),
			std::move(close_button)
		}
	);
	// clang-format on

	// use a weak reference to avoid a reference cycle
	// (dialog -> content container -> close_button -> click_handler -> dialog)
	auto dialog_weak = utki::make_weak_from(dialog.get());
	close_button_ref.click_handler = [dialog_weak](ruis::push_button&) {
		if (auto dlg = dialog_weak.lock()) {
			dlg->close();
		}
	};

	return dialog;
}

ruis::widget_list make_scroll_area_page_contents(const utki::shared_ref<ruis::context>& c)
{
	// clang-format off
	auto theme_selector = m::selection_box(c,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			},
			.params{
				.selection_box{
					.list{
						.provider = utki::make_shared<theme_selection_provider>(c)
					}
				},
				.specific{
					.title = U"Theme"s // TODO: localize
				}
			}
		}
	);
	// clang-format on

	theme_selector.get().set_selection(size_t(ruis::theme::dark));
	theme_selector.get().selection_handler = [](ruis::selection_box& sb) {
		apply_theme(
			sb.context.get(), //
			ruis::theme(sb.get_selection())
		);
	};

	// clang-format off
	auto button_1 = m::push_button(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, 200_pp}
			}
		},
		{
			m::text(c, {}, U"Button 1"s)
		}
	);
	// clang-format on

	button_1.get().click_handler = [](ruis::push_button& b) {
		auto dialog = make_dialog(b.context);

		auto& olay = b.get_ancestor<ruis::overlay>();
		b.context.get().post_to_ui_thread([olay = utki::make_shared_from(olay), dialog]() {
			olay.get().push_back(dialog);
		});
	};

	// clang-format off
	return {
		theme_selector,
		ruis::make::gap(c,
			{
				.layout_params{
					.dims = {ruis::dim::fill, c.get().style().get_len_gap_small().get()}
				}
			}
		),
		button_1,
		ruis::make::gap(c,
			{
				.layout_params{
					.dims = {ruis::dim::fill, c.get().style().get_len_gap_small().get()}
				}
			}
		),
		m::scroll_bar(c,
			{
				.layout_params{
					.dims{50_pp, 1000_pp}
				},
				.fraction = ruis::real(0.25),
				.band_fraction = ruis::real(0.2),
				.oriented_params{
					.vertical = true
				}
			}
		),
		ruis::make::gap(c,
			{
				.layout_params{
					.dims = {ruis::dim::fill, c.get().style().get_len_gap_small().get()}
				}
			}
		),
		m::text(c, {}, U"some text"s)
	};
	// clang-format on
}

class scroll_area_page :
	public ruis::page, //
	private ruis::touch::scroll_area
{
public:
	scroll_area_page(const utki::shared_ref<ruis::context>& context) :
		// clang-format off
		ruis::widget(
			context,
			{},
			{
				.clip = true
			}
		),
		// clang-format on
		ruis::page(context, {}),
		// clang-format off
		ruis::touch::scroll_area(
			context,
			{},
			{
				m::padding(context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim::min}
						},
						.params{
							.container{
								.layout = ruis::layout::column
							},
							.specific{
								.borders = {context.get().style().get_len_gap_small()}
							}
						}
					},
					make_scroll_area_page_contents(context)
				)
			}
		)
	// clang-format on
	{}
};

} // namespace

utki::shared_ref<ruis::page> make_scroll_area_page(const utki::shared_ref<ruis::context>& c)
{
	return utki::make_shared<scroll_area_page>(c);
}
