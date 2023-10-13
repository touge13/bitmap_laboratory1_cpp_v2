#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#ifndef BMP_HPP
#define BMP_HPP

//задаем макроопределения
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESSION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0
#define RESOLUTIONX 0x26
#define RESOLUTIONY 0x002A

//задаем синонимы
using int32 = uint32_t;
using int16 = int16_t;
using byte = unsigned char;

struct ImageData {
    byte *pixels;
    int32 width;
    int32 height;
    int32 bytesPerPixel;
    int32 resolutionX;
    int32 resolutionY;
};

class Functions {
public:
    void Dublication(FILE outputFile, int paddedRowSize, ImageData imageData){
        int unpaddedRowSize = imageData.width * imageData.bytesPerPixel;
        for (int i = 0; i < imageData.height; i++) {
            //начинаем запись с начала последней строки массива пикселей
            int pixelOffset = ((imageData.height - i) - 1) * unpaddedRowSize;
            fwrite(&imageData.pixels[pixelOffset], 1, paddedRowSize, &outputFile);	
        }
    }

    void TurnRight(FILE outputFile, int paddedRowSize, ImageData imageData){
        // Вычисление размера строк в оригинальном изображении
        int unpaddedRowSize = imageData.width * imageData.bytesPerPixel;
        // Запись пикселей (столбец за столбцом)
        for (int i = 0; i < imageData.width; i++) {
            for (int j = imageData.height - 1; j >= 0; j--) {
                int pixelOffset = j * unpaddedRowSize + ((imageData.width - i) - 1) * imageData.bytesPerPixel;
                fwrite(&imageData.pixels[pixelOffset], 1, imageData.bytesPerPixel, &outputFile);
            }
        }
    }

    void TurnLeft(FILE outputFile, int paddedRowSize, ImageData imageData){
        // Вычисление размера строк в оригинальном изображении
        int unpaddedRowSize = imageData.width * imageData.bytesPerPixel;
        // Запись пикселей (столбец за столбцом)
        for (int i = 0; i < imageData.width; i++) {
            for (int j = imageData.height - 1; j >= 0; j--) {
                int pixelOffset = ((imageData.height - j) - 1) * unpaddedRowSize + i * imageData.bytesPerPixel;
                fwrite(&imageData.pixels[pixelOffset], 1, imageData.bytesPerPixel, &outputFile);
            }
        }
    }

    void Reflected(FILE outputFile, int paddedRowSize, ImageData imageData){
        int unpaddedRowSize = imageData.width * imageData.bytesPerPixel;
        for (int i = imageData.height - 1; i >= 0; i--) {
            for (int j = imageData.width - 1; j >= 0; j--) {
                int pixelOffset = i * unpaddedRowSize + j * imageData.bytesPerPixel;
                fwrite(&imageData.pixels[pixelOffset], 1, imageData.bytesPerPixel, &outputFile); 
            }
        }
    }

    void Flipped(FILE outputFile, int paddedRowSize, ImageData imageData){
        int unpaddedRowSize = imageData.width * imageData.bytesPerPixel;
        for (int i = 0; i < imageData.height; i++) {
            for (int j = imageData.width - 1; j >= 0; j--) {
                int pixelOffset = i * unpaddedRowSize + j * imageData.bytesPerPixel;
                fwrite(&imageData.pixels[pixelOffset], 1, imageData.bytesPerPixel, &outputFile); 
            }
        }
    }

    void GaussianFilter(FILE outputFile, int paddedRowSize, ImageData imageData){
        printf("\nНасколько сильный эффект блюра вы хотите получить (от 1 до 10)?:\n");
        int blurRadius;
        scanf("%d", &blurRadius);
        while (blurRadius > 10 || blurRadius < 0) {
            printf("от 1 до 10..\n");
            scanf("%d", &blurRadius);
        } 

        //blur
        byte *blurredPixels = new byte[imageData.width * imageData.height * imageData.bytesPerPixel];
        
        for (int y = 0; y < imageData.height; ++y) {
            for (int x = 0; x < imageData.width; ++x) {
                int redSum = 0;
                int greenSum = 0;
                int blueSum = 0;
                int numPixels = 0;

                for (int j = -blurRadius; j <= blurRadius; ++j) {
                    for (int i = -blurRadius; i <= blurRadius; ++i) {
                        int offsetX = x + i;
                        int offsetY = y + j;

                        // проверим что мы находимся в пределах изображения
                        if (offsetX >= 0 && offsetX < imageData.width && offsetY >= 0 && offsetY < imageData.height) {
                            int pixelOffset = (offsetY * imageData.width + offsetX) * imageData.bytesPerPixel;
                            redSum += imageData.pixels[pixelOffset];
                            greenSum += imageData.pixels[pixelOffset + 1];
                            blueSum += imageData.pixels[pixelOffset + 2];
                            numPixels++;
                        }
                    }
                }

                // Вычисляем средний цвет размытого пикселя
                int pixelOffset = (y * imageData.width + x) * imageData.bytesPerPixel;
                blurredPixels[pixelOffset] = (byte)(redSum / numPixels);
                blurredPixels[pixelOffset + 1] = (byte)(greenSum / numPixels);
                blurredPixels[pixelOffset + 2] = (byte)(blueSum / numPixels);
            }
        }
        
        //write data
        int i = 0;
        int unpaddedRowSize = imageData.width * imageData.bytesPerPixel;
        for ( i = 0; i < imageData.height; i++) {
                //начинаем запись с начала последней строки массива пикселей
                int pixelOffset = ((imageData.height - i) - 1) * unpaddedRowSize;
                fwrite(&blurredPixels[pixelOffset], 1, paddedRowSize, &outputFile);	
        }
        delete [] blurredPixels;
    }

};

#endif