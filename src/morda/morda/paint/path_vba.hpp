#pragma once

#include "../render/renderer.hpp"

#include "path.hpp"

namespace morda{

class path_vba{
	std::shared_ptr<morda::vertex_array> core;
	std::shared_ptr<morda::vertex_array> border;
	
	std::shared_ptr<morda::renderer> renderer;
public:
	path_vba(){}

	path_vba(std::shared_ptr<morda::renderer> r, const path::vertices& path);
	
	path_vba(const path_vba&) = delete;
	path_vba& operator=(const path_vba&) = delete;

	path_vba(path_vba&&) = default;
	path_vba& operator=(path_vba&&) = default;
	
	void render(const morda::matrix4& matrix, uint32_t color)const;
};

}
