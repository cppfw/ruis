#pragma once

#include "widget.hpp"

namespace morda{

class layout{
public:

    virtual vector2 measure(const vector2& quotum, const_widget_list& widgets)const = 0;
    virtual void lay_out(const vector2& size, semiconst_widget_list& widgets)const = 0;

    virtual ~layout() = default;
};

}
