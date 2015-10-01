/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <papki/File.hpp>

#include "Vector2.hpp"



namespace morda{



class Image{
public:
	enum class EType{
		UNKNOWN = 0,
		GREY  = 1, //1 channel. Only Grey channel
		GREYA = 2, //2 channels. Grey with Alpha channel
		RGB   = 3, //3 channels. Red Green Blue channels
		RGBA  = 4  //4 channels. RGBA format (4 channels)
	};

	class Exc : public utki::Exc{
	public:
		Exc(const std::string& msg) :
				utki::Exc(msg.c_str())
		{}
	};
	
private:
	EType type_var;
	Vec2ui dim_var = Vec2ui(0);
	std::vector<std::uint8_t> buf_var;//image pixels data

public:
	//default constructor
	Image() :
			type_var(EType::UNKNOWN)
	{}

	//copy constructor
	Image(const Image& im);

	//Creates Image object with initialized image.
	Image(Vec2ui dimensions, EType imageType){
		this->init(dimensions, imageType);
	}

	Image(Vec2ui dimensions, EType typeOfImage, const std::uint8_t* srcBuf);

	//Creates Image object and copies a region from source image.
	Image(Vec2ui pos, Vec2ui dimensions, const Image& src);

	Image(const papki::File& f){
		this->load(f);
	}

	~Image();

	const Vec2ui& dim()const noexcept{
		return this->dim_var;
	}

	unsigned bitsPerPixel()const{
		return this->numChannels() * 8;
	}

	unsigned numChannels()const{
		return unsigned(this->type_var);
	}

	EType type()const{
		return this->type_var;
	}

	utki::Buf<std::uint8_t> buf(){
		return utki::wrapBuf(this->buf_var);
	}

	const utki::Buf<std::uint8_t> buf()const{
		return utki::wrapBuf(this->buf_var);
	}

public:
	void init(Vec2ui dimensions, EType typeOfImage);
	void reset();//destroys all image data
	void clear(std::uint8_t  val = 0);//fills each image channel with specified value
	void clear(unsigned chan, std::uint8_t val = 0);//fills specified channel with given value

	void flipVertical();

	void blit(unsigned x, unsigned y, const Image& src);

	void blit(unsigned x, unsigned y, const Image& src, unsigned dstChan, unsigned srcChan);

	const std::uint8_t& pixChan(unsigned x, unsigned y, unsigned chan)const{
		return this->buf_var[ASSCOND((y * this->dim().x + x) * this->numChannels() + chan, < this->buf_var.size())];
	}

	std::uint8_t& pixChan(unsigned x, unsigned y, unsigned chan){
		return this->buf_var[ASSCOND((y * this->dim().x + x) * this->numChannels() + chan, < this->buf_var.size())];
	}

	void loadPNG(const papki::File& f);//Load image from PNG-file
	void loadJPG(const papki::File& f);//Load image from JPG-file
//	void loadTGA(papki::File& f);//Load image from TGA-file

	void load(const papki::File& f);

private:

};//~class



}//~namespace
