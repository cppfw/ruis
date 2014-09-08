#include "NinePatch.hpp"


using namespace morda;


namespace{
const char* D_NinePatchLayout = R"qwertyuiop(
		{
			//left column
			LinearContainer{
				vertical{true}
				prop{layout{
					dim{min min}
					fill{false true}
				}}

				ImageLabel{
					name{morda_left_top}
					prop{layout{
						dim{min min}
					}}
				}

				ImageLabel{
					name{morda_left}
					prop{layout{
						dim{min 0}
						weight{1}
					}}
				}
				ImageLabel{
					name{morda_left_bottom}
					prop{layout{
						dim{min min}
					}}
				}
			}

			//middle column
			LinearContainer{
				vertical{true}
				prop{layout{
					dim{0 min}
					fill{false true}
					weight{1}
				}}

				ImageLabel{
					name{morda_top}
					prop{layout{
						dim{0 min}
					}}
				}
				FrameContainer{
					name{morda_content_area}
					prop{layout{
						dim{0 0}
						weight{1}
						fill{true true}
					}}
					ImageLabel{
						name{morda_middle}
						prop{layout{
							dim{0 0}
							fill{true true}
						}}
					}
				}
				ImageLabel{
					name{morda_bottom}
					prop{layout{
						dim{0 min}
					}}
				}
			}

			//right column
			LinearContainer{
				vertical{true}
				prop{layout{
					dim{min min}
					fill{false true}
				}}

				ImageLabel{
					name{morda_right_top}
					prop{layout{
						dim{min min}
					}}
				}

				ImageLabel{
					name{morda_right}
					prop{layout{
						dim{min 0}
						weight{1}
					}}
				}
				ImageLabel{
					name{morda_right_bottom}
					prop{layout{
						dim{min min}
					}}
				}
			}
		}
	)qwertyuiop";
}


NinePatch::NinePatch(const stob::Node* desc) :
		Widget(desc),
		LinearContainer(stob::Parse(D_NinePatchLayout).get())
{
	if(!desc){
		return;
	}
	
	
	//TODO:
}


void NinePatch::SetNinePatch(const std::shared_ptr<ResNinePatch>& np){
	//TODO:
}
