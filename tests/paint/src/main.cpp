#include <utki/debug.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/morda/widgets/widget.hpp"

#include "../../../src/morda/morda/paint/path_vba.hpp"

class path_widget : virtual public morda::widget{
	morda::path_vba vba;
public:
	path_widget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			morda::widget(std::move(c), desc)
	{}

	void render(const morda::matrix4& matrix)const override{
		this->vba.render(matrix, 0xff00ffff);
	}

	void on_resize()override{
		morda::path path;
		path.line_to(this->rect().d / 2);
		path.cubic_by(
				morda::vector2(this->rect().d.x / 2, 0),
				morda::vector2(0, this->rect().d.y / 2),
				this->rect().d / 2
			);
		this->vba = morda::path_vba(this->context->renderer, path.stroke());
	}
};

class application : public mordavokne::application{
public:
	application() :
			mordavokne::application(
					"morda-tests",
					[](){
						mordavokne::window_params wp(r4::vec2ui(1024, 800));
						return wp;
					}()
				)
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));
	
		this->gui.set_root(std::make_shared<path_widget>(this->gui.context, puu::forest()));
	}
};

std::unique_ptr<mordavokne::application> mordavokne::create_application(int argc, const char** argv){
	return std::make_unique<::application>();
}
