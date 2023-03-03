#pragma once

#include "../layout.hpp"

namespace morda{

class size_layout : public layout{
public:
    void lay_out(const vector2& size, semiconst_widget_list& widgets)const override;

    static const utki::shared_ref<size_layout> instance;
};

}
