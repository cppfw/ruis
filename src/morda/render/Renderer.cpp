#include "Renderer.hpp"

using namespace morda;


Renderer::Renderer(std::unique_ptr<Factory> factory) :
		factory(std::move(factory)),
		shaderPosTex(this->factory->createPosTexShader())
{
	
}
