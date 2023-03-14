#include "sample_page.hpp"

#include <morda/widgets/label/text.hpp>
#include <morda/widgets/button/push_button.hpp>

sample_page::sample_page(const utki::shared_ref<morda::context>& c, const std::string& text) :
        morda::widget(c, treeml::forest()),
        morda::page(this->context, treeml::forest()),
        morda::container(this->context, treeml::read(R"qwer(
                layout{row}
                
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
                this->context.get().run_from_ui_thread([pg = utki::make_shared_from(*this)]{
                        pg.get().tear_out();
                });
        };
}
