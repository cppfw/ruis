#include "pile_page.hpp"

#include <ruis/layouts/pile_layout.hpp>

pile_page::pile_page(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
		widget(std::move(c), treeml::forest()),
		page(this->context, treeml::forest()),
		container(this->context, desc, ruis::pile_layout::instance)
{}
