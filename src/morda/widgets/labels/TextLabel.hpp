/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once


#include "../Widget.hpp"
#include "../../resources/ResFont.hpp"
#include "../TextWidget.hpp"



namespace morda{


class TextLabel : public SingleLineTextWidget{
	
	
public:
	TextLabel(const stob::Node* chain = nullptr);
	
public:
	~TextLabel()noexcept{}
	
	void render(const morda::Matr4r& matrix)const override;
};



}//~namespace
