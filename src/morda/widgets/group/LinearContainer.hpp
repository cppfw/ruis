#pragma once

#include "../container.hpp"

#include "../base/oriented_widget.hpp"

namespace morda{

/**
 * @brief Linear container widget.
 * Linear container lays out its child widgets in a row from left to right or in a column from top to bottom.
 */
class LinearContainer :
		public Container,
		protected oriented_widget
{
	LinearContainer(const LinearContainer&) = delete;
	LinearContainer& operator=(const LinearContainer&) = delete;

public:
	LinearContainer(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical);

	void lay_out() override;

	morda::Vec2r measure(const morda::Vec2r& quotum)const override;

	/**
	 * @brief Layout parameters for LinearArea container.
	 */
	class layout_params : public container::layout_params{
	public:
		/**
		 * @brief Constructor.
		 * @param desc - description of the layout parameters.
		 * @param units - screen units information.
		 */
		layout_params(const puu::forest& desc, const morda::units& units);

		/**
		 * @brief Weight of the widget.
		 * Weight defines how much space widget occupies in addition to its minimal or explicitly set size.
		 * Default value is 0, which means that the widget will not occupy extra space.
		 */
		real weight = 0;
	};

	// TODO: deprecated, remove.
	typedef layout_params LayoutParams;
private:
	std::unique_ptr<Widget::LayoutParams> create_layout_params(const puu::forest& desc)const override;
};


}
