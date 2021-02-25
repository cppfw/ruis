#pragma once

#include "../container.hpp"

namespace morda{

/**
 * @brief Pile container widget.
 * Pile container is a container which lays out its children in a pile, each widget is centered inside of the container.
 * From GUI script it can be instantiated as 'pile'.
 */
class pile : public container{
public:
	pile(const pile&) = delete;
	pile& operator=(const pile&) = delete;

	/**
	 * @brief Constructor.
	 * @param c - context.
	 * @param desc - description of the widget.
	 */
	pile(std::shared_ptr<morda::context> c, const treeml::forest& desc);

public:
	morda::vector2 measure(const morda::vector2& quotum)const override;

	void lay_out() override;
};

}
