/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../ColorWidget.hpp"

#include "../../resources/ResGradient.hpp"

namespace morda{



class ColorLabel : public ColorWidget{
	
	std::shared_ptr<ResGradient> gradient;
public:
	ColorLabel(const stob::Node* chain = nullptr);
	
	ColorLabel(const ColorLabel&) = delete;
	ColorLabel& operator=(const ColorLabel&) = delete;
	
	void render(const morda::Matr4r& matrix)const override;
};



}
