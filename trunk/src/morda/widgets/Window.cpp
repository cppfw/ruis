#include "Window.hpp"


using namespace morda;


namespace{
const char* WindowDesc(){
	return R"qwerty(
			{
				LinearContainer{
					vertical{true}
					prop{layout{
						dim{2.5mm 100%}
					}}
				}
			
				LinearContainer{
					vertical{true}
			
					prop{layout{
						weight{1}
						dim{min 100%}
					}}
			
					Label{
						text{"The Window Title"}
						name{morda_title}
						prop{layout{
							dim{min min}
							gravity{0%}
						}}
					}
				}
				
				LinearContainer{
					vertical{true}
					prop{layout{
						dim{2.5mm 100%}
					}}
				}
			}
		)qwerty";
}
}



morda::Window::Window() :
		Widget(0),
		LinearContainer(*stob::Parse(WindowDesc()))
{}


morda::Window::Window(const stob::Node& desc) :
		Widget(desc),
		LinearContainer(*stob::Parse(WindowDesc()))
{
//	if(const stob::Node* t = desc.GetProperty("title")){
//		this->title->SetText(t->Value());
//	}
}
