#include "frame_buffer.hpp"

#include <utki/string.hpp>

using namespace ruis::render;

frame_buffer::frame_buffer(std::shared_ptr<texture_2d> color, std::shared_ptr<texture_depth> depth) :
	rasterimage::dimensioned([&]() {
		rasterimage::dimensioned* cur_tex = nullptr;

		std::array<rasterimage::dimensioned*, 2> tex_attachments = {color.get(), depth.get()};

		for (auto& t : tex_attachments) {
			if (!t) {
				continue;
			}
			if (cur_tex && t->dims() != cur_tex->dims()) {
				throw std::logic_error(utki::cat(
					"frame_buffer(): texture attachments have different dimensions: (",
					cur_tex->dims(),
					") and (",
					t->dims(),
					")"
				));
			}
			cur_tex = t;
		}

		if (!cur_tex) {
			throw std::logic_error("frame_buffer(): no any texture attachments given");
		}

		return cur_tex->dims();
	}()),
	color(std::move(color)),
	depth(std::move(depth))
{}
