#include "../../../src/morda/widgets/group/book.hpp"

#include "../../../src/morda/context.hpp"
#include "../../../src/morda/updateable.hpp"
#include "../../../src/morda/res/texture.hpp"
#include "../../../src/morda/render/renderer.hpp"
#include "../../../src/morda/widgets/button/push_button.hpp"

#include "cube_page.hpp"

namespace{

class CubeWidget : public morda::widget, public morda::updateable{
	std::shared_ptr<morda::res::texture> tex;
	
	morda::quaternion rot = morda::quaternion().set_identity();
public:
	std::shared_ptr<morda::vertex_array> cubeVAO;
	
	CubeWidget(std::shared_ptr<morda::context> c) :
			widget(std::move(c), treeml::forest())
	{
		std::array<morda::vector3, 36> cubePos = {{
			{-1, -1, 1}, {1, -1, 1}, {-1, 1, 1},
			{ 1, -1, 1}, {1,  1, 1}, {-1, 1, 1},
			
			{1, -1,  1}, {1, -1, -1}, {1, 1, 1},
			{1, -1, -1}, {1,  1, -1}, {1, 1, 1},
			
			{ 1, -1, -1}, {-1, -1, -1}, {1, 1, -1},
			{-1, -1, -1}, {-1,  1, -1}, {1, 1, -1},
			
			{-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1},
			{-1, -1,  1}, {-1,  1, 1}, {-1, 1, -1},
			
			{-1, 1, -1}, {-1, 1, 1}, {1, 1, -1},
			{-1, 1,  1}, { 1, 1, 1}, {1, 1, -1},
			
			{-1, -1, -1}, {1, -1, -1}, {-1, -1, 1},
			{-1, -1,  1}, {1, -1, -1}, { 1, -1, 1}
		}};
		
		auto posVBO = this->context->renderer->factory->create_vertex_buffer(utki::make_span(cubePos));
		
		std::array<morda::vector2, 36> cubeTex = {{
			{0, 0}, {1, 0}, {0, 1},
			{1, 0}, {1, 1}, {0, 1},
			{0, 0}, {1, 0}, {0, 1},
			{1, 0}, {1, 1}, {0, 1},
			{0, 0}, {1, 0}, {0, 1},
			{1, 0}, {1, 1}, {0, 1},
			{0, 0}, {1, 0}, {0, 1},
			{1, 0}, {1, 1}, {0, 1},
			{0, 0}, {1, 0}, {0, 1},
			{1, 0}, {1, 1}, {0, 1},
			{0, 0}, {1, 0}, {0, 1},
			{1, 0}, {1, 1}, {0, 1}
		}};
		
		auto texVBO = this->context->renderer->factory->create_vertex_buffer(utki::make_span(cubeTex));
		
		std::array<uint16_t, 36> indices = {{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};
		
		auto cubeIndices = this->context->renderer->factory->create_index_buffer(utki::make_span(indices));
		
		this->cubeVAO = this->context->renderer->factory->create_vertex_array({posVBO, texVBO}, cubeIndices, morda::vertex_array::mode::triangles);
		
		this->tex = this->context->loader.load<morda::res::texture>("tex_sample");
		this->rot.set_identity();
	}
	
	unsigned fps = 0;
	uint32_t fpsSecCounter = 0;
	
	void update(uint32_t dt) override{
		this->fpsSecCounter += dt;
		++this->fps;
		this->rot %= morda::quaternion().set_rotation(r4::vector3<float>(1, 2, 1).normalize(), 1.5f * (float(dt) / 1000));
		if(this->fpsSecCounter >= 1000){
			TRACE_ALWAYS(<< "fps = " << std::dec << fps << std::endl)
			this->fpsSecCounter = 0;
			this->fps = 0;
		}
		this->clear_cache();
	}
	
	void render(const morda::matrix4& matrix)const override{
		this->widget::render(matrix);
		
		morda::matrix4 matr(matrix);
		matr.scale(this->rect().d / 2);
		matr.translate(1, 1);
		matr.scale(1, -1);
		matr.frustum(-2, 2, -1.5, 1.5, 2, 100);
		matr.translate(0, 0, -4);
		matr.rotate(this->rot);
		
		this->context->renderer->shader->pos_tex->render(matr, *this->cubeVAO, this->tex->tex());
	}
};
}

cube_page::cube_page(std::shared_ptr<morda::context> c) :
		widget(std::move(c), treeml::forest()),
		page(this->context, treeml::forest()),
		pile(this->context, treeml::read(R"qwertyuiop(
				@column{
					layout{
						dx{fill}dy{fill}
					}
					@widget{
						id{placeholder}
						layout{dx{fill}dy{0}weight{1}}
					}
					@text{text{"cube page"}}
					@push_button{
						id{back_button}
						@text{
							text{back}
						}
					}
				}
			)qwertyuiop"))
{
	auto& ph = this->get_widget("placeholder");
	
	this->get_widget_as<morda::push_button>("back_button").click_handler = [this](morda::push_button&){
		this->tear_out();
	};
	
	auto cw = std::make_shared<CubeWidget>(this->context);
	this->cube = cw;
	
	cw->set_cache(true);
	
	ph.replace_by(cw);
}

void cube_page::on_show(){
	this->context->updater->start(this->cube, 0);
}

void cube_page::on_hide()noexcept{
	this->context->updater->stop(*this->cube);
}
