#include "cTGAImg.h"



cTGAImg::cTGAImg()
{
	m_pImage = m_pPalette = m_pData = NULL;
	m_iWidth = m_iHeight = m_iBPP = m_bEnc = 0;
	m_lImageSize = 0;
}


cTGAImg::~cTGAImg()
{
	if (m_pImage)
	{
		delete[] m_pImage;
		m_pImage = NULL;
	}

	if (m_pPalette)
	{
		delete[] m_pPalette;
		m_pPalette = NULL;
	}

	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
}

// saves an array of pixels as a TGA image
int cTGAImg::Save(char* _filename, short int _width, short int _height, unsigned char _pixelDepth, unsigned char* _imageData)
{
	unsigned char cGarbage = 0, type, mode, aux;
	short int iGarbage = 0;
	int i;
	FILE* file;
	errno_t err;

	if ((err = fopen_s(&file, _filename, "wb")) != 0)
	{
		return(IMG_ERR_NO_FILE);
	}

	// compute image type: 2 for RGB(A), 3 for greyscale
	mode = _pixelDepth / 8;
	if ((_pixelDepth == 24) || (_pixelDepth == 32))
		type = 2;
	else
		type = 3;

	// write the header
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	fwrite(&type, sizeof(unsigned char), 1, file);

	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);

	fwrite(&_width, sizeof(short int), 1, file);
	fwrite(&_height, sizeof(short int), 1, file);
	fwrite(&_pixelDepth, sizeof(unsigned char), 1, file);

	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	// convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
		for (i = 0; i < _width * _height * mode; i += mode) {
			aux = _imageData[i];
			_imageData[i] = _imageData[i + 2];
			_imageData[i + 2] = aux;
		}

	// save the image data
	fwrite(_imageData, sizeof(unsigned char), _width * _height * mode, file);
	fclose(file);
	file = NULL;
	// release the memory
	free(_imageData);

	return(IMG_OK);
}



int cTGAImg::Load(char* _szFilename)
{
	using namespace std;
	ifstream fIn;
	unsigned long ulSize;
	int iRet;

	// Clear out any existing image and palette
	if (m_pImage)
	{
		delete[] m_pImage;
		m_pImage = NULL;
	}

	if (m_pPalette)
	{
		delete[] m_pPalette;
		m_pPalette = NULL;
	}

	// Open the specified file
	fIn.open(_szFilename, ios::binary);

	if (!fIn)//==NULL)
		return IMG_ERR_NO_FILE;

	// Get file size
	fIn.seekg(0, ios_base::end);
	ulSize = fIn.tellg();
	fIn.seekg(0, ios_base::beg);

	// Allocate some space
	// Check and clear pDat, just in case
	if (m_pData)
		delete[] m_pData;

	m_pData = new unsigned char[ulSize];

	if (m_pData == NULL)
	{
		fIn.close();
		return IMG_ERR_MEM_FAIL;
	}

	// Read the file into memory
	fIn.read((char*)m_pData, ulSize);

	fIn.close();

	// Process the header
	iRet = ReadHeader();

	if (iRet != IMG_OK)
		return iRet;

	switch (m_bEnc)
	{
	case 1: // Raw Indexed
	{
		// Check filesize against header values
		if ((m_lImageSize + 18 + m_pData[0] + 768) > ulSize)
			return IMG_ERR_BAD_FORMAT;

		// Double check image type field
		if (m_pData[1] != 1)
			return IMG_ERR_BAD_FORMAT;

		// Load image data
		iRet = LoadRawData();
		if (iRet != IMG_OK)
			return iRet;

		// Load palette
		iRet = LoadTgaPalette();
		if (iRet != IMG_OK)
			return iRet;

		break;
	}

	case 2: // Raw RGB
	{
		// Check filesize against header values
		if ((m_lImageSize + 18 + m_pData[0]) > ulSize)
			return IMG_ERR_BAD_FORMAT;

		// Double check image type field
		if (m_pData[1] != 0)
			return IMG_ERR_BAD_FORMAT;

		// Load image data
		iRet = LoadRawData();
		if (iRet != IMG_OK)
			return iRet;

		BGRtoRGB(); // Convert to RGB
		break;
	}

	case 9: // RLE Indexed
	{
		// Double check image type field
		if (m_pData[1] != 1)
			return IMG_ERR_BAD_FORMAT;

		// Load image data
		iRet = LoadTgaRLEData();
		if (iRet != IMG_OK)
			return iRet;

		// Load palette
		iRet = LoadTgaPalette();
		if (iRet != IMG_OK)
			return iRet;

		break;
	}

	case 10: // RLE RGB
	{
		// Double check image type field
		if (m_pData[1] != 0)
			return IMG_ERR_BAD_FORMAT;

		// Load image data
		iRet = LoadTgaRLEData();
		if (iRet != IMG_OK)
			return iRet;

		BGRtoRGB(); // Convert to RGB
		break;
	}

	default:
		return IMG_ERR_UNSUPPORTED;
	}

	// Check flip bit
  //   if((m_pData[17] & 0x20)==0) 
  //     FlipImg();

	// Release file memory
	delete[] m_pData;
	m_pData = NULL;

	return IMG_OK;
}


int cTGAImg::ReadHeader() // Examine the header and populate our class attributes
{
	short ColMapStart, ColMapLen;
	short x1, y1, x2, y2;

	if (m_pData == NULL)
		return IMG_ERR_NO_FILE;

	if (m_pData[1] > 1)    // 0 (RGB) and 1 (Indexed) are the only types we know about
		return IMG_ERR_UNSUPPORTED;

	m_bEnc = m_pData[2];     // Encoding flag  1 = Raw indexed image
	//                2 = Raw RGB
	//                3 = Raw greyscale
	//                9 = RLE indexed
	//               10 = RLE RGB
	//               11 = RLE greyscale
	//               32 & 33 Other compression, indexed

	if (m_bEnc > 11)       // We don't want 32 or 33
		return IMG_ERR_UNSUPPORTED;


	// Get palette info
	memcpy(&ColMapStart, &m_pData[3], 2);
	memcpy(&ColMapLen, &m_pData[5], 2);

	// Reject indexed images if not a VGA palette (256 entries with 24 bits per entry)
	if (m_pData[1] == 1) // Indexed
	{
		if (ColMapStart != 0 || ColMapLen != 256 || m_pData[7] != 24)
			return IMG_ERR_UNSUPPORTED;
	}

	// Get image window and produce _width & _height values
	memcpy(&x1, &m_pData[8], 2);
	memcpy(&y1, &m_pData[10], 2);
	memcpy(&x2, &m_pData[12], 2);
	memcpy(&y2, &m_pData[14], 2);

	m_iWidth = (x2 - x1);
	m_iHeight = (y2 - y1);

	if (m_iWidth < 1 || m_iHeight < 1)
		return IMG_ERR_BAD_FORMAT;

	// Bits per Pixel
	m_iBPP = m_pData[16];

	// Check flip / interleave byte
	if (m_pData[17] > 32) // Interleaved data
		return IMG_ERR_UNSUPPORTED;

	// Calculate image size
	m_lImageSize = (m_iWidth * m_iHeight * (m_iBPP / 8));

	return IMG_OK;
}


int cTGAImg::LoadRawData() // Load uncompressed image data
{
	short iOffset;

	if (m_pImage) // Clear old data if present
		delete[] m_pImage;

	m_pImage = new unsigned char[m_lImageSize];

	if (m_pImage == NULL)
		return IMG_ERR_MEM_FAIL;

	iOffset = m_pData[0] + 18; // Add header to ident field size

	if (m_pData[1] == 1) // Indexed images
		iOffset += 768;  // Add palette offset

	memcpy(m_pImage, &m_pData[iOffset], m_lImageSize);

	return IMG_OK;
}


int cTGAImg::LoadTgaRLEData() // Load RLE compressed image data
{
	short iOffset, iPixelSize;
	unsigned char* pCur;
	unsigned long Index = 0;
	unsigned char bLength, bLoop;

	// Calculate offset to image data
	iOffset = m_pData[0] + 18;

	// Add palette offset for indexed images
	if (m_pData[1] == 1)
		iOffset += 768;

	// Get pixel size in bytes
	iPixelSize = m_iBPP / 8;

	// Set our pointer to the beginning of the image data
	pCur = &m_pData[iOffset];

	// Allocate space for the image data
	if (m_pImage != NULL)
		delete[] m_pImage;

	m_pImage = new unsigned char[m_lImageSize];

	if (m_pImage == NULL)
		return IMG_ERR_MEM_FAIL;

	// Decode
	while (Index < m_lImageSize)
	{
		if (*pCur & 0x80) // Run length chunk (High bit = 1)
		{
			bLength = *pCur - 127; // Get run length
			pCur++;            // Move to pixel data  

			// Repeat the next pixel bLength times
			for (bLoop = 0; bLoop != bLength; ++bLoop, Index += iPixelSize)
				memcpy(&m_pImage[Index], pCur, iPixelSize);

			pCur += iPixelSize; // Move to the next descriptor chunk
		}
		else // Raw chunk
		{
			bLength = *pCur + 1; // Get run length
			pCur++;          // Move to pixel data

			// Write the next bLength pixels directly
			for (bLoop = 0; bLoop != bLength; ++bLoop, Index += iPixelSize, pCur += iPixelSize)
				memcpy(&m_pImage[Index], pCur, iPixelSize);
		}
	}

	return IMG_OK;
}


int cTGAImg::LoadTgaPalette() // Load a 256 color palette
{
	unsigned char bTemp;
	short iIndex, iPalPtr;

	// Delete old palette if present
	if (m_pPalette)
	{
		delete[] m_pPalette;
		m_pPalette = NULL;
	}

	// Create space for new palette
	m_pPalette = new unsigned char[768];

	if (m_pPalette == NULL)
		return IMG_ERR_MEM_FAIL;

	// VGA palette is the 768 bytes following the header
	memcpy(m_pPalette, &m_pData[m_pData[0] + 18], 768);

	// Palette entries are BGR ordered so we have to convert to RGB
	for (iIndex = 0, iPalPtr = 0; iIndex != 256; ++iIndex, iPalPtr += 3)
	{
		bTemp = m_pPalette[iPalPtr];               // Get Blue value
		m_pPalette[iPalPtr] = m_pPalette[iPalPtr + 2]; // Copy Red to Blue
		m_pPalette[iPalPtr + 2] = bTemp;             // Replace Blue at the end
	}

	return IMG_OK;
}


void cTGAImg::BGRtoRGB() // Convert BGR to RGB (or back again)
{
	unsigned long Index, nPixels;
	unsigned char* bCur;
	unsigned char bTemp;
	short iPixelSize;

	// Set ptr to start of image
	bCur = m_pImage;

	// Calc number of pixels
	nPixels = m_iWidth * m_iHeight;

	// Get pixel size in bytes
	iPixelSize = m_iBPP / 8;

	for (Index = 0; Index != nPixels; Index++)  // For each pixel
	{
		bTemp = *bCur;      // Get Blue value
		*bCur = *(bCur + 2);  // Swap red value into first position
		*(bCur + 2) = bTemp;  // Write back blue to last position

		bCur += iPixelSize; // Jump to next pixel
	}

}


void cTGAImg::FlipImg() // Flips the image vertically (Why store images upside down?)
{
	unsigned char bTemp;
	unsigned char* pLine1, * pLine2;
	int iLineLen, iIndex;

	iLineLen = m_iWidth * (m_iBPP / 8);
	pLine1 = m_pImage;
	pLine2 = &m_pImage[iLineLen * (m_iHeight - 1)];

	for (; pLine1 < pLine2; pLine2 -= (iLineLen * 2))
	{
		for (iIndex = 0; iIndex != iLineLen; pLine1++, pLine2++, iIndex++)
		{
			bTemp = *pLine1;
			*pLine1 = *pLine2;
			*pLine2 = bTemp;
		}
	}

}


int cTGAImg::GetBPP()
{
	return m_iBPP;
}


int cTGAImg::GetWidth()
{
	return m_iWidth;
}


int cTGAImg::GetHeight()
{
	return m_iHeight;
}


unsigned char* cTGAImg::GetImg()
{
	return m_pImage;
}


unsigned char* cTGAImg::GetPalette()
{
	return m_pPalette;
}