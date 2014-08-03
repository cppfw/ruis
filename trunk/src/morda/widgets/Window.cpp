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

					LinearContainer{
						prop{layout{
							dim{100% 2.5mm}
						}}
					}
			
					LinearContainer{
						prop{layout{
							dim{100% min}
						}}
						name{morda_caption}
						Label{
							text{"The Window Title"}
							name{morda_title}
							gravity{0% 50%}
							prop{layout{
								dim{min min}
								weight{1}
							}}
						}
					}
			
					Widget{
						name{child_widget_area}
						prop{layout{
							dim{100% 0}
							weight{1}
						}}
					}
			
					LinearContainer{
						prop{layout{
							dim{100% 2.5mm}
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
