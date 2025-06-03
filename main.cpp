/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#include <iostream>
#include <string>
#include <chrono>
#include "bmp_reader.hpp"

int main() {
	
	std::cout << "The BMP file processing program welcomes you.\nEnter the file name: ";
	std::string filename; 
	std::cin >> filename;

	BMPFile bmp(filename);

	auto start = std::chrono::high_resolution_clock::now();	// start time

	BMPFile bmpRight = bmp.rotateRight();
	BMPFile bmpLeft = bmp.rotateLeft();

	auto end = std::chrono::high_resolution_clock::now();  // end time
	std::chrono::duration<double> duration = end - start;
	std::cout << "Sequential TIME of rotation: " << duration.count() << " seconds\n";

	bmpRight.writeBMP("RightSaved" + filename);
	bmpLeft.writeBMP("LeftSaved" + filename);

	std::cout << "TASK 3: The BMP file rotated to the right has been saved in a file " << "'RightSaved" << filename <<  "'" << "\n\n";
	std::cout << "TASK 3': And rotated to the left in " << "'LeftSaved" << filename <<  "'" << '\n';

	std::cout << "\nTASK 4: The Gaussian filter\n\n";

	std::cout << "Enter Kernel Size and Sigma (recommend 19 and 5): ";
	int kernelSize;
	double sigma;
	std::cin >> kernelSize >> sigma;
	std::cout << "Blurring image...\n";

	start = std::chrono::high_resolution_clock::now();

	Gauss gs(kernelSize, sigma);
	gs.createGaussKernel();

	BMPFile blurBmp = gs.computeBlur(bmp);

	end = std::chrono::high_resolution_clock::now();
	duration = end - start;
	std::cout << "Sequential TIME of blurring: " << duration.count() << " seconds\n";

	std::cout << "Image has been blurred\n";

	blurBmp.writeBMP("Blur_" + filename);

	std::cout << "The blurred image has been saved in a file " << "'Blur_" << filename << "'\n";

	return 0;
}