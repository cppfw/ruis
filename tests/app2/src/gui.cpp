#include "gui.hpp"

#include <ruis/widget/button/impl/check_box.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/button/selection_box.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/proxy/resize_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/slider/slider.hpp>
#include <ruisapp/application.hpp>

#include "rectangles_window.hpp"
#include "table_list_window.hpp"
#include "table_tree_view_window.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

namespace {
utki::shared_ref<ruis::window> make_sliders_window(
	utki::shared_ref<ruis::context> c, //
	ruis::vec2_length pos
)
{
	// clang-format off
	return m::window(c,
		{
			.widget_params = {
				.rectangle = {
					{
						pos.x().get(c),
						pos.y().get(c)
					},
					{
						ruis::length::make_pp(300).get(c),
						ruis::length::make_pp(200).get(c)
					}
				}
			},
			.container_params = {
				.layout = ruis::layout::column
			},
			.title = c.get().localization.get().get("sliders"sv)
		},
		{
			m::padding(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					.container_params = {
						.layout = ruis::layout::column
					},
					.padding_params = {
						.borders = {10_pp, 20_pp, 30_pp, 40_pp}
					}
				},
				{
					m::scroll_bar(c,
						{
							.layout_params = {
								.dims = {ruis::dim::fill, ruis::dim::min},
								.align = {ruis::align::front, ruis::align::front}
							},
							.fraction_band_params = {
								.band_fraction = 0.2
							},
							.oriented_params = {
								.vertical = false
							}
						}
					),
					m::slider(c,
						{
							.layout_params = {
								.dims = {ruis::dim::fill, ruis::dim::min}
							},
							.oriented_params = {
								.vertical = false
							}
						}
					)
				}
			),
			m::push_button(c,
				{},
				{
					m::text(c,
						{},
						c.get().localization.get().get("hello_world"sv)
					)
				}
			),
			m::nine_patch(c,
				{
					.nine_patch_params{
						.nine_patch = c.get().loader().load<ruis::res::nine_patch>("ruis_npt_window_bg")
					}
				},
				{
					m::nine_patch(c,
						{
							.nine_patch_params{
								.nine_patch = c.get().loader().load<ruis::res::nine_patch>("ruis_npt_button_normal")
							}
						},
						{
							m::gap(c,
								{
									.layout_params{
										.dims{10_pp, 10_pp}
									}
								}
							)
						}
					)
				}
			),
			m::row(c,
				{},
				{
					m::check_box(c,
						{}
					),
					m::text(c,
						{},
						c.get().localization.get().get("some_checkbox"sv)
					)
				}
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::window> make_image_window(
	utki::shared_ref<ruis::context> c, //
	ruis::vec2_length pos
)
{
	// clang-format off
	auto ret = m::window(c,
		{
			.widget_params = {
				.rectangle = {
					{
						pos.x().get(c),
						pos.y().get(c)
					},
					{
						ruis::length::make_pp(300).get(c),
						ruis::length::make_pp(200).get(c)
					}
				}
			},
			.container_params = {
				.layout = ruis::layout::pile
			},
			.title = c.get().localization.get().get("image"sv)
		},
		{
			m::image(c,
				{
					.layout_params = {
						.dims = {ruis::dim::min, ruis::dim::fill}
					},
					.widget_params = {
						.id = "image"s
					},
					.image_params = {
						.img = c.get().loader().load<ruis::res::image>("img_home"sv),
						.keep_aspect_ratio = true
					}
				}
			),
			m::resize_proxy(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params = {
						.id = "resize_proxy"s
					}
				}
			)
		}
	);
	// clang-format on

	auto& rp = ret.get().get_widget_as<ruis::resize_proxy>("resize_proxy"sv);

	rp.resize_handler = [&im = ret.get().get_widget("image"sv)](ruis::resize_proxy& rp) {
		std::cout << "image size = " << im.rect().d << std::endl;
	};

	return ret;
}
} // namespace

namespace {
class selection_box_provider : public ruis::list_provider
{
	std::vector<std::string> items;

public:
	selection_box_provider(
		utki::shared_ref<ruis::context> context, //
		std::vector<std::string> items
	) :
		list_provider(std::move(context)),
		items(std::move(items))
	{}

	size_t count() const noexcept override
	{
		return this->items.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		auto i = utki::next(this->items.begin(), index);
		ASSERT(i < this->items.end())
		return m::text(
			this->context, //
			{},
			utki::to_utf32(*i)
		);
	}
};
} // namespace

namespace {
constexpr const std::array<std::pair<std::string_view, std::u32string_view>, 3> language_id_to_name_mapping{
	{
     {"en"sv, U"English"sv},
     {"fi"sv, U"Suomi"sv},
     {"ru"sv, U"Русский"sv},
	 }
};
} // namespace

namespace {
class language_selection_provider : public ruis::list_provider
{
public:
	language_selection_provider(utki::shared_ref<ruis::context> context) :
		list_provider(std::move(context))
	{}

	size_t count() const noexcept override
	{
		return language_id_to_name_mapping.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		return m::text(
			this->context, //
			{},
			std::u32string(language_id_to_name_mapping.at(index).second)
		);
	}
};
} // namespace

namespace {
utki::shared_ref<ruis::window> make_selection_box_window(
	utki::shared_ref<ruis::context> c, //
	ruis::vec2_length pos
)
{
	// clang-format off
	auto lang_sel_box = m::selection_box(c,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			},
			.list_params{
				.provider = utki::make_shared<language_selection_provider>(c)
			}
		}
	);
	// clang-format on

	lang_sel_box.get().selection_handler = [](ruis::selection_box& sb) {
		auto sel = sb.get_selection();

		// std::cout << "localization selection changed = " << sel << std::endl;

		ASSERT(sel < language_id_to_name_mapping.size())

		sb.context.get().post_to_ui_thread([lng = language_id_to_name_mapping.at(sel).first]() {
			auto& app = ruisapp::inst();

			// std::cout << "new localization = " << lng << std::endl;

			app.gui.context.get().localization.get() =
				ruis::localization(tml::read(*app.get_res_file(utki::cat("res/localization/", lng, ".tml"))));
			app.gui.get_root().reload();
		});
	};

	// clang-format off
	return m::window(c,
		{
			.widget_params = {
				.rectangle = {
					{
						pos.x().get(c),
						pos.y().get(c)
					},
					{
						ruis::length::make_pp(300).get(c),
						ruis::length::make_pp(200).get(c)
					}
				}
			},
			.container_params = {
				.layout = ruis::layout::column
			},
			.title = c.get().localization.get().get("selection_box"sv)
		},
		{
			m::selection_box(c,
				{
					.layout_params = {
						.dims = {ruis::dim::max, ruis::dim::min}
					},
					.list_params = {
						.provider = utki::make_shared<selection_box_provider>(c,
							std::vector<std::string>{
								"Hello"s,
								"World!"s
							}
						)
					}
				}
			),
			m::text(c,
				{
					.layout_params{
						.align = {ruis::align::front, ruis::align::center}
					}
				},
				c.get().localization.get().get("language"sv)
			),
			std::move(lang_sel_box),
			m::text(c,
				{},
				c.get().localization.get().get("formatted_str"sv).format({U"world"s, U"Hello"s})
			)
		}
	);
	// clang-format on
}
} // namespace

utki::shared_ref<ruis::widget> make_root_widgets_structure(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::overlay(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		},
		{
			m::container(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.container_params = {
						.layout = ruis::layout::trivial
					}
				},
				{
					make_sliders_window(c, {10_pp, 20_pp}),
					make_image_window(c, {310_pp, 20_pp}),
					make_selection_box_window(c, {630_pp, 20_pp}),

					make_rectangles_window(c, {10_pp, 230_pp}),
					make_table_list_window(c, {310_pp, 230_pp}),
					make_table_tree_view_window(c, {630_pp, 230_pp})
				}
			)
		}
	);
	// clang-format on
}
