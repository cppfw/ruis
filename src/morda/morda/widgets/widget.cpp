#include "../context.hpp"

#include "../util/util.hpp"

#include "widget.hpp"

#include "container.hpp"

using namespace morda;

widget::widget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		context(std::move(c))
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		try{
			if(p.value == "layout"){
				this->layout_desc = p.children;
			}else if(p.value == "x"){
				this->rectangle.p.x() = parse_dimension_value(get_property_value(p), this->context->units);
			}else if(p.value == "y"){
				this->rectangle.p.y() = parse_dimension_value(get_property_value(p), this->context->units);
			}else if(p.value == "dx"){
				this->rectangle.d.x() = parse_dimension_value(get_property_value(p), this->context->units);
			}else if(p.value == "dy"){
				this->rectangle.d.y() = parse_dimension_value(get_property_value(p), this->context->units);
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
			TRACE(<< "could not parse value of " << puu::to_string(p) << std::endl)
			throw;
		}
	}
}

widget::layout_params::layout_params(const puu::forest& desc, const morda::units& units){
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		try{
			if(p.value == "dx"){
				this->dims.x() = parse_layout_dimension_value(get_property_value(p), units);
			}else if(p.value == "dy"){
				this->dims.y() = parse_layout_dimension_value(get_property_value(p), units);
			}
		}catch(std::invalid_argument&){
			TRACE(<< "could not parse value of " << puu::to_string(p) << std::endl)
			throw;
		}
	}
}

std::shared_ptr<widget> widget::try_get_widget(const std::string& id)noexcept{
	if(this->id == id){
		return utki::make_shared_from(*this);
	}
	return nullptr;
}

void widget::resize(const morda::vector2& newDims){
	if(this->rectangle.d == newDims){
		if(this->relayoutNeeded){
			this->clear_cache();
			this->relayoutNeeded = false;
			this->lay_out();
		}
		return;
	}

	using std::max;

	this->clear_cache();
	this->rectangle.d = max(newDims, real(0)); // clamp bottom
	this->relayoutNeeded = false;
	this->on_resize(); // call virtual method
}

std::shared_ptr<widget> widget::remove_from_parent(){
	if(!this->parent()){
		throw std::logic_error("widget::remove_from_parent(): widget is not added to the parent");
	}
	auto ret = utki::make_shared_from(*this);
	this->parent()->erase(this->parent()->find(*this));
	return ret;
}

std::shared_ptr<widget> widget::replace_by(std::shared_ptr<widget> w) {
	if(!this->parent()){
		throw std::logic_error("this widget is not added to any parent");
	}

	this->parent()->insert(w, this->parent()->find(*this));

	if(w && w->layout_desc.empty()){
		w->layout_desc = std::move(this->layout_desc);
	}

	return this->remove_from_parent();
}

void widget::invalidate_layout()noexcept{
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(this->parent()){
		this->parent()->invalidate_layout();
	}
	this->cacheTex.reset();
}

void widget::renderInternal(const morda::matrix4& matrix)const{
	if(!this->rect().d.is_positive()){
		return;
	}

	auto& r = *this->context->renderer;

	if(this->cache){
		if(this->cacheDirty){
			bool scissorTestWasEnabled = r.is_scissor_enabled();
			r.set_scissor_enabled(false);

			// check if can re-use old texture
			if(!this->cacheTex || this->cacheTex->dims() != this->rect().d){
				this->cacheTex = this->render_to_texture();
			}else{
				ASSERT(this->cacheTex->dims() == this->rect().d)
				this->cacheTex = this->render_to_texture(std::move(this->cacheTex));
			}

			r.set_scissor_enabled(scissorTestWasEnabled);
			this->cacheDirty = false;
		}

		// after rendering to texture it is most likely there will be transparent areas, so enable simple blending
		set_simple_alpha_blending(*this->context->renderer);

		this->renderFromCache(matrix);
	}else{
		if(this->clip_enabled){
	//		TRACE(<< "widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

			// set scissor test
			r4::rectangle<int> scissor = this->compute_viewport_rect(matrix);

			r4::rectangle<int> oldScissor;
			bool scissorTestWasEnabled = r.is_scissor_enabled();
			if(scissorTestWasEnabled){
				oldScissor = r.get_scissor();
				scissor.intersect(oldScissor);
			}else{
				r.set_scissor_enabled(true);
			}

			r.set_scissor(scissor);

			this->render(matrix);

			if(scissorTestWasEnabled){
				r.set_scissor(oldScissor);
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
	s.SetMatrix(matr);

	if(this->is_hovered()){
		s.SetColor(r4::vector3<float>(0, 1, 0));
	}else{
		s.SetColor(r4::vector3<float>(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

std::shared_ptr<texture_2d> widget::render_to_texture(std::shared_ptr<texture_2d> reuse)const{
	auto& r = *this->context->renderer;

	std::shared_ptr<texture_2d> tex;

	if(reuse && reuse->dims() == this->rect().d){
		tex = std::move(reuse);
	}else{
		tex = r.factory->create_texture_2d(
				morda::texture_2d::type::rgba,
				this->rect().d.to<unsigned>(),
				nullptr
			);
	}

	ASSERT(tex)

	r.set_framebuffer(r.factory->create_framebuffer(tex));

//	ASSERT_INFO(Render::isBoundFrameBufferComplete(), "tex.dims() = " << tex.dims())

	auto oldViewport = r.get_viewport();
	utki::scope_exit scope_exit([&oldViewport, &r](){
		r.set_viewport(oldViewport);
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

void widget::renderFromCache(const r4::matrix4<float>& matrix) const {
	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	auto& r = *this->context->renderer;
	ASSERT(this->cacheTex)
	r.shader->pos_tex->render(matr, *r.pos_tex_quad_01_vao, *this->cacheTex);
}

void widget::clear_cache(){
	this->cacheDirty = true;
	if(this->parent()){
		this->parent()->clear_cache();
	}
}

void widget::onKeyInternal(bool isDown, key keyCode){
	if(this->is_interactive()){
		if(this->on_key(isDown, keyCode)){
			return;
		}
	}

	if(this->parent()){
		this->parent()->onKeyInternal(isDown, keyCode);
	}
}

void widget::focus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(this->is_focused()){
		return;
	}

	this->context->set_focused_widget(utki::make_shared_from(*this));
}

void widget::unfocus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(!this->is_focused()){
		return;
	}

	ASSERT(this->context->focused_widget.lock() && this->context->focused_widget.lock().get() == this)

	this->context->set_focused_widget(nullptr);
}

r4::rectangle<int> widget::compute_viewport_rect(const matrix4& matrix)const noexcept{
	using std::round;
	r4::rectangle<int> ret(
			round(((matrix * vector2(0, 0) + vector2(1, 1)) / 2).comp_multiply(this->context->renderer->get_viewport().d.to<real>())).to<int>(),
			this->rect().d.to<int>()
		);
	ret.p.y() -= ret.d.y();
	return ret;
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

widget::layout_params& widget::get_layout_params(){
	if(!this->parent()){
		throw std::logic_error("widget::get_layout_params(): widget is not added to any container, cannot get layout params. In order to get layout params the widget should be added to some container.");
	}

	return this->parent()->get_layout_params(*this);
}

const widget::layout_params& widget::get_layout_params_const()const{
	if(!this->parent()){
		throw std::logic_error("widget::get_layout_params_const(): widget is not added to any container, cannot get layout params. In order to get layout params the widget should be added to some container.");
	}

	return this->parent()->get_layout_params_const(*this);
}

widget& widget::get_widget(const std::string& id){
	auto w = this->try_get_widget(id);
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
	if (!this->visible) {
		this->set_unhovered();
	}
}

void widget::set_unhovered(){
	auto hoverSet = std::move(this->hovered);
	ASSERT(this->hovered.size() == 0)
	for(auto h : hoverSet){
		this->on_hover_change(h);
	}
}

void widget::set_hovered(bool isHovered, unsigned pointerID){
	if(isHovered == this->is_hovered(pointerID)){
		return;
	}
//	TRACE(<< "widget::setHovered(): isHovered = " << isHovered << " this->name() = " << this->name() << std::endl)

	if(isHovered){
		ASSERT(!this->is_hovered(pointerID))
		this->hovered.insert(pointerID);
	}else{
		ASSERT(this->is_hovered(pointerID))
		this->hovered.erase(pointerID);
	}

	this->on_hover_change(pointerID);
}
