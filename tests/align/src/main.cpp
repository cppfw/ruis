#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widgets/container.hpp>
#include <ruis/layouts/layout.hpp>
#include <ruis/widgets/label/text.hpp>
#include <ruis/widgets/label/rectangle.hpp>

using namespace std::string_literals;

namespace m = ruis::make;
using lp = ruis::lp;

namespace{
utki::shared_ref<ruis::widget> make_pile_layout(utki::shared_ref<ruis::context> c, ruis::widget::parameters widget_params){
	return m::container(
		c,
		{
			.widget_params = widget_params,
			.container_params = {
				.layout = ruis::layout::pile
			}
		},
		{
			m::rectangle(
				c,
				{
					.widget_params = {
						.lp = {
							.dims = {lp::fill, lp::fill}
						}
					},
					.color_params = {
						.color = 0xff808080
					}
				}
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::center, lp::align::center}
						}
					}
				},
				U"center"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::front, lp::align::center}
						}
					}
				},
				U"left"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::front, lp::align::front}
						}
					}
				},
				U"left-top"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::center, lp::align::front}
						}
					}
				},
				U"top"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::back, lp::align::front}
						}
					}
				},
				U"right-top"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::back, lp::align::center}
						}
					}
				},
				U"right"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::back, lp::align::back}
						}
					}
				},
				U"right-bottom"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::center, lp::align::back}
						}
					}
				},
				U"bottom"s
			),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.align = {lp::align::front, lp::align::back}
						}
					}
				},
				U"left-bottom"s
			)
		}
	);
}
}

namespace{
utki::shared_ref<ruis::widget> make_separator(utki::shared_ref<ruis::context> c, bool vertical){
	return m::rectangle(
		c,
		{
			.widget_params = {
				.lp = {
					.dims = {vertical ? lp::fill : ruis::length::make_px(1), vertical ? ruis::length::make_px(1) : lp::fill}
				}
			},
			.color_params = {
				.color = 0xffffffff
			}
		}
	);
}

utki::shared_ref<ruis::widget> make_linear_layout(utki::shared_ref<ruis::context> c, ruis::widget::parameters widget_params, bool vertical){
	return m::container(
		c,
		{
			.widget_params = widget_params,
			.container_params = {
				.layout = vertical ? ruis::layout::column : ruis::layout::row
			}
		},
		{
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::front, lp::align::front}
						}
					}
				},
				U"left-top"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::center, lp::align::front}
						}
					}
				},
				U"center-top"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::back, lp::align::front}
						}
					}
				},
				U"right-top"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::back, lp::align::center}
						}
					}
				},
				U"right-center"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::back, lp::align::back}
						}
					}
				},
				U"right-bottom"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::center, lp::align::back}
						}
					}
				},
				U"center-bottom"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::front, lp::align::back}
						}
					}
				},
				U"left-bottom"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::front, lp::align::center}
						}
					}
				},
				U"left-center"s
			),
			make_separator(c, vertical),
			m::text(
				c,
				{
					.widget_params = {
						.lp = {
							.weight = 1,
							.align = {lp::align::center, lp::align::center}
						}
					}
				},
				U"center"s
			)
		}
	);
}
}

namespace{
utki::shared_ref<ruis::widget> make_layout(utki::shared_ref<ruis::context> c){	
	return m::container(
		c,
		{
			.container_params = {
				.layout = ruis::layout::row
			}
		},
		{
			m::container(
				c,
				{
					.widget_params = {
						.lp = {
							.dims = {lp::fill, lp::fill},
							.weight = 2
						}
					},
					.container_params = {
						.layout = ruis::layout::column
					}
				},
				{
					make_pile_layout(
						c,
						{
							.lp = {
								.dims = {lp::fill, lp::fill},
								.weight = 1
							}
						}
					),
					make_linear_layout(
						c,
						{
							.lp = {
								.dims = {lp::fill, lp::fill},
								.weight = 1
							}
						},
						false // vertical=false
					)
				}
			),
			make_linear_layout(
				c,
				{
					.lp = {
						.dims = {lp::fill, lp::fill},
						.weight = 1
					}
				},
				true // vertical=true
			)
		}
	);
}
}

class application : public ruisapp::application{
public:
	application() :
			ruisapp::application("ruis-tests", [](){
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));

				return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		this->gui.set_root(make_layout(this->gui.context));
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
