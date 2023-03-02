/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "context.hpp"

#include "util/util.hpp"

#include "widget.hpp"

#include "container.hpp"

using namespace morda;

widget::widget(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		context(c)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		try{
			if(p.value == "layout"){
				this->layout_parameters = layout_params(p.children, this->context.get().units);
			}else if(p.value == "x"){
				this->rectangle.p.x() = parse_dimension_value(get_property_value(p), this->context.get().units);
			}else if(p.value == "y"){
				this->rectangle.p.y() = parse_dimension_value(get_property_value(p), this->context.get().units);
			}else if(p.value == "dx"){
				this->rectangle.d.x() = parse_dimension_value(get_property_value(p), this->context.get().units);
			}else if(p.value == "dy"){
				this->rectangle.d.y() = parse_dimension_value(get_property_value(p), this->context.get().units);
			}else if(p.value == "id"){
				this->id = get_property_value(p).to_string();
				// TRACE(<< "inflating '" << this->id << "'" << std::endl)
			}else if(p.value == "clip"){
				this->clip_enabled = get_property_value(p).to_bool();
			}else if(p.value == "cache"){
				this->cache = get_property_value(p).to_bool();
			}else if(p.value == "visible"){
				this->visible = get_property_value(p).to_bool();
			}else if(p.value == "enabled"){
				this->enabled = get_property_value(p).to_bool();
			}
		}catch(std::invalid_argument&){
			LOG([&](auto&o){o << "could not parse value of " << treeml::to_string(p) << std::endl;})
			throw;
		}
	}
}

layout_params::layout_params(const treeml::forest& desc, const morda::units& units){
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		try{
			if(p.value == "dx"){
				this->dims.x() = parse_layout_dimension_value(get_property_value(p), units);
			}else if(p.value == "dy"){
				this->dims.y() = parse_layout_dimension_value(get_property_value(p), units);
			}else if(p.value == "weight"){
				this->weight = get_property_value(p).to_float();
			}
		}catch(std::invalid_argument&){
			LOG([&](auto&o){o << "could not parse value of " << treeml::to_string(p) << std::endl;})
			throw;
		}
	}
}

std::shared_ptr<widget> widget::try_get_widget(const std::string& id, bool allow_itself)noexcept{
	if(allow_itself && this->id == id){
		return utki::make_shared_from(*this).to_shared_ptr();
	}
	return nullptr;
}

widget& widget::get_root_widget(){
	if(!this->parent()){
		return *this;
	}
	return this->parent()->get_root_widget();
}

widget* widget::try_get_ancestor(const std::string& id){
	if(!this->parent()){
		return nullptr;
	}

	if(this->parent()->id == id){
		return this->parent();
	}

	return this->parent()->try_get_ancestor(id);
}

widget& widget::get_ancestor(const std::string& id){
	auto* a = this->try_get_ancestor(id);
	if(!a){
		std::stringstream ss;
		ss << "ancestor with id = '" << id << "' not found";
		throw std::logic_error(ss.str());
	}
	return *a;
}

void widget::move_to(const vector2& new_pos){
	this->rectangle.p = new_pos;
}

void widget::resize(const morda::vector2& new_dims){
	if(this->rectangle.d == new_dims){
		if(this->layout_dirty){
			this->clear_cache();
			this->layout_dirty = false;
			this->lay_out();
		}
		return;
	}

	using std::max;

	this->clear_cache();
	this->rectangle.d = max(new_dims, real(0)); // clamp bottom
	this->on_resize(); // call virtual method
}

void widget::on_resize(){
	this->layout_dirty = false;
	this->lay_out();
}

utki::shared_ref<widget> widget::remove_from_parent(){
	if(!this->parent()){
		throw std::logic_error("widget::remove_from_parent(): widget is not added to the parent");
	}
	auto ret = utki::make_shared_from(*this);
	this->parent()->erase(this->parent()->find(*this));
	return ret;
}

utki::shared_ref<widget> widget::replace_by(const utki::shared_ref<widget>& w) {
	if(!this->parent()){
		throw std::logic_error("this widget is not added to any parent");
	}

	// TODO: performace can be improved by adding dedicated container::replace_by(iter, widget) function
	this->parent()->insert(w, this->parent()->find(*this));
	
	w.get().layout_parameters = std::move(this->layout_parameters);

	return this->remove_from_parent();
}

void widget::invalidate_layout()noexcept{
	if(this->layout_dirty){
		return;
	}
	this->layout_dirty = true;
	if(this->parent()){
		this->parent()->invalidate_layout();
	}

	// TODO: this->clear_cache()?
	this->cache_texture.reset();
}

void widget::render_internal(const morda::matrix4& matrix)const{
	if(!this->rect().d.is_positive()){
		return;
	}

	auto& r = this->context.get().renderer.get();

	if(this->cache){
		if(this->cache_dirty){
			bool scissor_test_was_enabled = r.is_scissor_enabled();
			r.set_scissor_enabled(false);

			// TODO: this is already checked inside of render_to_texture()
			// check if can re-use old texture
			if(!this->cache_texture || this->cache_texture->dims() != this->rect().d){
				this->cache_texture = this->render_to_texture().to_shared_ptr();
			}else{
				ASSERT(this->cache_texture->dims() == this->rect().d)
				this->cache_texture = this->render_to_texture(std::move(this->cache_texture)).to_shared_ptr();
			}

			r.set_scissor_enabled(scissor_test_was_enabled);
			this->cache_dirty = false;
		}

		// after rendering to texture it is most likely there will be transparent areas, so enable simple blending
		set_simple_alpha_blending(this->context.get().renderer.get());

		this->render_from_cache(matrix);
	}else{
		if(this->clip_enabled){
	//		TRACE(<< "widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

			// set scissor test
			r4::rectangle<int> scissor = this->compute_viewport_rect(matrix);

			r4::rectangle<int> old_scissor;
			bool scissor_test_was_enabled = r.is_scissor_enabled();
			if(scissor_test_was_enabled){
				old_scissor = r.get_scissor();
				scissor.intersect(old_scissor);
			}else{
				r.set_scissor_enabled(true);
			}

			r.set_scissor(scissor);

			this->render(matrix);

			if(scissor_test_was_enabled){
				r.set_scissor(old_scissor);
			}else{
				r.set_scissor_enabled(false);
			}
		}else{
			this->render(matrix);
		}
	}

	// render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::ColorPosShader& s = App::inst().shaders.colorPosShader;
	s.Bind();
	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);
	s.set_matrix(matr);

	if(this->is_hovered()){
		s.SetColor(r4::vector3<float>(0, 1, 0));
	}else{
		s.SetColor(r4::vector3<float>(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

utki::shared_ref<texture_2d> widget::render_to_texture(std::shared_ptr<texture_2d> reuse)const{
	auto& r = this->context.get().renderer.get();

	utki::shared_ref<texture_2d> tex = [&](){
		if(reuse && reuse->dims() == this->rect().d){
			ASSERT(reuse)
			return utki::shared_ref(std::move(reuse));
		}else{
			return r.factory->create_texture_2d(
					morda::texture_2d::type::rgba,
					this->rect().d.to<unsigned>(),
					nullptr
				);
		}
	}();

	r.set_framebuffer(r.factory->create_framebuffer(tex).to_shared_ptr());

//	ASSERT_INFO(Render::isBoundFrameBufferComplete(), "tex.dims() = " << tex.dims())

	auto old_viewport = r.get_viewport();
	utki::scope_exit scope_exit([&old_viewport, &r](){
		r.set_viewport(old_viewport);
	});

	r.set_viewport(r4::rectangle<int>(0, this->rect().d.to<int>()));

	r.clear_framebuffer();

	matrix4 matrix = r.initial_matrix;
	matrix.translate(-1, 1);
	matrix.scale(vector2(2.0f, -2.0f).comp_divide(this->rect().d));

	this->render(matrix);

	r.set_framebuffer(nullptr);

	return tex;
}

void widget::render_from_cache(const r4::matrix4<float>& matrix)const{
	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	auto& r = this->context.get().renderer.get();
	ASSERT(this->cache_texture)
	r.shader->pos_tex->render(matr, r.pos_tex_quad_01_vao.get(), *this->cache_texture);
}

void widget::clear_cache(){
	this->cache_dirty = true;
	if(this->parent()){
		this->parent()->clear_cache();
	}
}

void widget::on_key_internal(const morda::key_event& e){
	if(this->is_interactive()){
		if(this->on_key(e)){
			return;
		}
	}

	if(this->parent()){
		this->parent()->on_key_internal(e);
	}
}

void widget::focus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(this->is_focused()){
		return;
	}

	this->context.get().set_focused_widget(utki::make_shared_from(*this).to_shared_ptr());
}

void widget::unfocus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(!this->is_focused()){
		return;
	}

	ASSERT(this->context.get().focused_widget.lock() && this->context.get().focused_widget.lock().get() == this)

	this->context.get().set_focused_widget(nullptr);
}

r4::rectangle<int> widget::compute_viewport_rect(const matrix4& matrix)const noexcept{
	using std::round;
	r4::rectangle<int> ret(
			round(((matrix * vector2(0, 0) + vector2(1, 1)) / 2).comp_multiply(this->context.get().renderer.get().get_viewport().d.to<real>())).to<int>(),
			this->rect().d.to<int>()
		);
	ret.p.y() -= ret.d.y();
	return ret;
}

morda::vector2 widget::get_absolute_pos()const noexcept{
	if(!this->parent()){
		return this->rect().p;
	}
	return this->parent()->get_absolute_pos() + this->rect().p;
}

morda::rectangle widget::get_absolute_rect()const noexcept{
	if(this->parent()){
		return {
			this->get_absolute_pos(),
			this->rect().d
		};
	}
	return this->rect();
}

vector2 widget::measure(const morda::vector2& quotum)const{
	vector2 ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 0;
		}
	}
	return ret;
}

vector2 widget::pos_in_ancestor(vector2 pos, const widget* ancestor){
	if(ancestor == this || !this->parent()){
		return pos;
	}

	ASSERT(this->parent())

	return this->parent()->pos_in_ancestor(this->rect().p + pos, ancestor);
}

layout_params& widget::get_layout_params(){
	if(this->parent()){
		this->parent()->invalidate_layout();
	}

	return const_cast<layout_params&>(this->get_layout_params_const());
}

const layout_params& widget::get_layout_params_const()const{
	return this->layout_parameters;
}

widget& widget::get_widget(const std::string& id, bool allow_itself){
	auto w = this->try_get_widget(id, allow_itself);
	if(!w){
		std::stringstream ss;
		ss << "widget '" << id << "' not found in '" << this->id << "'";

		// gui scripts are supposed to be static (not dynamically generated),
		// so if there is no expected widget the gui script, then it is a logic error
		throw std::logic_error(ss.str());
	}
	return *w;
}

void widget::set_enabled(bool enable){
//	TRACE(<< "widget::set_enabled(): enable = " << enable << " this->name() = " << this->name()<< std::endl)
	if(this->enabled == enable){
		return;
	}

	this->enabled = enable;

	// un-hover this widget if it becomes disabled because it is not supposed to receive mouse input
	if(!this->is_enabled()){
		this->set_unhovered();
	}

	this->on_enable_change();
}

void widget::set_visible(bool visible){
	this->visible = visible;
	if(!this->visible){
		this->set_unhovered();
	}
}

void widget::set_unhovered(){
	auto hover_set = std::move(this->hovered);
	ASSERT(this->hovered.size() == 0)
	for(auto h : hover_set){
		this->on_hover_change(h);
	}
}

void widget::set_hovered(bool is_hovered, unsigned pointer_id){
	if(is_hovered == this->is_hovered(pointer_id)){
		return;
	}
//	TRACE(<< "widget::setHovered(): isHovered = " << isHovered << " this->name() = " << this->name() << std::endl)

	if(is_hovered){
		ASSERT(!this->is_hovered(pointer_id))
		this->hovered.insert(pointer_id);
	}else{
		ASSERT(this->is_hovered(pointer_id))
		this->hovered.erase(pointer_id);
	}

	this->on_hover_change(pointer_id);
}
