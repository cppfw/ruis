#include "TextField.hpp"
#include "../App.hpp"


using namespace morda;



TextField::TextField(const stob::Node* chain) :
		Widget(chain),
		ti(ting::New<TextInput>(chain))
{
	this->SetNinePatch(morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_textfield"));
	
	this->Content().Add(this->ti);
}
