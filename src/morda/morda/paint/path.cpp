#include <cmath>

#include "path.hpp"

using namespace morda;

void path::line_to(morda::vector2 abs_pos){
	this->points.emplace_back(abs_pos);
}

void path::line_by(morda::vector2 rel_pos){
	ASSERT(this->points.size() != 0)
	this->line_to(this->points.back() + rel_pos);
}

void path::cubic_by(morda::vector2 rel_p1, morda::vector2 rel_p2, morda::vector2 rel_p3){
	ASSERT(this->points.size() != 0)
	auto& d = this->points.back();
	this->cubic_to(d + rel_p1, d + rel_p2, d + rel_p3);
}

void path::cubic_to(morda::vector2 p1, morda::vector2 p2, morda::vector2 p3){
	auto p0 = this->points.back();

	auto bezier = [p0, p1, p2, p3](morda::real t){
		using utki::pow3;
		using utki::pow2;
		return pow3(1 - t) * p0 + 3 * t * pow2(1 - t) * p1 + 3 * pow2(t) * (1 - t) * p2 + pow3(t) * p3;
	};

	auto lengthEst = (p1 - p0).norm() + (p2 - p1).norm() + (p3 - p2).norm();

	const morda::real maxStep_c = 10.0f; // 10 pixels

	auto numSteps = lengthEst / maxStep_c;

	auto dt = 1 / numSteps;

	// NOTE: start from dt because 0th point is already there in the path
	for(morda::real t = dt; t < 1; t += dt){
		this->line_to(bezier(t));
	}
	this->line_to(bezier(1));
}

path::vertices path::stroke(morda::real half_width, morda::real antialias_width, morda::real antialias_alpha)const{
	vertices ret;

	if(this->points.size() <= 1){
		return ret;
	}

	const decltype(this->points)::value_type *prev = nullptr, *cur = nullptr, *next = nullptr;

	std::uint16_t in_index = 0;

	for(auto i = this->points.begin(); i != this->points.end(); ++i){
		prev = cur;
		cur = &(*i);
		{
			auto ii = i;
			++ii;
			if(ii == this->points.end()){
				next = nullptr;
			}else{
				next = &(*ii);
			}
		}

		morda::vector2 prevNormal = 0, nextNormal;

		ASSERT(prev || next)

		if(prev){
			using std::swap;
			prevNormal = (*cur - *prev).normalize();
			swap(prevNormal.x(), prevNormal.y());
			prevNormal.x() = -prevNormal.x();
		}
		if(next){
			using std::swap;
			nextNormal = (*next - *cur).normalize();
			swap(nextNormal.x(), nextNormal.y());
			nextNormal.x() = -nextNormal.x();
		}else{
			ASSERT(prev)
			nextNormal = prevNormal;
		}
		if(!prev){
			ASSERT(next)
			prevNormal = nextNormal;
		}

		auto normal = (prevNormal + nextNormal).normalize();

		auto miterCoeff = 1 / (normal * prevNormal);
		auto miter = miterCoeff * half_width;
		auto antialiasMiter = miterCoeff * (half_width + antialias_width);

		using std::sqrt;
		using utki::pi;

		if(!prev){
			ASSERT(next)
			ret.pos.push_back((*cur) - normal * miter - normal.rot(-pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2)));
		}else if(!next){
			ASSERT(prev)
			ret.pos.push_back((*cur) - normal * miter - normal.rot(pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2)));
		}else{
			ret.pos.push_back((*cur) - normal * antialiasMiter);
		}
		ret.alpha.push_back(0);
		++in_index;

		ret.pos.push_back((*cur) - normal * miter);
		ret.alpha.push_back(antialias_alpha);
		ret.in_indices.push_back(in_index);
		++in_index;

		ret.pos.push_back((*cur) + normal * miter);
		ret.alpha.push_back(antialias_alpha);
		ret.in_indices.push_back(in_index);
		++in_index;

		if(!prev){
			ret.pos.push_back((*cur) + normal * miter + normal.rot(pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2)));
		}else if(!next){
			ret.pos.push_back((*cur) + normal * miter + normal.rot(-pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2)));
		}else{
			ret.pos.push_back((*cur) + normal * antialiasMiter);
		}
		ret.alpha.push_back(0);
		++in_index;
	}

	ret.out_indices.push_back(3);
	ret.out_indices.push_back(2);

	for(unsigned i = 0; i != this->points.size(); ++i){
		ret.out_indices.push_back(4 * i);
		ret.out_indices.push_back(4 * i + 1);
	}

	for(unsigned i = unsigned(this->points.size() - 1); i != 0; --i){
		ret.out_indices.push_back(4 * i + 3);
		ret.out_indices.push_back(4 * i + 2);
	}

	ret.out_indices.push_back(3);
	ret.out_indices.push_back(2);

	return ret;
}
