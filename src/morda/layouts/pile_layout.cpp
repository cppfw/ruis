#include "pile_layout.hpp"

using namespace morda;

const utki::shared_ref<pile_layout> pile_layout::instance = utki::make_shared<pile_layout>();

void pile_layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    // TODO:
}

vector2 pile_layout::measure(const vector2& quotum, const_widget_list& widgets)const{
    // TODO:
    return 0;
}
