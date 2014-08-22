#include "Window.hpp"


using namespace morda;


namespace{

const char* DWindowDesc = R"qwertyuiop(
		{
			//left border
			FrameContainer{
				background{color{0xff808080}}
				vertical{true}
				prop{layout{
					dim{2.5mm 100%}
				}}
				Widget{
					name{left_top_resize}
					prop{layout{
						dim{100% 5mm}
						gravity{0% 100%}
					}}
					background{color{0xffffff00}}
				}
				Widget{
					name{left_bottom_resize}
					prop{layout{
						dim{100% 5mm}
						gravity{0% 0%}
					}}
					background{color{0xffffff00}}
				}
			}

			LinearContainer{
				vertical{true}

				prop{layout{
					weight{1}
					dim{min 100%}
				}}

				//top border
				FrameContainer{
					background{color{0xff808080}}
					prop{layout{
						dim{100% 2.5mm}
					}}
					Widget{
						name{top_left_resize}
						prop{layout{
							dim{2.5mm 100%}
							gravity{0% 0%}
						}}
						background{color{0xffffff00}}
					}
					Widget{
						name{top_right_resize}
						prop{layout{
							dim{2.5mm 100%}
							gravity{100% 0%}
						}}
						background{color{0xffffff00}}
					}
				}

				//caption
				LinearContainer{
					prop{layout{
						dim{100% 7mm}
					}}
					background{color{0xffff0000}}
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

				FrameContainer{
					name{child_widget_area}
					prop{layout{
						dim{100% 0}
						weight{1}
					}}
				}

				//bottom border
				FrameContainer{
					background{color{0xff808080}}
					prop{layout{
						dim{100% 2.5mm}
					}}
					Widget{
						name{bottom_left_resize}
						prop{layout{
							dim{2.5mm 100%}
							gravity{0% 0%}
						}}
						background{color{0xffffff00}}
					}
					Widget{
						name{bottom_right_resize}
						prop{layout{
							dim{2.5mm 100%}
							gravity{100% 0%}
						}}
						background{color{0xffffff00}}
					}
				}
			}

			//right border
			FrameContainer{
				background{color{0xff808080}}
				vertical{true}
				prop{layout{
					dim{2.5mm 100%}
				}}
				Widget{
					name{right_top_resize}
					prop{layout{
						dim{100% 5mm}
						gravity{0% 100%}
					}}
					background{color{0xffffff00}}
				}
				Widget{
					name{right_bottom_resize}
					prop{layout{
						dim{100% 5mm}
						gravity{0% 0%}
					}}
					background{color{0xffffff00}}
				}
			}
		}
	)qwertyuiop";

}



morda::Window::Window() :
		Widget(0),
		LinearContainer(*stob::Parse(DWindowDesc))
{
	this->SetupWidgets();
}


morda::Window::Window(const stob::Node& desc) :
		Widget(desc),
		LinearContainer(*stob::Parse(DWindowDesc))
{
	this->SetupWidgets();
	
	if(const stob::Node* t = desc.GetProperty("title")){
		this->SetTitle(t->Value());
	}
}

void morda::Window::SetupWidgets(){
	this->caption = this->FindChildByName("morda_caption");
	
	std::function<decltype(Widget::onMouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return [this, &flag](Widget& widget, bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
			if(button != Widget::EMouseButton::LEFT){
				return false;
			}

			if(isDown){
				flag = true;
				this->capturePoint = pos;
				return true;
			}
			flag = false;
			return false;
		};
	};
	
	this->caption->onMouseButton = getButtonFunc(this->captionCaptured);
	
	this->caption->onMouseMove = [this](Widget& widget, const morda::Vec2f& pos, unsigned pointerId){
		if(this->captionCaptured){
			this->MoveBy(pos - this->capturePoint);
			return true;
		}
		return false;
	};
	
	this->title = std::dynamic_pointer_cast<Label>(this->caption->FindChildByName("morda_title"));
	
	{
		std::shared_ptr<Widget> w1 = this->FindChildByName("left_top_resize");
		std::shared_ptr<Widget> w2 = this->FindChildByName("top_left_resize");
		w1->onMouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w2->onMouseButton = getButtonFunc(this->leftTopResizeCaptured);
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2f& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2f d = pos - this->capturePoint;
				this->MoveBy(morda::Vec2f(d.x, 0));
				this->ResizeBy(morda::Vec2f(-d.x, d.y));
			}
			return false;
		};
		w1->onMouseMove = f;
		w2->onMouseMove = f;
	}
	
	{	
		std::shared_ptr<Widget> w1 = this->FindChildByName("left_bottom_resize");
		std::shared_ptr<Widget> w2 = this->FindChildByName("bottom_left_resize");
		w1->onMouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w2->onMouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2f& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2f d = pos - this->capturePoint;
				this->MoveBy(d);
				this->ResizeBy(morda::Vec2f(-d.x, -d.y));
			}
			return false;
		};
		w1->onMouseMove = f;
		w2->onMouseMove = f;
	}

	{
		std::shared_ptr<Widget> w1 = this->FindChildByName("top_right_resize");
		std::shared_ptr<Widget> w2 = this->FindChildByName("right_top_resize");
		w1->onMouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w2->onMouseButton = getButtonFunc(this->rightTopResizeCaptured);
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2f& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2f d = pos - this->capturePoint;
				this->ResizeBy(d);
			}
			return false;
		};
		w1->onMouseMove = f;
		w2->onMouseMove = f;
	}
	
	{
		std::shared_ptr<Widget> w1 = this->FindChildByName("bottom_right_resize");
		std::shared_ptr<Widget> w2 = this->FindChildByName("right_bottom_resize");
		w1->onMouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w2->onMouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2f& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2f d = pos - this->capturePoint;
				this->MoveBy(morda::Vec2f(0, d.y));
				this->ResizeBy(morda::Vec2f(d.x, -d.y));
			}
			return false;
		};
		w1->onMouseMove = f;
		w2->onMouseMove = f;
	}
}

void morda::Window::SetTitle(const std::string& str){
	this->title->SetText(str);
}
