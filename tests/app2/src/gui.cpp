#include "gui.hpp"

#include <ruis/widget/container.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/slider/slider.hpp>

using namespace std::string_literals;

namespace m = ruis::make;
using lp = ruis::lp;

utki::shared_ref<ruis::widget> make_gui(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::container(
		c,
		{
			.widget_params = {
				.lp = {
					.dims = {lp::fill, lp::fill}
				}
			},
			.container_params = {
				.layout = ruis::layout::trivial
			}
		},
		{
			m::window(c,
				{
					.widget_params = {
						.rectangle = {10, 20, 300, 200}
					},
					.container_params = {
						.layout = ruis::layout::column
					},
					.title = U"sliders"s
				},
				{
					m::scroll_bar(c,
						{
							.widget_params = {
								.lp = {
									.dims = {lp::fill, lp::min},
									.align = {lp::align::front, lp::align::front}
								}
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
							.widget_params ={
								.lp = {
									.dims = {lp::fill, lp::min},

								}
							},
							.oriented_params = {
								.vertical = false
							}
						}
					)
				}
			)
		}
	);
	// clang-format on
}
