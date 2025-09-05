#pragma once

#include <ruis/widget/widget.hpp>
#include <ruisapp/window.hpp>

utki::shared_ref<ruis::widget> make_new_native_window_root_widget(
    utki::shared_ref<ruis::context> c, //
    ruisapp::window& win
);
