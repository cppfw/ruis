#include "../Morda.hpp"

#include "../util/util.hpp"

#include "Widget.hpp"

#include "Container.hpp"



using namespace morda;



widget::widget(const stob::Node* chain){
	if(const stob::Node* n = getProperty(chain, "layout")){
		this->layout = n->cloneChain();
	}

	if(const stob::Node* n = getProperty(chain, "x")){
		this->rectangle.p.x = morda::dimValueFromSTOB(*n);
	}else{
		this->rectangle.p.x = 0;
	}

	if(const stob::Node* n = getProperty(chain, "y")){
		this->rectangle.p.y = morda::dimValueFromSTOB(*n);
	}else{
		this->rectangle.p.y = 0;
	}

	if(const stob::Node* n = getProperty(chain, "dx")){
		this->rectangle.d.x = morda::dimValueFromSTOB(*n);
	}else{
		this->rectangle.d.x = 0;
	}

	if(const stob::Node* n = getProperty(chain, "dy")){
		this->rectangle.d.y = morda::dimValueFromSTOB(*n);
	}else{
		this->rectangle.d.y = 0;
	}

	//TODO: remove reading 'name' because it was deprecated in favor of id
	if(const stob::Node* p = getProperty(chain, "name")){
		TRACE_ALWAYS(<< "DEPRECATED!!! the 'name' attribute is deprecated (used by '" << p->value() << "'), use 'id' instead" << std::endl)
		this->id = p->value();
	}

	if(const stob::Node* p = getProperty(chain, "id")){
		this->id = p->value();
	}

	if(const stob::Node* p = getProperty(chain, "clip")){
		this->clip_v = p->asBool();
	}else{
		this->clip_v = false;
	}

	if(const stob::Node* p = getProperty(chain, "cache")){
		this->cache = p->asBool();
	}else{
		this->cache = false;
	}

	if(const stob::Node* p = getProperty(chain, "visible")){
		this->isVisible_v = p->asBool();
	}else{
		this->isVisible_v = true;
	}

	if(const stob::Node* p = getProperty(chain, "enabled")){
		this->isEnabled_v = p->asBool();
	}else{
		this->isEnabled_v = true;
	}
}



widget::LayoutParams::LayoutParams(const stob::Node* chain){
	if(auto n = getProperty(chain, "dx")){
		this->dim.x = real(dimValueFromLayoutStob(*n));
	}else{
		this->dim.x = LayoutParams::min_c;
	}

	if(auto n = getProperty(chain, "dy")){
		this->dim.y = real(dimValueFromLayoutStob(*n));
	}else{
		this->dim.y = LayoutParams::min_c;
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
			this->layOut();
		}
		return;
	}

	this->clearCache();
	this->rectangle.d = newDims;
	utki::clampBottom(this->rectangle.d.x, real(0.0f));
	utki::clampBottom(this->rectangle.d.y, real(0.0f));
	this->relayoutNeeded = false;
	this->onResize();//call virtual method
}



std::shared_ptr<Widget> widget::removeFromParent(){
	if(!this->parent_v){
		throw morda::exception("widget::RemoveFromParent(): widget is not added to the parent");
	}
	auto ret = this->sharedFromThis(this);
	this->parent_v->erase(this->parent_v->find(this));
	return ret;
}



std::shared_ptr<Widget> widget::replaceBy(std::shared_ptr<Widget> w) {
	if(!this->parent()){
		throw morda::Exc("this widget is not added to any parent");
	}

	this->parent()->insert(w, this->parent()->find(this));

	if(w && !w->layout){
		w->layout = std::move(this->layout);
	}

	return this->removeFromParent();
}



void widget::setRelayoutNeeded()noexcept{
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(this->parent_v){
		this->parent_v->setRelayoutNeeded();
	}
	this->cacheTex.reset();
}



void widget::renderInternal(const morda::Matr4r& matrix)const{
	if(!this->rect().d.isPositive()){
		return;
	}

	if(this->cache){
		if(this->cacheDirty){
			bool scissorTestWasEnabled = morda::inst().renderer().isScissorEnabled();
			morda::inst().renderer().setScissorEnabled(false);

			//check if can re-use old texture
			if(!this->cacheTex || this->cacheTex->dim() != this->rect().d){
				this->cacheTex = this->renderToTexture();
			}else{
				ASSERT(this->cacheTex->dim() == this->rect().d)
				this->cacheTex = this->renderToTexture(std::move(this->cacheTex));
			}

			morda::inst().renderer().setScissorEnabled(scissorTestWasEnabled);
			this->cacheDirty = false;
		}

		//After rendering to texture it is most likely there will be transparent areas, so enable simple blending
		applySimpleAlphaBlending();

		this->renderFromCache(matrix);
	}else{
		if(this->clip_v){
	//		TRACE(<< "widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

			//set scissor test
			r4::recti scissor = this->computeViewportRect(matrix);

			r4::recti oldScissor;
			bool scissorTestWasEnabled = morda::inst().renderer().isScissorEnabled();
			if(scissorTestWasEnabled){
				oldScissor = morda::inst().renderer().getScissorRect();
				scissor.intersect(oldScissor);
			}else{
				morda::inst().renderer().setScissorEnabled(true);
			}

			morda::inst().renderer().setScissorRect(scissor);

			this->render(matrix);

			if(scissorTestWasEnabled){
				morda::inst().renderer().setScissorRect(oldScissor);
			}else{
				morda::inst().renderer().setScissorEnabled(false);
			}
		}else{
			this->render(matrix);
		}
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::ColorPosShader& s = App::inst().shaders.colorPosShader;
	s.Bind();
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);
	s.SetMatrix(matr);

	if(this->isHovered()){
		s.SetColor(r4::vec3f(0, 1, 0));
	}else{
		s.SetColor(r4::vec3f(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

std::shared_ptr<Texture2D> widget::renderToTexture(std::shared_ptr<Texture2D> reuse) const {
	std::shared_ptr<Texture2D> tex;

	if(reuse && reuse->dim() == this->rect().d){
		tex = std::move(reuse);
	}else{
		tex = morda::inst().renderer().factory->createTexture2D(
				morda::Texture2D::TexType_e::RGBA,
				this->rect().d.to<unsigned>(),
				nullptr
			);
	}

	auto& r = morda::inst().renderer();

	ASSERT(tex)

	r.setFramebuffer(r.factory->createFramebuffer(tex));

//	ASSERT_INFO(Render::isBoundFrameBufferComplete(), "tex.dim() = " << tex.dim())

	auto oldViewport = morda::inst().renderer().getViewport();
	utki::ScopeExit scopeExit([&oldViewport](){
		morda::inst().renderer().setViewport(oldViewport);
	});

	morda::inst().renderer().setViewport(r4::recti(r4::vec2i(0), this->rect().d.to<int>()));

	morda::inst().renderer().clearFramebuffer();

	Matr4r matrix = morda::inst().renderer().initialMatrix;
	matrix.translate(-1, 1);
	matrix.scale(Vec2r(2.0f, -2.0f).compDivBy(this->rect().d));

	this->render(matrix);

	r.setFramebuffer(nullptr);

	return tex;
}

void widget::renderFromCache(const r4::mat4f& matrix) const {
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	auto& r = morda::inst().renderer();
	ASSERT(this->cacheTex)
	r.shader->posTex->render(matr, *r.posTexQuad01VAO, *this->cacheTex);
}

void widget::clearCache(){
	this->cacheDirty = true;
	if(this->parent_v){
		this->parent_v->clearCache();
	}
}


void widget::onKeyInternal(bool isDown, Key_e keyCode){
	if(this->isInteractive()){
		if(this->onKey(isDown, keyCode)){
			return;
		}
	}

	if(this->parent()){
		this->parent()->onKeyInternal(isDown, keyCode);
	}
}



void widget::focus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(this->isFocused()){
		return;
	}

	Morda::inst().setFocusedWidget(this->sharedFromThis(this));
}



void widget::unfocus()noexcept{
//	ASSERT(App::inst().thisIsUIThread())

	if(!this->isFocused()){
		return;
	}

	ASSERT(Morda::inst().focusedWidget.lock() && Morda::inst().focusedWidget.lock().operator->() == this)

	Morda::inst().setFocusedWidget(nullptr);
}











r4::recti widget::computeViewportRect(const Matr4r& matrix) const noexcept{
	r4::recti ret(
			((matrix * Vec2r(0, 0) + Vec2r(1, 1)) / 2).compMulBy(morda::inst().renderer().getViewport().d.to<real>()).rounded().to<int>(),
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


Vec2r widget::calcPosInParent(Vec2r pos, const Widget* parent) {
	if(parent == this || !this->parent()){
		return pos;
	}

	ASSERT(this->parent())

	return this->parent()->calcPosInParent(this->rect().p + pos, parent);
}


widget::LayoutParams& widget::getLayoutParams() {
	if(!this->parent()){
		throw morda::Exc("widget::getLayoutParams(): widget is not added to any container, cannot get layout params. In order to get layout params the widget should be added to some container.");
	}

	return this->parent()->getLayoutParams(*this);
}


const widget::LayoutParams& widget::getLayoutParams()const {
	if(!this->parent()){
		throw morda::Exc("widget::getLayoutParams(): widget is not added to any container, cannot get layout params. In order to get layout params the widget should be added to some container.");
	}

	return this->parent()->getLayoutParams(*this);
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

void widget::setEnabled(bool enable) {
//	TRACE(<< "widget::setEnabled(): enable = " << enable << " this->name() = " << this->name()<< std::endl)
	if(this->isEnabled_v == enable){
		return;
	}

	this->isEnabled_v = enable;

	//Un-hover this widget if it becomes disabled because it is not supposed to receive mouse input.
	if(!this->isEnabled()){
		this->setUnhovered();
	}

	this->onEnabledChanged();
}

void widget::setVisible(bool visible) {
	this->isVisible_v = visible;
	if (!this->isVisible_v) {
		this->setUnhovered();
	}
}

void widget::setUnhovered() {
	auto hoverSet = std::move(this->hovered);
	ASSERT(this->hovered.size() == 0)
	for(auto h : hoverSet){
		this->onHoverChanged(h);
	}
}


void widget::setHovered(bool isHovered, unsigned pointerID) {
	if(isHovered == this->isHovered(pointerID)){
		return;
	}
//	TRACE(<< "widget::setHovered(): isHovered = " << isHovered << " this->name() = " << this->name() << std::endl)

	if (isHovered) {
		ASSERT(!this->isHovered(pointerID))
		this->hovered.insert(pointerID);
	} else {
		ASSERT(this->isHovered(pointerID))
		this->hovered.erase(pointerID);
	}

	this->onHoverChanged(pointerID);
}


Vec2r widget::posInAncestor(const Widget& ancestor) {
	Vec2r ret = this->rect().p;

	if(this->parent() && static_cast<const Widget*>(this->parent()) != &ancestor){
		ret += this->parent()->posInAncestor(ancestor);
	}

	return ret;
}
