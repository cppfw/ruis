#pragma once

#include "../layout.hpp"

namespace morda{

class linear_layout : public layout{
    bool is_vertical;
public:
    linear_layout(bool is_vertical);

    void lay_out(const vector2& size, semiconst_widget_list& widgets)const override;

    vector2 measure(const vector2& quotum, const_widget_list& widgets)const override;
};

}
