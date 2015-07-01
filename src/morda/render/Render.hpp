/* The MIT License:

Copyright (c) 2015 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <memory>

#include <ting/Buffer.hpp>

#include "../util/Matrix4.hpp"


namespace morda{


class Shader;


class Render{
	friend class Shader;
	
public:
	//NOTE: do not change order!!!
	enum class EMode{
		TRIANGLES,
		TRIANGLE_FAN,
		LINE_LOOP
	};
	
	class Program{
	public:
		virtual ~Program()noexcept{}
	};
	
	class InputID{
		friend class morda::Render;
		std::int64_t id;
		void* data;
	public:
		InputID(std::int64_t id, void* data = nullptr) :
			id(id),
			data(data)
		{}
	};
	
	//=== functions to be used by Shader class internally
private:
	static void renderArrays(EMode mode, unsigned numElements);
	
	static void renderElements(EMode mode, const ting::Buffer<const std::uint16_t>& i);
	
	//returns pointer to shader object
	static std::unique_ptr<Program> compileShader(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	static void bindShader(Program& p);
	
	static InputID getAttribute(Program& p, const char* n);
	
	static InputID getUniform(Program& p, const char* n);
	
	static void setUniformMatrix4f(InputID id, const Matr4f& m);
	
	static void setUniform1i(InputID id, int i);
	
	static void setUniform2f(InputID id, Vec2f v);
	
	static void setUniform4f(InputID id, float x, float y, float z, float a);
	
	static void setUniform4f(InputID id, ting::Buffer<const Vec4f> v);
	
	static void setVertexAttribArray(InputID id, const Vec3f* a);
	
	static void setVertexAttribArray(InputID id, const Vec2f* a);
	//=== ~~~
	
	
public:
	
	
};



}
