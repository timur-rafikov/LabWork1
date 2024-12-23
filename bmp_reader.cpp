/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#include "bmp_reader.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
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

BMPFile::BMPFile(const std::string& filename) {
	readBMP(filename);
}

// Constructor for BMPFile that initializes from headers and pixel data
BMPFile::BMPFile(const BMPHeader& _bmphdr, const DIBHeader& _dibhdr, const std::vector<std::vector<RGBPixel>>& _data)
    : bmpHeader(_bmphdr), dibHeader(_dibhdr), data(_data) // Member initializer list
{
    // Ensure that the pixel data is valid
    if (data.size() != dibHeader.height || (data.size() > 0 && data[0].size() != dibHeader.width)) {
        throw std::invalid_argument("Pixel data dimensions do not match DIB header dimensions.");
    }

    // Optionally, you can perform additional validation or processing here
}


BMPFile::BMPFile(BMPFile& p) 
    : bmpHeader(p.bmpHeader), 
      dibHeader(p.dibHeader), 
      data(p.data) // Deep copy the pixel data
{
    // Ensure that we create a new vector for pixel data
    data.resize(p.dibHeader.height);
    for (size_t i = 0; i < p.dibHeader.height; ++i) {
        data[i].resize(p.dibHeader.width);
        std::copy(p.data[i].begin(), p.data[i].end(), data[i].begin());
    }
}


void BMPFile::readBMP(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if (!file) {
        throw std::runtime_error("Error opening file.");
    }

    // Read BMP header
    file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(BMPHeader));
    if (bmpHeader.ID[0] != 'B' || bmpHeader.ID[1] != 'M') {
        throw std::runtime_error("Not a valid BMP file.");
    }

    // Read DIB header
    file.read(reinterpret_cast<char*>(&dibHeader), sizeof(DIBHeader));

    // Output BMP file size
    std::cout << "TASK 1: BMP file size: " << bmpHeader.fileSize << " bytes\n\n";

    // Adjust height if it's negative (top-down DIB)
    dibHeader.height = abs(dibHeader.height);
    
    if (dibHeader.width == 0 || dibHeader.height == 0) {
        throw std::runtime_error("Unexpected image dimensions.");
    }

    // Move to pixel data offset
    file.seekg(bmpHeader.pixelOffset, std::ios::beg);

    // Initialize pixel data vector
    data.resize(dibHeader.height, std::vector<RGBPixel>(dibHeader.width));

    // Calculate padding for each row
    size_t rowSize = dibHeader.width * sizeof(RGBPixel);
    size_t padding = (4 - (rowSize % 4)) % 4;

    // Read pixel data row by row
    for (int i = dibHeader.height - 1; i >= 0; --i) { // BMP files are stored bottom-to-top
        file.read(reinterpret_cast<char*>(data[i].data()), rowSize);
        file.ignore(padding); // Skip padding bytes
        if (!file) {
            throw std::runtime_error("Error reading pixel data.");
        }
    }

    file.close();
    std::cout << "TASK 2: The BMP file is uploaded!\n\n";
}

BMPFile BMPFile::rotateRight() {
    // Get original dimensions
    int originalHeight = dibHeader.height;
    int originalWidth = dibHeader.width;

    // Create a new vector for the rotated image
    std::vector<std::vector<RGBPixel>> rotatedData(originalWidth, std::vector<RGBPixel>(originalHeight));

    // Rotate the image 90 degrees clockwise
    for (int i = 0; i < originalHeight; ++i) {
        for (int j = 0; j < originalWidth; ++j) {
            // Place pixel in new position
            rotatedData[j][originalHeight - 1 - i] = data[i][j];
        }
    }

    // Update DIB header for new dimensions
    DIBHeader newDibHeader = dibHeader;
    newDibHeader.width = originalHeight;
    newDibHeader.height = originalWidth;

    // Calculate padding for the new width
    unsigned int paddingSize = (4 - (newDibHeader.width * sizeof(RGBPixel)) % 4) % 4;
    newDibHeader.dataSize = (newDibHeader.width * sizeof(RGBPixel) + paddingSize) * newDibHeader.height;

    // Update BMP header file size
    BMPHeader newBmpHeader = bmpHeader;
    newBmpHeader.fileSize = sizeof(BMPHeader) + sizeof(DIBHeader) + newDibHeader.dataSize;

    // Return a new BMPFile object with updated headers and rotated data
    return BMPFile(newBmpHeader, newDibHeader, rotatedData);
}



BMPFile BMPFile::rotateLeft() {
    // Get original dimensions
    int originalHeight = dibHeader.height;
    int originalWidth = dibHeader.width;

    // Create a new vector for the rotated image
    std::vector<std::vector<RGBPixel>> rotatedData(originalWidth, std::vector<RGBPixel>(originalHeight));

    // Rotate the image 90 degrees counterclockwise
    for (int i = 0; i < originalHeight; ++i) {
        for (int j = 0; j < originalWidth; ++j) {
            // Place pixel in new position
            rotatedData[originalWidth - 1 - j][i] = data[i][j];
        }
    }

    // Update DIB header for new dimensions
    DIBHeader newDibHeader = dibHeader;
    newDibHeader.width = originalHeight;
    newDibHeader.height = originalWidth;

    // Calculate padding for the new width
    unsigned int paddingSize = (4 - (newDibHeader.width * sizeof(RGBPixel)) % 4) % 4;
    newDibHeader.dataSize = (newDibHeader.width * sizeof(RGBPixel) + paddingSize) * newDibHeader.height;

    // Update BMP header file size
    BMPHeader newBmpHeader = bmpHeader;
    newBmpHeader.fileSize = sizeof(BMPHeader) + sizeof(DIBHeader) + newDibHeader.dataSize;

    // Return a new BMPFile object with updated headers and rotated data
    return BMPFile(newBmpHeader, newDibHeader, rotatedData);
}


void BMPFile::writeBMP(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error saving file.");
    }

    int byte = dibHeader.height * dibHeader.width * sizeof(RGBPixel);

    std::cout << "File " << filename << " uses " << byte << " bytes." << std::endl;

    file.write(reinterpret_cast<const char *>(&bmpHeader), sizeof(bmpHeader));
    file.write(reinterpret_cast<const char *>(&dibHeader), sizeof(dibHeader));

    unsigned int rowSize = dibHeader.width * sizeof(RGBPixel);
    unsigned int paddingSize = (4 - (rowSize % 4)) % 4;
    unsigned char padding[3] = {0, 0, 0}; // Padding bytes

    for (int i = dibHeader.height - 1; i >= 0; --i) {
        file.write(reinterpret_cast<const char *>(data[i].data()), dibHeader.width * sizeof(RGBPixel));

        file.write(reinterpret_cast<char*>(padding), paddingSize);
    }

    file.close();
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
	for (int i = 0; i < dibHeader.height; ++i) {
		for (int j = 0; j < dibHeader.width; ++j) {
			data[i][j].printPix();
			std::cout << ' ';
		}
		std::cout << '\n';
	}
}

unsigned int BMPFile::getHeight() {
	return dibHeader.height;
}

unsigned int BMPFile::getWidth() {
	return dibHeader.width;
}

unsigned int BMPFile::getBitsPerPixel() {
	return dibHeader.bitsPerPixel;
}

std::vector<std::vector<RGBPixel>> BMPFile::getData() {
	return data;
}

unsigned int BMPFile::getDataSize() {
	return dibHeader.dataSize;
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
	sigma(_sigma)
{}

void Gauss::createGaussKernel() {
	kernel.resize(kernelSize);
	for (int i = 0; i < kernelSize; ++i)
		kernel[i].resize(kernelSize);


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
	for (int i = 0; i < kernelSize; ++i) {
		for (int j = 0; j < kernelSize; ++j) {
			std::cout << kernel[i][j] << ' ';
		}
		std::cout << '\n';
	}
}

std::vector<std::vector<RGBPixel>> Gauss::applyConvolution(const std::vector<std::vector<RGBPixel>>& img, unsigned int height, unsigned int width) {
	unsigned int h = height;
	unsigned int w = width;

	std::vector<std::vector<RGBPixel>> res(h, std::vector<RGBPixel>(w));

	int radius = kernelSize / 2;

	for (uint x = 0; x < h; ++x) {
		for (uint y = 0; y < w; ++y) {
			double sumR = 0;
			double sumG = 0;
			double sumB = 0;

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

					sumR += 1.0 * img[pixelX][pixelY].red * kernel[kx][ky];
					sumG += 1.0 * img[pixelX][pixelY].green * kernel[kx][ky];
					sumB += 1.0 * img[pixelX][pixelY].blue * kernel[kx][ky];
				}
			}

			unsigned char newRed = std::min(std::max(sumR, 0.), 255.);
			unsigned char newGreen = std::min(std::max(sumG, 0.), 255.);
			unsigned char newBlue = std::min(std::max(sumB, 0.), 255.);

			res[x][y] = RGBPixel(newRed, newGreen, newBlue);
		}
	}

	return res;
}

BMPFile Gauss::computeBlur(BMPFile& img) {
	std::vector<std::vector<RGBPixel>> rgbarr = img.getData();
	
	std::vector<std::vector<RGBPixel>> convarr = applyConvolution(rgbarr, img.getHeight(), img.getWidth());
	
	BMPFile res(img.getBmpHeader(), img.getDibHeader(), convarr);
	
	return res;
}