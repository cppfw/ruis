#include "sample_page.hpp"

sample_page::sample_page(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
        morda::widget(std::move(c), desc),
        morda::page(this->context, desc),
        morda::text(this->context, desc)
{}
