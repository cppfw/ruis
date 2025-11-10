#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/widget.hpp>
#include <ruis/paint/path_vao.hpp>
#include <ruis/paint/frame_vao.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;

class path_widget : virtual public ruis::widget{
	ruis::path_vao vao;
public:
	struct all_parameters{
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget_params;
	};

	path_widget(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	) :
		widget(std::move(context), std::move(params.layout_params), std::move(params.widget_params)),
		vao(this->context.get().renderer)
	{}

	void render(const ruis::matrix4& matrix)const override{
		this->vao.render(matrix, 0xff00ffff);
	}

	void on_resize()override{
		ruis::path path;
		path.line_to(this->rect().d / 2);
		path.cubic_by(
				ruis::vector2(this->rect().d.x() / 2, 0),
				ruis::vector2(0, this->rect().d.y() / 2),
				this->rect().d / 2
			);
		this->vao.set(path.stroke());
	}
};

namespace make{
inline utki::shared_ref<::path_widget> path_widget(
	utki::shared_ref<ruis::context> context,
	::path_widget::all_parameters params
)
{
	return utki::make_shared<::path_widget>(
		std::move(context),
		std::move(params)
	);
}
}

class frame_widget : virtual public ruis::widget{
	ruis::frame_vao vao;
public:
	struct all_parameters{
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget_params;
	};

	frame_widget(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	) :
		widget(std::move(context), std::move(params.layout_params), std::move(params.widget_params)),
		vao(this->context.get().renderer)
	{}

	void render(const ruis::matrix4& matrix)const override{
		this->vao.render(matrix, 0xffff8080);
	}

	void on_resize()override{
		this->vao.set(
				this->rect().d,
				ruis::vector2{10, 20}
			);
	}
};

namespace make{
inline utki::shared_ref<::frame_widget> frame_widget(
	utki::shared_ref<ruis::context> context,
	::frame_widget::all_parameters params
)
{
	return utki::make_shared<::frame_widget>(
		std::move(context),
		std::move(params)
	);
}
}

namespace m{
using namespace ruis::make;
using namespace ::make;
}

utki::shared_ref<ruis::widget> make_root_widget(utki::shared_ref<ruis::context> c){
	// clang-format off
	return m::pile(c,
		{},
		{
			m::path_widget(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					}
				}
			),
			m::padding(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.container_params{
						.layout = ruis::layout::pile
					},
					.padding_params{
						.borders = {
							ruis::length::make_pp(5),
							ruis::length::make_pp(5),
							ruis::length::make_pp(5),
							ruis::length::make_pp(5)
						}
					}
				},
				{
					m::frame_widget(c,
						{
							.layout_params{
								.dims = {ruis::dim::fill, ruis::dim::fill}
							}
						}
					)
				}
			),
			m::scroll_bar(c,
				{
					.layout_params{
						.dims = {ruis::dim::min, ruis::dim::max}
					},
					.oriented_params{
						.vertical = true
					}
				}
			),
			m::scroll_bar(c,
				{
					.layout_params{
						.dims = {ruis::dim::max, ruis::dim::min}
					},
					.oriented_params{
						.vertical = false
					}
				}
			),
			m::push_button(c,
				{},
				{
					m::text(c,
						{},
						U"stuff"s
					)
				}
			)
		}
	);
	// clang-format on
}

class application : public ruisapp::application{
	ruisapp::window& window;
public:
	application() :
			ruisapp::application({
				.name = "ruis-tests"
			}),
			window(this->make_window({
					.dims = {1024, 800}
				}))
	{
		this->window.gui.context.get().window().close_handler = [this](){
			this->quit();
		};

		this->window.gui.init_standard_widgets(this->get_res_file("../../res/ruis_res/"));

		this->window.gui.set_root(make_root_widget(this->window.gui.context));
	}
};

const ruisapp::application_factory app_fac([](auto executbale, auto args){
	return std::make_unique<::application>();
});
