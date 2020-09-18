#pragma once

#include "../render/renderer.hpp"

#include "path.hpp"

namespace morda{

class path_vao{
	std::shared_ptr<morda::vertex_array> core;
	std::shared_ptr<morda::vertex_array> border;
	
	std::shared_ptr<morda::renderer> renderer;
public:
	path_vao(){}

	path_vao(std::shared_ptr<morda::renderer> r, const path::vertices& path);
	
	path_vao(const path_vao&) = delete;
	path_vao& operator=(const path_vao&) = delete;

	path_vao(path_vao&&) = default;
	path_vao& operator=(path_vao&&) = default;
	
	void render(const morda::matrix4& matrix, uint32_t color)const;
};

}
