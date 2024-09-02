#include "gui.hpp"

#include <ruis/widget/button/base/push_button.hpp>
#include <ruis/widget/button/base/selection_box.hpp>
#include <ruis/widget/button/check_box.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/margins.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/proxy/resize_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/slider/slider.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

namespace {
utki::shared_ref<ruis::window> make_sliders_window(utki::shared_ref<ruis::context> c, ruis::rect rect)
{
	// clang-format off
	return m::window(c,
		{
			.widget_params = {
				.rectangle = rect
			},
			.container_params = {
				.layout = ruis::layout::column
			},
			.title = U"sliders"s
		},
		{
			m::margins(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					.container_params = {
						.layout = ruis::layout::column
					},
					.frame_params = {
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
						U"Hello world!"s
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
						U"some checkbox"s
					)
				}
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::window> make_image_window(utki::shared_ref<ruis::context> c, ruis::rect rect)
{
	// clang-format off
	auto ret = m::window(c,
		{
			.widget_params = {
				.rectangle = rect
			},
			.container_params = {
				.layout = ruis::layout::pile
			},
			.title = U"image"s
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
						.img = c.get().loader.load<ruis::res::image>("img_home"sv),
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
utki::shared_ref<ruis::window> make_selection_box_window(utki::shared_ref<ruis::context> c, ruis::rect rect)
{
	// clang-format off
	return m::window(c,
		{
			.widget_params = {
				.rectangle = rect
			},
			.container_params = {
				.layout = ruis::layout::column
			},
			.title = U"selection box"s
		},
		{
			m::selection_box(c,
				{
					.layout_params = {
						.dims = {ruis::dim::max, ruis::dim::min}
					},
					.selection_params = {

					}
				}
			)
		}
	);
	// clang-format on
}
} // namespace

utki::shared_ref<ruis::widget> make_gui(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::container(
		c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.container_params = {
				.layout = ruis::layout::trivial
			}
		},
		{
			make_sliders_window(c, {10, 20, 300, 200}),
			make_image_window(c, {310, 20, 300, 200}),
			make_selection_box_window(c, {630, 20, 300, 200})
		}
	);
	// clang-format on
}
