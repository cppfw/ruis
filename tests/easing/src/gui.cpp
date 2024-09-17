#include "gui.hpp"

#include <ruis/animation/eased_animation.hpp>
#include <ruis/animation/easing.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/proxy/key_proxy.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/slider/slider.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

namespace {
constexpr uint32_t animation_duration_ms = 1000;
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_eased_animation_sample( //
	utki::shared_ref<ruis::context> c,
	std::function<ruis::real(ruis::real)> easing,
	std::string name
)
{
	// clang-format off
	auto ret = m::margins(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::min}
			},
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
						.dims = {ruis::dim::fill, ruis::dim::min}
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
		anim.get().stop();
		anim.get().rewind();
		anim.get().start();
	};

	return ret;
}
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_left_column(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::column(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill},
				.weight = 1
			}
		},
		{
			make_eased_animation_sample(c,
				ruis::easing::linear,
				"linear"
			),
			// sine
			make_eased_animation_sample(c,
				ruis::easing::in_sine,
				"in sine"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_sine,
				"out sine"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_sine,
				"in out sine"
			),
			// quadratic
			make_eased_animation_sample(c,
				ruis::easing::in_quadratic,
				"in quadratic"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_quadratic,
				"out quadratic"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_quadratic,
				"in out quadratic"
			),
			// cubic
			make_eased_animation_sample(c,
				ruis::easing::in_cubic,
				"in cubic"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_cubic,
				"out cubic"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_cubic,
				"in out cubic"
			),
			// quartic
			make_eased_animation_sample(c,
				ruis::easing::in_quartic,
				"in quartic"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_quartic,
				"out quartic"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_quartic,
				"in out quartic"
			),
			// quintic
			make_eased_animation_sample(c,
				ruis::easing::in_quintic,
				"in quintic"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_quintic,
				"out quintic"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_quintic,
				"in out quintic"
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_right_column(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::column(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill},
				.weight = 1
			}
		},
		{
			// exponential
			make_eased_animation_sample(c,
				ruis::easing::in_exponent,
				"in exponent"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_exponent,
				"out exponent"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_exponent,
				"in out exponent"
			),
			// circular
			make_eased_animation_sample(c,
				ruis::easing::in_circular,
				"in circular"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_circular,
				"out circular"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_circular,
				"in out circular"
			),
			// back
			make_eased_animation_sample(c,
				ruis::easing::in_back,
				"in back"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_back,
				"out back"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_back,
				"in out back"
			),
			// elastic
			make_eased_animation_sample(c,
				ruis::easing::in_elastic,
				"in elastic"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_elastic,
				"out elastic"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_elastic,
				"in out elastic"
			),
			// bounce
			make_eased_animation_sample(c,
				ruis::easing::in_bounce,
				"in bounce"
			),
			make_eased_animation_sample(c,
				ruis::easing::out_bounce,
				"out bounce"
			),
			make_eased_animation_sample(c,
				ruis::easing::in_out_bounce,
				"in out bounce"
			)
		}
	);
	// clang-format on
}
} // namespace

utki::shared_ref<ruis::widget> make_gui(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::row(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		},
		{
			make_left_column(c),
			make_right_column(c)
		}
	);
	// clang-format on
}
