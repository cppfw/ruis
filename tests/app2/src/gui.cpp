#include "gui.hpp"

#include <ruis/widget/button/base/push_button.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/margins.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/slider/slider.hpp>
#include <ruis/widget/button/check_box.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;
using lp = ruis::layout_parameters;

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
						.dims = {lp::fill, lp::min}
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
								.dims = {lp::fill, lp::min},
								.align = {lp::align::front, lp::align::front}
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
								.dims = {lp::fill, lp::min}
							},
							.oriented_params = {
								.vertical = false
							}
						}
					)
				}
			),
			m::push_button(c,
				{
					.container_params = {
						.layout = ruis::layout::pile
					}
				},
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
					// TODO: uncomment and fix the segfault
					// m::check_box(c,
					// 	{}
					// ),
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

utki::shared_ref<ruis::widget> make_gui(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::container(
		c,
		{
			.layout_params = {
				.dims = {lp::fill, lp::fill}
			},
			.container_params = {
				.layout = ruis::layout::trivial
			}
		},
		{
			make_sliders_window(c, {10, 20, 300, 200})
		}
	);
	// clang-format on
}
