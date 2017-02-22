#pragma once

#include "Container.hpp"


namespace morda{



/**
 * @brief Linear area container widget.
 * Linear area container lays out its child widgets in a row from left to right or in a column from top to bottom.
 * This is a base class for VerticalArea and HorizontalArea.
 */
class LinearArea : public Container{
	LinearArea(const LinearArea&) = delete;
	LinearArea& operator=(const LinearArea&) = delete;

	bool isVertical_v;
	
	unsigned GetLongIndex()const noexcept{
		return this->isVertical_v ? 1 : 0;
	}

	unsigned GetTransIndex()const noexcept{
		return this->isVertical_v ? 0 : 1;
	}
	
public:
	LinearArea(bool isVertical, const stob::Node* chain = nullptr);

	void layOut() override;	
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
	bool isVertical()const noexcept{
		return this->isVertical_v;
	}
	
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


/**
 * @brief Vertical area container widget.
 * Vertical variant of LinearArea container. From GUI scripts it can be instantiated as "VerticalArea".
 */
class VerticalArea : public LinearArea{
public:
	VerticalArea(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearArea(true, chain)
	{}
	
	VerticalArea(const VerticalArea&) = delete;
	VerticalArea& operator=(const VerticalArea&) = delete;
};


/**
 * @brief Horizontal area container widget.
 * Horizontal variant of HorizontalArea container. From GUI scripts it can be instantiated as "HorizontalArea".
 */
class HorizontalArea : public LinearArea{
public:
	HorizontalArea(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearArea(false, chain)
	{}
	
	HorizontalArea(const HorizontalArea&) = delete;
	HorizontalArea& operator=(const HorizontalArea&) = delete;
};


}
