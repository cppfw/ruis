#include "TextField.hpp"


using namespace morda;



TextField::TextField(const stob::Node* desc) :
		Widget(desc),
		ti(ting::New<TextInput>(desc))
{
	this->Content().Add(this->ti);
}
