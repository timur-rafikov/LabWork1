/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#include <iostream>
#include <string>
#include "bmp_reader.hpp"

int main() {
	
	std::cout << "The BMP file processing program welcomes you.\nEnter the file name: ";
	std::string filename; 
	std::cin >> filename;

	BMPFile bmp(filename);
	/*
	BMPFile bmpRight = bmp.rotateRight();
	BMPFile bmpLeft = bmp.rotateLeft();

	bmpRight.writeBMP("RightSaved" + filename);
	bmpLeft.writeBMP("LeftSaved" + filename);

	std::cout << "TASK 3: The BMP file rotated to the right has been saved in a file " << "'RightSaved" << filename <<  "'" << '\n';
	std::cout << "TASK 3': And rotated to the left in " << "'LeftSaved" << filename <<  "'" << '\n';
	*/

	//bmp.printData();
	//std::cout << "\n\n";

	

	std::cout << "Blurring image...\n";

	Gauss gs(7, 5);
	gs.createGaussKernel();
	BMPFile blurBmp = gs.computeBlur(bmp);

	std::cout << "Image has been blurred\n";

	blurBmp.writeBMP("Blur" + filename);

	std::cout << "The blurred image has been saved in a file " << "'Blur" << filename << "'\n";

	/*
	for (uint i = 0; i < bmp.getHeight(); ++i) {
		for (uint j = 0; j < bmp.getWidth(); ++j) {
			convImg[i][j].printPix();
			std::cout << " | ";
		}
		std::cout << '\n';
	}
	*/

}