#include "../shaders/PosShader.hpp"

#include "../render/FrameBuffer.hpp"

#include "Widget.hpp"

#include "containers/Container.hpp"

#include "../App.hpp"
#include "../util/util.hpp"

using namespace morda;



Widget::Widget(const stob::Node* chain){
	if(const stob::Node* n = GetProperty(chain, "layout")){
		this->layout = n->cloneChain();
	}

	if(const stob::Node* n = GetProperty(chain, "posX")){
		this->rectangle.p.x = morda::DimValueFromSTOB(*n);
	}else{
		this->rectangle.p.x = 0;
	}
	
	if(const stob::Node* n = GetProperty(chain, "posY")){
		this->rectangle.p.y = morda::DimValueFromSTOB(*n);
	}else{
		this->rectangle.p.y = 0;
	}

	if(const stob::Node* n = GetProperty(chain, "dimX")){
		this->rectangle.d.x = morda::DimValueFromSTOB(*n);
	}else{
		this->rectangle.d.x = 0;
	}
	
	if(const stob::Node* n = GetProperty(chain, "dimY")){
		this->rectangle.d.y = morda::DimValueFromSTOB(*n);
	}else{
		this->rectangle.d.y = 0;
	}

	if(const stob::Node* p = GetProperty(chain, "name")){
		this->nameOfWidget = p->value();
	}

	if(const stob::Node* p = GetProperty(chain, "clip")){
		this->clip_var = p->asBool();
	}else{
		this->clip_var = false;
	}
	
	if(const stob::Node* p = GetProperty(chain, "cache")){
		this->cache = p->asBool();
	}else{
		this->cache = false;
	}
	
	if(const stob::Node* p = GetProperty(chain, "visible")){
		this->isVisible_var = p->asBool();
	}else{
		this->isVisible_var = true;
	}
	
	if(const stob::Node* p = GetProperty(chain, "enabled")){
		this->isEnabled_var = p->asBool();
	}else{
		this->isEnabled_var = true;
	}
}



namespace{
real dimValueFromLayoutStob(const stob::Node& n){
	if(n == "min"){
		return Widget::LayoutParams::D_Min;
	}else if(n == "max"){
		return Widget::LayoutParams::D_Max;
	}
	return DimValueFromSTOB(n);
}
}



Widget::LayoutParams::LayoutParams(const stob::Node* chain){
	if(auto n = GetProperty(chain, "dimX")){
		this->dim.x = dimValueFromLayoutStob(*n);
	}else{
		this->dim.x = LayoutParams::D_Min;
	}
	
	if(auto n = GetProperty(chain, "dimY")){
		this->dim.y = dimValueFromLayoutStob(*n);
	}else{
		this->dim.y = LayoutParams::D_Min;
	}
}



std::shared_ptr<Widget> Widget::findChildByName(const std::string& name)noexcept{
	if(this->name() == name){
		return this->sharedFromThis(this);
	}
	return nullptr;
}



void Widget::resize(const morda::Vec2r& newDims){
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



std::shared_ptr<Widget> Widget::removeFromParent(){
	if(!this->parentContainer){
		throw morda::Exc("Widget::RemoveFromParent(): widget is not added to the parent");
	}
	return this->parentContainer->Remove(*this);
}



void Widget::setRelayoutNeeded()noexcept{
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(this->parentContainer){
		this->parentContainer->setRelayoutNeeded();
	}
	this->cacheTex = Texture2D();
}



void Widget::renderInternal(const morda::Matr4r& matrix)const{
	if(this->cache){
		if(!this->cacheTex){
			bool scissorTestWasEnabled = Render::isScissorEnabled();
			Render::setScissorEnabled(false);
			
			this->cacheTex = this->renderToTexture();//TODO: make reusing the tex
			
			Render::setScissorEnabled(scissorTestWasEnabled);
		}
		this->renderFromCache(matrix);
	}else{
		if(this->clip_var){
	//		TRACE(<< "Widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

			//set scissor test
			Rect2i scissor = this->ComputeViewportRect(matrix);

			Rect2i oldScissor;
			bool scissorTestWasEnabled = Render::isScissorEnabled();
			if(scissorTestWasEnabled){
				oldScissor = Render::getScissorRect();
				scissor.Intersect(oldScissor);
			}else{
				Render::setScissorEnabled(true);
			}

			Render::setScissorRect(scissor);

			this->render(matrix);

			if(scissorTestWasEnabled){
				Render::setScissorRect(oldScissor);
			}else{
				Render::setScissorEnabled(false);
			}
		}else{
			this->render(matrix);
		}
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::ColorPosShader& s = App::inst().Shaders().colorPosShader;
	s.Bind();
	morda::Matr4r matr(matrix);
	matr.Scale(this->rect().d);
	s.SetMatrix(matr);

	if(this->isHovered()){
		s.SetColor(morda::Vec3f(0, 1, 0));
	}else{
		s.SetColor(morda::Vec3f(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

Texture2D Widget::renderToTexture(Texture2D&& reuse) const {
	Texture2D tex;
	
	if(reuse && reuse.Dim() == this->rect().d){
		tex = std::move(reuse);
	}else{
		tex = Texture2D(
				this->rect().d.to<unsigned>(),
				4,
				Render::ETexFilter::NEAREST,
				Render::ETexFilter::NEAREST
			);
	}
	
	Render::unbindTexture(0);
	
	FrameBuffer fb;
	
	fb.bind();
	
	fb.attachColor(std::move(tex));
	
	ASSERT(Render::isBoundFrameBufferComplete())
	
	Render::setViewport(Rect2i(Vec2i(0), this->rect().d.to<int>()));
	
	Render::clearColor(Vec4f(0.0f));
	
	Matr4r matrix;
	matrix.Identity();
	matrix.Translate(-1, -1);
	matrix.Scale(Vec2r(2.0f).CompDivBy(this->rect().d));
	
	this->render(matrix);
	
	tex = fb.detachColor();
	
	fb.unbind();
	Render::setViewport(App::inst().winRect().to<int>());
	
	return std::move(tex);
}

void Widget::renderFromCache(const Matr4f& matrix) const {
	morda::Matr4r matr(matrix);
	matr.Scale(this->rect().d);
	
	morda::PosTexShader &s = App::inst().Shaders().posTexShader;

	ASSERT(this->cacheTex)
	this->cacheTex.bind();
	
	s.SetMatrix(matr);
	
	s.render(utki::wrapBuf(morda::PosShader::quad01Fan), utki::wrapBuf(s.quadFanTexCoords));
}

void Widget::clearCache(){
	this->cacheTex = Texture2D();
	if(this->parentContainer){
		this->parentContainer->clearCache();
	}
}


void Widget::onKeyInternal(bool isDown, EKey keyCode){
	if(this->isInteractive()){
		if(this->onKey(isDown, keyCode)){
			return;
		}
	}

	if(this->parent()){
		this->parent()->onKeyInternal(isDown, keyCode);
	}
}



void Widget::focus()noexcept{
	ASSERT(App::inst().ThisIsUIThread())

	if(this->isFocused()){
		return;
	}

	App::inst().SetFocusedWidget(this->sharedFromThis(this));
}



void Widget::unfocus()noexcept{
	ASSERT(App::inst().ThisIsUIThread())

	if(!this->isFocused()){
		return;
	}

	ASSERT(App::inst().focusedWidget.lock() && App::inst().focusedWidget.lock().operator->() == this)

	App::inst().SetFocusedWidget(nullptr);
}



bool Widget::isTopmost()const noexcept{
	if(!this->parent()){
		return false;
	}
	
	ASSERT(this->parent()->children().size() != 0)
	
	return this->parent()->children().back().get() == this;
}



void Widget::makeTopmost(){
	if(!this->parent()){
		return;
	}
	
	ASSERT(this->parent()->children().size() != 0)
	
	if(this->parent()->children().size() == 1){
		return;
	}
	
	if(this->parent()->children().rbegin()->get() == this){
		return;//already topmost
	}
	
	Container* p = this->parent();
	
	auto w = this->removeFromParent();
	
	p->Add(w);
}



morda::Rect2i Widget::ComputeViewportRect(const Matr4r& matrix) const noexcept{
	return Rect2i(
			((matrix * Vec2r(0, 0) + Vec2r(1, 1)) / 2).compMulBy(Render::getViewport().d.to<real>()).Rounded().to<int>(),
			this->rect().d.to<int>()
		);
}


Vec2r Widget::measure(const morda::Vec2r& quotum) const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 0;
		}
	}
	return ret;
}


Vec2r Widget::calcPosInParent(Vec2r pos, const Widget* parent) {
	if(parent == this || !this->parent()){
		return pos;
	}
	
	ASSERT(this->parent())
	
	return this->parent()->calcPosInParent(this->rect().p + pos, parent);
}
