#pragma once

#include "context.hpp"

namespace ruis {

enum class theme {
	dark,
	light
};

void init_standard_widgets(
	ruis::context& context, //
	const fsif::file& fi,
	theme th = theme::dark
);

} // namespace ruis
