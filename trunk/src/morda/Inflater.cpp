#include "Inflater.hpp"

#include "widgets/Container.hpp"
#include "widgets/ColorLabel.hpp"
#include "widgets/Label.hpp"
#include "widgets/LinearContainer.hpp"
#include "widgets/FrameContainer.hpp"
#include "widgets/TextInput.hpp"
#include "widgets/Slider.hpp"
#include "widgets/ImageLabel.hpp"
#include "widgets/Window.hpp"
#include "widgets/MouseProxy.hpp"
#include "widgets/NinePatch.hpp"
#include "widgets/PushButton.hpp"
#include "widgets/TextField.hpp"

#include "App.hpp"

#include "util/util.hpp"



using namespace morda;



Inflater::Inflater(){
	this->AddWidget<Widget>("Widget");
	this->AddWidget<Container>("Container");
	this->AddWidget<LinearContainer>("LinearContainer");
	this->AddWidget<FrameContainer>("FrameContainer");
	this->AddWidget<Label>("Label");
	this->AddWidget<TextInput>("TextInput");
	this->AddWidget<Slider>("Slider");
	this->AddWidget<ImageLabel>("ImageLabel");
	this->AddWidget<Window>("Window");
	this->AddWidget<MouseProxy>("MouseProxy");
	this->AddWidget<NinePatch>("NinePatch");
	this->AddWidget<PushButton>("PushButton");
	this->AddWidget<ColorLabel>("ColorLabel");
	this->AddWidget<TextField>("TextField");
}



void Inflater::AddWidgetFactory(const std::string& widgetName, std::unique_ptr<WidgetFactory> factory){
	std::pair<T_FactoryMap::iterator, bool> ret = this->widgetFactories.insert(
			std::pair<std::string, std::unique_ptr<Inflater::WidgetFactory> >(
					widgetName,
					std::move(factory)
				)
		);
	if(!ret.second){
		throw Inflater::Exc("Failed registering widget type, widget type with given name is already added");
	}
}



bool Inflater::RemoveWidget(const std::string& widgetName)NOEXCEPT{
	if(this->widgetFactories.erase(widgetName) == 0){
		return false;
	}
	return true;
}



std::shared_ptr<morda::Widget> Inflater::Inflate(ting::fs::File& fi) {
	std::unique_ptr<stob::Node> root = this->Load(fi);
	ASSERT(root)

	return std::move(this->Inflate(*root));
}


namespace{
const char* D_Templates = "templates";
}


std::shared_ptr<morda::Widget> Inflater::Inflate(const stob::Node& chain){
	if(!App::Inst().ThisIsUIThread()){
		throw Inflater::Exc("Inflate called not from UI thread");
	}
	
	const stob::Node* n = &chain;
	for(; n && n->IsProperty(); n = n->Next()){
		if(*n == D_Templates && n->Child()){
			this->PushTemplates(n->Child()->CloneChain());
		}else{
			throw Exc("Inflater::Inflate(): unknown declaration encountered before first widget");
		}
	}
	
	T_FactoryMap::const_iterator i = this->widgetFactories.find(n->Value());

	if(i == this->widgetFactories.end()){
		TRACE(<< "Inflater::Inflate(): n->Value() = " << n->Value() << std::endl)
		throw Exc("Failed to inflate, no matching factory found for requested widget name");
	}

	bool needPop = false;
	ting::util::ScopeExit scopeExit([this, &needPop](){
		if(needPop){
			this->PopTemplates();
		}
	});
	
	if(auto t = n->Child(D_Templates).node()){
		if(auto c = t->Child()){
			needPop = this->PushTemplates(c->CloneChain());
		}
	}
	
	return i->second->Create(n->Child());
}



std::unique_ptr<stob::Node> Inflater::Load(ting::fs::File& fi){
	std::unique_ptr<stob::Node> ret = stob::Load(fi);
	
	ret = std::move(std::get<0>(ResolveIncludes(fi, std::move(ret))));

	return ret;
}



namespace{

std::unique_ptr<stob::Node> MergeGUIChain(const stob::Node* from, std::unique_ptr<stob::Node> to){
	if(!to){
		if(!from){
			return nullptr;
		}
		return from->CloneChain();
	}
	
	std::unique_ptr<stob::Node> children; //children will be stored in reverse order
	
	for(auto s = from; s; s = s->Next()){
		if(!s->IsProperty()){
			auto c = s->Clone();
			c->SetNext(std::move(children));
			children = std::move(c);
			continue;
		}
		
		if(!s->Child()){
			return to;//no children means that it is a property value, stop further processing of this chain
		}
		
		auto d = to->ThisOrNext(s->Value()).node();
		if(!d){
			to->InsertNext(s->Clone());
			continue;
		}
		
		if(!d->Child()){
			continue;//no children means that the property is removed in derived template
		}
		
		d->SetChildren(MergeGUIChain(s->Child(), d->RemoveChildren()));
	}
	
	//add children in reverse order again, so it will be in normal order in the end
	for(; children;){
		auto c = std::move(children);
		children = c->ChopNext();
		
		to->InsertNext(std::move(c));
	}
	
	return to;
}

}


bool Inflater::PushTemplates(std::unique_ptr<stob::Node> chain){
	decltype(this->templates)::value_type m;
	
	for(; chain; chain = chain->ChopNext()){
		if(chain->Child()){
			throw Exc("Inflater::PushTemplates(): template name has children, error.");
		}
		
		if(!chain->Next()){
			throw Exc("Inflater::PushTemplates(): template name is not followed by template definition, error.");
		}
		
		if(!m.insert(std::make_pair(chain->Value(), chain->RemoveNext())).second){
			throw Exc("Inflater::PushTemplates(): template name is already defined in given templates chain, error.");
		}
	}
	
	if(m.size() == 0){
		return false;
	}
	
	for(auto i = m.begin(); i != m.end(); ++i){
		if(auto s = this->FindTemplate(i->second->Value())){
			i->second->SetValue(s->Value());
			ASSERT(s->Child())
			i->second->SetChildren(MergeGUIChain(s->Child(), i->second->RemoveChildren()));
		}
	}
	
	this->templates.push_back(std::move(m));
	
	return true;
}



void Inflater::PopTemplates(){
	if(this->templates.size() != 0){
		this->templates.pop_back();
	}
}



const stob::Node* Inflater::FindTemplate(const std::string& name)const{
	for(auto i = this->templates.rbegin(); i != this->templates.rend(); ++i){
		auto r = i->find(name);
		if(r != i->end()){
			return r->second.get();
		}
	}
	
	return nullptr;
}

