#include "TableRow.hpp"


using namespace morda;


TableRow::TableRow(const stob::Node* chain) :
		Widget(chain),
		HorizontalContainer(chain)
{ }


TableRow::LayoutParams::LayoutParams(const stob::Node* chain) :
		HorizontalContainer::LayoutParams(chain),
		modifiedParams(nullptr)
{
}
