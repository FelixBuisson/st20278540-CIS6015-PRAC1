// TGA Loader - 16/11/04 Codehead
//this is very much from the original code base and is effectivley used as a black box to load TGA image files

#pragma once

#include <iostream>
#include <fstream>
#include <memory.h>

#define IMG_OK              0x1
#define IMG_ERR_NO_FILE     0x2
#define IMG_ERR_MEM_FAIL    0x4
#define IMG_ERR_BAD_FORMAT  0x8
#define IMG_ERR_UNSUPPORTED 0x40

class cTGAImg
{
public:
	cTGAImg();
	~cTGAImg();
	int Save(char* _filename, short int _width, short int _height, unsigned char _pixelDepth, unsigned char* _imageData);
	int Load(char* _szFilename);
	int GetBPP();
	int GetWidth();
	int GetHeight();
	unsigned char* GetImg();       // Return a pointer to image data
	unsigned char* GetPalette();   // Return a pointer to VGA palette

private:
	short int m_iWidth, m_iHeight, m_iBPP;
	unsigned long m_lImageSize;
	char m_bEnc;
	unsigned char* m_pImage, * m_pPalette, * m_pData;

	// Internal workers
	int ReadHeader();
	int LoadRawData();
	int LoadTgaRLEData();
	int LoadTgaPalette();
	void BGRtoRGB();
	void FlipImg();
};