#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widgets/widget.hpp>
#include <ruis/paint/path_vao.hpp>
#include <ruis/paint/frame_vao.hpp>

class path_widget : virtual public ruis::widget{
	ruis::path_vao vao;
public:
	path_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
			ruis::widget(c, desc),
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

class frame_widget : virtual public ruis::widget{
	ruis::frame_vao vao;
public:
	frame_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
			ruis::widget(c, desc),
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

class application : public ruisapp::application{
public:
	application() :
			ruisapp::application(
					"ruis-tests",
					[](){
						ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));
						return wp;
					}()
				)
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		this->gui.context.get().inflater.register_widget<path_widget>("path_widget");
		this->gui.context.get().inflater.register_widget<frame_widget>("frame_widget");
	
		// this->gui.set_root(std::make_shared<path_widget>(this->gui.context, tml::forest()));

		this->gui.set_root(
			this->gui.context.get().inflater.inflate(tml::read(R"(
			@pile{
				@path_widget{
					lp{
						dx{fill} dy{fill}
					}
				}

				@margins{
					left{5dp}
					right{5dp}
					top{5dp}
					bottom{5dp}

					lp{
						dx{fill} dy{fill}
					}

					@frame_widget{
						lp{
							dx{fill} dy{fill}
						}
					}
				}

				@vertical_scroll_bar{
					lp{
						dx{min} dy{max}
					}
				}
				@horizontal_scroll_bar{
					lp{
						dx{max} dy{min}
					}
				}
				@push_button{
					@text{
						text{stuff}
					}
				}
			}
		)")));
	}
};

const ruisapp::application_factory app_fac([](auto executbale, auto args){
	return std::make_unique<::application>();
});
