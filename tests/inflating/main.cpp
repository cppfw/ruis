#include "../../src/morda/Morda.hpp"

#include "FakeRenderer.hpp"


class TestMorda : public morda::Morda{
	
public:
	TestMorda() : morda::Morda(utki::makeShared<FakeRenderer>(), 0, 0){}
	void postToUiThread_ts(std::function<void()>&& f) override{
		
	}
};

int main(int argc, char** argv){
	TestMorda m;
	
	auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
		Container{
			defs{
				a{123}
			}

			templates{
				Cont Container{
				}
			}

			Cont{}
			Cont{}
		}
	)qwertyuiop"));
	
	ASSERT_ALWAYS(w)
	ASSERT_ALWAYS(std::dynamic_pointer_cast<morda::Container>(w))
	
	return 0;
}