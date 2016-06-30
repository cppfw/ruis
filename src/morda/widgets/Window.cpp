#include "Window.hpp"

#include "../App.hpp"

#include "../util/util.hpp"


using namespace morda;


namespace{

const char* DWindowDesc = R"qwertyuiop(
		Table{
			layout{
				dimX{max} dimY{max}
			}

			//1st row
			TableRow{
				MouseProxy{
					name{morda_lt_proxy}
					layout{
						dimX{fill} dimY{fill}
					}
				}

				MouseProxy{
					name{morda_t_proxy}
					layout{
						dimX{0}
					}
				}

				MouseProxy{
					name{morda_rt_proxy}
					layout{
						dimX{fill} dimY{fill}
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
						dimY{fill}
					}
				}

				//middle
				VerticalArea{
					layout{
						weight{1}
						dimY{max}
					}

					//caption
					Frame{
						layout{
							dimX{max}
						}

						clip{true}

						MouseProxy{
							name{morda_caption_proxy}
							layout{
								dimX{max} dimY{max}
							}
						}

						ColorLabel{
							name{morda_window_title_bg}
							layout{
								dimX{max} dimY{max}
							}
						}

						HorizontalArea{
							layout{
								dimX{max} dimY{max}
							}

							TextLabel{
								name{morda_title}
								layout{
									dimX{0}
									weight{1}
								}
							}
						}
					}

					Frame{
						name{morda_content}
						clip{true}
						layout{
							dimX{fill} dimY{0}
							weight{1}
						}
					}
				}

				MouseProxy{
					name{morda_r_proxy}
					layout{
						dimY{fill}
					}
				}
			}//~TableRow

			//3rd row
			TableRow{
				MouseProxy{
					name{morda_lb_proxy}
					layout{
						dimX{fill} dimY{fill}
					}
				}

				MouseProxy{
					name{morda_b_proxy}
					layout{
						dimX{fill}
					}
				}

				MouseProxy{
					name{morda_rb_proxy}
					layout{
						dimX{fill} dimY{fill}
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
		Frame(stob::parse(DWindowDesc).get()),
		emptyMinDim(this->measure(Vec2r(-1)))
{
	this->setupWidgets();
	
	if(auto n = getProperty(chain, "title")){
		this->setTitle(n->value());
	}
	
	if(auto n = getProperty(chain, "titleColorTopmost")){
		this->titleBgColorTopmost = n->asUint32();
	}else{
		this->titleBgColorTopmost = 0xffff0000;
	}

	if(auto n = getProperty(chain, "titleColorNonTopmost")){
		this->titleBgColorNonTopmost = n->asUint32();
	}else{
		this->titleBgColorNonTopmost = 0xff808080;
	}

	if(auto n = getProperty(chain, "background")){
		this->setBackground(morda::App::inst().inflater.inflate(*n));
	}

	{
		Sidesr borders;
		
		if(auto n = getProperty(chain, "left")){
			borders.left() = dimValueFromSTOB(*n);
		}else{
			borders.left() = 0;
		}
		
		if(auto n = getProperty(chain, "top")){
			borders.top() = dimValueFromSTOB(*n);
		}else{
			borders.top() = 0;
		}
		
		if(auto n = getProperty(chain, "right")){
			borders.right() = dimValueFromSTOB(*n);
		}else{
			borders.right() = 0;
		}
		
		if(auto n = getProperty(chain, "bottom")){
			borders.bottom() = dimValueFromSTOB(*n);
		}else{
			borders.bottom() = 0;
		}
		
		this->setBorders(borders);
	}
	
	if(chain){
		this->contentArea->add(*chain);
	}
}

void morda::Window::setupWidgets(){
	this->contentArea = this->findChildByNameAs<Frame>("morda_content");
	ASSERT(this->contentArea)
	
	this->title = this->findChildByNameAs<TextLabel>("morda_title");
	ASSERT(this->title)
	
	this->titleBg = this->findChildByNameAs<ColorLabel>("morda_window_title_bg");
	ASSERT(this->titleBg);
	
	std::function<decltype(MouseProxy::mouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(MouseProxy::mouseButton)([this, &flag](Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
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


bool morda::Window::onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(isDown){
		morda::App::inst().postToUiThread_ts(
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
