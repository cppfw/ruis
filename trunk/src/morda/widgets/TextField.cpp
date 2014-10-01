#include "TextField.hpp"
#include "../App.hpp"


using namespace morda;



TextField::TextField(const stob::Node* desc) :
		Widget(desc),
		ti(ting::New<TextInput>(desc))
{
	this->SetNinePatch(morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_textfield"));
	
	this->Content().Add(this->ti);
}
