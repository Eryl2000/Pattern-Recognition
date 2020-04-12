#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

#include "Eigen/Dense"
#include "Data.h"

using namespace Eigen;

class CrCb{
public:
    float Cr;
    float Cb;

    static std::vector<Data> ToDataVector(std::vector<CrCb> pixels_CrCb, std::vector<int> flattenedMask, int singleClass = -1)
    {
        if(singleClass == -1 && pixels_CrCb.size() != flattenedMask.size())
        {
            throw std::invalid_argument("ToDataVector must either take a single class value or the pixels and mask vectors must be the same length");
        }

        std::vector<Data> features_CrCb;
        features_CrCb.resize(pixels_CrCb.size());
        for(unsigned int i = 0; i < pixels_CrCb.size(); ++i){
            features_CrCb[i] = Data(2);
            features_CrCb[i].feature(0, 0) = pixels_CrCb[i].Cr;
            features_CrCb[i].feature(1, 0) = pixels_CrCb[i].Cb;

            if(singleClass == -1)
            {
                features_CrCb[i].label = flattenedMask[i];
            } else
            {
                features_CrCb[i].label = singleClass;
            }
        }

        return features_CrCb;
    }
};

class NormalRGB{
public:
    float r;
    float g;

    static std::vector<Data> ToDataVector(std::vector<NormalRGB> pixels_NormalRGB, std::vector<int> flattenedMask, int singleClass = -1)
    {
        if(singleClass == -1 && pixels_NormalRGB.size() != flattenedMask.size())
        {
            throw std::invalid_argument("ToDataVector must either take a single class value or the pixels and mask vectors must be the same length");
        }

        std::vector<Data> features_NormalRGB;
        features_NormalRGB.resize(pixels_NormalRGB.size());
        for(unsigned int i = 0; i < pixels_NormalRGB.size(); ++i){
            features_NormalRGB[i] = Data(2);
            features_NormalRGB[i].feature(0, 0) = pixels_NormalRGB[i].r;
            features_NormalRGB[i].feature(1, 0) = pixels_NormalRGB[i].g;

            if(singleClass == -1)
            {
                features_NormalRGB[i].label = flattenedMask[i];
            } else
            {
                features_NormalRGB[i].label = singleClass;
            }
        }

        return features_NormalRGB;
    }
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

    bool operator==(const RGB& other) const{
        return (r == other.r) && (g == other.g) && (b == other.b);
    }

    bool operator!=(const RGB& other) const{
        return !(*this == other);
    }

    void operator=(const RGB& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
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
    Image();
    Image(std::string fileName);
    Image(const Image &other);
    Image(VectorXf vect, int _Rows, int _Cols, int _PixelValueRange = 255);
    ~Image();

    Image & operator=(const Image & other);

    void WriteToFile(std::string fileName);
    std::vector<RGB> ExtractSkinPixels(Image<RGB> mask) const;
    std::vector<PixelType> FlattenedPixels() const;
    VectorXf FlattenedVector() const;
    std::vector<int> GetFlattenedMask() const;
    Image<PixelType> GetClassifiedImage(std::vector<int> flattenedSkinClassification);

    void SetBlack(PixelType & pixel);
    bool IsBlack(PixelType pixel) const;

    int Rows;
    int Cols;
    int PixelValueRange;
    std::vector<std::vector<PixelType>> Pixels;

private:
    void setData(std::string fileName, std::ifstream &ifp);
};

#endif