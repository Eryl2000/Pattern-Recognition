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
    RGB(){
        r = 0;
        g = 0;
        b = 0;
    }

    RGB(int _r, int _g, int _b){
        r = _r;
        g = _g;
        b = _b;
    }

    bool operator==(const RGB& other){
        return (r == other.r) && (g == other.g) && (b == other.b);
    }

    bool operator!=(const RGB& other){
        return !(*this == other);
    }

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

    static RGB Black(){
        return RGB(0, 0, 0);
    }

    static RGB White(){
        return RGB(255, 255, 255);
    }

    int r;
    int g;
    int b;
};

template <typename PixelType>
class Image {
public:
    Image(char *fileName);
    Image(Image &other);
    ~Image();

    void WriteToFile(char *fileName);
    std::vector<RGB> ExtractSkinPixels(Image<RGB> mask) const;

    int Rows;
    int Cols;
    int PixelValueRange;
    std::vector<std::vector<PixelType>> Pixels;

private:
    void setData(char *fileName, std::ifstream &ifp);
};

#include "Image.cpp"

#endif