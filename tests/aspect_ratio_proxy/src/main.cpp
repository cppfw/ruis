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
	ruisapp::window& window;
public:
	application() :
			ruisapp::application({
				.name = "ruis-tests"}
			),
			window(this->make_window({
					.dims = {1024, 800}
				}))
	{
		this->window.gui.context.get().window().close_handler = [this](){
			this->quit();
		};

		this->window.gui.init_standard_widgets(this->get_res_file("../../res/ruis_res/").get());

		auto ctx = this->window.gui.context;

		// clang-format off
		auto c = m::container(ctx,
			{
				.container_params{
					.layout = ruis::layout::trivial
				}
			},
			{
				m::window(ctx,
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
						m::pile(ctx,
							{
								.layout_params{
									.dims = {ruis::dim::min, ruis::dim::fill}
								}
							},
							{
								m::aspect_ratio_proxy(ctx,
									{
										.layout_params{
											.dims = {ruis::dim::min, ruis::dim::fill}
										},
										.aspect_ratio_params{
											.x_above_y = 0.5
										}
									}
								),
								m::rectangle(ctx,
									{
										.layout_params{
											.dims = {ruis::dim::fill, ruis::dim::fill}
										},
										.color_params{
											.color = 0xff008080
										}
									}
								),
								m::text(ctx,
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

		this->window.gui.set_root(c);
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
