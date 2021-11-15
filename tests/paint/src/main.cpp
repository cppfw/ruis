#include <utki/debug.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/widgets/widget.hpp"

#include "../../../src/morda/paint/path_vao.hpp"
#include "../../../src/morda/paint/frame_vao.hpp"

class path_widget : virtual public morda::widget{
	morda::path_vao vao;
public:
	path_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			morda::widget(std::move(c), desc)
	{}

	void render(const morda::matrix4& matrix)const override{
		this->vao.render(matrix, 0xff00ffff);
	}

	void on_resize()override{
		morda::path path;
		path.line_to(this->rect().d / 2);
		path.cubic_by(
				morda::vector2(this->rect().d.x() / 2, 0),
				morda::vector2(0, this->rect().d.y() / 2),
				this->rect().d / 2
			);
		this->vao = morda::path_vao(this->context->renderer, path.stroke());
	}
};

class frame_widget : virtual public morda::widget{
	morda::frame_vao vao;
public:
	frame_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			morda::widget(std::move(c), desc)
	{}

	void render(const morda::matrix4& matrix)const override{
		this->vao.render(matrix, 0xffff8080);
	}

	void on_resize()override{
		this->vao = morda::frame_vao(
				this->context->renderer,
				this->rect().d,
				morda::vector2{10, 20}
			);
	}
};

class application : public mordavokne::application{
public:
	application() :
			mordavokne::application(
					"morda-tests",
					[](){
						mordavokne::window_params wp(r4::vector2<unsigned>(1024, 800));
						return wp;
					}()
				)
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		this->gui.context->inflater.register_widget<path_widget>("path_widget");
		this->gui.context->inflater.register_widget<frame_widget>("frame_widget");
	
		// this->gui.set_root(std::make_shared<path_widget>(this->gui.context, treeml::forest()));

		this->gui.set_root(this->gui.context->inflater.inflate(treeml::read(R"(
			@pile{
				@path_widget{
					layout{
						dx{fill} dy{fill}
					}
				}

				@margins{
					left{5dp}
					right{5dp}
					top{5dp}
					bottom{5dp}

					layout{
						dx{fill} dy{fill}
					}

					@frame_widget{
						layout{
							dx{fill} dy{fill}
						}
					}
				}
			}
		)")));
	}
};

mordavokne::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
