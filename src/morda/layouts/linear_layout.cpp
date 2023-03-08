#include "linear_layout.hpp"

using namespace morda;

linear_layout::linear_layout(bool is_vertical) :
    is_vertical(is_vertical)
{}

void linear_layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{
    // TODO:
}

vector2 linear_layout::measure(const vector2& quotum, const_widget_list& widgets)const{
    // TODO:
    return 0;
}
