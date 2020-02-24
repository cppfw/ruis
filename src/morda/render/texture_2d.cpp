#include "texture_2d.hpp"

using namespace morda;

unsigned texture_2d::bytes_per_pixel(texture_2d::type t){
	switch(t){
		case texture_2d::type::grey:
			return 1;
		case texture_2d::type::grey_alpha:
			return 2;
		case texture_2d::type::rgb:
			return 3;
		case texture_2d::type::rgba:
			return 4;
		default:
			return 0;
	}
}
