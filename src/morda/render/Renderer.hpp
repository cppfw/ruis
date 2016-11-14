#pragma once

#include "Factory.hpp"

#include "ShaderPosTex.hpp"

namespace morda{

class Renderer : virtual public utki::Shared{
public:
	const std::unique_ptr<Factory> factory;
	
	const std::unique_ptr<ShaderPosTex> shaderPosTex;

private:
	const std::shared_ptr<VertexBuffer> quad01VBO;
	const std::shared_ptr<IndexBuffer> quadIndices;
public:
	const std::shared_ptr<VertexArray> posTexQuad01VAO;
	
	Renderer(std::unique_ptr<Factory> factory);
	
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
};

}
