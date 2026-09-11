#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/widget.hpp>
#include <ruis/paint/path_vao.hpp>
#include <ruis/paint/ellipse_vao.hpp>
#include <ruis/paint/rectangle_vao.hpp>
#include <ruis/paint/capsule_vao.hpp>
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
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	) :
		widget(
			context,//
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		vao(context.get().renderer)
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
	const utki::shared_ref<ruis::context>& context,
	::path_widget::all_parameters params
)
{
	return utki::make_shared<::path_widget>(
		context,
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
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	) :
		widget(
			context, //
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		vao(context.get().renderer)
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
	const utki::shared_ref<ruis::context>& context,
	::ellipse_widget::all_parameters params
)
{
	return utki::make_shared<::ellipse_widget>(
		context,
		std::move(params)
	);
}
}

class rectangle_widget : virtual public ruis::widget{
	ruis::paint::rectangle_vao vao;
	ruis::color_widget::parameters color_params;
public:
	struct all_parameters{
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
		ruis::color_widget::parameters color_params;

		ruis::paint::rectangle_vao::parameters rectangle_vao_params;
	};

	rectangle_widget(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	) :
		widget(
			context, //
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		vao(
			context.get().renderer, //
			std::move(params.rectangle_vao_params)
		),
		color_params(std::move(params.color_params))
	{}

	void render(const ruis::mat4& matrix)const override{
		this->vao.render(
			matrix, //
			this->rect().d, //
			this->color_params.color.get()
		);
	}
};

namespace make{
inline utki::shared_ref<::rectangle_widget> rectangle_widget(
	const utki::shared_ref<ruis::context>& context,
	::rectangle_widget::all_parameters params
)
{
	return utki::make_shared<::rectangle_widget>(
		context,
		std::move(params)
	);
}
}

class capsule_widget : virtual public ruis::widget{
	ruis::paint::capsule_vao vao;
	bool is_vertical;
	ruis::length stroke_width;
	ruis::color color;
public:
	struct all_parameters{
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
		bool is_vertical = false;
		ruis::length stroke_width;
		ruis::color color;
	};

	capsule_widget(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	) :
		widget(
			context, //
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		vao(
			context.get().renderer, //
			0,
			0
		),
		is_vertical(params.is_vertical),
		stroke_width(params.stroke_width),
		color(params.color)
	{}

	void render(const ruis::mat4& matrix)const override{
		this->vao.render(
			matrix, //
			this->color,
			this->is_vertical ? this->rect().d.y() : this->rect().d.x(),
			this->is_vertical
		);
	}

	void on_resize()override{
		// diameter is the non-longitudinal dimension of the widget
		auto d = this->rect().d;
		this->vao.set(
			this->is_vertical ? d.x() : d.y(), //
			this->stroke_width.is_undefined()? 0 :
			this->stroke_width.get(this->context)
		);
	}
};

namespace make{
inline utki::shared_ref<::capsule_widget> capsule_widget(
	const utki::shared_ref<ruis::context>& context,
	::capsule_widget::all_parameters params
)
{
	return utki::make_shared<::capsule_widget>(
		context,
		std::move(params)
	);
}
}

namespace m{
using namespace ruis::make;
using namespace ::make;
}

utki::shared_ref<ruis::widget> make_root_widget(const utki::shared_ref<ruis::context>& c){
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
							},
							.color_params{
								.color = 0x80ff8080
							},
							.rectangle_vao_params{
								.corner_radii = {10, 20, 30, 40},
								.stroke_width = 0
							}
						}
					),
					m::rectangle_widget(c,
						{
							.layout_params{
								.dims = {200_pp, 100_pp}
							},
							.color_params{
								.color = 0x8000ff00
							},
							.rectangle_vao_params{
								.corner_radii = {10, 20, 30, 40},
								.stroke_width = 15
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
			m::capsule_widget(c,
				{
					.layout_params{
						.dims = {80_pp, 300_pp},
						.align = {ruis::align::center, ruis::align::front}
					},
					.is_vertical = true,
					.color = 0xffff8080
				}
			),
			m::capsule_widget(c,
				{
					.layout_params{
						.dims = {70_pp, 280_pp},
						.align = {ruis::align::center, ruis::align::front}
					},
					.is_vertical = true,
					.stroke_width = 2_pp,
					.color = 0xff80ff80
				}
			),
			m::capsule_widget(c,
				{
					.layout_params{
						.dims = {300_pp, 80_pp},
						.align = {ruis::align::center, ruis::align::center}
					},
					.is_vertical = false,
					.color = 0xffff8080
				}
			),
			m::capsule_widget(c,
				{
					.layout_params{
						.dims = {280_pp, 70_pp},
						.align = {ruis::align::center, ruis::align::center}
					},
					.is_vertical = false,
					.stroke_width = 2_pp,
					.color = 0xff80ff80
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
