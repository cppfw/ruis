#include "Renderer.hpp"

using namespace morda;


Renderer::Renderer(std::unique_ptr<Factory> factory) :
		factory(std::move(factory)),
		shaderPosTex(this->factory->createPosTexShader()),
		quad01VBO(this->factory->createVertexBuffer(utki::wrapBuf(std::array<kolme::Vec2f, 4>({{
			kolme::Vec2f(0, 0), kolme::Vec2f(1, 0), kolme::Vec2f(1, 1), kolme::Vec2f(0, 1)
		}})))),
		quadIndices(this->factory->createIndexBuffer(utki::wrapBuf(std::array<std::uint16_t, 4>({{0, 1, 2, 3}})))),
		posTexQuad01VAO(this->factory->createVertexArray({this->quad01VBO, this->quad01VBO}, this->quadIndices))
{}
