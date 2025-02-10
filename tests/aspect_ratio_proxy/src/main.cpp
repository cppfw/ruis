#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/group/window.hpp>
#include <ruis/widget/proxy/aspect_ratio_proxy.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/label/rectangle.hpp>

using namespace std::string_literals;

namespace m{
using namespace ruis::make;
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

		// clang-format off
		auto c = m::container(this->gui.context,
			{
				.container_params{
					.layout = ruis::layout::trivial
				}
			},
			{
				m::window(this->gui.context,
					{
						.widget_params{
							.rectangle = {{200, 200}, {200, 100}}
						},
						.container_params{
							.layout = ruis::layout::pile
						},
						.title = U"aspect ratio proxy"s
					},
					{
						m::pile(this->gui.context,
							{
								.layout_params{
									.dims = {ruis::dim::min, ruis::dim::fill}
								}
							},
							{
								m::aspect_ratio_proxy(this->gui.context,
									{
										.layout_params{
											.dims = {ruis::dim::min, ruis::dim::fill}
										},
										.aspect_ratio_params{
											.x_above_y = 0.5
										}
									}
								),
								m::rectangle(this->gui.context,
									{
										.layout_params{
											.dims = {ruis::dim::fill, ruis::dim::fill}
										},
										.color_params{
											.color = 0xff008080
										}
									}
								),
								m::text(this->gui.context,
									{},
									U"1:2"s
								)
							}
						)
					}
				)
			}
		);
		// clang-format on

		this->gui.set_root(c);
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
