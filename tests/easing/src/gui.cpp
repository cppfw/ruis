#include "gui.hpp"

#include <ruis/animation/eased_animation.hpp>
#include <ruis/animation/easing.hpp>
#include <ruis/widget/button/base/push_button.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/margins.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/slider/slider.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;
using lp = ruis::layout_parameters;

namespace {
constexpr uint32_t animation_duration_ms = 1000;
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_eased_animation_sample( //
	utki::shared_ref<ruis::context> c,
	ruis::layout_parameters layout_params,
	std::function<ruis::real(ruis::real)> easing,
	std::string name
)
{
	// clang-format off
	auto ret = m::margins(c,
		{
			.layout_params = std::move(layout_params),
			.container_params = {
				.layout = ruis::layout::column
			},
			.frame_params = {
				.borders = {10_pp, 5_pp, 30_pp, 2_pp}
			}
		},
		{
			m::slider(c,
				{
					.layout_params = {
						.dims = {lp::fill, lp::min}
					},
					.widget_params = {
						.id = "slider"s
					},
					.oriented_params = {
						.vertical = false
					}
				}
			),
			m::push_button(c,
				{
					.widget_params = {
						.id = "button"s
					},
					.container_params = {
						.layout = ruis::layout::pile
					}
				},
				{
					m::text(c,
						{},
						utki::to_utf32(name)
					)
				}
			)
		}
	);
	// clang-format on

	auto& slider = ret.get().get_widget_as<ruis::fraction_widget>("slider"sv);
	auto& button = ret.get().get_widget_as<ruis::push_button>("button"sv);

	auto animation = utki::make_shared<ruis::eased_animation>(
		button.context.get().updater,
		animation_duration_ms,
		std::move(easing)
	);

	animation.get().update_handler = [&slider = slider](ruis::real factor) {
		slider.set_fraction(factor);
	};
	animation.get().end_handler = [&slider = slider](uint32_t over_ms) {
		slider.set_fraction(1);
	};

	button.click_handler = [anim = std::move(animation)](ruis::push_button& b) {
		anim.get().reset();
		anim.get().start();
	};

	return ret;
}
} // namespace

namespace {
utki::shared_ref<ruis::window> make_sliders_window(utki::shared_ref<ruis::context> c, ruis::rect rect)
{
	auto make_anim_sample =
		[](utki::shared_ref<ruis::context> c, std::function<ruis::real(ruis::real)> easing, std::string name) {
			return make_eased_animation_sample(
				c,
				{
					.dims = {lp::fill, lp::min}
            },
				std::move(easing),
				std::move(name)
			);
		};

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
			make_anim_sample(c,
				ruis::easing::linear,
				"linear"
			),
			make_anim_sample(c,
				ruis::easing::in_sine,
				"in sine"
			),
			make_anim_sample(c,
				ruis::easing::out_sine,
				"out sine"
			),
			make_anim_sample(c,
				ruis::easing::in_out_sine,
				"in out sine"
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
