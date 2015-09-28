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
	EType type;
	Vec2ui dim = Vec2ui(0);
	std::vector<std::uint8_t> buf;//image pixels data

public:
	//default constructor
	Image() :
			type(EType::UNKNOWN)
	{}

	//copy constructor
	Image(const Image& im);

	//Creates Image object with initialized image.
	Image(Vec2ui dimensions, EType imageType){
		this->Init(dimensions, imageType);
	}

	Image(Vec2ui dimensions, EType typeOfImage, const std::uint8_t* srcBuf);

	//Creates Image object and copies a region from source image.
	Image(Vec2ui pos, Vec2ui dimensions, const Image& src);

	Image(const papki::File& f){
		this->Load(f);
	}

	~Image();

	const Vec2ui& Dim()const noexcept{
		return this->dim;
	}

	unsigned BitsPerPixel()const{
		return this->NumChannels() * 8;
	}

	unsigned NumChannels()const{
		return unsigned(this->type);
	}

	EType Type()const{
		return this->type;
	}

	utki::Buf<std::uint8_t> Buf(){
		return utki::wrapBuf(this->buf);
	}

	const utki::Buf<std::uint8_t> Buf()const{
		return utki::wrapBuf(this->buf);
	}

public:
	void Init(Vec2ui dimensions, EType typeOfImage);
	void Reset();//destroys all image data
	void Clear(std::uint8_t  val = 0);//fills each image channel with specified value
	void Clear(unsigned chan, std::uint8_t val = 0);//fills specified channel with given value

	void FlipVertical();

	void Blit(unsigned x, unsigned y, const Image& src);

	void Blit(unsigned x, unsigned y, const Image& src, unsigned dstChan, unsigned srcChan);

	const std::uint8_t& PixChan(unsigned x, unsigned y, unsigned chan)const{
		return this->buf[ASSCOND((y * this->Dim().x + x) * this->NumChannels() + chan, < this->buf.size())];
	}

	std::uint8_t& PixChan(unsigned x, unsigned y, unsigned chan){
		return this->buf[ASSCOND((y * this->Dim().x + x) * this->NumChannels() + chan, < this->buf.size())];
	}

	void LoadPNG(const papki::File& f);//Load image from PNG-file
	void LoadJPG(const papki::File& f);//Load image from JPG-file
//	void LoadTGA(papki::File& f);//Load image from TGA-file

	void Load(const papki::File& f);

private:

};//~class Image



}//~namespace
