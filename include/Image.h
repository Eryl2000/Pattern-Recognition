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
        ret.Cr = 0.500f * r + 0.419f * g - 0.081f * b;
        ret.Cb = -0.169f * r - 0.332f * g + 0.500f * b;
        return ret;
    }
    
    NormalRGB ToNormalRGB() const{
        NormalRGB ret;
        ret.r = (float)r / (r + g + b);
        ret.g = (float)g / (r + g + b);
        return ret;
    }

    static std::vector<CrCb> ToCrCb(std::vector<RGB> rgb){
        std::vector<CrCb> ret;
        ret.resize(rgb.size());
        for(unsigned int i = 0; i < rgb.size(); ++i){
            ret[i] = rgb[i].ToCrCb();
        }
        return ret;
    }

    static std::vector<NormalRGB> ToNormalRGB(std::vector<RGB> rgb){
        std::vector<NormalRGB> ret;
        ret.resize(rgb.size());
        for(unsigned int i = 0; i < rgb.size(); ++i){
            ret[i] = rgb[i].ToNormalRGB();
        }
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
        Rows = strtol(header, &nextChar, 0);
        Cols = atoi(nextChar);
        ifp.getline(header, bufferSize, '\n');
        PixelValueRange = strtol(header, &nextChar, 0);

        //Create Pixels vector
        Pixels.resize(Rows);
        for(int i = 0; i < Rows; ++i){
            Pixels[i].resize(Cols);
        }

        //Transfer the data from the file to Pixels array
        setData(fileName, ifp);
    }

    Image(Image &other){
        Rows = other.Rows;
        Cols = other.Cols;
        PixelValueRange = other.PixelValueRange;
        Pixels = other.Pixels;
    }

    ~Image(){

    }

    void WriteToFile(char *fileName);

    int Rows;
    int Cols;
    int PixelValueRange;
    std::vector<std::vector<PixelType>> Pixels;

private:

    void setData(char *fileName, std::ifstream &ifp);
};

template <>
void Image<GreyScale>::setData(char *fileName, std::ifstream &ifp){
    unsigned char *charImage = (unsigned char *) new unsigned char [Rows * Cols];
    ifp.read(reinterpret_cast<char *>(charImage), (Rows * Cols) * sizeof(unsigned char));
    if (ifp.fail()) {
        std::cout << "Image " << fileName << " has wrong size" << std::endl;
        exit(1);
    }
    ifp.close();

    GreyScale val;
    for(int i = 0; i < Rows; ++i){
        for(int j = 0; j < Cols; ++j) {
            val.value = (int)charImage[i * Cols + j];
            Pixels[i][j] = val;
        }
    }
    delete[] charImage;
}

template <>
void Image<RGB>::setData(char *fileName, std::ifstream &ifp){
    unsigned char *charImage = (unsigned char *) new unsigned char [3 * Rows * Cols];
    ifp.read(reinterpret_cast<char *>(charImage), (3 * Rows * Cols) * sizeof(unsigned char));
    if (ifp.fail()) {
        std::cout << "Image " << fileName << " has wrong size" << std::endl;
        exit(1);
    }
    ifp.close();

    RGB val;
    for(int i = 0; i < Rows; ++i){
        for(int j = 0; j < Cols; ++j) {
            val.r = (int)charImage[3 * (i * Cols + j) + 0];
            val.g = (int)charImage[3 * (i * Cols + j) + 1];
            val.b = (int)charImage[3 * (i * Cols + j) + 2];
            Pixels[i][j] = val;
        }
    }
    delete[] charImage;
}

template <>
void Image<GreyScale>::WriteToFile(char *fileName){
    std::ofstream ofp;
    unsigned char *charImage = (unsigned char *) new unsigned char[Rows * Cols];

    for(int i = 0; i < Rows; ++i){
        for(int j = 0; j < Cols; ++j) {
            charImage[i * Cols + j] = (unsigned char)Pixels[i][j].value;
        }
    }
    
    ofp.open(fileName, std::ios::out | std::ios::binary);
    if(!ofp) {
        std::cout << "Can't open file: " << fileName << std::endl;
        exit(1);
    }

    ofp << "P5" << std::endl;
    ofp << Rows << " " << Cols << std::endl;
    ofp << PixelValueRange << std::endl;

    ofp.write(reinterpret_cast<char *>(charImage), (Rows * Cols) * sizeof(unsigned char));
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
    unsigned char *charImage = (unsigned char *) new unsigned char[3 * Rows * Cols];

    for(int i = 0; i < Rows; ++i){
        for(int j = 0; j < Cols; ++j) {
            charImage[3 * (i * Cols + j) + 0] = Pixels[i][j].r;
            charImage[3 * (i * Cols + j) + 1] = Pixels[i][j].g;
            charImage[3 * (i * Cols + j) + 2] = Pixels[i][j].b;
        }
    }
    
    ofp.open(fileName, std::ios::out | std::ios::binary);
    if(!ofp) {
        std::cout << "Can't open file: " << fileName << std::endl;
        exit(1);
    }

    ofp << "P6" << std::endl;
    ofp << Rows << " " << Cols << std::endl;
    ofp << PixelValueRange << std::endl;

    ofp.write(reinterpret_cast<char *>(charImage), (3 * Rows * Cols) * sizeof(unsigned char));
    if(ofp.fail()) {
        std::cout << "Can't write image " << fileName << std::endl;
        exit(0);
    }
    ofp.close();

    delete[] charImage;
}

#endif