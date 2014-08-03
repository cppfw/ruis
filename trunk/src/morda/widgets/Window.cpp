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
							dim{100% 7mm}
						}}
						name{morda_caption}
						Label{
							name{morda_title}
							gravity{0% 50%}
							prop{layout{
								dim{20mm min}
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
{
	this->FindWidgets();
}


morda::Window::Window(const stob::Node& desc) :
		Widget(desc),
		LinearContainer(*stob::Parse(WindowDesc()))
{
	this->FindWidgets();
	
	if(const stob::Node* t = desc.GetProperty("title")){
		this->title->SetText(t->Value());
	}
}

void morda::Window::FindWidgets(){
	this->caption = this->FindChildByName("morda_caption");
	
	this->caption->onMouseButton = [this](Widget& widget, bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
		if(isDown){
			this->captionCaptured = true;
			this->capturePoint = pos;
			return true;
		}
		this->captionCaptured = false;
		return false;
	};
	
	this->caption->onMouseMove = [this](Widget& widget, const morda::Vec2f& pos, unsigned pointerId){
		if(this->captionCaptured){
			this->MoveBy(pos - this->capturePoint);
			return true;
		}
		return false;
	};
	
	
	this->title = this->caption->FindChildByName("morda_title").DynamicCast<Label>();
}
