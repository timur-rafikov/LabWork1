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