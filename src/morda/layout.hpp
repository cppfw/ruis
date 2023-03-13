#pragma once

#include "widget.hpp"

namespace morda{

// TODO: doxygen
class layout{
protected:
    layout() = default;
public:

    virtual vector2 measure(const vector2& quotum, const_widget_list& widgets)const;

    /**
     * @brief Arrange widgets.
     * @param size - size of the area available to the layout.
     * @param widgets - widgets to arrange.
     */
    virtual void lay_out(const vector2& size, semiconst_widget_list& widgets)const;

    virtual ~layout() = default;
};

}
