#include "../../../src/morda/morda/widgets/group/book.hpp"

#include "../../../src/morda/morda/context.hpp"
#include "../../../src/morda/morda/updateable.hpp"
#include "../../../src/morda/morda/res/texture.hpp"
#include "../../../src/morda/morda/render/renderer.hpp"
#include "../../../src/morda/morda/widgets/button/push_button.hpp"

#include <GLES2/gl2.h>

#include "cube_page.hpp"

namespace{

class CubeWidget : public morda::widget, public morda::updateable{
	std::shared_ptr<morda::res::texture> tex;
	
	morda::quaternion rot = morda::quaternion().identity();
public:
	std::shared_ptr<morda::vertex_array> cubeVAO;
	
	CubeWidget(std::shared_ptr<morda::context> c) :
			widget(std::move(c), puu::forest())
	{
		std::array<morda::vector3, 36> cubePos = {{
			r4::vec3f(-1, -1, 1), r4::vec3f(1, -1, 1), r4::vec3f(-1, 1, 1),
			r4::vec3f(1, -1, 1), r4::vec3f(1, 1, 1), r4::vec3f(-1, 1, 1),
			
			r4::vec3f(1, -1, 1), r4::vec3f(1, -1, -1), r4::vec3f(1, 1, 1),
			r4::vec3f(1, -1, -1), r4::vec3f(1, 1, -1), r4::vec3f(1, 1, 1),
			
			r4::vec3f(1, -1, -1), r4::vec3f(-1, -1, -1), r4::vec3f(1, 1, -1),
			r4::vec3f(-1, -1, -1), r4::vec3f(-1, 1, -1), r4::vec3f(1, 1, -1),
			
			r4::vec3f(-1, -1, -1), r4::vec3f(-1, -1, 1), r4::vec3f(-1, 1, -1),
			r4::vec3f(-1, -1, 1), r4::vec3f(-1, 1, 1), r4::vec3f(-1, 1, -1),
			
			r4::vec3f(-1, 1, -1), r4::vec3f(-1, 1, 1), r4::vec3f(1, 1, -1),
			r4::vec3f(-1, 1, 1), r4::vec3f(1, 1, 1), r4::vec3f(1, 1, -1),
			
			r4::vec3f(-1, -1, -1), r4::vec3f(1, -1, -1), r4::vec3f(-1, -1, 1),
			r4::vec3f(-1, -1, 1), r4::vec3f(1, -1, -1), r4::vec3f(1, -1, 1)
		}};
		
		auto posVBO = this->context->renderer->factory->create_vertex_buffer(utki::make_span(cubePos));
		
		std::array<r4::vec2f, 36> cubeTex = {{
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),
			
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),
			
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),
		
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),
			
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1),
			
			r4::vec2f(0, 0), r4::vec2f(1, 0), r4::vec2f(0, 1),
			r4::vec2f(1, 0), r4::vec2f(1, 1), r4::vec2f(0, 1)
		}};
		
		auto texVBO = this->context->renderer->factory->create_vertex_buffer(utki::make_span(cubeTex));
		
		std::array<std::uint16_t, 36> indices = {{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};
		
		auto cubeIndices = this->context->renderer->factory->create_index_buffer(utki::make_span(indices));
		
		this->cubeVAO = this->context->renderer->factory->create_vertex_array({posVBO, texVBO}, cubeIndices, morda::vertex_array::mode::triangles);
		
		this->tex = this->context->loader.load<morda::res::texture>("tex_sample");
		this->rot.identity();	
	}
	
	unsigned fps = 0;
	std::uint32_t fpsSecCounter = 0;
	
	void update(std::uint32_t dt) override{
		this->fpsSecCounter += dt;
		++this->fps;
		this->rot %= morda::quaternion().rotation(r4::vec3f(1, 2, 1).normalize(), 1.5f * (float(dt) / 1000));
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
		
		morda::matrix4 m(matr);
		m.translate(0, 0, -4);
		
		m.rotate(this->rot);

		glEnable(GL_CULL_FACE);
		
		this->context->renderer->shader->pos_tex->render(m, *this->cubeVAO, this->tex->tex());
		
		glDisable(GL_CULL_FACE);
	}
};
}

cube_page::cube_page(std::shared_ptr<morda::context> c) :
		widget(std::move(c), puu::forest()),
		page(this->context, puu::forest()),
		pile(this->context, puu::read(R"qwertyuiop(
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
