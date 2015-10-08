/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/config.hpp>
#include <utki/Singleton.hpp>
#include <utki/Buf.hpp>

#include "../render/Render.hpp"

#include <kolme/Matrix4.hpp>
#include <kolme/Vector3.hpp>
#include <kolme/Vector2.hpp>

#include "../Exc.hpp"



namespace morda{



/**
 * @brief Abstract shader.
 * Each shader has at least matrix uniform.
 * Matrix uniform should be 'mat4' and named 'matrix'.
 */
class Shader{
	friend class Render;
	
	static Shader* boundShader;
	static bool renderIsInProgress;
	
	std::unique_ptr<utki::Void> program;

	const Render::InputID matrixUniform;
	
	void Bind(){
		if(this == boundShader){
			return;
		}
		
		if(renderIsInProgress){
			throw morda::Exc("Shader: cannot bind shader because another shader is bound and one of its render() methods was not called");
		}
		
		renderIsInProgress = true;
		
		Render::bindShader(*this->program);
		boundShader = this;
	}
	
	
protected:
	Render::InputID getAttribute(const char* n){
		return Render::getAttribute(*this->program, n);
	}
	
	Render::InputID getUniform(const char* n){
		return Render::getUniform(*this->program, n);
	}

	Shader(const char* vertexShaderCode = nullptr, const char* fragmentShaderCode = nullptr);

	void renderArrays(Render::EMode mode, size_t numElements){
		this->Bind();
		Render::renderArrays(mode, numElements);
		this->renderIsInProgress = false;
	}
	
	void renderElements(Render::EMode mode, const utki::Buf<std::uint16_t> i){
		this->Bind();
		Render::renderElements(mode, i);
		this->renderIsInProgress = false;
	}
	
	void setUniformMatrix4f(Render::InputID id, const kolme::Matr4f& m){
		this->Bind();
		Render::setUniformMatrix4f(id, m);
	}
	
	void setUniform1i(Render::InputID id, int i){
		this->Bind();
		Render::setUniform1i(id, i);
	}
	
	void setUniform2f(Render::InputID id, kolme::Vec2f v){
		this->Bind();
		Render::setUniform2f(id, v);
	}
	
	void setUniform4f(Render::InputID id, float x, float y, float z, float a){
		this->Bind();
		Render::setUniform4f(id, x, y, z, a);
	}
	
	void setUniform4f(Render::InputID id, const utki::Buf<kolme::Vec4f> v){
		this->Bind();
		Render::setUniform4f(id, v);
	}
	
	void setVertexAttribArray(Render::InputID id, const utki::Buf<kolme::Vec3f> a){
		this->Bind();
		Render::setVertexAttribArray(id, &*a.begin());
	}
	
	void setVertexAttribArray(Render::InputID id, const utki::Buf<kolme::Vec2f> a){
		this->Bind();
		Render::setVertexAttribArray(id, &*a.begin());
	}
public:
	/**
	 * @brief Terminates render sequence.
     */
	void renderNothing(){
		if (this != boundShader) {
			throw morda::Exc("Shader::Unbind(): another shader is bound");
		}
		renderIsInProgress = false;
	}
	
	virtual ~Shader()noexcept{}
	
	void SetMatrix(const kolme::Matr4f &m){
		this->setUniformMatrix4f(this->matrixUniform, m);
	}
};//~class Shader



}//~namespace
