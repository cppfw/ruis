#include "TreeView.hpp"
#include "List.hpp"



using namespace morda;



TreeView::TreeView(const stob::Node* chain) :
		Widget(chain)
{
	this->Add(ting::New<VerticalList>());
}
