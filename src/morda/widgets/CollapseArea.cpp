#include "../Morda.hpp"

#include "CollapseArea.hpp"
#include "button/Button.hpp"

using namespace morda;

namespace{
const char* layout_c = R"qwertyuiop(
	Pile{
		layout{dx{max}}
		ColorLabel{
			layout{dx{fill}dy{fill}}
			color{@{morda_color_fg}}
		}
		Row{
			layout{dx{max}dy{max}}
			Margins{
				defs{
					marHor{5pt}
					marVer{3pt}
				}
				left{@{marHor}}
				top{@{marVer}}
				right{@{marHor}}
				bottom{@{marVer}}

				ImageToggle{
					name{switch}
					look{
						unchecked{morda_img_dropdown_arrow}
						checked{morda_img_dropright_arrow}
					}
				}
			}
			Pile{
				name{title}
			}
		}
	}
	Pile{
		name{content}
	}
)qwertyuiop";
}

CollapseArea::CollapseArea(const stob::Node* chain) :
		Widget(chain),
		Column(stob::parse(layout_c).get())
{
	this->contentArea = this->findChildByNameAs<Pile>("content");
	ASSERT(this->contentArea)
	
	this->title_v = this->findChildByNameAs<Pile>("title");
	ASSERT(this->title_v)
			
	if(chain){
		this->contentArea->add(*chain);
	}
	
	{
		auto sw = this->findChildByNameAs<ToggleButton>("switch");
		ASSERT(sw)
		sw->checkedChanged = [this](ToggleButton& tb){
			if(tb.isChecked()){
				this->contentArea->getLayoutParams().dim.y = 0;
			}else{
				this->contentArea->getLayoutParams().dim.y = Widget::LayoutParams::min_c;
			}
		};
	}
}
