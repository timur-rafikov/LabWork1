/* Author: Timur Rafikov
 * e-mail: st128186@student.spbu.ru
 * Assignment3: Transformers classes
 * 28.10.2024
 */

#include <iostream>
#include <string>
#include "bmp_reader.hpp"

int main(int argc, char* argv[]) {
	BMPFile mybmp;

	mybmp.readBMP(argv[1]);

	std::cout << '\n';

	mybmp.printInfo();
	
	std::cout << '\n';

	mybmp.printData();

	std::cout << '\n';
}