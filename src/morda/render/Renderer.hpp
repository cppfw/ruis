#pragma once

#include "Factory.hpp"

#include "ShaderPosTex.hpp"

namespace morda{

class Renderer : virtual public utki::Shared{
public:
	const std::unique_ptr<Factory> factory;
	
	const std::unique_ptr<ShaderPosTex> shaderPosTex;

	Renderer(std::unique_ptr<Factory> factory);
	
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
};

}
