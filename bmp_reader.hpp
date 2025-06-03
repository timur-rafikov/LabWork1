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

#include <string>
#include <vector>
#include <stdint.h>

#pragma pack(push, 1) // for alignment in the data

struct BMPHeader {
	uint16_t type;
    uint32_t fileSize;
    uint16_t reserved2;
    uint16_t reserved1;
    uint32_t pixelOffset;

	BMPHeader();
	BMPHeader(const BMPHeader& p);

	void printInfo();
};

struct DIBHeader {

	uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t colorPlanes;
    uint16_t bitsPerPixel;
    uint32_t BI_RGB;  //  (0 = none)
    uint32_t dataSize;
    int32_t pwidth;
    int32_t pheight;
    uint32_t colorsCount;
    uint32_t impColorsCount;

	DIBHeader();
	DIBHeader(const DIBHeader& p);

	void printInfo();
};

#pragma pack(pop)

struct RGBPixel {
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	RGBPixel();
	RGBPixel(const uint8_t& _red, const uint8_t& _green, const uint8_t& _blue);

	void printPix();
};

class BMPFile {
private:
	BMPHeader bmpHeader;
	DIBHeader dibHeader;
	std::vector<std::vector<RGBPixel>> data;
public:
	BMPFile();
	BMPFile(BMPFile& p);
	BMPFile(const std::string& filename);
	BMPFile(const BMPHeader& _bmphdr, const DIBHeader& _dibhdr, const std::vector<std::vector<RGBPixel>>& _data);

	void readBMP(const std::string& filename);
	void writeBMP(const std::string& filename);
	bool allocateMemory(int height, int width);

	BMPFile rotateRight();
	BMPFile rotateLeft();

	unsigned int getHeight();
	unsigned int getWidth();
	unsigned int getBitsPerPixel();
	unsigned int getDataSize();
	std::vector<std::vector<RGBPixel>> getData();
	BMPHeader getBmpHeader();
	DIBHeader getDibHeader();

	void printInfo();
	void printData();
};

struct Gauss {
	unsigned int kernelSize;
	double sigma;
	std::vector<std::vector<double>> kernel;

	Gauss();
	Gauss(const unsigned int& _kernelSize, const double& _sigma);

	void createGaussKernel();
	void printKernel();

	std::vector<std::vector<RGBPixel>> applyConvolution(const std::vector<std::vector<RGBPixel>>& img, unsigned int height, unsigned int width);
	BMPFile computeBlur(BMPFile& img);

	double gaussFunc(int x, int y, double sigma);
};




#endif