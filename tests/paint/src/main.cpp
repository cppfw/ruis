#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/widget.hpp>
#include <ruis/paint/path_vao.hpp>
#include <ruis/paint/frame_vao.hpp>
#include <ruis/paint/ellipse_vao.hpp>
#include <ruis/paint/rectangle_vao.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/standard_widgets.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

class path_widget : virtual public ruis::widget{
	ruis::paint::path_vao vao;
public:
	struct all_parameters{
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
	};

	path_widget(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	) :
		widget(std::move(context), std::move(params.layout_params), std::move(params.widget_params)),
		vao(this->context.get().renderer)
	{}

	void render(const ruis::mat4& matrix)const override{
		this->vao.render(matrix, 0xff00ffff);
	}

	void on_resize()override{
		ruis::paint::path path;
		path.line_to(this->rect().d / 2);
		path.cubic_by(
				ruis::vec2(this->rect().d.x() / 2, 0),
				ruis::vec2(0, this->rect().d.y() / 2),
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
	ruis::paint::frame_vao vao;
public:
	struct all_parameters{
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
	};

	frame_widget(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	) :
		widget(std::move(context), std::move(params.layout_params), std::move(params.widget_params)),
		vao(this->context.get().renderer)
	{}

	void render(const ruis::mat4& matrix)const override{
		this->vao.render(matrix, 0xffff8080);
	}

	void on_resize()override{
		this->vao.set(
				this->rect().d,
				ruis::vec2{10, 20}
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

class ellipse_widget : virtual public ruis::widget{
	ruis::paint::ellipse_vao vao;
public:
	struct all_parameters{
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
	};

	ellipse_widget(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	) :
		widget(
			std::move(context), //
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		vao(this->context.get().renderer)
	{}

	void render(const ruis::mat4& matrix)const override{
		this->vao.render(
			matrix, //
			0xff80ff80
		);
	}

	void on_resize()override{
		this->vao.set(this->rect().d);
	}
};

namespace make{
inline utki::shared_ref<::ellipse_widget> ellipse_widget(
	utki::shared_ref<ruis::context> context,
	::ellipse_widget::all_parameters params
)
{
	return utki::make_shared<::ellipse_widget>(
		std::move(context),
		std::move(params)
	);
}
}

class rectangle_widget : virtual public ruis::widget{
	ruis::paint::rectangle_vao vao;
public:
	struct all_parameters{
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
	};

	rectangle_widget(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	) :
		widget(
			std::move(context), //
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		vao(this->context.get().renderer)
	{}

	void render(const ruis::mat4& matrix)const override{
		this->vao.render(
			matrix, //
			this->rect().d, //
			0xff8080ff
		);
	}

	void on_resize()override{
		this->vao.set(
			{
				10,
				20,
				30,
				40
			},
			0
		);
	}
};

namespace make{
inline utki::shared_ref<::rectangle_widget> rectangle_widget(
	utki::shared_ref<ruis::context> context,
	::rectangle_widget::all_parameters params
)
{
	return utki::make_shared<::rectangle_widget>(
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
			m::ellipse_widget(c,
				{
					.layout_params{
						.dims = {400_pp, 200_pp},
						.align = {ruis::align::front, ruis::align::front}
					}
				}
			),
			m::padding(c,
				{
					.layout_params{
						.dims = {300_pp, 150_pp},
						.align = {ruis::align::front, ruis::align::back}
					},
					.container_params{
						.layout = ruis::layout::pile
					},
					.padding_params{
						// gaps to the left and bottom window edges
						.borders = {
							ruis::length::make_pp(40),
							ruis::length::make_pp(0),
							ruis::length::make_pp(0),
							ruis::length::make_pp(40)
						}
					}
				},
				{
					m::rectangle_widget(c,
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

		ruis::init_standard_widgets(
			this->window.gui.context, //
			this->get_res_file("../../res/ruis_res/")
		);

		this->window.gui.set_root(make_root_widget(this->window.gui.context));
	}
};

const ruisapp::application_factory app_fac([](auto executbale, auto args){
	return std::make_unique<::application>();
});
