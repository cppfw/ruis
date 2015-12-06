/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../ColorWidget.hpp"


namespace morda{



class ColorLabel : public ColorWidget{
public:
	ColorLabel(const stob::Node* chain = nullptr);
	
	ColorLabel(const ColorLabel&) = delete;
	ColorLabel& operator=(const ColorLabel&) = delete;
	
	void render(const morda::Matr4r& matrix)const override;
};



}
