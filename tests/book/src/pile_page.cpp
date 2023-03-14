#include "pile_page.hpp"

#include "../../../src/morda/layouts/pile_layout.hpp"

pile_page::pile_page(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		widget(std::move(c), treeml::forest()),
		page(this->context, treeml::forest()),
		container(this->context, desc, morda::pile_layout::instance)
{}
