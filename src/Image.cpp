#include <stdexcept>
#include <algorithm>
#include "Image.h"

template <typename PixelType>
Image<PixelType>::Image()
{
    Rows = 0;
    Cols = 0;
    PixelValueRange = 0;
}

template <typename PixelType>
Image<PixelType>::Image(char *fileName){
    unsigned int bufferSize = 100;
    char header[bufferSize];
    std::ifstream ifp;

    //Open file and check for errors
    ifp.open(fileName, std::ios::in | std::ios::binary);
    if (!ifp) {
        std::cout << "Can't read image: " << fileName << std::endl;
        exit(1);
    }

    /*
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
    */

   std::string magicNumber;
   ifp >> magicNumber;
   // TODO: Handle comments, works for this assignment
   ifp >> Rows;
   ifp >> Cols;
   ifp >> PixelValueRange;

    //Create Pixels vector
    Pixels.resize(Rows);
    for(int i = 0; i < Rows; ++i){
        Pixels[i].resize(Cols);
    }

    //Transfer the data from the file to Pixels array
    setData(fileName, ifp);
}

template <typename PixelType>
Image<PixelType>::Image(const Image &other){
    Rows = other.Rows;
    Cols = other.Cols;
    PixelValueRange = other.PixelValueRange;
    Pixels = other.Pixels;
}

template <typename PixelType>
Image<PixelType> & Image<PixelType>::operator=(const Image<PixelType> & other)
{
    Rows = other.Rows;
    Cols = other.Cols;
    PixelValueRange = other.PixelValueRange;
    Pixels = other.Pixels;

    return *this;
}

template<>
Image<GreyScale>::Image(VectorXf vect, int _Rows, int _Cols, int _PixelValueRange)
{
    Rows= _Rows;
    Cols = _Cols;
    PixelValueRange = _PixelValueRange;
    Pixels.resize(Rows);
    for(int i = 0; i < Rows; i++){
        Pixels[i].resize(Cols);
        for(int j = 0; j < Cols; j++)
        {
            int value = vect(i*Cols+j);
            value = std::clamp(value, 0, PixelValueRange);
            Pixels[i][j].value = value;
        }
    }
}

template <typename PixelType>
Image<PixelType>::~Image(){

}

template <>
std::vector<RGB> Image<RGB>::ExtractSkinPixels(Image<RGB> mask) const{
    std::vector<RGB> ret;
    for(int i = 0; i < mask.Rows; ++i){
        for(int j = 0; j < mask.Cols; ++j)
        {
            if(mask.Pixels[i][j] != RGB::Black()){
                ret.push_back(Pixels[i][j]);
            }
        }
    }
    return ret;
}

template <typename PixelType>
std::vector<PixelType> Image<PixelType>::FlattenedPixels() const
{
    std::vector<PixelType> ret;
    for(unsigned int i = 0; i < Pixels.size(); i++)
    {
        ret.insert(ret.end(), Pixels[i].begin(), Pixels[i].end());
    }

    return ret;
}

template <>
VectorXf Image<GreyScale>::FlattenedVector() const
{
    VectorXf ret = VectorXf::Zero(Rows*Cols);
    for(int i = 0; i < Rows; i++)
    {
        for(int j = 0; j < Cols; j++)
        {
            ret(i*Cols+j) = (float) Pixels[i][j].value;
        }
        
    }

    return ret;
}

template <typename PixelType>
std::vector<int> Image<PixelType>::GetFlattenedMask() const
{
    std::vector<int> ret;
    ret.reserve(Rows*Cols);
    for(int i = 0; i < Rows; ++i){
        for(int j = 0; j < Cols; ++j)
        {
            if(!IsBlack(Pixels[i][j])){
                ret.push_back(1);
            } else
            {
                ret.push_back(0);
            }
        }
        
    }
    return ret;
}

template <typename PixelType>
Image<PixelType> Image<PixelType>::GetClassifiedImage(std::vector<int> flattenedSkinClassification)
{
    if(flattenedSkinClassification.size() != (unsigned int)(Rows * Cols))
    {
        throw std::invalid_argument("GetClassifiedImage flattenedSkinClassification must be Rows * Cols size");
    }

    int no_skinCount = 0;
    Image<PixelType> ret(*this);
    for(int i = 0; i < Rows; ++i){
        for(int j = 0; j < Cols; ++j)
        {
            if(flattenedSkinClassification[i * Cols + j] == 0)
            {
                //std::cout << "Non-skin!" << std::endl;
                SetBlack(ret.Pixels[i][j]);
                no_skinCount++;
            }
        }
        
    }

    std::cout << "No skin: " << no_skinCount << std::endl;

    return ret;
}

template <>
void Image<GreyScale>::SetBlack(GreyScale & pixel)
{
    pixel.value = 0;
}

template <>
bool Image<GreyScale>::IsBlack(GreyScale pixel) const
{
    return pixel.value == 0;
}

template <>
void Image<RGB>::SetBlack(RGB & pixel)
{
    pixel = RGB::Black();
}

template <>
bool Image<RGB>::IsBlack(RGB pixel) const
{
    return pixel == RGB::Black();
}

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

    RGB val(0, 0, 0);
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

template class Image<RGB>;
template class Image<GreyScale>;