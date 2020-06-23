#include "pile_page.hpp"

pile_page::pile_page(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), puu::forest()),
		page(this->context, puu::forest()),
		pile(this->context, desc)
{}
