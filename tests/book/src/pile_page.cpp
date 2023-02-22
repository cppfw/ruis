#include "pile_page.hpp"

pile_page::pile_page(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		widget(std::move(c), treeml::forest()),
		page(this->context, treeml::forest()),
		pile(this->context, desc)
{}
