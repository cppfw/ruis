#pragma once

#include "../layout.hpp"

namespace morda{

class pile_layout : public layout{
public:
    void lay_out(const vector2& size, semiconst_widget_list& widgets)const override;

    vector2 measure(const vector2& quotum, const_widget_list& widgets)const override;

    static const utki::shared_ref<pile_layout> instance;
};

}
