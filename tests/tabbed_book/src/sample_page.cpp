#include "sample_page.hpp"

#include <morda/widgets/label/text.hpp>
#include <morda/widgets/button/push_button.hpp>

sample_page::sample_page(std::shared_ptr<morda::context> c, const std::string& text) :
        morda::widget(std::move(c), treeml::forest()),
        morda::page(this->context, treeml::forest()),
        morda::row(this->context, treeml::read(R"qwer(
                @text{
                        id{text}
                }
                @push_button{
                        id{button}
                        @text{
                                text{"tear out"}
                        }
                }
        )qwer"))
{
        this->get_widget_as<morda::text>("text").set_text(text);

        this->get_widget_as<morda::push_button>("button").click_handler = [this](morda::push_button&){
                this->context->run_from_ui_thread([pg = utki::make_shared_from(*this)]{
                        pg->tear_out();
                });
        };
}
