#include "bmp.hpp"

//pixels: указатель на массив байтов. Он будет содержать данные пикселей
void ReadImage(const char *fileName, ImageData &imageData) {
        //открываем файл в бинарном режиме
        FILE *imageFile = fopen(fileName, "rb");
        if (!imageFile) {
                printf("Файл не найден\n");
                exit(1);
        }
        
        //чтение data offset
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        fread(&dataOffset, 4, 1, imageFile);      
        //чтение resolutionX
        fseek(imageFile, RESOLUTIONX, SEEK_SET);
        fread(&imageData, 4, 4, imageFile);
        //чтение resolutionY
        fseek(imageFile, RESOLUTIONY, SEEK_SET);
        fread(&imageData.resolutionY, 4, 1, imageFile);
        //чтение ширины
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        fread(&imageData.width, 4, 1, imageFile);
        //чтение высоты
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        fread(&imageData.height, 4, 1, imageFile);
        //чтение количества бита на пиксель
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        fread(&bitsPerPixel, 2, 1, imageFile);

        //выделяем массив пикселей
        *(&imageData.bytesPerPixel) = ((int32)bitsPerPixel) / 8;
        //Каждая строка дополняется так, чтобы ее длина была кратна 4 байтам
        //Рассчитываем размер дополненной строки в байтах
        int paddedRowSize = (((*(&imageData.width)) * (*(&imageData.bytesPerPixel)) + 3) / 4) * 4;
        //Нас не интересуют дополненные байты, поэтому мы выделяем память для данных пикселей
        int unpaddedRowSize = (*(&imageData.width)) * (*(&imageData.bytesPerPixel));
        //Общий размер данных пикселей в байтах
        int totalSize = paddedRowSize * (*(&imageData.height));
        *(&imageData.pixels) = (byte*)malloc(totalSize);

        //Читаем строку данных пикселя
        //Данные дополняются и сохраняются снизу вверх
        int i = 0;
        //укажем на последнюю строку нашего массива пикселей (unpadded)
        byte *currentRowPointer = *(&imageData.pixels) + ((*(&imageData.height) - 1) * unpaddedRowSize);
        for (i = 0; i < *(&imageData.height); i++) {
                //помещаем курсор файла в следующую строку сверху вниз
	        fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
	        //читаем только байты unpaddedRowSize
	        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
	        //укажем на следующую строку (снизу вверх)
	        currentRowPointer -= unpaddedRowSize;
        }

        fclose(imageFile);
}
