#include <gtest/gtest.h>
#include "bmp_reader.hpp"
#include <filesystem>
#include <fstream>

const std::string TEST_IMAGE_PATH = "img.bmp";
const std::string TEST_OUTPUT_PATH = "out_img.bmp";

TEST(BMPFileTest, CanReadValidBMP) {
    EXPECT_NO_THROW({
        BMPFile bmp(TEST_IMAGE_PATH);
        EXPECT_GT(bmp.getHeight(), 0);
        EXPECT_GT(bmp.getWidth(), 0);
        EXPECT_EQ(bmp.getBitsPerPixel(), 24);  // BMP 24-bit format
    });
}

TEST(BMPFileTest, ThrowsOnInvalidPath) {
    EXPECT_THROW({
        BMPFile bmp("non_existent.bmp");
    }, std::runtime_error);
}

TEST(BMPFileTest, CanWriteBMP) {
    BMPFile bmp(TEST_IMAGE_PATH);
    EXPECT_NO_THROW({
        bmp.writeBMP(TEST_OUTPUT_PATH);
        EXPECT_TRUE(std::filesystem::exists(TEST_OUTPUT_PATH));
    });
}

TEST(BMPRotationTest, RotateRightDimensionsAreCorrect) {
    BMPFile bmp(TEST_IMAGE_PATH);
    BMPFile rotated = bmp.rotateRight();

    EXPECT_EQ(rotated.getWidth(), bmp.getHeight());
    EXPECT_EQ(rotated.getHeight(), bmp.getWidth());
}

TEST(BMPRotationTest, RotateLeftDimensionsAreCorrect) {
    BMPFile bmp(TEST_IMAGE_PATH);
    BMPFile rotated = bmp.rotateLeft();

    EXPECT_EQ(rotated.getWidth(), bmp.getHeight());
    EXPECT_EQ(rotated.getHeight(), bmp.getWidth());
}

TEST(BMPFileTest, CopyConstructorCreatesEqualData) {
    BMPFile bmp1(TEST_IMAGE_PATH);
    BMPFile bmp2(bmp1);

    EXPECT_EQ(bmp1.getWidth(), bmp2.getWidth());
    EXPECT_EQ(bmp1.getHeight(), bmp2.getHeight());
    EXPECT_EQ(bmp1.getBitsPerPixel(), bmp2.getBitsPerPixel());

    auto data1 = bmp1.getData();
    auto data2 = bmp2.getData();

    for (size_t i = 0; i < data1.size(); ++i) {
        for (size_t j = 0; j < data1[i].size(); ++j) {
            EXPECT_EQ(data1[i][j].red, data2[i][j].red);
            EXPECT_EQ(data1[i][j].green, data2[i][j].green);
            EXPECT_EQ(data1[i][j].blue, data2[i][j].blue);
        }
    }
}

TEST(GaussianTest, CanCreateKernelAndApplyBlur) {
    BMPFile bmp(TEST_IMAGE_PATH);
    Gauss blur(3, 1.0);

    EXPECT_NO_THROW({
        blur.createGaussKernel();
        BMPFile blurred = blur.computeBlur(bmp);

        EXPECT_EQ(blurred.getWidth(), bmp.getWidth());
        EXPECT_EQ(blurred.getHeight(), bmp.getHeight());
    });
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}