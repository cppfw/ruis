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

#include "widget.hpp"

#include "../context.hpp"
#include "../util/util.hpp"

#include "container.hpp"

using namespace ruis;

widget::widget(
	utki::shared_ref<ruis::context> context, //
	layout_parameters layout_params,
	parameters params
) :
	context(std::move(context)),
	layout_params(std::move(layout_params)),
	params(std::move(params))
{}

std::shared_ptr<widget> widget::try_get_widget(std::string_view id, bool allow_itself) noexcept
{
	if (allow_itself && this->id() == id) {
		return utki::make_shared_from(*this).to_shared_ptr();
	}
	return nullptr;
}

widget& widget::get_root_widget()
{
	if (!this->parent()) {
		return *this;
	}
	return this->parent()->get_root_widget();
}

widget* widget::try_get_ancestor(const std::string& id)
{
	if (!this->parent()) {
		return nullptr;
	}

	if (this->parent()->id() == id) {
		return this->parent();
	}

	return this->parent()->try_get_ancestor(id);
}

widget& widget::get_ancestor(const std::string& id)
{
	auto* a = this->try_get_ancestor(id);
	if (!a) {
		std::stringstream ss;
		ss << "ancestor with id = '" << id << "' not found";
		throw std::logic_error(ss.str());
	}
	return *a;
}

void widget::move_to(const vector2& new_pos)
{
	this->params.rectangle.p = new_pos;
}

void widget::resize(const ruis::vector2& new_dims)
{
	if (this->params.rectangle.d == new_dims) {
		if (this->is_layout_dirty()) {
			this->lay_out();
		}
		return;
	}

	using std::max;

	this->params.rectangle.d = max(new_dims, real(0)); // clamp bottom
	this->on_resize();
}

void widget::on_resize()
{
	this->lay_out();
}

void widget::lay_out()
{
	this->clear_cache();
	this->layout_dirty = false;
	this->on_lay_out();
}

utki::shared_ref<widget> widget::remove_from_parent()
{
	if (!this->parent()) {
		throw std::logic_error("widget::remove_from_parent(): widget is not added to the parent");
	}
	auto ret = utki::make_shared_from(*this);
	this->parent()->erase(this->parent()->find(*this));
	return ret;
}

utki::shared_ref<widget> widget::replace_by(const utki::shared_ref<widget>& w)
{
	if (!this->parent()) {
		throw std::logic_error("this widget is not added to any parent");
	}

	// TODO: performace can be improved by adding dedicated container::replace_by(iter, widget) function
	this->parent()->insert(w, this->parent()->find(*this));

	w.get().layout_params = std::move(this->layout_params);

	return this->remove_from_parent();
}

void widget::invalidate_layout() noexcept
{
	if (this->layout_dirty) {
		return;
	}
	this->layout_dirty = true;
	if (this->parent()) {
		this->parent()->invalidate_layout();
	}

	// TODO: this->clear_cache()?
	this->cache_frame_buffer.reset();
}

void widget::render_internal(const ruis::matrix4& matrix) const
{
	if (!this->rect().d.is_positive()) {
		return;
	}

	auto& r = this->context.get().renderer.get();

	if (this->params.cache) {
		if (this->cache_dirty) {
			utki::scope_exit scissor_test_enabled_scope_exit(
				[&r, scissor_test_was_enabled = r.render_context.get().is_scissor_enabled()]() {
					r.render_context.get().enable_scissor(scissor_test_was_enabled);
				}
			);
			r.render_context.get().enable_scissor(false);

			this->cache_frame_buffer = this->render_to_texture(std::move(this->cache_frame_buffer)).to_shared_ptr();

			this->cache_dirty = false;
		}

		// after rendering to texture it is most likely there will be transparent areas, so enable simple blending
		this->context.get().renderer.get().render_context.get().set_simple_alpha_blending();

		this->render_from_cache(matrix);
	} else {
		if (this->params.depth) {
			r.render_context.get().enable_depth(true);
			r.render_context.get().clear_framebuffer_depth();
		}

		if (this->params.clip) {
			r4::rectangle<uint32_t> scissor = this->compute_viewport_rect(matrix);

			r4::rectangle<uint32_t> old_scissor{};
			bool scissor_test_was_enabled = r.render_context.get().is_scissor_enabled();
			if (scissor_test_was_enabled) {
				old_scissor = r.render_context.get().get_scissor();
				scissor.intersect(old_scissor);
			} else {
				r.render_context.get().enable_scissor(true);
			}

			r.render_context.get().set_scissor(scissor);

			this->render(matrix);

			if (scissor_test_was_enabled) {
				r.render_context.get().set_scissor(old_scissor);
			} else {
				r.render_context.get().enable_scissor(false);
			}
		} else {
			this->render(matrix);
		}

		r.render_context.get().enable_depth(false);
	}

	// render border
#ifdef M_RUIS_RENDER_WIDGET_BORDERS
	ruis::ColorPosShader& s = App::inst().shaders.colorPosShader;
	s.Bind();
	ruis::matrix4 matr(matrix);
	matr.scale(this->rect().d);
	s.set_matrix(matr);

	if (this->is_hovered()) {
		s.SetColor(r4::vector3<float>(0, 1, 0));
	} else {
		s.SetColor(r4::vector3<float>(1, 0, 1));
	}
	s.render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}

utki::shared_ref<render::frame_buffer> widget::render_to_texture(std::shared_ptr<render::frame_buffer> reuse) const
{
	auto& r = this->context.get().renderer.get();

	auto fb = [&]() {
		auto dims = this->rect().d.to<uint32_t>();

		if (reuse && reuse->dims() == dims) {
			ASSERT(reuse)
			return utki::shared_ref(std::move(reuse));
		}

		return r.render_context.get().create_framebuffer(
			r.render_context.get().create_texture_2d( //
				rasterimage::format::rgba,
				dims,
				{}
			),
			this->params.depth ? r.render_context.get().create_texture_depth(dims)
							   : std::shared_ptr<ruis::render::texture_depth>(nullptr),
			nullptr
		);
	}();

	fb.get().apply([&]() {
		r.render_context.get().clear_framebuffer_color();

		utki::scope_exit depth_scope_exit([old_depth = r.render_context.get().is_depth_enabled(), &r]() {
			r.render_context.get().enable_depth(old_depth);
		});
		r.render_context.get().enable_depth(this->params.depth);

		if (this->params.depth) {
			r.render_context.get().clear_framebuffer_depth();
		}

		this->render(make_viewport_matrix(r.render_context.get().initial_matrix, this->rect().d));
	});

	return fb;
}

void widget::render_from_cache(const r4::matrix4<float>& matrix) const
{
	ruis::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	auto& r = this->context.get().renderer.get();
	ASSERT(this->cache_frame_buffer)
	ASSERT(this->cache_frame_buffer->color)
	r.shaders().pos_tex->render(
		matr, //
		r.obj().pos_tex_quad_01_vao.get(),
		*this->cache_frame_buffer->color
	);
}

void widget::clear_cache()
{
	this->cache_dirty = true;
	if (this->parent()) {
		this->parent()->clear_cache();
	}
}

void widget::on_key_internal(const ruis::key_event& e)
{
	if (this->is_interactive()) {
		if (this->on_key(e)) {
			return;
		}
	}

	if (this->parent()) {
		this->parent()->on_key_internal(e);
	}
}

void widget::focus() noexcept
{
	//	ASSERT(App::inst().thisIsUIThread())

	if (this->is_focused()) {
		return;
	}

	this->context.get().set_focused_widget(utki::make_shared_from(*this).to_shared_ptr());
}

void widget::unfocus() noexcept
{
	//	ASSERT(App::inst().thisIsUIThread())

	if (!this->is_focused()) {
		return;
	}

	ASSERT(this->context.get().focused_widget.lock() && this->context.get().focused_widget.lock().get() == this)

	this->context.get().set_focused_widget(nullptr);
}

r4::rectangle<uint32_t> widget::compute_viewport_rect(const matrix4& matrix) const noexcept
{
	using std::round;

	r4::rectangle<uint32_t> ret{
		{this->context.get().renderer.get().render_context.get().to_window_coords(matrix * vector2(0, 0)),
		 this->context.get().renderer.get().render_context.get().to_window_coords(matrix * this->rect().d)}
	};
	return ret;
}

ruis::vector2 widget::get_absolute_pos() const noexcept
{
	if (!this->parent()) {
		return this->rect().p;
	}
	return this->parent()->get_absolute_pos() + this->rect().p;
}

ruis::rect widget::get_absolute_rect() const noexcept
{
	if (this->parent()) {
		return {this->get_absolute_pos(), this->rect().d};
	}
	return this->rect();
}

vector2 widget::measure(const ruis::vector2& quotum) const
{
	return max(quotum, 0);
}

vector2 widget::pos_in_ancestor(vector2 pos, const widget* ancestor)
{
	if (ancestor == this || !this->parent()) {
		return pos;
	}

	ASSERT(this->parent())

	return this->parent()->pos_in_ancestor(this->rect().p + pos, ancestor);
}

layout_parameters& widget::get_layout_params()
{
	if (this->parent()) {
		this->parent()->invalidate_layout();
	}

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
	return const_cast<layout_parameters&>(this->get_layout_params_const());
}

widget& widget::get_widget(std::string_view id, bool allow_itself)
{
	auto w = this->try_get_widget(id, allow_itself);
	if (!w) {
		std::stringstream ss;
		ss << "widget '" << id << "' not found in '" << this->id() << "'";

		// gui scripts are supposed to be static (not dynamically generated),
		// so if there is no expected widget the gui script, then it is a logic error
		throw std::logic_error(ss.str());
	}
	return *w;
}

void widget::set_enabled(bool enable)
{
	//	TRACE(<< "widget::set_enabled(): enable = " << enable << " this->name() = " << this->name()<< std::endl)
	if (this->params.enabled == enable) {
		return;
	}

	this->params.enabled = enable;

	// un-hover this widget if it becomes disabled because it is not supposed to receive mouse input
	if (!this->is_enabled()) {
		this->set_unhovered();
	}

	this->on_enabled_change();
}

void widget::set_visible(bool visible)
{
	this->params.visible = visible;
	if (!this->params.visible) {
		this->set_unhovered();
	}
}

void widget::set_unhovered()
{
	auto hover_set = std::move(this->hovered);
	ASSERT(this->hovered.size() == 0)
	for (auto h : hover_set) {
		this->on_hovered_change(h);
	}
}

void widget::set_hovered(bool is_hovered, unsigned pointer_id)
{
	if (is_hovered == this->is_hovered(pointer_id)) {
		return;
	}
	//	TRACE(<< "widget::setHovered(): isHovered = " << isHovered << " this->name() = " << this->name() << std::endl)

	if (is_hovered) {
		ASSERT(!this->is_hovered(pointer_id))
		this->hovered.insert(pointer_id);
	} else {
		ASSERT(this->is_hovered(pointer_id))
		this->hovered.erase(pointer_id);
	}

	this->on_hovered_change(pointer_id);
}

void widget::reload()
{
	this->invalidate_layout();
	this->on_reload();
}

vector2 ruis::dims_for_widget(const widget& w, const vector2& parent_dims)
{
	const layout_parameters& lp = w.get_layout_params_const();
	vector2 d;
	for (unsigned i = 0; i != 2; ++i) {
		const auto& dim = lp.dims[i].get();
		switch (dim.get_type()) {
			case ruis::dim::type::max:
				[[fallthrough]];
			case ruis::dim::type::fill:
				d[i] = parent_dims[i];
				break;
			case ruis::dim::type::undefined:
				[[fallthrough]];
			case ruis::dim::type::min:
				d[i] = -1; // will be updated below
				break;
			case ruis::dim::type::length:
				d[i] = dim.get_length().get(w.context);
				break;
		}
	}
	if (!d.is_positive_or_zero()) {
		vector2 md = w.measure(d);
		for (unsigned i = 0; i != md.size(); ++i) {
			if (d[i] < 0) {
				d[i] = md[i];
			}
		}
	}
	return d;
}
