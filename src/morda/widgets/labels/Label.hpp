/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once


#include "../Widget.hpp"
#include "../../resources/ResFont.hpp"
#include "../TextWidget.hpp"



namespace morda{


//TODO: rename to TextLabel
class Label : public SingleLineTextWidget{
	
	
public:
	Label(const stob::Node* chain = nullptr);
	
public:
	~Label()noexcept{}
	
	void render(const morda::Matr4r& matrix)const override;
};



}//~namespace
