# Performance Report
## Before parallelization
The values are given in *seconds*
The names of the images match the names in the repository

| Image Name File | Resolution  | Rotation  | Blurring |
| --------------- | ----------- | --------- | -------- |
| img.bmp         | 1920 x 1080 | 0.208484  | 23.4414  |
| input.bmp       | 600 x 400   | 0.0513021 | 2.70748  |
| sample.bmp      | 1000 x 667  | 0.0843066 | 7.53282  |


## After parallelization
| Image Name File | Resolution  | Rotation  | Blurring |
| --------------- | ----------- | --------  | ---------|
| img.bmp         | 1920 x 1080 | 0.15928   | 7.69221  |
| input.bmp       | 600 x 400   | 0.0373708 | 0.900161 |
| sample.bmp      | 1000 x 667  | 0.0999856 | 2.47886  |

### Results
| Image      | Resolution  | Rotation Performance Growth (%) | Blurring Performance Growth (%) |
| ---------- | ----------- | ------------------------------- | ------------------------------- |
| img.bmp    | 1920 x 1080 | 23.63%                          | 67.19%                          |
| input.bmp  | 600 x 400   | 27.15%                          | 66.75%                          |
| sample.bmp | 1000 x 667  | -18.59%                         | 67.09%                          |


Average performance gain in image *rotation*: **+10.75 %**
Average performance gain in image *blurring*: **+67.01 %**
  
Parallelization provides a big boost to program performance. Blurring shows stable significant growth everywhere — about 3.0–3.2 times faster, which indicates a good effect from OpenMP.

