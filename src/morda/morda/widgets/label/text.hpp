#pragma once

#include "../widget.hpp"
#include "../../res/font.hpp"
#include "../base/text_widget.hpp"

namespace morda{

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "text".
 */
class text : public single_line_text_widget{
public:
	text(std::shared_ptr<morda::context> c, const puu::forest& desc);

public:
	~text()noexcept{}

	void render(const morda::matrix4& matrix)const override;
};

}
