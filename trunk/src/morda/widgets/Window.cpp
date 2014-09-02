#include "Window.hpp"

#include "../App.hpp"


using namespace morda;


namespace{

const char* DWindowDesc = R"qwertyuiop(
		{
			//left border
			LinearContainer{
				background{color{0xff808080}}
				vertical{true}
				prop{layout{
					dim{2mm min}
					band{100%}
				}}
				Widget{
					name{left_top_resize}
					prop{layout{
						dim{min 4mm}
						band{100%}
					}}
					background{color{0xffffff00}}
				}
				Widget{
					name{left_resize}
					prop{layout{
						weight{1}
						dim{min 0}
						band{100%}
					}}
				}
				Widget{
					name{left_bottom_resize}
					prop{layout{
						dim{min 4mm}
						band{100%}
					}}
					background{color{0xffffff00}}
				}
			}

			LinearContainer{
				vertical{true}

				prop{layout{
					weight{1}
					dim{min min}
					band{100%}
				}}

				//top border
				LinearContainer{
					background{color{0xff808080}}
					prop{layout{
						dim{min 2mm}
						band{100%}
					}}
					Widget{
						name{top_left_resize}
						prop{layout{
							dim{2mm min}
							band{100%}
						}}
						background{color{0xffffff00}}
					}
					Widget{
						name{top_resize}
						prop{layout{
							weight{1}
							dim{0 min}
							band{100%}
						}}
					}
					Widget{
						name{top_right_resize}
						prop{layout{
							dim{2mm min}
							band{100%}
						}}
						background{color{0xffffff00}}
					}
				}

				//caption
				LinearContainer{
					prop{layout{
						dim{0 7mm}
						band{100%}
					}}
					background{color{0xffff0000}}
					name{morda_caption}
					Label{
						name{morda_title}
						gravity{0% 50%}
						prop{layout{
							dim{0 min}
							weight{1}
						}}
					}
				}

				FrameContainer{
					name{child_widget_area}
					prop{layout{
						dim{0 0}
						weight{1}
						band{100%}
					}}
//					background{color{0xff800080}}
				}

				//bottom border
				LinearContainer{
					background{color{0xff808080}}
					prop{layout{
						dim{min 2mm}
						band{100%}
					}}
					Widget{
						name{bottom_left_resize}
						prop{layout{
							dim{2mm min}
							band{100%}
						}}
						background{color{0xffffff00}}
					}
					Widget{
						name{bottom_resize}
						prop{layout{
							weight{1}
							dim{0 min}
							band{100%}
						}}
					}
					Widget{
						name{bottom_right_resize}
						prop{layout{
							dim{2mm min}
							band{100%}
						}}
						background{color{0xffffff00}}
					}
				}
			}

			//right border
			LinearContainer{
				background{color{0xff808080}}
				vertical{true}
				prop{layout{
					dim{2mm min}
					band{100%}
				}}
				Widget{
					name{right_top_resize}
					prop{layout{
						dim{min 4mm}
						band{100%}
					}}
					background{color{0xffffff00}}
				}
				Widget{
						name{right_resize}
						prop{layout{
							weight{1}
							dim{0 0}
							band{100%}
						}}
					}
				Widget{
					name{right_bottom_resize}
					prop{layout{
						dim{min 4mm}
						band{100%}
					}}
					background{color{0xffffff00}}
				}
			}
		}
	)qwertyuiop";

}



morda::Window::Window(const stob::Node* desc) :
		Widget(desc),
		LinearContainer(stob::Parse(DWindowDesc).get()),
		emptyMinDim(this->LinearContainer::GetMinDim())
{
	this->SetupWidgets();
	
	if(!desc){
		return;
	}
	
	this->contentArea->SetClip(this->IsClip());
	this->SetClip(false);
	
	if(const stob::Node* t = desc->GetProperty("title")){
		this->SetTitle(t->Value());
	}
	
	for(const stob::Node* n = desc->ChildNonProperty().node(); n; n = n->NextNonProperty().node()){		
		this->contentArea->Add(morda::App::Inst().inflater().Inflate(*n));
	}
}

void morda::Window::SetupWidgets(){
	this->contentArea = std::dynamic_pointer_cast<FrameContainer>(this->FindChildByName("child_widget_area"));
	
	this->caption = this->FindChildByName("morda_caption");
	
	std::function<decltype(Widget::onMouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(Widget::onMouseButton)([this, &flag](Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
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
		});
	};
	
	this->caption->onMouseButton = getButtonFunc(this->captionCaptured);
	
	this->caption->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
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
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->Rect().d.x - this->emptyMinDim.x);
				ting::util::ClampBottom(d.y, -(this->Rect().d.y - this->emptyMinDim.y));
				this->MoveBy(morda::Vec2r(d.x, 0));
				this->ResizeBy(morda::Vec2r(-d.x, d.y));
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
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->Rect().d.x - this->emptyMinDim.x);
				ting::util::ClampTop(d.y, this->Rect().d.y - this->emptyMinDim.y);
				this->MoveBy(d);
				this->ResizeBy(morda::Vec2r(-d.x, -d.y));
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
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->Rect().d.x - this->emptyMinDim.x));
				ting::util::ClampBottom(d.y, -(this->Rect().d.y - this->emptyMinDim.y));
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
		decltype(Widget::onMouseMove) f = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->Rect().d.x - this->emptyMinDim.x));
				ting::util::ClampTop(d.y, this->Rect().d.y - this->emptyMinDim.y);
				this->MoveBy(morda::Vec2r(0, d.y));
				this->ResizeBy(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
		w1->onMouseMove = f;
		w2->onMouseMove = f;
	}
	
	{
		std::shared_ptr<Widget> w = this->FindChildByName("left_resize");
		w->onMouseButton = getButtonFunc(this->leftResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->Rect().d.x - this->emptyMinDim.x);
				this->MoveBy(morda::Vec2r(d.x, 0));
				this->ResizeBy(morda::Vec2r(-d.x, 0));
			}
			return false;
		};
	}
	
	{
		std::shared_ptr<Widget> w = this->FindChildByName("right_resize");
		w->onMouseButton = getButtonFunc(this->rightResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->Rect().d.x - this->emptyMinDim.x));
				this->ResizeBy(morda::Vec2r(d.x, 0));
			}
			return false;
		};
	}
	
	{
		std::shared_ptr<Widget> w = this->FindChildByName("top_resize");
		w->onMouseButton = getButtonFunc(this->topResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.y, -(this->Rect().d.y - this->emptyMinDim.y));
				this->ResizeBy(morda::Vec2r(0, d.y));
			}
			return false;
		};
	}
	
	{
		std::shared_ptr<Widget> w = this->FindChildByName("bottom_resize");
		w->onMouseButton = getButtonFunc(this->bottomResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.y, this->Rect().d.y - this->emptyMinDim.y);
				this->MoveBy(morda::Vec2r(0, d.y));
				this->ResizeBy(morda::Vec2r(0, -d.y));
			}
			return false;
		};
	}
}

void morda::Window::SetTitle(const std::string& str){
	this->title->SetText(str);
}
