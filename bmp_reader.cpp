/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#include "bmp_reader.hpp"

void BMPFile::readBMP(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file\n";
		return; 
	}

	std::size_t fsize = std::filesystem::file_size(filename); // reading the amount of data
	std::cout << "BMP file size: " << fsize << " bytes\n";
	
	// Reading Headers
	file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
	file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

	// Reading Pixel Data
	data = new unsigned char[dibHeader.getDataSize()];

	file.seekg(bmpHeader.getPixelOffset(), std::ios::beg);
	file.read(reinterpret_cast<char*>(data), dibHeader.getDataSize());

	file.close();
}

void BMPFile::printInfo() {
	bmpHeader.printInfo();
	dibHeader.printInfo();
}

void BMPFile::printData() {
	for (unsigned int i = 0; i < dibHeader.getDataSize(); ++i) {
		if (i % 16 == 0)
			printf("\n%04x: ", i);
		printf("%02x ", data[i]);
	}
}





void BMPHeader::printInfo() {
	std::cout << "ID: " << ' ' << ID[0] << ' ' << ID[1] << '\n';
	std::cout << "file_size: " << file_size << '\n';
	std::cout << "unused: " << unused[0] << ' ' << unused[1] << ' ' << unused[2] << ' ' << unused[3] << '\n';
	std::cout << "pixel_offset: " << pixel_offset << '\n';
	std::cout << '\n';
}

unsigned int BMPHeader::getPixelOffset() {
	return pixel_offset;
}






void DIBHeader::printInfo() {
	std::cout << "header_size: " << header_size << '\n';
	std::cout << "width: " << width << '\n';
	std::cout << "height: " << height << '\n';
	std::cout << "color_planes: " << color_planes << '\n';
	std::cout << "bits_per_pixel: " << bits_per_pixel << '\n';
	std::cout << "BI_RGB: " << BI_RGB << '\n';
	std::cout << "data_size: " << data_size << '\n';
	std::cout << "pwidth: " << pwidth << '\n';
	std::cout << "pheight: " << pheight << '\n';
	std::cout << "colors_count: " << colors_count << '\n';
	std::cout << "imp_colors_count: " << imp_colors_count << '\n';
	std::cout << '\n';
}

unsigned int DIBHeader::getDataSize() {
	return data_size;
}

unsigned int DIBHeader::getWidth() {
	return width;
}

unsigned int DIBHeader::getHeight() {
	return height;
}