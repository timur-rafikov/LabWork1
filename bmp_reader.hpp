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

#pragma pack(push, 1) // for alignment in the data

struct BMPHeader {
	unsigned char ID[2];
	unsigned int fileSize;
	unsigned char unused[4];
	unsigned int pixelOffset;

	BMPHeader();
	BMPHeader(const BMPHeader& p);

	void printInfo();
};

struct DIBHeader {
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

	DIBHeader();
	DIBHeader(const DIBHeader& p);

	void printInfo();
};

struct ArrayPixel;

class BMPFile {
private:
	BMPHeader bmpHeader;
	DIBHeader dibHeader;
	unsigned char* data;
public:
	BMPFile();
	BMPFile(BMPFile& p);
	BMPFile(const std::string& filename);
	BMPFile(const BMPHeader& _bmphdr, const DIBHeader& _dibhdr, const ArrayPixel& arr);
	~BMPFile();

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

	void printInfo();
	void printData();
};

struct RGBPixel {
	unsigned char red;
	unsigned char green;
	unsigned char blue;

	RGBPixel();
	RGBPixel(const unsigned char& _red, const unsigned char& _green, const unsigned char& _blue);

	void printPix();
};

struct ArrayPixel {
	unsigned int height;
	unsigned int width;
	RGBPixel** data;

	ArrayPixel(const uint& _height, const uint& _width);
	ArrayPixel(BMPFile& img);
	~ArrayPixel();

	//unsigned char* toRaw(unsigned int dataSize, unsigned int bitsPerPixel);
};

struct Gauss {
	unsigned int kernelSize;
	double sigma;
	double** kernel;

	Gauss();
	Gauss(const unsigned int& _kernelSize, const double& _sigma);
	~Gauss();

	void createGaussKernel();
	void printKernel();

	ArrayPixel applyConvolution(const ArrayPixel& img, unsigned int height, unsigned int width);
	BMPFile computeBlur(BMPFile& img);

	double gaussFunc(int x, int y, double sigma);
};



#pragma pack(pop)


#endif