#include "Window.hpp"

#include "../App.hpp"

#include "../util/util.hpp"

using namespace morda;


namespace{

const char* DWindowDesc = R"qwertyuiop(
		{
			//left border
			LinearContainer{
				vertical{true}
				reverse{true}
				layout{
					dim{min min}
					fill{false true}
				}
				ImageLabel{
					name{left_top_resize}
					layout{
						dim{min min}
					}
					image{morda_img_window_lt}
				}
				ImageLabel{
					name{left_resize}
					layout{
						weight{1}
						dim{min 0}
						fill{false true}
					}
					image{morda_img_window_l}
				}
				ImageLabel{
					name{left_bottom_resize}
					layout{
						dim{min min}
						fill{true true}
					}
					image{morda_img_window_lb}
				}
			}

			LinearContainer{
				vertical{true}
				reverse{true}
				layout{
					weight{1}
					dim{min min}
					fill{true true}
				}

				//top border
				ImageLabel{
					name{top_resize}
					image{morda_img_window_t}
					layout{
						dim{0 min}
						fill{true false}
					}
				}

				//caption
				FrameContainer{
					layout{
						dim{0 min}
						fill{true true}
					}

					ColorLabel{
						layout{
							dim{0 0}
							fill{true true}
						}
						color{0xffff0000}
					}

					LinearContainer{
						layout{
							dim{0 min}
							fill{true true}
						}

						name{morda_caption}
						Label{
							name{morda_title}
							layout{
								dim{0 min}
								gravity{0% 50%}
								weight{1}
							}
						}
					}
				}

				FrameContainer{
					name{child_widget_area}
					layout{
						dim{0 0}
						weight{1}
						fill{true true}
					}
//					color{0xff800080}
				}

				//bottom border
				ImageLabel{
					name{bottom_resize}
					layout{
						dim{0 min}
						fill{true false}
					}
					image{morda_img_window_b}
				}
			}

			//right border
			LinearContainer{
				vertical{true}
				reverse{true}
				layout{
					dim{min min}
					fill{false true}
				}
				ImageLabel{
					name{right_top_resize}
					image{morda_img_window_rt}
					layout{
						dim{min min}
					}
				}
				ImageLabel{
					name{right_resize}
					layout{
						weight{1}
						dim{min 0}
						fill{false true}
					}
					image{morda_img_window_r}
				}
				ImageLabel{
					name{right_bottom_resize}
					image{morda_img_window_rb}
					layout{
						dim{min min}
					}
				}
			}
		}
	)qwertyuiop";

}



morda::Window::Window(const stob::Node* desc) :
		Widget(desc),
		LinearContainer(stob::Parse(DWindowDesc).get()),
		emptyMinDim(this->LinearContainer::ComputeMinDim())
{
	this->SetupWidgets();
	
	this->contentArea->SetClip(this->IsClip());
	this->SetClip(false);
	
	if(const stob::Node* n = GetProperty(desc, "title")){
		this->SetTitle(n->Value());
	}
	
	if(desc){
		this->contentArea->Add(*desc);
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
		std::shared_ptr<Widget> w = this->FindChildByName("left_top_resize");
		w->onMouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->Rect().d.x - this->emptyMinDim.x);
				ting::util::ClampBottom(d.y, -(this->Rect().d.y - this->emptyMinDim.y));
				this->MoveBy(morda::Vec2r(d.x, 0));
				this->ResizeBy(morda::Vec2r(-d.x, d.y));
			}
			return false;
		};
	}
	
	{	
		std::shared_ptr<Widget> w = this->FindChildByName("left_bottom_resize");
		w->onMouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->Rect().d.x - this->emptyMinDim.x);
				ting::util::ClampTop(d.y, this->Rect().d.y - this->emptyMinDim.y);
				this->MoveBy(d);
				this->ResizeBy(morda::Vec2r(-d.x, -d.y));
			}
			return false;
		};
	}

	{
		std::shared_ptr<Widget> w = this->FindChildByName("right_top_resize");
		w->onMouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->Rect().d.x - this->emptyMinDim.x));
				ting::util::ClampBottom(d.y, -(this->Rect().d.y - this->emptyMinDim.y));
				this->ResizeBy(d);
			}
			return false;
		};
	}
	
	{
		std::shared_ptr<Widget> w = this->FindChildByName("right_bottom_resize");
		w->onMouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->Rect().d.x - this->emptyMinDim.x));
				ting::util::ClampTop(d.y, this->Rect().d.y - this->emptyMinDim.y);
				this->MoveBy(morda::Vec2r(0, d.y));
				this->ResizeBy(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
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
	this->title->SetText(ting::utf8::ToUTF32(str));
}
