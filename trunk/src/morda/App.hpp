/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

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

#include <ting/Singleton.hpp>
#include <ting/types.hpp>

#include "config.hpp"

#ifdef M_MORDA_OGLES2
#	include <GLES2/gl2.h>
#	include <EGL/egl.h>
#else
#	include <GL/glew.h>
#	include <GL/glx.h>
#endif


#include "Exc.hpp"
#include "Widget.hpp"
#include "Container.hpp"
#include "GuiInflater.hpp"
#include "resman/ResourceManager.hpp"

#include "shaders/SimpleSingleColoringShader.hpp"
#include "shaders/SimpleTexturingShader.hpp"


namespace morda{



class App : public ting::IntrusiveSingleton<App>{
	friend class ting::IntrusiveSingleton<App>;
	static ting::IntrusiveSingleton<App>::T_Instance instance;
	
	struct XDisplayWrapper{
		Display* d;
		XDisplayWrapper();
		~XDisplayWrapper()throw();
	} xDisplay;
	
	struct XVisualInfoWrapper{
		XVisualInfo *vi;
		XVisualInfoWrapper(XDisplayWrapper& xDisplay);
		~XVisualInfoWrapper()throw();
	} xVisualInfo;
	
	struct XWindowWrapper{
		Window w;
		
		XDisplayWrapper& d;

		XWindowWrapper(unsigned width, unsigned height, XDisplayWrapper& xDisplay, XVisualInfoWrapper& xVisualInfo);
		~XWindowWrapper()throw();
	} xWindow;
	
	struct GLXContextWrapper{
		GLXContext glxContext;
		
		XDisplayWrapper& d;
		XWindowWrapper& w;
		
		GLXContextWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow, XVisualInfoWrapper& xVisualInfo);
		~GLXContextWrapper()throw(){
			this->Destroy();
		}
		
		void Destroy()throw();
	} glxContex;
	
	tride::Vec2f curWinDim;
	
	ting::Inited<volatile bool, false> quitFlag;
	
	ting::Ref<morda::Container> rootContainer;
	
	ResourceManager resMan;
	
	GuiInflater inflater;
	
	void SetGLViewport(const tride::Vec2f& dim);
	
	void Render();
	
	
public:
	struct DefaultShaders{
		SimpleSingleColoringShader simpleSingleColoring;
		SimpleTexturingShader simpleTexturing;
	};
	
private:
	DefaultShaders shaders;
	
	friend void Main(int argc, char** argv);
	void Exec();
	
protected:
	App(unsigned w, unsigned h);

public:
	
	inline DefaultShaders& Shaders()throw(){
		return this->shaders;
	}
	
	virtual ~App()throw(){}
	
	virtual void Init() = 0;
	
	inline void SetRootContainer(const ting::Ref<morda::Container>& c){
		this->rootContainer = c;
		this->rootContainer->SetPos(tride::Vec2f(0));
		this->rootContainer->Resize(this->curWinDim);
	}
	
	inline ResourceManager& ResMan()throw(){
		return this->resMan;
	}
	
	inline GuiInflater& Inflater()throw(){
		return this->inflater;
	}
};



/**
 * @brief Create application instance
 * User needs to define this factory function to create his application instance.
 * @return New application instance.
 */
ting::Ptr<App> CreateApp(int argc, char** argv);



}//~namespace
