#include "../context.hpp"

#include "../util/util.hpp"

#include "widget.hpp"

#include "container.hpp"

//TODO: remove
#include "../gui.hpp"


using namespace morda;


widget::widget(const puu::forest& desc){
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		try{
			if(p.value == "layout"){
				this->layout_desc = p.children;
			}else if(p.value == "x"){
				this->rectangle.p.x = parse_dimension_value(get_property_value(p));
			}else if(p.value == "y"){
				this->rectangle.p.y = parse_dimension_value(get_property_value(p));
			}else if(p.value == "dx"){
				this->rectangle.d.x = parse_dimension_value(get_property_value(p));
			}else if(p.value == "dy"){
				this->rectangle.d.y = parse_dimension_value(get_property_value(p));
			}else if(p.value == "id"){
				this->id = get_property_value(p).to_string();
			}else if(p.value == "name"){ //TODO: remove deprecated stuff.
				this->id = get_property_value(p).to_string();
				TRACE_ALWAYS(<< "DEPRECATED!!! the 'name' attribute is deprecated (used by '" << this->id << "'), use 'id' instead" << std::endl)
			}else if(p.value == "clip"){
				this->clip_v = get_property_value(p).to_bool();
			}else if(p.value == "cache"){
				this->cache = get_property_value(p).to_bool();
			}else if(p.value == "visible"){
				this->isVisible_v = get_property_value(p).to_bool();
			}else if(p.value == "enabled"){
				this->isEnabled_v = get_property_value(p).to_bool();
			}
		}catch(std::invalid_argument&){
			TRACE(<< "could not parse value of " << puu::to_string(p) << std::endl)
			throw;
		}
	}
}

widget::layout_params::layout_params(const puu::forest& desc){
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		try{
			if(p.value == "dx"){
				this->dims.x = parse_layout_dimension_value(get_property_value(p));
			}else if(p.value == "dy"){
				this->dims.y = parse_layout_dimension_value(get_property_value(p));
			}
		}catch(std::invalid_argument&){
			TRACE(<< "could not parse value of " << puu::to_string(p) << std::endl)
			throw;
		}
	}
}



std::shared_ptr<Widget> widget::try_get_widget(const std::string& id)noexcept{
	if(this->id == id){
		return this->sharedFromThis(this);
	}
	return nullptr;
}



void widget::resize(const morda::Vec2r& newDims){
	if(this->rectangle.d == newDims){
		if(this->relayoutNeeded){
			this->clearCache();
			this->relayoutNeeded = false;
			this->lay_out();
		}
		return;
	}

	this->clearCache();
	this->rectangle.d = newDims;
	utki::clampBottom(this->rectangle.d.x, real(0.0f));
	utki::clampBottom(this->rectangle.d.y, real(0.0f));
	this->relayoutNeeded = false;
	this->on_resize(); // call virtual method
}



std::shared_ptr<Widget> widget::remove_from_parent(){
	if(!this->parent_v){
		throw morda::exception("widget::RemoveFromParent(): widget is not added to the parent");
	}
	auto ret = this->sharedFromThis(this);
	this->parent_v->erase(this->parent_v->find(this));
	return ret;
}



std::shared_ptr<Widget> widget::replace_by(std::shared_ptr<Widget> w) {
	if(!this->parent()){
		throw morda::Exc("this widget is not added to any parent");
	}

	this->parent()->insert(w, this->parent()->find(this));

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
	if(this->parent_v){
		this->parent_v->invalidate_layout();
	}
	this->cacheTex.reset();
}



void widget::renderInternal(const morda::Matr4r& matrix)const{
	if(!this->rect().d.isPositive()){
		return;
	}

	if(this->cache){
		if(this->cacheDirty){
			bool scissorTestWasEnabled = morda::inst().context->renderer->isScissorEnabled();
			morda::inst().context->renderer->setScissorEnabled(false);

			//check if can re-use old texture
			if(!this->cacheTex || this->cacheTex->dims() != this->rect().d){
				this->cacheTex = this->render_to_texture();
			}else{
				ASSERT(this->cacheTex->dims() == this->rect().d)
				this->cacheTex = this->render_to_texture(std::move(this->cacheTex));
			}

			morda::inst().context->renderer->setScissorEnabled(scissorTestWasEnabled);
			this->cacheDirty = false;
		}

		// after rendering to texture it is most likely there will be transparent areas, so enable simple blending
		applySimpleAlphaBlending();

		this->renderFromCache(matrix);
	}else{
		if(this->clip_v){
	//		TRACE(<< "widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

			// set scissor test
			r4::recti scissor = this->compute_viewport_rect(matrix);

			r4::recti oldScissor;
			auto& r = *morda::inst().context->renderer;
			bool scissorTestWasEnabled = r.isScissorEnabled();
			if(scissorTestWasEnabled){
				oldScissor = r.getScissorRect();
				scissor.intersect(oldScissor);
			}else{
				r.setScissorEnabled(true);
			}

			r.setScissorRect(scissor);

			this->render(matrix);

			if(scissorTestWasEnabled){
				r.setScissorRect(oldScissor);
			}else{
				r.setScissorEnabled(false);
			}
		}else{
			this->render(matrix);
		}
	}

	// render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::ColorPosShader& s = App::inst().shaders.colorPosShader;
	s.Bind();
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);
	s.SetMatrix(matr);

	if(this->is_hovered()){
		s.SetColor(r4::vec3f(0, 1, 0));
	}else{
		s.SetColor(r4::vec3f(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

std::shared_ptr<Texture2D> widget::render_to_texture(std::shared_ptr<Texture2D> reuse)const{
	auto& r = *morda::inst().context->renderer;

	std::shared_ptr<Texture2D> tex;

	if(reuse && reuse->dims() == this->rect().d){
		tex = std::move(reuse);
	}else{
		tex = r.factory->createTexture2D(
				morda::Texture2D::TexType_e::RGBA,
				this->rect().d.to<unsigned>(),
				nullptr
			);
	}

	ASSERT(tex)

	r.setFramebuffer(r.factory->createFramebuffer(tex));

//	ASSERT_INFO(Render::isBoundFrameBufferComplete(), "tex.dims() = " << tex.dims())

	auto oldViewport = r.getViewport();
	utki::ScopeExit scopeExit([&oldViewport, &r](){
		r.setViewport(oldViewport);
	});

	r.setViewport(r4::recti(r4::vec2i(0), this->rect().d.to<int>()));

	r.clearFramebuffer();

	Matr4r matrix = r.initialMatrix;
	matrix.translate(-1, 1);
	matrix.scale(Vec2r(2.0f, -2.0f).compDivBy(this->rect().d));

	this->render(matrix);

	r.setFramebuffer(nullptr);

	return tex;
}

void widget::renderFromCache(const r4::mat4f& matrix) const {
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	auto& r = *morda::inst().context->renderer;
	ASSERT(this->cacheTex)
	r.shader->posTex->render(matr, *r.posTexQuad01VAO, *this->cacheTex);
}

void widget::clearCache(){
	this->cacheDirty = true;
	if(this->parent_v){
		this->parent_v->clearCache();
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

	gui::inst().context->set_focused_widget(this->sharedFromThis(this));
}



void widget::unfocus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(!this->is_focused()){
		return;
	}

	ASSERT(gui::inst().context->focused_widget.lock() && gui::inst().context->focused_widget.lock().get() == this)

	gui::inst().context->set_focused_widget(nullptr);
}



r4::recti widget::compute_viewport_rect(const Matr4r& matrix) const noexcept{
	r4::recti ret(
			((matrix * Vec2r(0, 0) + Vec2r(1, 1)) / 2).compMulBy(morda::inst().context->renderer->getViewport().d.to<real>()).rounded().to<int>(),
			this->rect().d.to<int>()
		);
	ret.p.y -= ret.d.y;
	return ret;
}


Vec2r widget::measure(const morda::Vec2r& quotum) const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 0;
		}
	}
	return ret;
}


vector2 widget::pos_in_ancestor(vector2 pos, const widget* ancestor) {
	if(ancestor == this || !this->parent()){
		return pos;
	}

	ASSERT(this->parent())

	return this->parent()->pos_in_ancestor(this->rect().p + pos, ancestor);
}


widget::LayoutParams& widget::get_layout_params() {
	if(!this->parent()){
		throw morda::Exc("widget::get_layout_params(): widget is not added to any container, cannot get layout params. In order to get layout params the widget should be added to some container.");
	}

	return this->parent()->get_layout_params(*this);
}


const widget::LayoutParams& widget::get_layout_params()const {
	if(!this->parent()){
		throw morda::Exc("widget::get_layout_params(): widget is not added to any container, cannot get layout params. In order to get layout params the widget should be added to some container.");
	}

	return this->parent()->get_layout_params(*this);
}

Widget& widget::get_widget(const std::string& id) {
	auto w = this->try_get_widget(id);
	if(!w){
		std::stringstream ss;
		ss << "Widget '" << id << "' not found in '" << this->id << "'";
		throw utki::not_found(ss.str());
	}
	return *w;
}

void widget::set_enabled(bool enable) {
//	TRACE(<< "widget::set_enabled(): enable = " << enable << " this->name() = " << this->name()<< std::endl)
	if(this->isEnabled_v == enable){
		return;
	}

	this->isEnabled_v = enable;

	//Un-hover this widget if it becomes disabled because it is not supposed to receive mouse input.
	if(!this->is_enabled()){
		this->set_unhovered();
	}

	this->on_enabled_changed();
}

void widget::set_visible(bool visible) {
	this->isVisible_v = visible;
	if (!this->isVisible_v) {
		this->set_unhovered();
	}
}

void widget::set_unhovered() {
	auto hoverSet = std::move(this->hovered);
	ASSERT(this->hovered.size() == 0)
	for(auto h : hoverSet){
		this->on_hover_changed(h);
	}
}


void widget::set_hovered(bool isHovered, unsigned pointerID) {
	if(isHovered == this->is_hovered(pointerID)){
		return;
	}
//	TRACE(<< "widget::setHovered(): isHovered = " << isHovered << " this->name() = " << this->name() << std::endl)

	if (isHovered) {
		ASSERT(!this->is_hovered(pointerID))
		this->hovered.insert(pointerID);
	} else {
		ASSERT(this->is_hovered(pointerID))
		this->hovered.erase(pointerID);
	}

	this->on_hover_changed(pointerID);
}
