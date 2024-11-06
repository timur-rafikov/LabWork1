# Laboratory work 1
## Author
Рафиков Тимур Ришатович, группа 24.Б81-мм
## Contacts
st128186@student.spbu.ru  
@trafikprog
## Description
Loading and processing a bitmap image  
Tasks:  
1. Determine the amount of memory allocated to load the image.  
2. Upload the image to memory.  
3. Rotate the image 90 degrees clockwise. Save the result.  
3'. Rotate the image 90 degrees counterclockwise. Save the result.  
4. Apply a Gaussian filter to the rotated image. Save the result.  

The BMPHeader, DIBHeader, and BMPFile classes are declared in the  
bmp_header.hpp header file. The BMPFile class contains the  
main methods of working with a file: loading, rotating the  
image to the right \ to the left and applying the Gauss filter.

The program applies a Gaussian filter to blur the BMP file, and   
asks the user to enter the kernel size and sigma in the console.  
The program works correctly for 24-bit images (Red Green Blue).  
  
## Build
make
## Run
./app
## Clean
make clean