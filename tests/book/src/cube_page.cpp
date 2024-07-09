#include <ruis/context.hpp>
#include <ruis/updateable.hpp>
#include <ruis/res/texture_2d.hpp>
#include <ruis/render/renderer.hpp>
#include <ruis/widget/button/base/push_button.hpp>
#include <ruis/widget/group/book.hpp>

#include "cube_page.hpp"

namespace{

class cube_widget : public ruis::widget, public ruis::updateable{
	std::shared_ptr<ruis::res::texture_2d> tex;
	
	ruis::quaternion rot = ruis::quaternion().set_identity();
public:
	std::shared_ptr<ruis::render::vertex_array> cube_vao;
	
	cube_widget(const utki::shared_ref<ruis::context>& c) :
			widget(c, tml::forest())
	{
		std::array<ruis::vector3, 36> cube_pos = {{
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
		
		auto pos_vbo = this->context.get().renderer.get().factory->create_vertex_buffer(utki::make_span(cube_pos));
		
		std::array<ruis::vector2, 36> cube_tex = {{
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
		
		auto tex_vbo = this->context.get().renderer.get().factory->create_vertex_buffer(utki::make_span(cube_tex));
		
		std::array<uint16_t, 36> indices = {{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		}};
		
		auto cube_indices = this->context.get().renderer.get().factory->create_index_buffer(utki::make_span(indices));
		
		this->cube_vao = this->context.get().renderer.get().factory->create_vertex_array({pos_vbo, tex_vbo}, cube_indices, ruis::render::vertex_array::mode::triangles)
			.to_shared_ptr();
		
		this->tex = this->context.get().loader.load<ruis::res::texture_2d>("tex_sample").to_shared_ptr();
		this->rot.set_identity();
	}
	
	unsigned fps = 0;
	uint32_t fpsSecCounter = 0;
	
	void update(uint32_t dt) override{
		this->fpsSecCounter += dt;
		++this->fps;
		this->rot *= ruis::quaternion().set_rotation(r4::vector3<float>(1, 2, 1).normalize(), 1.5f * (float(dt) / 1000));
		if(this->fpsSecCounter >= 1000){
			std::cout << "fps = " << std::dec << fps << std::endl;
			this->fpsSecCounter = 0;
			this->fps = 0;
		}
		this->clear_cache();
	}
	
	void render(const ruis::matrix4& matrix)const override{
		this->widget::render(matrix);
		
		ruis::matrix4 matr(matrix);
		matr.scale(this->rect().d / 2);
		matr.translate(1, 1);
		matr.scale(1, -1);
		matr.frustum(-2, 2, -1.5, 1.5, 2, 100);
		matr.translate(0, 0, -4);
		matr.rotate(this->rot);
		
		this->context.get().renderer.get().shader->pos_tex->render(matr, *this->cube_vao, this->tex->tex());
	}
};
}

cube_page::cube_page(const utki::shared_ref<ruis::context>& c) :
		widget(c, tml::forest()),
		page(this->context, tml::forest()),
		container(this->context, tml::read(R"qwertyuiop(
				layout{pile}
				@column{
					lp{
						dx{fill}dy{fill}
					}
					@widget{
						id{placeholder}
						lp{dx{fill}dy{fill}weight{1}}
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
	
	this->get_widget_as<ruis::push_button>("back_button").click_handler = [this](ruis::push_button&){
		this->context.get().post_to_ui_thread([book = utki::make_shared_from(*this->get_parent_book()), this]{
			this->tear_out();
		});
	};
	
	auto cw = utki::make_shared<cube_widget>(this->context);
	this->cube = cw.to_shared_ptr();
	
	cw.get().set_cache(true);
	
	ph.replace_by(cw);
}

void cube_page::on_show(){
	this->context.get().updater.get().start(this->cube, 0);
}

void cube_page::on_hide()noexcept{
	this->context.get().updater.get().stop(*this->cube);
}
