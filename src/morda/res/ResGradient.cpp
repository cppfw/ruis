#include <r4/vector4.hpp>

#include "ResGradient.hpp"

#include "../util/util.hpp"
#include "../Morda.hpp"



using namespace morda;



ResGradient::ResGradient(const std::vector<std::tuple<real,std::uint32_t> >& stops, bool vertical){
	std::vector<r4::vec2f> vertices;
//	std::vector<std::uint32_t> colors;
	std::vector<r4::vec4f> colors;
	for(auto& s : stops){
		{
			auto c = std::get<1>(s);
			r4::vec4f clr(
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
			vertices.push_back(r4::vec2f(0, std::get<0>(s)));
			vertices.push_back(r4::vec2f(1, std::get<0>(s)));
		}else{ ASSERT(!vertical)
			vertices.push_back(r4::vec2f(std::get<0>(s), 1));
			vertices.push_back(r4::vec2f(std::get<0>(s), 0));
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



std::shared_ptr<ResGradient> ResGradient::load(const puu::forest& desc, const papki::File& fi) {
	bool vertical = false;

	std::vector<std::tuple<real,std::uint32_t>> stops;

	const char* stop_c = "Stop";

	for(auto& p : desc){
		if(p.value == "vertical"){
			vertical = get_property_value(p).to_bool();
		}else if(p.value == stop_c){
			real pos = 0;
			uint32_t color = 0xffffffff;
			for(auto& pp : p.children){
				if(pp.value == "pos"){
					pos = get_property_value(pp).to_float();
				}else if(pp.value == "color"){
					color = get_property_value(pp).to_uint32();
				}
			}
			stops.push_back(std::make_tuple(pos, color));
		}
	}
	
	return std::make_shared<ResGradient>(stops, vertical);
}


void ResGradient::render(const morda::Matr4r& m) const {
	morda::inst().renderer().shader->posClr->render(m, *this->vao);
}

