// Author: Ivan Gagis <igagis@gmail.com>

// File description:
//	Texture font class

#include <algorithm>

#include <ting/debug.hpp>

#include "TexFont.hpp"
#include "Image.hpp"

//freetype includes
#include <ft2build.h>
#include FT_FREETYPE_H



using namespace morda;



namespace{

static void BlitIfGreater(Image& dst, unsigned dstChan, const Image& src, unsigned srcChan, unsigned x, unsigned y){
	ASSERT(dst.Buf().Size())
	ASSERT(dstChan < dst.NumChannels())
	ASSERT(srcChan < src.NumChannels())

	unsigned blitAreaW = std::min(src.Width(), dst.Width() - x);
	unsigned blitAreaH = std::min(src.Height(), dst.Height() - y);

	for(unsigned j = 0; j < blitAreaH; ++j){
		for(unsigned i = 0; i < blitAreaW; ++i){
			ting::u8 &d = dst.PixChan(i + x, j + y, dstChan);
			ting::u8 s = src.PixChan(i, j, srcChan);
			if(s > d){
				d = s;
			}
		}
	}
}


static unsigned FindNextPowOf2(unsigned n){
	unsigned ret = 1;
	while(ret <= n){
		ret *= 2;
	}
	return ret;
}

const unsigned DXGap = 1;
const unsigned DYGap = 1;



}//~namespace



void TexFont::Load(ting::fs::File& fi, const wchar_t* chars, unsigned size, unsigned outline){
//	TRACE(<< "TexFont::Load(): enter" << std::endl)

	this->glyphs.clear();//clear glyphs map if some other font was loaded previously

	this->fontSize = float(size);
	
	class FreeTypeLibWrapper{
		FT_Library lib;// handle to freetype library object
	public:
		inline FreeTypeLibWrapper(){
			if(FT_Init_FreeType(&this->lib)){
				throw ting::Exc("TexFont::Load(): unable to init freetype library");
			}
		}
		inline ~FreeTypeLibWrapper(){
			FT_Done_FreeType(this->lib);
		}
		inline operator FT_Library& (){
			return this->lib;
		}
	} library;

//	TRACE(<< "TexFont::Load(): FreeType library inited" << std::endl)

	class FreeTypeFaceWrapper{
		FT_Face face; // handle to face object
		ting::Array<ting::u8> fontFile;//the buffer should be alive as long as the Face is alive!!!
	public:
		FreeTypeFaceWrapper(FT_Library& lib, ting::fs::File& fi){
			this->fontFile = fi.LoadWholeFileIntoMemory();
			if(FT_New_Memory_Face(lib, this->fontFile.Begin(), this->fontFile.Size(), 0/* face_index */, &this->face) != 0){
				throw ting::Exc("TexFont::Load(): unable to crate font face object");
			}
		}
		inline ~FreeTypeFaceWrapper(){
			FT_Done_Face(this->face);
		}
		inline operator FT_Face& (){
			return this->face;
		}
	} face(library, fi);

//	TRACE(<< "TexFont::Load(): FreeType font face loaded" << std::endl)

	//set character size in pixels
	{
		FT_Error error = FT_Set_Pixel_Sizes(
				face,// handle to face object
				0,// pixel_width (0 means "same as height")
				size
			); // pixel_height

		if(error != 0){
			throw ting::Exc("TexFont::Load(): unable to set char size");
		}
	}

	unsigned maxTexSize;
	{
		GLint val;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
		ASSERT(val > 0)
		maxTexSize = unsigned(val);
	}

	//guess for texture width
	unsigned texWidth;
	{
		unsigned numChars = 0;
		for(const wchar_t* c = chars; *c != 0; ++c)
			++numChars;

		texWidth = std::max(unsigned(128), FindNextPowOf2((numChars / 8) * size)); //divide by 8 is a good guess that all font characters will be placed in 8 rows on texture
		texWidth = std::min(std::min(maxTexSize, unsigned(1024)), texWidth);//clamp width to min of max texture size and 1024
	}

	unsigned curTexHeight = FindNextPowOf2(size);//first guess of texture height
	unsigned curX = DXGap;
	unsigned curY = DYGap;
	unsigned maxHeightInRow = 0;

	Image texImg(texWidth, curTexHeight, Image::GREYA);
	//clear the image because image buffer may contain trash data
	//and glyphs will have artifacts on their edges
	texImg.Clear();

	//init bounding box to zero
	this->boundingBox.SetToZero();

//	TRACE(<< "TexFont::Load(): entering for loop" << std::endl)

	//print all the glyphs to the image
	for(unsigned i = 0; chars[i] != 0; ++i){
		if(FT_Load_Char(static_cast<FT_Face&>(face), FT_ULong(chars[i]), FT_LOAD_RENDER) != 0){
			throw ting::Exc("TexFont::Load(): unable to load char");
		}

//		TRACE(<< "TexFont::Load(): char loaded" << std::endl)

		FT_GlyphSlot slot = static_cast<FT_Face&>(face)->glyph;

		if(!slot->bitmap.buffer){//if glyph is empty (e.g. space character)
			Glyph &g = this->glyphs[chars[i]];
			g.advance = float(slot->metrics.horiAdvance) / (64.0f);
			ASSERT(g.verts.Size() == g.texCoords.Size())
			for(unsigned i = 0; i < g.verts.Size(); ++i){
				g.verts[i].SetToZero();
				g.texCoords[i].SetToZero();
			}
			continue;
		}
		Image glyphim(slot->bitmap.width, slot->bitmap.rows, Image::GREY, slot->bitmap.buffer);

		Image im(glyphim.Width() + 2 * outline, glyphim.Height() + 2 * outline, Image::GREYA);
		im.Clear();
		if(outline == 0){
			im.Blit(0, 0, glyphim, 1, 0);
		}else{
			im.Blit(outline, outline, glyphim, 0, 0);

			for(unsigned y = 0; y < 2 * outline + 1; ++y){
				for(unsigned x = 0; x < 2 * outline + 1; ++x){
					int dx = int(x) - int(outline);
					int dy = int(y) - int(outline);
					if(ting::math::Pow2(dx) + ting::math::Pow2(dy) <= int(ting::math::Pow2(outline))){
//					if(ting::Abs(dx) + ting::Abs(dy) <= int(outline)){
						BlitIfGreater(im, 1, glyphim, 0, x, y);
					}
				}
			}
		}

//		TRACE(<< "TexFont::Load(): glyph image created" << std::endl)

		if(texImg.Width() < curX + im.Width() + DXGap){
			curX = DXGap;
			curY += maxHeightInRow + DYGap;
			maxHeightInRow = 0;
		}

		if(texImg.Height() < curY + im.Height() + DYGap){
			//grow texture size
			unsigned newHeight = FindNextPowOf2(curY + im.Height() + DYGap);
			if(newHeight > maxTexSize){//if it is impossible to grow texture image height anymore
				throw ting::Exc("TexFont::Load(): there's not enough room on the texture for all the glyphs of the requested size");
			}

			//TODO: optimize somehow?
			//resize texture image
			Image copy(texImg);
			texImg.Init(copy.Width(), newHeight, copy.Type());
			texImg.Clear();
			texImg.Blit(0, 0, copy);
		}

		if(im.Height() > maxHeightInRow){
			maxHeightInRow = im.Height();
		}

		//record glyph information
		{
			FT_Glyph_Metrics *m = &slot->metrics;
			
			Glyph &g = this->glyphs[chars[i]];
			g.advance = float(m->horiAdvance) / (64.0f);

			ASSERT(outline < (unsigned(-1) >> 1))
			g.verts[0] = (tride::Vec2f(float(m->horiBearingX), float(m->horiBearingY - m->height)) / (64.0f)) + tride::Vec2f(-int(outline), -int(outline));
			g.verts[1] = (tride::Vec2f(float(m->horiBearingX), float(m->horiBearingY)) / (64.0f)) + tride::Vec2f(-int(outline), int(outline));
			g.verts[2] = (tride::Vec2f(float(m->horiBearingX + m->width), float(m->horiBearingY)) / (64.0f)) + tride::Vec2f(int(outline), int(outline));
			g.verts[3] = (tride::Vec2f(float(m->horiBearingX + m->width), float(m->horiBearingY - m->height)) / (64.0f)) + tride::Vec2f(int(outline), -int(outline));

			g.texCoords[0] = tride::Vec2f(float(curX), float(curY + im.Height()));
			g.texCoords[1] = tride::Vec2f(float(curX), float(curY));
			g.texCoords[2] = tride::Vec2f(float(curX + im.Width()), float(curY));
			g.texCoords[3] = tride::Vec2f(float(curX + im.Width()), float(curY + im.Height()));

			//update bounding box if needed
			if(this->boundingBox.left < -g.verts[0].x){
				this->boundingBox.left = -g.verts[0].x;
			}

			if(this->boundingBox.right < g.verts[2].x){
				this->boundingBox.right = g.verts[2].x;
			}

			if(this->boundingBox.bottom < -g.verts[0].y){
				this->boundingBox.bottom = -g.verts[0].y;
			}

			if(this->boundingBox.top < g.verts[2].y){
				this->boundingBox.top = g.verts[2].y;
			}

			ASSERT(this->boundingBox.Width() >= 0)
			ASSERT(this->boundingBox.Height() >= 0)
		}

		texImg.Blit(curX, curY, im);
		curX += im.Width() + DXGap;
	}//~for(i)

//	TRACE(<< "TexFont::Load(): for loop finished" << std::endl)

	//if there is only one row of characters on the texture image and it does not reach the right edge
	//of the image, we can shrink the image, possibly.
	if(curY == 0){
		//TODO: optimize somehow?
		//resize
		Image copy(texImg);
		texImg.Init(FindNextPowOf2(curX), copy.Height(), copy.Type());
		texImg.Clear();
		texImg.Blit(0, 0, copy);
	}

	//fill luminance channel with 0xff
	if(outline == 0){
		texImg.Clear(0, 0xff);
	}

	//now the font image has its final width and heights (no more resizes will be done)

	//normalize texture coordinates
	for(T_GlyphsIter i = this->glyphs.begin(); i != this->glyphs.end(); ++i){
		for(unsigned j = 0; j < i->second.texCoords.Size(); ++j){
			i->second.texCoords[j].x /= texImg.Width();
			i->second.texCoords[j].y /= texImg.Height();
		}
	}

//	TRACE(<< "TexFont::Load(): initing texture" << std::endl)
	this->tex.Init(texImg);
}



#ifdef DEBUG
void TexFont::RenderTex(TexturingShader& shader, const tride::Matr4f& matrix)const{
	tride::Matr4f matr(matrix);
	matr.Scale(this->tex.Dim());
	shader.SetMatrix(matr);

	this->tex.Bind();

//	s.EnablePositionPointer();
//	s.EnableTexCoordPointer();
//	s.SetPositionPointer(this->verts.Buf());
//	s.SetTexCoordPointer(this->texCoords.Buf());
//	ASSERT(this->verts.Size() == this->texCoords.Size())
//	shader.DrawArrays(GL_TRIANGLE_FAN, this->verts.Size());
	shader.DrawQuad01();
}
#endif
