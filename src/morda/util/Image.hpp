/* The MIT License:

Copyright (c) 2008-2012 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <ting/types.hpp>
#include <ting/fs/File.hpp>

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

	class Exc : public ting::Exc{
	public:
		Exc(const std::string& msg) :
				ting::Exc(msg.c_str())
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

	Image(const ting::fs::File& f){
		this->Load(f);
	}

	~Image();

	const Vec2ui& Dim()const NOEXCEPT{
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

	ting::Buffer<std::uint8_t> Buf(){
		return this->buf;
	}

	ting::Buffer<const std::uint8_t> Buf()const{
		return this->buf;
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

	void LoadPNG(const ting::fs::File& f);//Load image from PNG-file
	void LoadJPG(const ting::fs::File& f);//Load image from JPG-file
//	void LoadTGA(ting::fs::File& f);//Load image from TGA-file

	void Load(const ting::fs::File& f);

private:

};//~class Image



}//~namespace
