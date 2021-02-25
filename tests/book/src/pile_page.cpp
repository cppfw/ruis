#include "pile_page.hpp"

pile_page::pile_page(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), treeml::forest()),
		page(this->context, treeml::forest()),
		pile(this->context, desc)
{}
