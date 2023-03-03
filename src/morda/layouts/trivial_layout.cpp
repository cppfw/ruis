#include "trivial_layout.hpp"

using namespace morda;

vector2 trivial_layout::measure(const vector2& quotum, const_widget_list& widgets)const{
    return max(quotum, 0);
}
void trivial_layout::lay_out(const vector2& size, semiconst_widget_list& widgets)const{}
