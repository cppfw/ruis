#pragma once

#include "../Container.hpp"

#include "../base/OrientedWidget.hpp"


namespace morda{



/**
 * @brief Linear container widget.
 * Linear container lays out its child widgets in a row from left to right or in a column from top to bottom.
 */
class LinearContainer :
		public Container,
		protected OrientedWidget
{
	LinearContainer(const LinearContainer&) = delete;
	LinearContainer& operator=(const LinearContainer&) = delete;
	
public:
	LinearContainer(const stob::Node* chain, bool vertical);

	void layOut() override;	
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
	/**
	 * @brief Layout parameters for LinearArea container.
	 */
	class LayoutParams : public Container::LayoutParams{
	public:
		/**
		 * @brief Constructor.
		 * @param chain - STOB chain describing the layout parameters. Can be nullptr, then all layout params are set to default values.
		 */
		LayoutParams(const stob::Node* chain = nullptr);
		
		/**
		 * @brief Weight of the widget.
		 * Weight defines how much space widget occupies in addition to its minimal or explicitly set size.
		 * Default value is 0, which means that the widget will not occupy extra space.
		 */
		real weight;
	};
private:
	std::unique_ptr<Widget::LayoutParams> createLayoutParams(const stob::Node* chain)const override{
		return utki::makeUnique<LayoutParams>(chain);
	}
};


}
