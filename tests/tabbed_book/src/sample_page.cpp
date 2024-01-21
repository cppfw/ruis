#include "sample_page.hpp"

#include <ruis/widgets/label/text.hpp>
#include <ruis/widgets/button/push_button.hpp>

sample_page::sample_page(const utki::shared_ref<ruis::context>& c, const std::string& text) :
        ruis::widget(c, treeml::forest()),
        ruis::page(this->context, treeml::forest()),
        ruis::container(this->context, treeml::read(R"qwer(
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
        this->get_widget_as<ruis::text>("text").set_text(text);

        this->get_widget_as<ruis::push_button>("button").click_handler = [this](ruis::push_button&){
                this->context.get().run_from_ui_thread([pg = utki::make_shared_from(*this)]{
                        pg.get().tear_out();
                });
        };
}
