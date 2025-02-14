#pragma once

#include <ruis/widget/group/window.hpp>

utki::shared_ref<ruis::window> make_spinning_cube_window(
    utki::shared_ref<ruis::context> c, //
    ruis::vec2_length pos
);
