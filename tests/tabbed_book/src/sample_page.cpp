#include "sample_page.hpp"

sample_page::sample_page(std::shared_ptr<morda::context> c, const std::string& text) :
        morda::widget(std::move(c), treeml::forest()),
        morda::page(this->context, treeml::forest()),
        morda::text(this->context, treeml::forest())
{
        this->set_text(text);
}
