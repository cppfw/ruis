#include "CollapseArea.hpp"

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
				ImageLabel{
					name{switch_arrow}
					image{morda_img_dropdown_arrow}
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
	
	if(chain){
		this->contentArea->add(*chain);
	}
}
