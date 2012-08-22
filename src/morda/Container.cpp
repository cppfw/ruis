#include "Container.hpp"

#include <tride/Vector3.hpp>
#include <ting/Singleton.hpp>

#include "App.hpp"
#include "util/util.hpp"



using namespace morda;



void Container::ApplyProperties(stob::Node* properties){
	if(!properties){
		return;
	}
	
	if(const stob::Node* n = properties->GetProperty("containerLayout")){
		this->SetLayout(morda::App::Inst().Inflater().CreateLayout(*n));
	}
	
	while(stob::Node *p = properties->Child()){
//		TRACE(<< "Container::Container(): Child->Value() = " << p->Value() << std::endl)
		if(p->Value().size() == 0 || !morda::IsUpperCase(p->Value()[0])){
			break;
		}

		this->Add(
				morda::App::Inst().Inflater().Inflate(
						properties->RemoveFirstChild()
					)
			);
	}

	if(stob::Node* prev = properties->Child()){
		for(stob::Node* p = prev->Next(); p;){
//			TRACE(<< "Container::Container(): p->Value() = " << p->Value() << std::endl)
//			TRACE(<< "Container::Container(): p->Next() = " << p->Next() << std::endl)
			if(p->Value().size() == 0 || !morda::IsUpperCase(p->Value()[0])){
				prev = p;
				p = p->Next();
				continue;
			}

			this->Add(
					morda::App::Inst().Inflater().Inflate(
							prev->RemoveNext()
						)
				);
			
			p = prev->Next();
		}
	}
}



//override
void Container::Render(const tride::Matr4f& matrix)const{
	//render border
	{
		morda::SimpleSingleColoringShader& s = App::Inst().shaders.simpleSingleColoring;
		s.Bind();
		tride::Matr4f matr(matrix);
		matr.Scale(this->Rect().d);
		s.SetMatrix(matr);
		s.SetColor(tride::Vec3f(1, 0, 1));
		s.DrawQuad01(GL_LINE_LOOP);
	}
	
	for(T_ChildList::const_iterator i = this->children.begin(); i != this->children.end(); ++i){
		if((*i)->IsHidden()){
			continue;
		}
		
		tride::Matr4f matr(matrix);
		matr.Translate((*i)->Rect().p);

		(*i)->Render(matr);
	}
}



//override
bool Container::OnMouseButtonDown(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Container::OnMouseButtonDown(): enter, button = " << button << ", pos = " << pos << std::endl)
	//Copy children list to iterate through it later, because the original list of children
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}
	
	//call children in reverse order
	for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
		if((*i)->isHidden || (*i)->isDisabled){
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget is hovered.
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}
		
		if((*i)->OnMouseButtonDown((pos - (*i)->Rect().p), button, pointerId)){
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseButtonUp(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Container::OnMouseButtonUp(): enter, button = " << button << ", pos = " << pos << std::endl)
	
	//Copy children list to iterate through it later, because the original list of children
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}
	
	//call children in reverse order
	for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
		if((*i)->isHidden || (*i)->isDisabled){
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget is hovered.
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}
		
		if((*i)->OnMouseButtonUp(pos - (*i)->Rect().p, button, pointerId)){
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseMove(const tride::Vec2f& pos, unsigned pointerId){
	//Copy children list to iterate through it later, because the original list of children
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}
	
	//call children in reverse order
	for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
		if((*i)->isHidden){
			ASSERT(!(*i)->IsHovered())
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			if((*i)->IsHovered()){
				(*i)->isHovered = false;
				(*i)->OnMouseOut();
			}
			continue;
		}
		
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}
		
		if((*i)->OnMouseMove(pos - (*i)->Rect().p, pointerId)){//consumed mouse move event
			//un-hover rest of the children
			for(++i; i != childs.rend(); ++i){
				if((*i)->IsHovered()){
					(*i)->isHovered = false;
					(*i)->OnMouseOut();
				}
			}
			return true;
		}		
	}
	return false;
}



void Container::OnMouseOut(){
	//un-hover all the children
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		if((*i)->IsHovered()){
			(*i)->isHovered = false;
			(*i)->OnMouseOut();
		}
	}
}



void Container::OnResize(){
	//TODO:
}



void Container::Add(const ting::Ref<Widget>& w){
	ASSERT_INFO(w, "Widget::Add(): widget pointer is 0")
	ASSERT(ting::Ref<Widget>(w->parent).IsNotValid())
	this->children.push_back(w);
	w->parent = this;

	ASSERT(!w->IsHovered())
}



bool Container::Remove(const ting::Ref<Widget>& w){
	ASSERT(w.IsValid())

	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		if(w == (*i)){
			this->children.erase(i);
			w->parent.Reset();
			if(w->IsHovered()){
				w->isHovered = false;
				w->OnMouseOut();
			}
			return true;
		}
	}
	return false;
}
