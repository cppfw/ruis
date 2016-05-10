/* 
 * File:   ResGradient.hpp
 * Author: ivan
 *
 * Created on April 29, 2016, 11:53 AM
 */

#pragma once

#include "../ResourceManager.hpp"

#include "../shaders/ColorPosShader.hpp"

#include "../config.hpp"

namespace morda{

class ResGradient : public Resource{
	
	std::vector<real> stops;
public:
	ResGradient(std::vector<real>&& stops) :
			stops(stops)
	{}
	
	ResGradient(const ResGradient&) = delete;
	ResGradient& operator=(const ResGradient&) = delete;
	
	
	void render(const Matr4r& matrix)const;
	
private:

};

}
