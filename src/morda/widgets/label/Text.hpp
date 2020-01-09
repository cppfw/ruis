#pragma once


#include "../widget.hpp"
#include "../../res/ResFont.hpp"
#include "../base/TextWidget.hpp"



namespace morda{

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "Text".
 */
class Text : public SingleLineTextWidget{


public:
	Text(const puu::forest& desc);
	Text(const stob::Node* chain) : Text(stob_to_puu(chain)){}

public:
	~Text()noexcept{}

	void render(const morda::Matr4r& matrix)const override;
};



}
