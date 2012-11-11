#pragma once

typedef struct
{
	unsigned char Header[12];									// TGA File Header
} TGAHeader;


typedef struct
{
	unsigned char		header[6];								// First 6 Useful Bytes From The Header
	unsigned int		bytesPerPixel;							// Holds Number Of Bytes Per Pixel Used In The TGA File
	unsigned int		imageSize;								// Used To Store The Image Size When Setting Aside Ram
	unsigned int		temp;									// Temporary Variable
	unsigned int		type;	
	unsigned int		height;									// Height of Image
	unsigned int		width;									// Width ofImage
	unsigned int		bpp;									// Bits Per Pixel
} TGA;