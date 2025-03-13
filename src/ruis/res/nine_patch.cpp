/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "nine_patch.hpp"

#include <iomanip>

#include "../context.hpp"
#include "../render/vertex_array.hpp"
#include "../util/util.hpp"

using namespace ruis;
using namespace ruis::res;

namespace {

class res_subimage :
	public res::image, //
	public res::image::texture
{
	friend class res::image;

	const utki::shared_ref<const res::image::texture> tex;

	const utki::shared_ref<const render::vertex_array> vao;

public:
	// rect is a rectangle on the texture, Y axis down.
	res_subimage( //
		utki::shared_ref<ruis::render::renderer> renderer,
		decltype(tex) tex,
		const ruis::rect& rect // TODO: uint32_t rect?
	) :
		res::image::texture(
			std::move(renderer),
			[&]() {
				ASSERT(rect.d.is_positive_or_zero(), [&](auto& o) {
					o << "rect.d = " << rect.d;
				})
				return rect.d.to<uint32_t>();
			}()
		),
		tex(std::move(tex)),
		vao([&]() {
			std::array<vector2, 4> tex_coords = {
				rect.p.comp_div(this->tex.get().dims().to<real>()),
				rect.x1_y2().comp_div(this->tex.get().dims().to<real>()),
				rect.x2_y2().comp_div(this->tex.get().dims().to<real>()),
				rect.x2_y1().comp_div(this->tex.get().dims().to<real>()),
			};

			// TRACE(<< "this->tex_coords = (" << tex_coords[0] << ", " << tex_coords[1] << ", " << tex_coords[2] << ",
			// " << tex_coords[3] << ")" << std::endl)

			const auto& r = this->renderer.get();

			// clang-format off
			return r.render_context.get().create_vertex_array(
				{
					r.obj().quad_01_vbo,
					r.render_context.get().create_vertex_buffer(tex_coords)
				},
				r.obj().quad_indices,
				render::vertex_array::mode::triangle_fan
			);
			// clang-format on
		}())
	{}

	res_subimage(const res_subimage&) = delete;
	res_subimage& operator=(const res_subimage&) = delete;

	res_subimage(res_subimage&&) = delete;
	res_subimage& operator=(res_subimage&&) = delete;

	~res_subimage() override = default;

	r4::vector2<uint32_t> dims(const ruis::units& units) const noexcept override
	{
		return this->res::image::texture::dims();
	}

	utki::shared_ref<const res::image::texture> get(
		const ruis::units& units, //
		vector2 for_dims
	) const override
	{
		return utki::make_shared_from(*this);
	}

	void render(
		const matrix4& matrix, //
		const render::vertex_array& vao
	) const override
	{
		this->tex.get().render(matrix, this->vao.get());
	}
};

} // namespace

utki::shared_ref<nine_patch> nine_patch::load(
	ruis::resource_loader& loader,
	const tml::forest& desc,
	const papki::file& fi
)
{
	sides<real> fraction_borders(-1);
	for (auto& p : desc) {
		if (p.value == "borders") {
			fraction_borders = parse_sides(p.children);
		} else if (p.value == "file") {
			fi.set_path(get_property_value(p).string);
		}
	}
	if (fraction_borders.left() < 0) {
		throw std::invalid_argument("nine_patch::load(): could not read borders");
	}

	for (const auto& b : fraction_borders) {
		if (b < 0 || 1 < b) {
			throw std::invalid_argument("nine_patch::load(): one of the borders is out of [0:1] range");
		}
	}

	auto image = res::image::load(
		loader, //
		fi
	);

	// TODO: is this check needed?
	if (fraction_borders.left() + fraction_borders.right() > ruis::real(1) ||
		fraction_borders.top() + fraction_borders.bottom() > ruis::real(1))
	{
		throw std::invalid_argument("nine_patch::load(): borders are bigger than image dimensions");
	}

	return utki::make_shared<nine_patch>( //
		loader.renderer,
		std::move(image),
		fraction_borders
	);
}

nine_patch::image_matrix::image_matrix(
	std::array<std::array<utki::shared_ref<const res::image>, 3>, 3> l,
	std::weak_ptr<const nine_patch> parent,
	real mul
) :
	img_matrix(std::move(l)),
	parent(std::move(parent)),
	mul(mul)
{}

// NOLINTNEXTLINE(bugprone-exception-escape, "false positive")
nine_patch::image_matrix::~image_matrix()
{
	if (auto p = this->parent.lock()) {
		p->cache.erase(this->mul);
	}
}

nine_patch::nine_patch(
	utki::shared_ref<ruis::render::renderer> renderer,
	utki::shared_ref<const res::image> image,
	sides<real> fraction_borders
) :
	renderer(std::move(renderer)),
	image(std::move(image)),
	fraction_borders(fraction_borders)
{}

sides<real> nine_patch::get_borders(const ruis::units& units) const noexcept
{
	auto dims = this->image.get().dims(units).to<real>();

	using std::round;

	return {
		round(this->fraction_borders.left() * dims.x()),
		round(this->fraction_borders.top() * dims.y()),
		round(this->fraction_borders.right() * dims.x()),
		round(this->fraction_borders.bottom() * dims.y())
	};
}

std::shared_ptr<nine_patch::image_matrix> nine_patch::get(
	const ruis::units& units, //
	sides<length> requested_borders
) const
{
	// TODO: refactor calculation of the scaled_borders?

	auto actual_borders = this->get_borders(units);

	real mul = 1;
	for (auto [orig, req] : utki::views::zip(actual_borders, requested_borders)) {
		if (orig <= 0 || req.is_undefined()) {
			continue;
		}

		auto req_px = req.get(units);

		if (req_px > orig * mul) {
			ASSERT(orig > 0)
			mul = req_px / orig;
		}
	}

	{
		// TODO: ignore svg dpi, store cache by actual borders
		auto i = this->cache.find(mul);
		if (i != this->cache.end()) {
			if (auto r = i->second.lock()) {
				return r;
			}
		}
	}

	using std::round;
	auto quad_tex = this->image.get().get(
		units, //
		round(this->image.get().dims(units).to<real>() * mul)
	);

	// std::cout << "quad_tex dims = " << quad_tex.get().dims << std::endl;

	vector2 act_mul = quad_tex.get().dims().to<real>().comp_div(this->image.get().dims(units).to<real>());

	sides<real> scaled_borders(actual_borders);
	scaled_borders.left() *= act_mul.x();
	scaled_borders.right() *= act_mul.x();
	scaled_borders.top() *= act_mul.y();
	scaled_borders.bottom() *= act_mul.y();

	// std::cout << "this->borders = " << this->borders << std::endl;
	// std::cout << "scaled_borders = " << scaled_borders << std::endl;

	auto ret = std::make_shared<image_matrix>( // TODO: make shared_ref
		std::array<std::array<utki::shared_ref<const res::image>, 3>, 3>{
			{{{
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 0,
						 0,
						 scaled_borders.left(),
						 scaled_borders.top()
					 )
				 ), // left top
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 scaled_borders.left(),
						 0,
						 round(real(quad_tex.get().dims().x()) - scaled_borders.left() - scaled_borders.right()),
						 scaled_borders.top()
					 )
				 ), // top
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 round(real(quad_tex.get().dims().x()) - scaled_borders.right()),
						 0,
						 scaled_borders.right(),
						 scaled_borders.top()
					 )
				 ) // right top
			 }},
			 {{
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 0,
						 scaled_borders.top(),
						 scaled_borders.left(),
						 round(real(quad_tex.get().dims().y()) - scaled_borders.top() - scaled_borders.bottom())
					 )
				 ), // left
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 scaled_borders.left(),
						 scaled_borders.top(),
						 round(real(quad_tex.get().dims().x()) - scaled_borders.left() - scaled_borders.right()),
						 round(real(quad_tex.get().dims().y()) - scaled_borders.top() - scaled_borders.bottom())
					 )
				 ), // middle
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 round(real(quad_tex.get().dims().x()) - scaled_borders.right()),
						 scaled_borders.top(),
						 scaled_borders.right(),
						 round(real(quad_tex.get().dims().y()) - scaled_borders.top() - scaled_borders.bottom())
					 )
				 ) // right
			 }},
			 {{
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 0,
						 round(real(quad_tex.get().dims().y()) - scaled_borders.bottom()),
						 scaled_borders.left(),
						 scaled_borders.bottom()
					 )
				 ), // left bottom
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 scaled_borders.left(),
						 round(real(quad_tex.get().dims().y()) - scaled_borders.bottom()),
						 round(real(quad_tex.get().dims().x()) - scaled_borders.left() - scaled_borders.right()),
						 scaled_borders.bottom()
					 )
				 ), // bottom
				 utki::make_shared<res_subimage>(
					 this->renderer,
					 quad_tex,
					 ruis::rect(
						 round(real(quad_tex.get().dims().x()) - scaled_borders.right()),
						 round(real(quad_tex.get().dims().y()) - scaled_borders.bottom()),
						 scaled_borders.right(),
						 scaled_borders.bottom()
					 )
				 ) // right bottom
			 }}}
    },
		utki::make_shared_from(*this).to_shared_ptr(),
		mul
	);

	this->cache[mul] = ret;
	return ret;
}
