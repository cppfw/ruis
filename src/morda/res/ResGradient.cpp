#include <kolme/Vector4.hpp>

#include "ResGradient.hpp"

#include "../util/util.hpp"
#include "../Morda.hpp"



using namespace morda;



ResGradient::ResGradient(const std::vector<std::tuple<real,std::uint32_t> >& stops, bool vertical){
	std::vector<kolme::Vec2f> vertices;
//	std::vector<std::uint32_t> colors;
	std::vector<kolme::Vec4f> colors;
	for(auto& s : stops){
		{
			auto c = std::get<1>(s);
			kolme::Vec4f clr(
					float(c & 0xff) / 255,
					float((c >> 8) & 0xff) / 255,
					float((c >> 16) & 0xff) / 255,
					float((c >> 24) & 0xff) / 255
				);

			colors.push_back(clr);
			colors.push_back(clr);
		}
		
//		TRACE(<< "put color = " << std::hex << colors.back() << std::endl)
		
		if(vertical){
			vertices.push_back(kolme::Vec2f(0, std::get<0>(s)));
			vertices.push_back(kolme::Vec2f(1, std::get<0>(s)));
		}else{ ASSERT(!vertical)
			vertices.push_back(kolme::Vec2f(std::get<0>(s), 1));
			vertices.push_back(kolme::Vec2f(std::get<0>(s), 0));
		}
//		TRACE(<< "put pos = " << vertices.back() << std::endl)
	}
	ASSERT(vertices.size() == colors.size())
	
	std::vector<std::uint16_t> indices;
	for(size_t i = 0; i != vertices.size(); ++i){
		indices.push_back(std::uint16_t(i));
	}
	
	auto& r = morda::inst().renderer();
	this->vao = r.factory->createVertexArray(
			{
				r.factory->createVertexBuffer(utki::wrapBuf(vertices)),
				r.factory->createVertexBuffer(utki::wrapBuf(colors))
			},
			r.factory->createIndexBuffer(utki::wrapBuf(indices)),
			VertexArray::Mode_e::TRIANGLE_STRIP
		);
}



std::shared_ptr<ResGradient> ResGradient::load(const stob::Node& chain, const papki::File& fi) {
	bool vertical;
	
	if(auto n = getProperty(&chain, "vertical")){
		vertical = n->asBool();
	}else{
		vertical = false;
	}
	
	std::vector<std::tuple<real,std::uint32_t>> stops;
	
	const char* stop_c = "Stop";
	for(auto n = chain.thisOrNext(stop_c).get_node(); n; n = n->next(stop_c).get_node()){
		real pos;
		if(auto p = getProperty(n->child(), "pos")){
			pos = p->asFloat();
		}else{
			pos = 0;
		}
		
		std::uint32_t color;
		if(auto p = getProperty(n->child(), "color")){
			color = p->asUint32();
		}else{
			color = 0xffffffff;
		}
		
		stops.push_back(std::make_tuple(pos, color));
	}
	
	return std::make_shared<ResGradient>(stops, vertical);
}


void ResGradient::render(const morda::Matr4r& m) const {
	morda::inst().renderer().shader->posClr->render(m, *this->vao);
}

