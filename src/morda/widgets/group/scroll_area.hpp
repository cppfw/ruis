#pragma once

#include "../container.hpp"

#include <functional>

namespace morda{

/**
 * @brief Scroll area container widget.
 * Scroll area is a container which can add an offset to its children widget positions.
 * From GUI scripts it can be instantiated as "scroll_area".
 * Note, that Scrollarea has same layout parameters as simple container and those work similarly,
 * except 'max' value. If layout dimension is specified as 'max' then child widget will be stretched to the
 * parent (scroll_area) size in case child's minimal size is less than scroll_area size, otherwise child will be assigned
 * its minimal size.
 */
class scroll_area : public container{
	//offset from top left corner
	Vec2r curScrollPos = Vec2r(0);

	//cached effectiveDim
	Vec2r effectiveDim;

	//cached scroll factor
	Vec2r curScrollFactor;

protected:
	Vec2r dims_for_widget(const widget& w, const layout_params& lp)const;

public:
	scroll_area(std::shared_ptr<morda::context> c, const puu::forest& desc);

	scroll_area(const scroll_area&) = delete;
	scroll_area& operator=(const scroll_area&) = delete;


	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID)override;

	bool on_mouse_move(const morda::Vec2r& pos, unsigned pointerID)override;

	void render(const morda::matrix4& matrix) const override;

	morda::Vec2r measure(const morda::Vec2r& quotum) const override{
		return this->widget::measure(quotum);
	}

	void lay_out()override;

	void on_children_changed()override;

	/**
	 * @brief Get current scroll position.
	 * @return Current scrolling position in pixels.
	 */
	const Vec2r& get_scroll_pos()const{
		return this->curScrollPos;
	}

	/**
	 * @brief Set scroll position.
	 * @param newScrollPos - new scroll position.
	 */
	void set_scroll_pos(const Vec2r& newScrollPos);

	/**
	 * @brief Set scroll position as factor.
	 * @param factor - factor with components from range [0:1].
	 */
	void set_scroll_factor(const Vec2r& factor);

	/**
	 * @brief Get current scroll position as factor.
	 * @return Current scroll position as factor with components from range [0:1].
	 */
	const Vec2r& get_scroll_factor()const{
		return this->curScrollFactor;
	}

	Vec2r get_visible_area_fraction()const noexcept;

private:
	void updateEffectiveDim();

	void updateScrollFactor();

	void clampScrollPos();

	void arrangeWidgets();
};

}
