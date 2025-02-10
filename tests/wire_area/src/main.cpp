#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <utki/unicode.hpp>
#include <papki/fs_file.hpp>

#include <ruis/widget/label/text.hpp>
#include <ruis/widget/group/window.hpp>

#include <ruisapp/application.hpp>

#include "wire_area.hpp"
#include "wire_socket.hpp"

using namespace std::string_literals;

namespace{
namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::widget> make_socket(
	utki::shared_ref<ruis::context> c, //
	std::string text,
	ruis::wire_socket::orientation outlet_orientation,
	bool in_socket
)
{
	// clang-format off
	return m::pile(c,
		{},
		{
			m::text(c,
				{
					.color_params{
						.color = in_socket ? 0xff0000ff : 0xff00ff00
					}
				},
				utki::to_utf32(text)
			),
			m::wire_socket(c,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill}
					},
					// .widget_params{
					// 	.id = "ws"s
					// }
					.wire_socket_params{
						.outlet_orientation = outlet_orientation
					}
				}
			)
		}
	);
	// clang-format on
}

utki::shared_ref<ruis::widget> make_root_widget(utki::shared_ref<ruis::context> c){
	// clang-format off
	return m::wire_area(c,
		{
			.widget_params{
				.id = "wire_area"s
			}
		},
		{
			m::window(c,
				{
					.layout_params{
						.dims{
							ruis::length::make_pp(100),
							ruis::length::make_pp(50)
						}
					},
					.widget_params{
						.rectangle{{0, 20}, {100, 50}}
					},
					.container_params{
						.layout = ruis::layout::pile
					},
					.title = U"window 1"s
				},
				{
					make_socket(c, "out1"s, ruis::wire_socket::orientation::top, false)
				}
			),
			m::window(c,
				{
					.layout_params{
						.dims{
							ruis::length::make_pp(100),
							ruis::length::make_pp(50)
						}
					},
					.widget_params{
						.rectangle{{150, 20}, {100, 50}}
					},
					.container_params{
						.layout = ruis::layout::pile
					},
					.title = U"window 2"s
				},
				{
					make_socket(c, "in2"s, ruis::wire_socket::orientation::right, true)
				}
			)
		}
	);
	// clang-format on
}
}

class application : public ruisapp::application{
public:
	application() :
			ruisapp::application("ruis-tests", [](){
				ruisapp::window_params wp(r4::vector2<unsigned>(640, 480));

				return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		auto c = make_root_widget(this->gui.context);

		this->gui.set_root(c);
	}
};

const ruisapp::application_factory app_fac([](auto executbale, auto args){
	return std::make_unique<::application>();
});
