#pragma once


#include "Container.hpp"


namespace morda{


/**
 * @brief Frame container widget.
 * Frame container is a container which lays out its children in a pile, each widget is centered inside of the container.
 * From GUI script it can be instantiated as 'Frame'.
 */
class Frame : public Container{
public:
	Frame(const Frame&) = delete;
	Frame& operator=(const Frame&) = delete;
	
	/**
	 * @brief Constructor.
	 * @param chain - STOB chain describing the widget.
	 */
	Frame(const stob::Node* chain = nullptr);
	
public:	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;

	void layOut() override;
};


}
