#include "Renderer.hpp"

using namespace morda;


Renderer::Renderer(std::unique_ptr<RenderFactory> factory, unsigned maxTextureSize) :
		factory(std::move(factory)),
		shader(this->factory->createShaders()),
		quad01VBO(this->factory->createVertexBuffer(utki::wrapBuf(std::array<kolme::Vec2f, 4>({{
			kolme::Vec2f(0, 0), kolme::Vec2f(1, 0), kolme::Vec2f(1, 1), kolme::Vec2f(0, 1)
		}})))),
		quadIndices(this->factory->createIndexBuffer(utki::wrapBuf(std::array<std::uint16_t, 4>({{0, 1, 2, 3}})))),
		posQuad01VAO(this->factory->createVertexArray({this->quad01VBO}, this->quadIndices, VertexArray::Mode_e::TRIANGLE_FAN)),
		posTexQuad01VAO(this->factory->createVertexArray({this->quad01VBO, this->quad01VBO}, this->quadIndices, VertexArray::Mode_e::TRIANGLE_FAN)),
		maxTextureSize(maxTextureSize)
{
}



void Renderer::setFramebuffer(std::shared_ptr<FrameBuffer> fb) {
	this->curFB = std::move(fb);
	this->setFramebufferInternal(this->curFB.operator ->());
}

