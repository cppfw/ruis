#include "sample_page.hpp"

#include <ruis/widget/label/text.hpp>
#include <ruis/widget/button/push_button.hpp>

sample_page::sample_page(const utki::shared_ref<ruis::context>& c, const std::string& text) :
        ruis::widget(c, tml::forest()),
        ruis::page(this->context, tml::forest()),
        // TODO: modernize
        ruis::container(this->context, tml::read(R"qwer(
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
                this->context.get().post_to_ui_thread([pg = utki::make_shared_from(*this)]{
                        pg.get().tear_out();
                });
        };
}
