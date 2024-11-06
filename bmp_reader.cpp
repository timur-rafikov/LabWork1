/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#include "bmp_reader.hpp"

/*
	I read whether it is worth making several cpp files, 
	since there are quite a lot of functions. I have decided that I will not do this, 
	as it may complicate the development.

	Therefore, this file contains the implementation of the 
	BMPFile, BMPHeader and DIBHeader methods in order.
*/

// BMPFile

BMPFile::BMPFile() {
	bmpHeader = BMPHeader();
	dibHeader = DIBHeader();
}

BMPFile::BMPFile(BMPFile& img) :
	bmpHeader(img.getBmpHeader()),
	dibHeader(img.getDibHeader())
{}

BMPFile::BMPFile(const std::string& filename) {
	readBMP(filename);
}

void BMPFile::readBMP(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file\n";
		return; 
	}

	std::cout << "Reading the BMP file ...\n";

	std::size_t fsize = std::filesystem::file_size(filename); // reading the amount of data
	std::cout << "TASK 1: BMP file size: " << fsize << " bytes\n\n";
	
	// Reading Headers
	file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

	// Reading Pixel Data
	data = new unsigned char[dibHeader.getDataSize()];

	file.seekg(bmpHeader.getPixelOffset(), std::ios::beg);
	file.read(reinterpret_cast<char*>(data), dibHeader.getDataSize());

	file.close();

	std::cout << "TASK 2: The BMP file is uploaded!\n\n";
}

BMPFile BMPFile::rotateRight() {
	BMPFile res;
	res.bmpHeader = BMPHeader(bmpHeader);
	res.dibHeader = DIBHeader(dibHeader);

	res.dibHeader.setWidth(dibHeader.getHeight()); // swapping height and width
	res.dibHeader.setHeight(dibHeader.getWidth());
	res.dibHeader.setPWidth(dibHeader.getPHeight());
	res.dibHeader.setPHeight(dibHeader.getPWidth());

	res.data = new unsigned char[dibHeader.getDataSize()];

	unsigned int h = dibHeader.getHeight(), w = dibHeader.getWidth();
	unsigned int cntBytePix = dibHeader.getBitsPerPixel() / 8;
	unsigned char tmp[h][w * cntBytePix];
	/*
		Next comes the pixel data conversion.
		I sat with a piece of paper for a very long time 
		and thought about how to expand the bytes carefully.

		And in the end, I couldn't think of anything better than 
		to first bring the pixel data into a two-dimensional array (as in the picture), and 
		then rewrite this data into an rotated view.

		Since the bmp file has the property of data alignment, 
		we need to process the zeros at the end of each line separately.
		Next, I count the number of zeros that will be written (this is just the inverse remainder 
		of the number of bytes in a string modulo 4), skip them and write the data further
	*/
	unsigned int countOfByte = cntBytePix * w;
	unsigned int countOfNull = ((4 - (countOfByte % 4)) % 4);
	
	unsigned int curDataIndex = 0;
	int i = h - 1, j = 0;
	while (i >= 0) {
		while (j < (int)countOfByte) {
			for (uint el = 0; el < cntBytePix; ++el) {
				tmp[i][j + cntBytePix - el - 1] = data[curDataIndex + el]; 
			}
			j += cntBytePix;
			curDataIndex += cntBytePix;
		}
		curDataIndex += countOfNull;

		i--;
		j = 0;
	}

	/*
		I write data in a similar way. To do this, you need to count the number of 
		zeros separately and write them at the end of each line
	*/

	unsigned int countOfByte2 = cntBytePix * h;
	unsigned int countOfNull2 = ((4 - (countOfByte2 % 4)) % 4);
	char null = 0;

	curDataIndex = 0;
	i = h - 1;
	j = w * cntBytePix - 1;
	while (j >= 0) {
		while (i >= 0) {
			for (uint el = 0; el < cntBytePix; ++el) {
				res.data[curDataIndex + el] = tmp[i][j - el];
			}
			i--;
			curDataIndex += cntBytePix;
		}

		for (uint el = 0; el < countOfNull2; ++el)
			res.data[curDataIndex++] = null;

		j -= cntBytePix;
		i = h - 1;
	}

	return res;
}

BMPFile BMPFile::rotateLeft() {
	BMPFile res = rotateRight().rotateRight().rotateRight();
	return res;
}

void BMPFile::writeBMP(const std::string& filename) {
	std::ofstream file(filename, std::ios::out | std::ios::binary);

	file.write(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.write(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));
	file.write(reinterpret_cast<char*>(data), dibHeader.getDataSize());

	file.close();
}

void BMPFile::setData(unsigned char* _data) {
	data = _data;
}

void BMPFile::printInfo() {
	bmpHeader.printInfo();
	dibHeader.printInfo();
}

void BMPFile::printData() {
	/*for (unsigned int i = 0; i < dibHeader.getDataSize(); ++i) {
		if (i % 16 == 0)
			printf("\n%04x: ", i);
		printf("%02x ", data[i]);
	}*/
	for (uint i = 0; i < dibHeader.getDataSize(); ++i) {
		printf("%02x ", data[i]);
	}
}

unsigned int BMPFile::getHeight() {
	return dibHeader.getHeight();
}

unsigned int BMPFile::getWidth() {
	return dibHeader.getWidth();
}

unsigned int BMPFile::getBitsPerPixel() {
	return dibHeader.getBitsPerPixel();
}

unsigned char* BMPFile::getData() {
	return data;
}

unsigned int BMPFile::getDataSize() {
	return dibHeader.getDataSize();
}

BMPHeader BMPFile::getBmpHeader() {
	return bmpHeader;
}

DIBHeader BMPFile::getDibHeader() {
	return dibHeader;
}

// BMPHeader

BMPHeader::BMPHeader() {
	ID[0] = 0;
	ID[1] = 1;
	fileSize = 0;
	for (int i = 0; i < 4; ++i)
		unused[i] = 0;
	pixelOffset = 0;
}

BMPHeader::BMPHeader(const BMPHeader& p) {
	ID[0] = p.ID[0];
	ID[1] = p.ID[1];
	fileSize = p.fileSize;
	for (int i = 0; i < 4; ++i)
		unused[i] = p.unused[i];
	pixelOffset = p.pixelOffset;
}

void BMPHeader::printInfo() {
	std::cout << "ID: " << ' ' << ID[0] << ' ' << ID[1] << '\n';
	std::cout << "fileSize: " << fileSize << '\n';
	std::cout << "unused: " << unused[0] << ' ' << unused[1] << ' ' << unused[2] << ' ' << unused[3] << '\n';
	std::cout << "pixelOffset: " << pixelOffset << '\n';
	std::cout << '\n';
}

unsigned int BMPHeader::getPixelOffset() {
	return pixelOffset;
}



//DIBHeader

DIBHeader::DIBHeader() {
	headerSize = 0;
	width = 0;
	height = 0;
	colorPlanes = 0;
	bitsPerPixel = 0;
	BI_RGB = 0;
	dataSize = 0;
	pwidth = 0;
	pheight = 0;
	colorsCount = 0;
	impColorsCount = 0;
}

DIBHeader::DIBHeader(const DIBHeader& p) {
	headerSize = p.headerSize;
	width = p.width;
	height = p.height;
	colorPlanes = p.colorPlanes;
	bitsPerPixel = p.bitsPerPixel;
	BI_RGB = p.BI_RGB;
	dataSize = p.dataSize;
	pwidth = p.pwidth;
	pheight = p.pheight;
	colorsCount = p.colorsCount;
	impColorsCount = p.impColorsCount;
}

void DIBHeader::printInfo() {
	std::cout << "headerSize: " << headerSize << '\n';
	std::cout << "width: " << width << '\n';
	std::cout << "height: " << height << '\n';
	std::cout << "colorPlanes: " << colorPlanes << '\n';
	std::cout << "bitsPerPixel: " << bitsPerPixel << '\n';
	std::cout << "BI_RGB: " << BI_RGB << '\n';
	std::cout << "dataSize: " << dataSize << '\n';
	std::cout << "pwidth: " << pwidth << '\n';
	std::cout << "pheight: " << pheight << '\n';
	std::cout << "colorsCount: " << colorsCount << '\n';
	std::cout << "impColorsCount: " << impColorsCount << '\n';
	std::cout << '\n';
}

unsigned int DIBHeader::getDataSize() {
	return dataSize;
}

unsigned int DIBHeader::getWidth() {
	return width;
}

unsigned int DIBHeader::getHeight() {
	return height;
}

unsigned int DIBHeader::getPWidth() {
	return pwidth;
}

unsigned int DIBHeader::getPHeight() {
	return pheight;
}

unsigned int DIBHeader::getBitsPerPixel() {
	return bitsPerPixel;
}

void DIBHeader::setWidth(const unsigned int& newWidth) {
	width = newWidth;
}

void DIBHeader::setHeight(const unsigned int& newHeight) {
	height = newHeight;
}

void DIBHeader::setPWidth(const unsigned int& newPWidth) {
	pwidth = newPWidth;
}

void DIBHeader::setPHeight(const unsigned int& newPHeight) {
	pheight = newPHeight;
}



// RGBPixel

RGBPixel::RGBPixel() :
	red(0),
	green(0),
	blue(0)
{}

RGBPixel::RGBPixel(const unsigned char& _red, const unsigned char& _green, const unsigned char& _blue) :
	red(_red),
	green(_green),
	blue(_blue)
{}


unsigned char RGBPixel::getRed() {
	return red;
}

unsigned char RGBPixel::getGreen() {
	return green;
}

unsigned char RGBPixel::getBlue() {
	return blue;
}

void RGBPixel::setRed(const unsigned char& _red) {
	red = _red;
}

void RGBPixel::setGreen(const unsigned char& _green) {
	green = _green;
}

void RGBPixel::setBlue(const unsigned char& _blue) {
	blue = _blue;
}

void RGBPixel::setAll(const unsigned char& _red, const unsigned char& _green, const unsigned char& _blue) {
	red = _red;
	green = _green;
	blue = _blue;
}

void RGBPixel::printPix() {
	printf("%02x ", red);
	printf("%02x ", green);
	printf("%02x ", blue);
}


// Gauss

Gauss::Gauss() :
	kernelSize(0),
	sigma(0),
	kernel(0)
{}

Gauss::Gauss(const unsigned int& _kernelSize, const double& _sigma) :
	kernelSize(_kernelSize),
	sigma(_sigma),
	kernel(0)
{}

void Gauss::createGaussKernel() {
	kernel = new double*[kernelSize];
	for (uint i = 0; i < kernelSize; ++i)
		kernel[i] = new double[kernelSize];


	int radius = kernelSize / 2;

	double sum = 0.0;
	for (int y = -radius; y <= radius; ++y) {
		for (int x = -radius; x <= radius; ++x) {
			double value = gaussFunc(x, y, sigma);
			sum += value;
			kernel[y + radius][x + radius] = value;
		}
	}

	for (uint x = 0; x < kernelSize; ++x) {
		for (uint y = 0; y < kernelSize; ++y) {
			kernel[x][y] /= sum;
		}
	}
}

double Gauss::gaussFunc(int x, int y, double sigma) {
	return ((1. / (2 * M_PI * sigma * sigma)) * exp(-((x * x + y * y) / (2 * sigma * sigma))));
}

void Gauss::printKernel() {
	for (uint i = 0; i < kernelSize; ++i) {
		for (uint j = 0; j < kernelSize; ++j) {
			std::cout << kernel[i][j] << ' ';
		}
		std::cout << '\n';
	}
}

RGBPixel** Gauss::rawToRGB(BMPFile& img) {
	unsigned char* data = img.getData();

	unsigned int h = img.getHeight(), w = img.getWidth();
	unsigned int cntBytePix = img.getBitsPerPixel() / 8;
	unsigned int countOfByte = cntBytePix * w;
	unsigned int countOfNull = ((4 - (countOfByte % 4)) % 4);

	RGBPixel** res = new RGBPixel*[h];
	for (uint i = 0; i < h; ++i)
		res[i] = new RGBPixel[w];

	unsigned int curDataIndex = 0;
	int ri = h - 1, rj = 0;
	while (ri >= 0) {
		while (rj < (int)w) {
			res[ri][rj].setBlue(data[curDataIndex]);
			res[ri][rj].setGreen(data[curDataIndex + 1]);
			res[ri][rj].setRed(data[curDataIndex + 2]);

			rj++;
			curDataIndex += cntBytePix;
		}
		curDataIndex += countOfNull;

		ri--;
		rj = 0;
	}

	return res;
}

RGBPixel** Gauss::applyConvolution(RGBPixel** img, unsigned int height, unsigned int width) {
	unsigned int h = height;
	unsigned int w = width;

	RGBPixel** res = new RGBPixel*[h];
	for (uint i = 0; i < h; ++i)
		res[i] = new RGBPixel[w];

	int radius = kernelSize / 2;

	for (uint x = 0; x < h; ++x) {
		for (uint y = 0; y < w; ++y) {
			int sumR = 0;
			int sumG = 0;
			int sumB = 0;

			for (uint kx = 0; kx < kernelSize; ++kx) {
				for (uint ky = 0; ky < kernelSize; ++ky) {

					int pixelX = (int)x - radius + (int)kx;
					int pixelY = (int)y - radius + (int)ky;

					if (pixelX < 0)
						pixelX = -pixelX;
					else if (pixelX >= (int)h)
						pixelX = (int)h - (pixelX - (int)h) - 1;

					if (pixelY < 0)
						pixelY = -pixelY;
					else if (pixelY >= (int)w)
						pixelY = (int)w - (pixelY - (int)w) - 1;

					sumR += img[pixelX][pixelY].getRed();
					sumG += img[pixelX][pixelY].getGreen();
					sumB += img[pixelX][pixelY].getBlue();
				}
			}

			unsigned char newRed = std::min(std::max(sumR, 0), 255);
			unsigned char newGreen = std::min(std::max(sumG, 0), 255);
			unsigned char newBlue = std::min(std::max(sumB, 0), 255);

			res[x][y].setAll(newRed, newGreen, newBlue);
		}
	}

	return res;
}

unsigned char* Gauss::RGBToRaw(RGBPixel** img, unsigned int height, unsigned int width, unsigned int dataSize, unsigned int bitsPerPixel) {
	unsigned char* data = new unsigned char[dataSize];

	unsigned int h = height, w = width;
	unsigned int cntBytePix = bitsPerPixel / 8;
	unsigned int countOfByte = cntBytePix * w;
	unsigned int countOfNull = ((4 - (countOfByte % 4)) % 4);

	unsigned int curDataIndex = 0;
	int i = h - 1;
	int j = 0;
	while (i >= 0) {
		while (j < (int)w) {
			data[curDataIndex++] = img[i][j].getBlue();
			data[curDataIndex++] = img[i][j].getGreen();
			data[curDataIndex++] = img[i][j].getRed();

			j++;
		}
		for (uint el = 0; el < countOfNull; ++el)
			data[curDataIndex++] = 0;

		i--;
		j = 0;
	}

	return data;
}

BMPFile Gauss::computeBlur(BMPFile& img) {
	BMPFile res(img);
	
	RGBPixel** rgbarr = rawToRGB(img);
	RGBPixel** convarr = applyConvolution(rgbarr, img.getHeight(), img.getWidth());
	unsigned char* convdata = RGBToRaw(convarr, img.getHeight(), img.getWidth(), img.getDataSize(), img.getBitsPerPixel());

	res.setData(convdata);

	return res;
}