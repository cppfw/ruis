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
	// offset from top left corner
	vector2 curScrollPos = vector2(0);

	// cached effectiveDim
	vector2 effectiveDim;

	// cached scroll factor
	vector2 curScrollFactor;

protected:
	vector2 dims_for_widget(const widget& w, const layout_params& lp)const;

public:
	scroll_area(std::shared_ptr<morda::context> c, const puu::forest& desc);

	scroll_area(const scroll_area&) = delete;
	scroll_area& operator=(const scroll_area&) = delete;

	bool on_mouse_button(const mouse_button_event& event)override;

	bool on_mouse_move(const mouse_move_event& event)override;

	void render(const morda::matrix4& matrix)const override;

	morda::vector2 measure(const morda::vector2& quotum)const override{
		return this->widget::measure(quotum);
	}

	void lay_out()override;

	void on_children_change()override;

	/**
	 * @brief Get current scroll position.
	 * @return Current scrolling position in pixels.
	 */
	const vector2& get_scroll_pos()const{
		return this->curScrollPos;
	}

	/**
	 * @brief Set scroll position.
	 * @param newScrollPos - new scroll position.
	 */
	void set_scroll_pos(const vector2& newScrollPos);

	/**
	 * @brief Set scroll position as factor.
	 * @param factor - factor with components from range [0:1].
	 */
	void set_scroll_factor(const vector2& factor);

	/**
	 * @brief Get current scroll position as factor.
	 * @return Current scroll position as factor with components from range [0:1].
	 */
	const vector2& get_scroll_factor()const{
		return this->curScrollFactor;
	}

	vector2 get_visible_area_fraction()const noexcept;

private:
	void updateEffectiveDim();

	void updateScrollFactor();

	void clampScrollPos();

	void arrangeWidgets();
};

}
