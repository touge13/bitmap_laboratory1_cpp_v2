#include "bmp.hpp"

void WriteImage(const char *fileName, ImageData &imageData, char *action) {
    int32 fileSize;
    int paddedRowSize;

    // Открытие файла в двоичном режиме
    FILE *outputFile = fopen(fileName, "wb");
    // Проверка, открыт ли файл успешно
    if (outputFile == NULL) {
        printf("Не удалось открыть файл\n");
        return;
    }
    
    int32 newWidth;
    int32 newHeight;
    if (strcmp (action, "2") == 0 || strcmp (action, "3") == 0) {
        // Вычисление новых размеров изображения после поворота на 90 градусов
        newWidth = imageData.height;
        newHeight = imageData.width;
        // Вычисление размеров строк и изображения с учетом выравнивания
        paddedRowSize = (((newWidth) * (imageData.bytesPerPixel) + 3) / 4) * 4;
        fileSize = paddedRowSize * newHeight + HEADER_SIZE + INFO_HEADER_SIZE;
    } else {
        // Вычисление размеров строк и изображения с учетом выравнивания
        paddedRowSize = (((imageData.width) * (imageData.bytesPerPixel) + 3) / 4) * 4;
        fileSize = paddedRowSize * imageData.height + HEADER_SIZE + INFO_HEADER_SIZE;
    }

    //*****HEADER************//
    // Запись сигнатуры
    const char *BM = "BM";
    fwrite(&BM[0], 1, 1, outputFile);
    fwrite(&BM[1], 1, 1, outputFile);
    // Запись размера файла
    fwrite(&fileSize, 4, 1, outputFile);
    // Запись зарезервированных значений
    int32 reserved = 0x0000;
    fwrite(&reserved, 4, 1, outputFile);
    // Запись смещения данных
    int32 dataOffset = HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&dataOffset, 4, 1, outputFile);

    //*******INFO*HEADER******//
    // Запись размера информационного заголовка
    int32 infoHeaderSize = INFO_HEADER_SIZE;
    fwrite(&infoHeaderSize, 4, 1, outputFile);

    if (strcmp (action, "2") == 0 || strcmp (action, "3") == 0) {
        // Запись новой ширины и высоты
        fwrite(&newWidth, 4, 1, outputFile);
        fwrite(&newHeight, 4, 1, outputFile);
    } else {
        // Запись новой ширины и высоты
        fwrite(&imageData.width, 4, 1, outputFile);
        fwrite(&imageData.height, 4, 1, outputFile);
    }

    // Запись количества плоскостей (всегда 1)
    int16 planes = 1;
    fwrite(&planes, 2, 1, outputFile);
    // Запись количества бит на пиксель
    int16 bitsPerPixel = imageData.bytesPerPixel * 8;
    fwrite(&bitsPerPixel, 2, 1, outputFile);
    // Запись типа сжатия
    int32 compression = NO_COMPRESSION;
    fwrite(&compression, 4, 1, outputFile);
    // Запись размера изображения
    int32 imageSize = imageData.height * imageData.width * imageData.bytesPerPixel;
    fwrite(&imageSize, 4, 1, outputFile);
    // Запись разрешения по горизонтали и вертикали
    fwrite(&imageData.resolutionX, 4, 1, outputFile);
    fwrite(&imageData.resolutionY, 4, 1, outputFile);
    // Запись количества используемых цветов
    int32 colorsUsed = MAX_NUMBER_OF_COLORS;
    fwrite(&colorsUsed, 4, 1, outputFile);
    // Запись количества важных цветов
    int32 importantColors = ALL_COLORS_REQUIRED;
    fwrite(&importantColors, 4, 1, outputFile);

    // Запись данных
    Functions func;
    if (action == std::string("1")) { //dublication  //or "if (action.compare("1") == 0)" 
        func.Dublication(*outputFile, paddedRowSize, imageData);

    } else if (action == std::string("2")) { //turn right
        func.TurnRight(*outputFile, paddedRowSize, imageData);

    } else if (action == std::string("3")) { //turn left
        func.TurnLeft(*outputFile, paddedRowSize, imageData);

    } else if (action == std::string("4")) { //reflected
        func.Reflected(*outputFile, paddedRowSize, imageData);

    } else if (action == std::string("5")) { //flipped
        func.Flipped(*outputFile, paddedRowSize, imageData);
        
    } else if (action == std::string("6")) { //Gaussian filter
        func.GaussianFilter(*outputFile, paddedRowSize, imageData);
    }

    fclose(outputFile);
}