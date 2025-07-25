#pragma once

#include <ruis/context.hpp>

utki::shared_ref<ruis::widget> make_rectangles_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
);
