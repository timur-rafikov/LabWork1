/**
 * @file bmp_reader.cpp
 * @author Timur Rafikov
 * @brief Implementation of BMP image processing classes and methods.
 * @date 2024-10-28
 * @see bmp_reader.hpp
 */

#include "bmp_reader.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <omp.h>
#define _USE_MATH_DEFINES
#include <cmath>

// ==================== BMPFile ====================

/**
 * @brief Default constructor. Initializes BMPHeader and DIBHeader.
 */
BMPFile::BMPFile() {
	bmpHeader = BMPHeader();
	dibHeader = DIBHeader();
}

/**
 * @brief Constructs a BMPFile by reading from file.
 * @param filename Path to the BMP file.
 */
BMPFile::BMPFile(const std::string& filename) {
	readBMP(filename);
}

/**
 * @brief Constructs a BMPFile with explicit headers and pixel data.
 * @param _bmphdr BMPHeader structure.
 * @param _dibhdr DIBHeader structure.
 * @param _data 2D vector of RGBPixel representing image data.
 * @throws std::invalid_argument if data dimensions do not match header info.
 */
BMPFile::BMPFile(const BMPHeader& _bmphdr, const DIBHeader& _dibhdr, const std::vector<std::vector<RGBPixel>>& _data)
    : bmpHeader(_bmphdr), dibHeader(_dibhdr), data(_data)
{
    if ((int)data.size() != dibHeader.height || (data.size() > 0 && (int)data[0].size() != dibHeader.width)) {
        throw std::invalid_argument("Pixel data dimensions do not match DIB header dimensions.");
    }
}

/**
 * @brief Copy constructor.
 * @param p BMPFile object to copy.
 */
BMPFile::BMPFile(BMPFile& p) 
    : bmpHeader(p.bmpHeader), 
      dibHeader(p.dibHeader), 
      data(p.data)
{
    data.resize(p.dibHeader.height);
    for (int i = 0; i < p.dibHeader.height; ++i) {
        data[i].resize(p.dibHeader.width);
        std::copy(p.data[i].begin(), p.data[i].end(), data[i].begin());
    }
}

/**
 * @brief Allocates memory for image data.
 * @param height Image height.
 * @param width Image width.
 * @return true on success, false on failure.
 */
bool BMPFile::allocateMemory(int height, int width) {
	try {
        data.resize(height);
        for (int i = 0; i < height; ++i) {
            data[i].resize(width);
        }
        return true;
    } catch (const std::bad_alloc&) {
        std::cerr << "Failed to allocate memory for image data." << std::endl;
        data.clear();
        return false;
    }
}

/**
 * @brief Reads BMP file and populates headers and data.
 * @param filename Path to BMP file.
 * @throws std::runtime_error on file or read error.
 */
void BMPFile::readBMP(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file.");
    }

    file.read(reinterpret_cast<char *>(&bmpHeader), sizeof(bmpHeader));
    file.read(reinterpret_cast<char *>(&dibHeader), sizeof(dibHeader));

    dibHeader.height = abs(dibHeader.height);
    dibHeader.width = abs(dibHeader.width);

    if (dibHeader.width == 0 || dibHeader.height == 0) {
        throw std::runtime_error("Unexpected file size.");
    }

    file.seekg(bmpHeader.pixelOffset, file.beg);

    if (!allocateMemory(dibHeader.height, dibHeader.width)) {
        throw std::runtime_error("Failed to allocate memory for image.");
    }

    for (int i = dibHeader.height - 1; i >= 0; --i) {
        file.read(reinterpret_cast<char *>(data[i].data()), dibHeader.width * sizeof(RGBPixel));
        if (!file) {
            throw std::runtime_error("Error reading file.");
        }
    }

    file.close();
}

/**
 * @brief Rotates the image 90 degrees clockwise.
 * @return Rotated BMPFile object.
 */
BMPFile BMPFile::rotateRight() {
    // Get original dimensions
    int originalHeight = dibHeader.height;
    int originalWidth = dibHeader.width;

    // Create a new vector for the rotated image
    std::vector<std::vector<RGBPixel>> rotatedData(originalWidth, std::vector<RGBPixel>(originalHeight));

    // Rotate the image 90 degrees clockwise
    #pragma omp parallel for collapse(2)
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

    newDibHeader.pheight = newDibHeader.height;
    newDibHeader.pwidth = newDibHeader.width * sizeof(RGBPixel) + paddingSize;

    // Update BMP header file size
    BMPHeader newBmpHeader = bmpHeader;
    newBmpHeader.fileSize = sizeof(BMPHeader) + sizeof(DIBHeader) + newDibHeader.dataSize;

    // Return a new BMPFile object with updated headers and rotated data
    return BMPFile(newBmpHeader, newDibHeader, rotatedData);
}

/**
 * @brief Rotates the image 90 degrees counter-clockwise.
 * @return Rotated BMPFile object.
 */
BMPFile BMPFile::rotateLeft() {
    // Get original dimensions
    int originalHeight = dibHeader.height;
    int originalWidth = dibHeader.width;

    // Create a new vector for the rotated image
    std::vector<std::vector<RGBPixel>> rotatedData(originalWidth, std::vector<RGBPixel>(originalHeight));

    // Rotate the image 90 degrees counterclockwise
    #pragma omp parallel for collapse(2)
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

    newDibHeader.pheight = newDibHeader.height;
    newDibHeader.pwidth = newDibHeader.width * sizeof(RGBPixel) + paddingSize;

    // Update BMP header file size
    BMPHeader newBmpHeader = bmpHeader;
    newBmpHeader.fileSize = sizeof(BMPHeader) + sizeof(DIBHeader) + newDibHeader.dataSize;

    // Return a new BMPFile object with updated headers and rotated data
    return BMPFile(newBmpHeader, newDibHeader, rotatedData);
}

/**
 * @brief Writes BMP image to file.
 * @param filename Path to save the image.
 * @throws std::runtime_error on write failure.
 */
void BMPFile::writeBMP(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error saving file.");
    }

    // Update BMP header file size
    bmpHeader.fileSize = sizeof(BMPHeader) + sizeof(DIBHeader) + dibHeader.dataSize;

    // Write headers
    file.write(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
    file.write(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

    unsigned int rowSize = dibHeader.width * sizeof(RGBPixel);
    unsigned int paddingSize = (4 - (rowSize % 4)) % 4;
    unsigned char padding[3] = {0, 0, 0}; // Padding bytes

    // Write pixel data with padding
    for (int i = 0; i < dibHeader.height; ++i) {
        file.write(reinterpret_cast<char*>(data[dibHeader.height - 1 - i].data()), rowSize); // Write rows from bottom to top
        file.write(reinterpret_cast<char*>(padding), paddingSize); // Write padding for each row
    }

    file.close(); // Ensure the file is closed after writing

}

/**
 * @brief Prints header info to stdout.
 */
void BMPFile::printInfo() {
	bmpHeader.printInfo();
	dibHeader.printInfo();
}

/**
 * @brief Prints image pixel data to stdout.
 */
void BMPFile::printData() {
	for (int i = 0; i < dibHeader.height; ++i) {
        for (int j = 0; j < dibHeader.width; ++j) {
            data[i][j].printPix();
            std::cout << ' ';
        }
        std::cout << '\n';
    }
}

/** @brief Returns image height. */
unsigned int BMPFile::getHeight() { return dibHeader.height; }

/** @brief Returns image width. */
unsigned int BMPFile::getWidth() { return dibHeader.width; }

/** @brief Returns bits per pixel. */
unsigned int BMPFile::getBitsPerPixel() { return dibHeader.bitsPerPixel; }

/** @brief Returns image pixel data. */
std::vector<std::vector<RGBPixel>> BMPFile::getData() { return data; }

/** @brief Returns size of image data. */
unsigned int BMPFile::getDataSize() { return dibHeader.dataSize; }

/** @brief Returns BMP file header. */
BMPHeader BMPFile::getBmpHeader() { return bmpHeader; }

/** @brief Returns DIB header. */
DIBHeader BMPFile::getDibHeader() { return dibHeader; }

// ==================== BMPHeader ====================

/** @brief Default constructor. */
BMPHeader::BMPHeader() {
	type = 0;
    fileSize = 0;
    reserved1 = 0;
    reserved2 = 0;
    pixelOffset = 0;
}

/** @brief Copy constructor. */
BMPHeader::BMPHeader(const BMPHeader& p) {
	type = p.type;
    fileSize = p.fileSize;
    reserved1 = p.reserved1;
    reserved2 = p.reserved2;
    pixelOffset = p.pixelOffset;
}

/** @brief Prints BMPHeader fields to stdout. */
void BMPHeader::printInfo() {
	std::cout << "Type: " << type << '\n';
    std::cout << "fileSize: " << fileSize << '\n';
    std::cout << "reserved: " << reserved1 << ' ' << reserved2 << '\n';
    std::cout << "pixelOffset: " << pixelOffset << '\n';
    std::cout << '\n';
}

// ==================== DIBHeader ====================

/** @brief Default constructor. */
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

/** @brief Copy constructor. */
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

/** @brief Prints DIBHeader fields to stdout. */
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

// ==================== RGBPixel ====================

/** @brief Default constructor. */
RGBPixel::RGBPixel() : red(0), green(0), blue(0) {}

/**
 * @brief Parameterized constructor.
 * @param _red Red component.
 * @param _green Green component.
 * @param _blue Blue component.
 */
RGBPixel::RGBPixel(const uint8_t& _red, const uint8_t& _green, const uint8_t& _blue) :
	red(_red), green(_green), blue(_blue) {}

/** @brief Prints RGB values to stdout. */
void RGBPixel::printPix() {
	printf("%02x ", red);
    printf("%02x ", green);
    printf("%02x ", blue);
}

// ==================== Gauss ====================

/** @brief Default constructor. */
Gauss::Gauss() : kernelSize(0), sigma(0), kernel(0) {}

/**
 * @brief Parameterized constructor.
 * @param _kernelSize Size of the Gaussian kernel.
 * @param _sigma Standard deviation of the Gaussian function.
 */
Gauss::Gauss(const unsigned int& _kernelSize, const double& _sigma) :
	kernelSize(_kernelSize), sigma(_sigma) {}

/** 
 * @brief Generates a 2D Gaussian kernel matrix.
 */
void Gauss::createGaussKernel() {
	kernel.resize(kernelSize);
    for (uint i = 0; i < kernelSize; ++i)
        kernel[i].resize(kernelSize);


    int radius = kernelSize / 2;

    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum) collapse(2)
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            double value = gaussFunc(x, y, sigma);
            sum += value;
            kernel[y + radius][x + radius] = value;
        }
    }

    #pragma omp parallel for collapse(2)
    for (uint x = 0; x < kernelSize; ++x) {
        for (uint y = 0; y < kernelSize; ++y) {
            kernel[x][y] /= sum;
        }
    }
}

/**
 * @brief Computes the Gaussian function value for given coordinates.
 * @param x X-offset.
 * @param y Y-offset.
 * @param sigma Standard deviation.
 * @return Gaussian weight.
 */
double Gauss::gaussFunc(int x, int y, double sigma) {
	return ((1. / (2 * M_PI * sigma * sigma)) * exp(-((x * x + y * y) / (2 * sigma * sigma))));
}

/**
 * @brief Prints the generated kernel matrix.
 */
void Gauss::printKernel() {
	for (uint i = 0; i < kernelSize; ++i) {
        for (uint j = 0; j < kernelSize; ++j) {
            std::cout << kernel[i][j] << ' ';
        }
        std::cout << '\n';
    }
}

/**
 * @brief Applies convolution with the Gaussian kernel.
 * @param img Input image as 2D vector of RGBPixel.
 * @param height Image height.
 * @param width Image width.
 * @return Blurred image data.
 */
std::vector<std::vector<RGBPixel>> Gauss::applyConvolution(const std::vector<std::vector<RGBPixel>>& img, unsigned int height, unsigned int width) {
	unsigned int h = height;
    unsigned int w = width;

    std::vector<std::vector<RGBPixel>> res(h, std::vector<RGBPixel>(w));

    int radius = kernelSize / 2;

    #pragma omp parallel for collapse(2)
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

/**
 * @brief Applies Gaussian blur to an image.
 * @param img Input BMPFile.
 * @return Blurred BMPFile.
 */
BMPFile Gauss::computeBlur(BMPFile& img) {
	std::vector<std::vector<RGBPixel>> rgbarr = img.getData();
    
    std::vector<std::vector<RGBPixel>> convarr = applyConvolution(rgbarr, img.getHeight(), img.getWidth());
    
    BMPFile res(img.getBmpHeader(), img.getDibHeader(), convarr);
    
    return res;
}
