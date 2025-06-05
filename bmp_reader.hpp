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
#include <vector>
#include <stdint.h>

#pragma pack(push, 1) // for alignment in the data

/**
 * @brief Represents the BMP file header (14 bytes)
 */
struct BMPHeader {
    uint16_t type;         ///< Magic identifier: 0x4d42 ("BM")
    uint32_t fileSize;     ///< Size of the BMP file in bytes
    uint16_t reserved2;    ///< Reserved; actual order depends on platform
    uint16_t reserved1;    ///< Reserved
    uint32_t pixelOffset;  ///< Offset to start of pixel data

    BMPHeader();
    BMPHeader(const BMPHeader& p);

    /**
     * @brief Prints BMP header info to stdout
     */
    void printInfo();
};

/**
 * @brief Represents the DIB header (BITMAPINFOHEADER, 40 bytes)
 */
struct DIBHeader {
    uint32_t headerSize;      ///< Size of the DIB header (40 bytes)
    int32_t width;            ///< Image width in pixels
    int32_t height;           ///< Image height in pixels
    uint16_t colorPlanes;     ///< Number of color planes (must be 1)
    uint16_t bitsPerPixel;    ///< Bits per pixel (e.g. 24 for RGB)
    uint32_t BI_RGB;          ///< Compression type (0 = none)
    uint32_t dataSize;        ///< Size of raw bitmap data
    int32_t pwidth;           ///< Print resolution (horizontal, pixels/meter)
    int32_t pheight;          ///< Print resolution (vertical, pixels/meter)
    uint32_t colorsCount;     ///< Number of colors in the palette
    uint32_t impColorsCount;  ///< Number of important colors used

    DIBHeader();
    DIBHeader(const DIBHeader& p);

    /**
     * @brief Prints DIB header info to stdout
     */
    void printInfo();
};

#pragma pack(pop)

/**
 * @brief Represents a single RGB pixel
 */
struct RGBPixel {
    uint8_t red;    ///< Red component
    uint8_t green;  ///< Green component
    uint8_t blue;   ///< Blue component

    RGBPixel();
    RGBPixel(const uint8_t& _red, const uint8_t& _green, const uint8_t& _blue);

    /**
     * @brief Prints pixel RGB values
     */
    void printPix();
};

/**
 * @brief Represents a BMP image file and provides methods for reading, writing, and processing
 */
class BMPFile {
private:
    BMPHeader bmpHeader;                            ///< BMP file header
    DIBHeader dibHeader;                            ///< DIB header
    std::vector<std::vector<RGBPixel>> data;        ///< 2D vector representing pixel data

public:
    BMPFile();
    BMPFile(BMPFile& p);
    BMPFile(const std::string& filename);
    BMPFile(const BMPHeader& _bmphdr, const DIBHeader& _dibhdr, const std::vector<std::vector<RGBPixel>>& _data);

    /**
     * @brief Reads BMP file from disk
     * @param filename Path to BMP file
     */
    void readBMP(const std::string& filename);

    /**
     * @brief Writes BMP file to disk
     * @param filename Output file name
     */
    void writeBMP(const std::string& filename);

    /**
     * @brief Allocates memory for pixel data
     * @param height Image height
     * @param width Image width
     * @return true if memory allocation is successful
     */
    bool allocateMemory(int height, int width);

    /**
     * @brief Rotates image 90 degrees to the right
     * @return Rotated BMPFile object
     */
    BMPFile rotateRight();

    /**
     * @brief Rotates image 90 degrees to the left
     * @return Rotated BMPFile object
     */
    BMPFile rotateLeft();

    /**
     * @brief Gets image height
     * @return Height in pixels
     */
    unsigned int getHeight();

    /**
     * @brief Gets image width
     * @return Width in pixels
     */
    unsigned int getWidth();

    /**
     * @brief Gets bits per pixel
     * @return Number of bits per pixel (e.g., 24)
     */
    unsigned int getBitsPerPixel();

    /**
     * @brief Gets size of image data in bytes
     * @return Data size
     */
    unsigned int getDataSize();

    /**
     * @brief Gets pixel data
     * @return 2D vector of RGBPixel
     */
    std::vector<std::vector<RGBPixel>> getData();

    /**
     * @brief Gets BMP header
     * @return BMPHeader struct
     */
    BMPHeader getBmpHeader();

    /**
     * @brief Gets DIB header
     * @return DIBHeader struct
     */
    DIBHeader getDibHeader();

    /**
     * @brief Prints file header information
     */
    void printInfo();

    /**
     * @brief Prints image pixel data (RGB values)
     */
    void printData();
};

/**
 * @brief Represents a Gaussian blur filter
 */
struct Gauss {
    unsigned int kernelSize;                         ///< Size of the convolution kernel
    double sigma;                                    ///< Standard deviation for Gaussian
    std::vector<std::vector<double>> kernel;         ///< 2D Gaussian kernel

    Gauss();
    Gauss(const unsigned int& _kernelSize, const double& _sigma);

    /**
     * @brief Generates the Gaussian kernel based on kernelSize and sigma
     */
    void createGaussKernel();

    /**
     * @brief Prints the Gaussian kernel
     */
    void printKernel();

    /**
     * @brief Applies Gaussian convolution to image data
     * @param img 2D array of RGBPixel
     * @param height Image height
     * @param width Image width
     * @return Convolved image data
     */
    std::vector<std::vector<RGBPixel>> applyConvolution(const std::vector<std::vector<RGBPixel>>& img, unsigned int height, unsigned int width);

    /**
     * @brief Applies Gaussian blur to BMP image
     * @param img BMPFile object
     * @return Blurred BMPFile object
     */
    BMPFile computeBlur(BMPFile& img);

    /**
     * @brief Computes Gaussian function value for given x, y
     * @param x X-coordinate
     * @param y Y-coordinate
     * @param sigma Standard deviation
     * @return Gaussian value at (x, y)
     */
    double gaussFunc(int x, int y, double sigma);
};

#endif
