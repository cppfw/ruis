#include "../shaders/PosShader.hpp"

#include "Widget.hpp"

#include "../App.hpp"
#include "containers/Container.hpp"
#include "../util/util.hpp"

using namespace morda;



Widget::Widget(const stob::Node* chain){
	if(const stob::Node* n = GetProperty(chain, "layout")){
		this->layout = n->CloneChain();
	}

	if(const stob::Node* n = GetProperty(chain, "posX")){
		this->rect.p.x = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.p.x = 0;
	}
	
	if(const stob::Node* n = GetProperty(chain, "posY")){
		this->rect.p.y = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.p.y = 0;
	}

	if(const stob::Node* n = GetProperty(chain, "dimX")){
		this->rect.d.x = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.d.x = 0;
	}
	
	if(const stob::Node* n = GetProperty(chain, "dimY")){
		this->rect.d.y = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.d.y = 0;
	}

	if(const stob::Node* p = GetProperty(chain, "name")){
		this->name = p->Value();
	}

	if(const stob::Node* p = GetProperty(chain, "clip")){
		this->clip = p->AsBool();
	}else{
		this->clip = false;
	}
	
	if(const stob::Node* p = GetProperty(chain, "cache")){
		this->cache = p->AsBool();
	}else{
		this->cache = false;
	}
	
	if(const stob::Node* p = GetProperty(chain, "visible")){
		this->isVisible = p->AsBool();
	}else{
		this->isVisible = true;
	}
	
	if(const stob::Node* p = GetProperty(chain, "enabled")){
		this->isEnabled = p->AsBool();
	}else{
		this->isEnabled = true;
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



std::shared_ptr<Widget> Widget::FindChildByName(const std::string& name)NOEXCEPT{
	if(this->Name() == name){
		return this->SharedFromThis(this);
	}
	return nullptr;
}


std::unique_ptr<Widget::LayoutParams> Widget::ResetLayoutParams(std::unique_ptr<Widget::LayoutParams> params)NOEXCEPT{
	std::swap(this->layoutParams, params);
	this->SetRelayoutNeeded();
	return std::move(params);
}


void Widget::Resize(const morda::Vec2r& newDims){
	if(this->rect.d == newDims){
		if(this->relayoutNeeded){
			this->OnResize();
			this->relayoutNeeded = false;
		}
		return;
	}

	this->rect.d = newDims;
	ting::util::ClampBottom(this->rect.d.x, real(0.0f));
	ting::util::ClampBottom(this->rect.d.y, real(0.0f));
	this->OnResize();//call virtual method
	this->relayoutNeeded = false;
}



std::shared_ptr<Widget> Widget::RemoveFromParent(){
	if(!this->parent){
		throw morda::Exc("Widget::RemoveFromParent(): widget is not added to the parent");
	}
	return this->parent->Remove(*this);
}



void Widget::SetRelayoutNeeded()NOEXCEPT{
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(this->parent){
		this->parent->SetRelayoutNeeded();
	}
}



void Widget::RenderInternal(const morda::Matr4r& matrix)const{
	if(this->clip){
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
		
		this->Render(matrix);
		
		if(scissorTestWasEnabled){
			Render::setScissorRect(oldScissor);
		}else{
			Render::setScissorEnabled(false);
		}
	}else{
		this->Render(matrix);
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::ColorPosShader& s = App::Inst().Shaders().colorPosShader;
	s.Bind();
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);
	s.SetMatrix(matr);

	if(this->IsHovered()){
		s.SetColor(morda::Vec3f(0, 1, 0));
	}else{
		s.SetColor(morda::Vec3f(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

std::unique_ptr<Texture2D> Widget::renderToTexture() const {
	//TODO:
	return nullptr;
}

void Widget::renderFromCache(const Matr4f& matrix) const {
	//TODO:
}



void Widget::OnKeyInternal(bool isDown, EKey keyCode){
	if(this->OnKey(isDown, keyCode)){
		return;
	}

	//pass key event to parent
	if(this->Parent()){
		this->Parent()->OnKeyInternal(isDown, keyCode);
	}
}



void Widget::Focus()NOEXCEPT{
	ASSERT(App::Inst().ThisIsUIThread())

	if(this->IsFocused()){
		return;
	}

	App::Inst().SetFocusedWidget(this->SharedFromThis(this));
}



void Widget::Unfocus()NOEXCEPT{
	ASSERT(App::Inst().ThisIsUIThread())

	if(!this->IsFocused()){
		return;
	}

	ASSERT(App::Inst().focusedWidget.lock() && App::Inst().focusedWidget.lock().operator->() == this)

	App::Inst().SetFocusedWidget(nullptr);
}



bool Widget::IsTopmost()const NOEXCEPT{
	if(!this->Parent()){
		return false;
	}
	
	ASSERT(this->Parent()->Children().size() != 0)
	
	return this->Parent()->Children().back().get() == this;
}



void Widget::MakeTopmost(){
	if(!this->Parent()){
		return;
	}
	
	ASSERT(this->Parent()->Children().size() != 0)
	
	if(this->Parent()->Children().size() == 1){
		return;
	}
	
	if(this->Parent()->Children().rbegin()->get() == this){
		return;//already topmost
	}
	
	Container* p = this->Parent();
	
	auto w = this->RemoveFromParent();
	
	p->Add(w);
}



morda::Rect2i Widget::ComputeViewportRect(const Matr4r& matrix) const NOEXCEPT{
	return Rect2i(
			((matrix * Vec2r(0, 0) + Vec2r(1, 1)) / 2).CompMulBy(App::Inst().viewportDim()).Rounded().ConvertTo<int>(),
			this->Rect().d.ConvertTo<int>()
		);
}


Vec2r Widget::onMeasure(const morda::Vec2r& quotum) const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 0;
		}
	}
	return ret;
}
