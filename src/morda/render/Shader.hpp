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
 * @brief Base class for shader.
 * Each shader has at least matrix uniform.
 * Matrix uniform should be 'mat4' and named 'matrix' in GLSL.
 */
class Shader{
	friend class Render;
	
	static Shader* boundShader;
	static bool renderIsInProgress;
	
	std::unique_ptr<utki::Void> program;

	const Render::InputID matrixUniform;
	
	void bind(){
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
	/**
	 * @brief Find shader attribute.
	 * @param n - name of the attribute to look for.
	 * @return The ID of the attribute.
	 * @throw utki::Exc if attribute with given name was not found.
	 */
	Render::InputID getAttribute(const char* n){
		return Render::getAttribute(*this->program, n);
	}
	
	/**
	 * @brief Find shader uniform.
	 * @param n - name of the uniform to look for.
	 * @return The ID of the uniform.
	 * @throw utki::Exc if uniform with given name was not found.
	 */
	Render::InputID getUniform(const char* n){
		return Render::getUniform(*this->program, n);
	}

	/**
	 * @brief Create a shader from vertex and fragment program source codes.
	 * @param vertexShaderCode - source code for vertext shader.
	 * @param fragmentShaderCode - source code for fragment shader.
	 */
	Shader(const char* vertexShaderCode, const char* fragmentShaderCode);

	/**
	 * @brief Render arrays of attributes.
	 * @param mode - rendering mode ov vertex data.
	 * @param numElements - number of vertices to render.
	 */
	void renderArrays(Render::Mode_e mode, size_t numElements){
		this->bind();
		Render::renderArrays(mode, numElements);
		this->renderIsInProgress = false;
	}
	
	/**
	 * @brief Render vertices using their indices.
	 * @param mode - rendering mode of vertex data.
	 * @param i - buffer of vertex indices to render.
	 */
	void renderElements(Render::Mode_e mode, const utki::Buf<std::uint16_t> i){
		this->bind();
		Render::renderElements(mode, i);
		this->renderIsInProgress = false;
	}
	
	/**
	 * @brief Set value of 4x4 matrix shader uniform.
	 * @param id - ID of the unifor to set the value of.
	 * @param m - the value to set.
	 */
	void setUniformMatrix4f(Render::InputID id, const kolme::Matr4f& m){
		this->bind();
		Render::setUniformMatrix4f(id, m);
	}
	
	void setUniform1i(Render::InputID id, int i){
		this->bind();
		Render::setUniform1i(id, i);
	}
	
	void setUniform2f(Render::InputID id, kolme::Vec2f v){
		this->bind();
		Render::setUniform2f(id, v);
	}
	
	void setUniform4f(Render::InputID id, float x, float y, float z, float a){
		this->bind();
		Render::setUniform4f(id, x, y, z, a);
	}
	
	void setUniform4f(Render::InputID id, const utki::Buf<kolme::Vec4f> v){
		this->bind();
		Render::setUniform4f(id, v);
	}
	
	void setVertexAttribArray(Render::InputID id, const utki::Buf<std::uint32_t> a){
		this->bind();
		Render::setVertexAttribArray(id, &*a.begin());
	}
	
	void setVertexAttribArray(Render::InputID id, const utki::Buf<kolme::Vec4f> a){
		this->bind();
		Render::setVertexAttribArray(id, &*a.begin());
	}
	
	void setVertexAttribArray(Render::InputID id, const utki::Buf<kolme::Vec3f> a){
		this->bind();
		Render::setVertexAttribArray(id, &*a.begin());
	}
	
	void setVertexAttribArray(Render::InputID id, const utki::Buf<kolme::Vec2f> a){
		this->bind();
		Render::setVertexAttribArray(id, &*a.begin());
	}
public:
	/**
	 * @brief Terminates render sequence.
	 * Use this function to explicitly terminate the rendering sequence. I.e. calling
	 * a render() method of shader will not be required after that and another shader could be bound right a way.
     */
	void renderNothing(){
		if (this != boundShader) {
			throw morda::Exc("Shader::Unbind(): another shader is bound");
		}
		renderIsInProgress = false;
	}
	
	virtual ~Shader()noexcept{}
	
	/**
	 * @brief Set value of the matrix uniform.
	 * @param m - value to set as a matrix uniform of the shader.
	 */
	void setMatrix(const kolme::Matr4f &m){
		this->setUniformMatrix4f(this->matrixUniform, m);
	}
};



}//~namespace
