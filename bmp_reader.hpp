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
// Не нужно добавлять те include, которые в этом файле не используются
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>

#pragma pack(push, 1) // for alignment in the data
// Не следует делать классами то, к чему пользователь не будет иметь непосредственного доступа. 
// Пользовтель будет работать с классом BMPFile, который внутри себя содержит все эти, только вот 
// так как они приватные, то не нужно для них делать инкапсуляцию и мучиться с реализацией ненужных вещей
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
	BMPFile(BMPFile& p);
	BMPFile(const std::string& filename);

	void readBMP(const std::string& filename);
	void writeBMP(const std::string& filename);

	BMPFile rotateRight();
	BMPFile rotateLeft();

	unsigned int getHeight();
	unsigned int getWidth();
	unsigned int getBitsPerPixel();
	unsigned int getDataSize();
	unsigned char* getData();
	BMPHeader getBmpHeader();
	DIBHeader getDibHeader();
	// Наличие таких сеттеров и неконстантныъ геттеров нарушает инкапсуляцию. Работу с Гауссом 
	// нужно сделать либо как метод этого класса либо как дружественный, чтобы по-настоящему
	// ограничить доступ к внутренним данным
	void setData(unsigned char* _data);

	void printInfo();
	void printData();
};

class RGBPixel {
private:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
public:
	RGBPixel();
	RGBPixel(const unsigned char& _red, const unsigned char& _green, const unsigned char& _blue);
	// Вот поэтому это не нужно делать классом. Будь эта структурой, никакие из этих методов не были бы нужны
	unsigned char getRed();
	unsigned char getGreen();
	unsigned char getBlue();

	void setRed(const unsigned char& _red);
	void setGreen(const unsigned char& _green);
	void setBlue(const unsigned char& _blue);
	void setAll(const unsigned char& _red, const unsigned char& _green, const unsigned char& _blue);
	void printPix();
};
// Можно в принципе агрегировать этот объект внутрь BMPFile класса, и добавить метод, который бы его использовал
class Gauss {
private:
	unsigned int kernelSize;
	double sigma;
	double** kernel;
public:
	Gauss();
	Gauss(const unsigned int& _kernelSize, const double& _sigma);

	void createGaussKernel();
	void printKernel();

	RGBPixel** rawToRGB(BMPFile& img);
	RGBPixel** applyConvolution(RGBPixel** img, unsigned int height, unsigned int width);
	unsigned char* RGBToRaw(RGBPixel** img, unsigned int height, unsigned int width, unsigned int dataSize, unsigned int bitsPerPixel);
	BMPFile computeBlur(BMPFile& img);

	double gaussFunc(int x, int y, double sigma);
};

#pragma pack(pop)


#endif
