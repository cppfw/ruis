#pragma once


#include "../widget.hpp"
#include "../../res/res_font.hpp"
#include "../base/TextWidget.hpp"



namespace morda{

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "Text".
 */
class Text : public SingleLineTextWidget{


public:
	Text(std::shared_ptr<morda::context> c, const puu::forest& desc);

public:
	~Text()noexcept{}

	void render(const morda::Matr4r& matrix)const override;
};



}
