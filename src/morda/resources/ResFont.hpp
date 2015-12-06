/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <string>

#include <stob/dom.hpp>

#include "../ResourceManager.hpp"

#include "../fonts/TexFont.hpp"



namespace morda{



class ResFont : public morda::Resource{
	friend class morda::ResourceManager;

	morda::TexFont f;

public:
	ResFont(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline);

	~ResFont()noexcept{}

	const morda::Font& font()noexcept{
		return this->f;
	}
	
private:
	static std::shared_ptr<ResFont> load(const stob::Node& chain, const papki::File &fi);
};



}//~namespace
