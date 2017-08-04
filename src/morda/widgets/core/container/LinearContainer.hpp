#pragma once

#include "Container.hpp"


namespace morda{



/**
 * @brief Linear container widget.
 * Linear container lays out its child widgets in a row from left to right or in a column from top to bottom.
 */
class LinearContainer : public Container{
	LinearContainer(const LinearContainer&) = delete;
	LinearContainer& operator=(const LinearContainer&) = delete;

	bool isVertical_v;
	
	unsigned GetLongIndex()const noexcept{
		return this->isVertical_v ? 1 : 0;
	}

	unsigned GetTransIndex()const noexcept{
		return this->isVertical_v ? 0 : 1;
	}
	
public:
	LinearContainer(bool isVertical, const stob::Node* chain = nullptr);

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
 * @brief Vertical container widget.
 * Column is a vertical variant of linear container. From GUI scripts it can be instantiated as "Column".
 */
class Column : public LinearContainer{
public:
	Column(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearContainer(true, chain)
	{}
	
	Column(const Column&) = delete;
	Column& operator=(const Column&) = delete;
};


/**
 * @brief Horizontal container widget.
 * Row is a horizontal variant of linear container. From GUI scripts it can be instantiated as "Row".
 */
class Row : public LinearContainer{
public:
	Row(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearContainer(false, chain)
	{}
	
	Row(const Row&) = delete;
	Row& operator=(const Row&) = delete;
};


}
