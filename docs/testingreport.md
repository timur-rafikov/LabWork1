#BMP Module Test Report

##General Information

- **Project Name:** BMP Image Processor  
- **Modules:** `BMPFile` and `Gauss`
- **Test Framework:** [Google Test](https://github.com/google/googletest)  
- **File Format:** 24-bit BMP (uncompressed)

##Test Structure

| Category                  | Test Cases                                              |
|---------------------------|----------------------------------------------------------|
| BMP Reading               | `CanReadValidBMP`, `ThrowsOnInvalidPath`                |
| BMP Writing               | `CanWriteBMP`                                           |
| Image Rotation            | `RotateRightDimensionsAreCorrect`, `RotateLeftDimensionsAreCorrect` |
| Copy Constructor          | `CopyConstructorCreatesEqualData`                       |
| Gaussian Blur             | `CanCreateKernelAndApplyBlur`                           |

##Test Results

| Test Name                              | Status   | Comments                                      |
|----------------------------------------|----------|-----------------------------------------------|
| `CanReadValidBMP`                      | ✅ Passed | Successfully loaded a valid BMP file          |
| `ThrowsOnInvalidPath`                  | ✅ Passed | Exception thrown for non-existent path        |
| `CanWriteBMP`                          | ✅ Passed | Image file saved successfully                 |
| `RotateRightDimensionsAreCorrect`      | ✅ Passed | Image dimensions correctly updated (right)    |
| `RotateLeftDimensionsAreCorrect`       | ✅ Passed | Image dimensions correctly updated (left)     |
| `CopyConstructorCreatesEqualData`      | ✅ Passed | Deep copy created with matching pixel data    |
| `CanCreateKernelAndApplyBlur`          | ✅ Passed | Gaussian filter applied without errors        |

##Conclusion

All tests passed successfully