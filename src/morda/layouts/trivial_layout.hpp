#pragma once

#include "../layout.hpp"

namespace morda{

class trivial_layout : public layout{
public:

    vector2 measure(const vector2& quotum, const_widget_list& widgets)const override;
    void lay_out(const vector2& size, semiconst_widget_list& widgets)const override;

};

}
