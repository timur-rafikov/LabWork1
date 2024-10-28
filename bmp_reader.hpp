/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#ifndef BMP_READER
#define BMP_READER

#include <fstream>
#include <filesystem>
#include <iostream>

#pragma pack(push, 1) // for alignment in the data

class BMPHeader {
private:
	unsigned char ID[2];
	unsigned int file_size;
	unsigned char unused[4];
	unsigned int pixel_offset;
public:
	void printInfo();

	unsigned int getPixelOffset();
};

class DIBHeader {
private:
	unsigned int header_size;
	unsigned int width;
	unsigned int height;
	unsigned short color_planes;
	unsigned short bits_per_pixel;
	unsigned int BI_RGB;
	unsigned int data_size;
	unsigned int pwidth;
	unsigned int pheight;
	unsigned int colors_count;
	unsigned int imp_colors_count;
public:
	void printInfo();

	unsigned int getDataSize();
	unsigned int getWidth();
	unsigned int getHeight();
};

class BMPFile {
private:
	BMPHeader bmpHeader;
	DIBHeader dibHeader;
	unsigned char* data;
public:
	void readBMP(const char* filename);

	void printInfo();
	void printData();
};

#pragma pack(pop)


#endif