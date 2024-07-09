#include "pile_page.hpp"

#include <ruis/layout/pile_layout.hpp>

pile_page::pile_page(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(std::move(c), tml::forest()),
		page(this->context, tml::forest()),
		container(this->context, desc, ruis::layout::pile)
{}
