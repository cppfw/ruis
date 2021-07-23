#pragma once

#include <vector>
#include "../config.hpp"

namespace morda{

class path{
	std::vector<morda::vector2> points = {{ morda::vector2(0) }};
	
public:
	path() = default;
	
	path(const path&) = delete;
	path& operator=(const path&) = delete;
	
	void line_to(morda::vector2 abs_pos);
	void line_to(morda::real x, morda::real y){
		this->line_to(morda::vector2(x, y));
	}
	void line_by(morda::vector2 rel_pos);
	
	void cubic_to(morda::vector2 abs_p1, morda::vector2 abs_p2, morda::vector2 abs_p3);
	void cubic_by(morda::vector2 rel_p1, morda::vector2 rel_p2, morda::vector2 rel_p3);
		
	struct vertices{
		std::vector<morda::vector2> pos;
		std::vector<morda::real> alpha;
		
		std::vector<uint16_t> in_indices;
		std::vector<uint16_t> out_indices;
	};
	
	vertices stroke(
			morda::real half_width = morda::real(0.5f),
			morda::real antialias_width = morda::real(1.0f),
			morda::real antialias_alpha = morda::real(0.35f)
		)const;
};

}
