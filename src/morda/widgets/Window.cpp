#include "Window.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"



using namespace morda;


namespace{

const char* windowDesc_c = R"qwertyuiop(
		Table{
			layout{
				dx{max} dy{max}
			}

			//1st row
			TableRow{
				MouseProxy{
					name{morda_lt_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}

				MouseProxy{
					name{morda_t_proxy}
					layout{
						dx{0}
					}
				}

				MouseProxy{
					name{morda_rt_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}
			}//~TableRow

			//2nd row
			TableRow{
				layout{
					weight{1}
				}
				
				MouseProxy{
					name{morda_l_proxy}
					layout{
						dy{fill}
					}
				}

				//middle
				Column{
					layout{
						weight{1}
						dy{max}
					}

					//caption
					Pile{
						layout{
							dx{max}
						}

						clip{true}

						MouseProxy{
							name{morda_caption_proxy}
							layout{
								dx{max} dy{max}
							}
						}

						Color{
							name{morda_window_title_bg}
							layout{
								dx{max} dy{max}
							}
						}

						Row{
							layout{
								dx{max} dy{max}
							}

							Margins{
								left{3pt}
								top{2pt}
								bottom{2pt}

								layout{
									dx{0}
									weight{1}
								}

								Text{
									name{morda_title}
								}
							}
						}
					}

					Pile{
						name{morda_content}
						clip{true}
						layout{
							dx{fill} dy{0}
							weight{1}
						}
					}
				}

				MouseProxy{
					name{morda_r_proxy}
					layout{
						dy{fill}
					}
				}
			}//~TableRow

			//3rd row
			TableRow{
				MouseProxy{
					name{morda_lb_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}

				MouseProxy{
					name{morda_b_proxy}
					layout{
						dx{fill}
					}
				}

				MouseProxy{
					name{morda_rb_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}
			}//~TableRow
		}
	)qwertyuiop";

}


void morda::Window::setBackground(std::shared_ptr<Widget> w) {
	ASSERT(this->children().size() == 1 || this->children().size() == 2)
	if(this->children().size() == 2){
		this->remove(this->children().begin());
	}
	
	ASSERT(this->children().size() == 1)
	
	if(w){
		this->add(std::move(w), this->children().begin());
	}
}



morda::Window::Window(const stob::Node* chain) :
		Widget(chain),
		Pile(stob::parse(windowDesc_c).get())
{
	this->setupWidgets();
	
	if(auto n = getProperty(chain, "title")){
		this->setTitle(n->value());
	}
	
	{
		auto ch = getProperty(chain, "look");
		
		if(auto n = getProperty(ch, "titleColorTopmost")){
			this->titleBgColorTopmost = n->asUint32();
		}else{
			this->titleBgColorTopmost = 0xffff0000;
		}

		if(auto n = getProperty(ch, "titleColorNonTopmost")){
			this->titleBgColorNonTopmost = n->asUint32();
		}else{
			this->titleBgColorNonTopmost = 0xff808080;
		}

		if(auto n = getProperty(ch, "background")){
			this->setBackground(morda::Morda::inst().inflater.inflate(*n));
		}

		{
			Sidesr borders;

			if(auto n = getProperty(ch, "left")){
				borders.left() = dimValueFromSTOB(*n);
			}else{
				borders.left() = 0;
			}

			if(auto n = getProperty(ch, "top")){
				borders.top() = dimValueFromSTOB(*n);
			}else{
				borders.top() = 0;
			}

			if(auto n = getProperty(ch, "right")){
				borders.right() = dimValueFromSTOB(*n);
			}else{
				borders.right() = 0;
			}

			if(auto n = getProperty(ch, "bottom")){
				borders.bottom() = dimValueFromSTOB(*n);
			}else{
				borders.bottom() = 0;
			}

			this->setBorders(borders);
		}
	}
	
	//this should go after initializing borders
	this->emptyMinDim = this->measure(Vec2r(-1));
	
	if(chain){
		this->contentArea->add(*chain);
	}
}

void morda::Window::setupWidgets(){
	this->contentArea = this->findChildByNameAs<Pile>("morda_content");
	ASSERT(this->contentArea)
	
	this->title = this->findChildByNameAs<Text>("morda_title");
	ASSERT(this->title)
	
	this->titleBg = this->findChildByNameAs<Color>("morda_window_title_bg");
	ASSERT(this->titleBg);
	
	std::function<decltype(MouseProxy::mouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(MouseProxy::mouseButton)([this, &flag](Widget& widget, bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
			if(button != MouseButton_e::LEFT){
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
	
	{
		auto caption = this->findChildByNameAs<MouseProxy>("morda_caption_proxy");
		ASSERT(caption)
	
		caption->mouseButton = getButtonFunc(this->captionCaptured);

		caption->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->captionCaptured){
				this->moveBy(pos - this->capturePoint);
				return true;
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_lt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->moveBy(morda::Vec2r(d.x, 0));
				this->resizeBy(morda::Vec2r(-d.x, d.y));
			}
			return false;
		};
	}
	
	{	
		auto w = this->findChildByNameAs<MouseProxy>("morda_lb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(d);
				this->resizeBy(morda::Vec2r(-d.x, -d.y));
			}
			return false;
		};
	}

	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_rt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resizeBy(d);
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_rb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(morda::Vec2r(0, d.y));
				this->resizeBy(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_l_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				this->moveBy(morda::Vec2r(d.x, 0));
				this->resizeBy(morda::Vec2r(-d.x, 0));
			}
			return false;
		};
		this->lBorder = w;
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_r_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				this->resizeBy(morda::Vec2r(d.x, 0));
			}
			return false;
		};
		this->rBorder = w;
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_t_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->topResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resizeBy(morda::Vec2r(0, d.y));
			}
			return false;
		};
		this->tBorder = w;
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_b_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->bottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(morda::Vec2r(0, d.y));
				this->resizeBy(morda::Vec2r(0, -d.y));
			}
			return false;
		};
		this->bBorder = w;
	}
}



void morda::Window::setTitle(const std::string& str){
	this->title->setText(unikod::toUtf32(str));
}

void morda::Window::setBorders(Sidesr borders) {
	this->lBorder->getLayoutParams().dim.x = borders.left();
	this->tBorder->getLayoutParams().dim.y = borders.top();
	this->rBorder->getLayoutParams().dim.x = borders.right();
	this->bBorder->getLayoutParams().dim.y = borders.bottom();
}


bool morda::Window::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
	if(isDown){
		morda::Morda::inst().postToUiThread_ts(
				[this](){
					this->makeTopmost();
				}
			);

		if(!this->isTopmost()){
			this->focus();
		}
	}
	
	this->Container::onMouseButton(isDown, pos, button, pointerId);
	
	return true;
}



bool morda::Window::onMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	this->Container::onMouseMove(pos, pointerId);
	return true;
}

void morda::Window::onTopmostChanged(){
	this->titleBg->setColor(this->isTopmost() ? this->titleBgColorTopmost : this->titleBgColorNonTopmost);
}
