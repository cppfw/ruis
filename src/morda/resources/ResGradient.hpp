/* 
 * File:   ResGradient.hpp
 * Author: ivan
 *
 * Created on April 29, 2016, 11:53 AM
 */

#pragma once

#include "../ResourceManager.hpp"

#include "../shaders/ClrPosShader.hpp"

#include "../config.hpp"

#include <kolme/Vector2.hpp>

namespace morda{

class ResGradient : public Resource{
	friend class ResourceManager;
	
	std::vector<kolme::Vec2f> vertices;
	std::vector<std::uint32_t> colors;
	
public:
	ResGradient(const std::vector<std::tuple<real, std::uint32_t>>& stops, bool vertical);
	
	ResGradient(const ResGradient&) = delete;
	ResGradient& operator=(const ResGradient&) = delete;
	
	void render(const Matr4r& matrix, ClrPosShader& s)const;
	
private:
	static std::shared_ptr<ResGradient> load(const stob::Node& chain, const papki::File& fi);
};

}
