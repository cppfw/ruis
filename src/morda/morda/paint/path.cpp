#include <cmath>

#include "path.hpp"

using namespace morda;

void path::lineTo(morda::vector2 absPos){
	this->points.emplace_back(absPos);
}

void path::lineBy(morda::vector2 relPos){
	ASSERT(this->points.size() != 0)
	this->lineTo(this->points.back() + relPos);
}

void path::cubicBy(morda::vector2 relP1, morda::vector2 relP2, morda::vector2 relP3){
	ASSERT(this->points.size() != 0)
	auto& d = this->points.back();
	this->cubicTo(d + relP1, d + relP2, d + relP3);
}

void path::cubicTo(morda::vector2 p1, morda::vector2 p2, morda::vector2 p3){
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
		this->lineTo(bezier(t));
	}
	this->lineTo(bezier(1));
}

path::vertices path::stroke(morda::real halfWidth, morda::real antialiasWidth, morda::real antialiasAlpha)const{
	vertices ret;

	if(this->points.size() <= 1){
		return ret;
	}

	const decltype(this->points)::value_type *prev = nullptr, *cur = nullptr, *next = nullptr;

	std::uint16_t inIndex = 0;

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

		morda::vector2 prevNormal, nextNormal;

		ASSERT(prev || next)

		if(prev){
			prevNormal = (*cur - *prev).normalize();
			std::swap(prevNormal.x, prevNormal.y);
			prevNormal.x = -prevNormal.x;
		}
		if(next){
			nextNormal = (*next - *cur).normalize();
			std::swap(nextNormal.x, nextNormal.y);
			nextNormal.x = -nextNormal.x;
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
		auto miter = miterCoeff * halfWidth;
		auto antialiasMiter = miterCoeff * (halfWidth + antialiasWidth);

		using std::sqrt;
		using utki::pi;

		if(!prev){
			ASSERT(next)
			ret.pos.push_back((*cur) - normal * miter - normal.rotated(-pi<morda::real>() / 4) * antialiasWidth * sqrt(2));
		}else if(!next){
			ASSERT(prev)
			ret.pos.push_back((*cur) - normal * miter - normal.rotated(pi<morda::real>() / 4) * antialiasWidth * sqrt(2));
		}else{
			ret.pos.push_back((*cur) - normal * antialiasMiter);
		}
		ret.alpha.push_back(0);
		++inIndex;

		ret.pos.push_back((*cur) - normal * miter);
		ret.alpha.push_back(antialiasAlpha);
		ret.inIndices.push_back(inIndex);
		++inIndex;

		ret.pos.push_back((*cur) + normal * miter);
		ret.alpha.push_back(antialiasAlpha);
		ret.inIndices.push_back(inIndex);
		++inIndex;

		if(!prev){
			ret.pos.push_back((*cur) + normal * miter + normal.rotated(utki::pi<morda::real>() / 4) * antialiasWidth * std::sqrt(2));
		}else if(!next){
			ret.pos.push_back((*cur) + normal * miter + normal.rotated(-utki::pi<morda::real>() / 4) * antialiasWidth * std::sqrt(2));
		}else{
			ret.pos.push_back((*cur) + normal * antialiasMiter);
		}
		ret.alpha.push_back(0);
		++inIndex;
	}

	ret.outIndices.push_back(3);
	ret.outIndices.push_back(2);

	for(unsigned i = 0; i != this->points.size(); ++i){
		ret.outIndices.push_back(4 * i);
		ret.outIndices.push_back(4 * i + 1);
	}

	for(unsigned i = this->points.size() - 1; i != 0; --i){
		ret.outIndices.push_back(4 * i + 3);
		ret.outIndices.push_back(4 * i + 2);
	}

	ret.outIndices.push_back(3);
	ret.outIndices.push_back(2);

	return ret;
}
