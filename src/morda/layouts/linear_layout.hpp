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

class row_layout : public linear_layout{
public:
    row_layout() :
        linear_layout(false)
    {}

    static const utki::shared_ref<row_layout> instance;
};

class column_layout : public linear_layout{
public:
    column_layout() :
        linear_layout(true)
    {}

    static const utki::shared_ref<column_layout> instance;
};

}
