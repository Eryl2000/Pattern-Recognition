#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

class CrCb{
public:
    float Cr;
    float Cb;
};

class NormalRGB{
public:
    float r;
    float g;
};

class GreyScale{
public:
    int value;
};

class RGB{
public:
    int r;
    int g;
    int b;

    CrCb ToCrCb() const{
        CrCb ret;
        ret.Cr = 0;
        ret.Cb = 0;
        return ret;
    }
    
    NormalRGB ToNormalRGB() const{
        NormalRGB ret;
        ret.r = (float)r / (r + g + b);
        ret.g = (float)g / (r + g + b);
        return ret;
    }
};

template <typename PixelType>
class Image {
public:
    Image(char *fileName){
        unsigned int bufferSize = 100;
        char header[bufferSize];
        std::ifstream ifp;

        //Open file and check for errors
        ifp.open(fileName, std::ios::in | std::ios::binary);
        if (!ifp) {
            std::cout << "Can't read image: " << fileName << std::endl;
            exit(1);
        }

        //Make sure file type is correct
        ifp.getline(header, bufferSize, '\n');
        if (header[0] != 'P'){
            std::cout << "Image " << fileName << " is not PGM" << std::endl;
            exit(1);
        }

        //Read and skip all comments
        ifp.getline(header, bufferSize, '\n');
        while(header[0] == '#'){
            ifp.getline(header, bufferSize, '\n');
        }
        
        //Read header data
        char *nextChar;
        rows = strtol(header, &nextChar, 0);
        cols = atoi(nextChar);
        ifp.getline(header, bufferSize, '\n');
        valueRange = strtol(header, &nextChar, 0);

        //Create pixelValue array
        pixelValue = new PixelType* [rows];
        for(int i = 0; i < rows; ++i){
            pixelValue[i] = new PixelType[cols];
        }

        //Transfer the data from the file to pixelValue array
        setData(fileName, ifp);
    }

    Image(Image &other){
        rows = other.rows;
        cols = other.cols;
        valueRange = other.valueRange;

        pixelValue = new PixelType* [rows];
        for(int i = 0; i < rows; ++i){
            pixelValue[i] = new PixelType[cols];
        }

        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < cols; ++j){
                pixelValue[i][j] = other.pixelValue[i][j];
            }
        }
    }

    ~Image(){
        for(int j = 0; j < cols; ++j){
            delete[] pixelValue[j];
        }
        delete[] pixelValue;
    }

    int GetRows() const{
        return rows;
    }

    int GetCols() const{
        return cols;
    }

    int GetValueRange() const{
        return valueRange;
    }

    PixelType GetPixelValue(int i, int j) const{
        return pixelValue[i][j];
    }

    void SetPixelValue(int i, int j, PixelType value){
        pixelValue[i][j] = value;
    }

    void WriteToFile(char *fileName);

private:
    int rows, cols, valueRange;
    PixelType **pixelValue;

    void setData(char *fileName, std::ifstream &ifp);
};

template <>
void Image<GreyScale>::setData(char *fileName, std::ifstream &ifp){
    unsigned char *charImage = (unsigned char *) new unsigned char [rows * cols];
    ifp.read(reinterpret_cast<char *>(charImage), (rows * cols) * sizeof(unsigned char));
    if (ifp.fail()) {
        std::cout << "Image " << fileName << " has wrong size" << std::endl;
        exit(1);
    }
    ifp.close();

    GreyScale val;
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j) {
            val.value = (int)charImage[i * cols + j];
            pixelValue[i][j] = val;
        }
    }
    delete[] charImage;
}

template <>
void Image<RGB>::setData(char *fileName, std::ifstream &ifp){
    unsigned char *charImage = (unsigned char *) new unsigned char [3 * rows * cols];
    ifp.read(reinterpret_cast<char *>(charImage), (3 * rows * cols) * sizeof(unsigned char));
    if (ifp.fail()) {
        std::cout << "Image " << fileName << " has wrong size" << std::endl;
        exit(1);
    }
    ifp.close();

    RGB val;
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j) {
            val.r = (int)charImage[3 * (i * cols + j) + 0];
            val.g = (int)charImage[3 * (i * cols + j) + 1];
            val.b = (int)charImage[3 * (i * cols + j) + 2];
            pixelValue[i][j] = val;
        }
    }
    delete[] charImage;
}

template <>
void Image<GreyScale>::WriteToFile(char *fileName){
    std::ofstream ofp;
    unsigned char *charImage = (unsigned char *) new unsigned char[rows * cols];

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j) {
            charImage[i * cols + j] = (unsigned char)pixelValue[i][j].value;
        }
    }
    
    ofp.open(fileName, std::ios::out | std::ios::binary);
    if(!ofp) {
        std::cout << "Can't open file: " << fileName << std::endl;
        exit(1);
    }

    ofp << "P5" << std::endl;
    ofp << rows << " " << cols << std::endl;
    ofp << valueRange << std::endl;

    ofp.write(reinterpret_cast<char *>(charImage), (rows * cols) * sizeof(unsigned char));
    if(ofp.fail()) {
        std::cout << "Can't write image " << fileName << std::endl;
        exit(0);
    }
    ofp.close();

    delete[] charImage;
}

template <>
void Image<RGB>::WriteToFile(char *fileName){
    std::ofstream ofp;
    unsigned char *charImage = (unsigned char *) new unsigned char[3 * rows * cols];

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j) {
            charImage[3 * (i * cols + j) + 0] = pixelValue[i][j].r;
            charImage[3 * (i * cols + j) + 1] = pixelValue[i][j].g;
            charImage[3 * (i * cols + j) + 2] = pixelValue[i][j].b;
        }
    }
    
    ofp.open(fileName, std::ios::out | std::ios::binary);
    if(!ofp) {
        std::cout << "Can't open file: " << fileName << std::endl;
        exit(1);
    }

    ofp << "P6" << std::endl;
    ofp << rows << " " << cols << std::endl;
    ofp << valueRange << std::endl;

    ofp.write(reinterpret_cast<char *>(charImage), (3 * rows * cols) * sizeof(unsigned char));
    if(ofp.fail()) {
        std::cout << "Can't write image " << fileName << std::endl;
        exit(0);
    }
    ofp.close();

    delete[] charImage;
}

#endif