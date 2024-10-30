/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */


/* The BMPHeader and DIBHeader classes will be useless separately, 
 * so I decided to put them in the same header file with the BMPFile class, 
 * which will contain the main functions of working with the file.
*/

#ifndef BMP_READER
#define BMP_READER

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#pragma pack(push, 1) // for alignment in the data

class BMPHeader {
private:
	unsigned char ID[2];
	unsigned int fileSize;
	unsigned char unused[4];
	unsigned int pixelOffset;
public:
	BMPHeader();
	BMPHeader(const BMPHeader& p);

	void printInfo();

	unsigned int getPixelOffset();
};

class DIBHeader {
private:
	unsigned int headerSize;
	unsigned int width;
	unsigned int height;
	unsigned short colorPlanes;
	unsigned short bitsPerPixel;
	unsigned int BI_RGB;
	unsigned int dataSize;
	unsigned int pwidth;
	unsigned int pheight;
	unsigned int colorsCount;
	unsigned int impColorsCount;
public:
	DIBHeader();
	DIBHeader(const DIBHeader& p);

	void setWidth(const unsigned int& newWidth);
	void setHeight(const unsigned int& newHeight);
	void setPWidth(const unsigned int& newPWidth);
	void setPHeight(const unsigned int& newPHeight);

	void printInfo();

	unsigned int getDataSize();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getPWidth();
	unsigned int getPHeight();
	unsigned int getBitsPerPixel();
};

class BMPFile {
private:
	BMPHeader bmpHeader;
	DIBHeader dibHeader;
	unsigned char* data;
public:
	BMPFile();
	BMPFile(const std::string& filename);

	void readBMP(const std::string& filename);
	void writeBMP(const std::string& filename);

	BMPFile rotateRight();
	BMPFile rotateLeft();

	void printInfo();
	void printData();
};

#pragma pack(pop)


#endif