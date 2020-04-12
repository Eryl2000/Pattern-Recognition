#include <iostream>
#include <vector>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"

int main(int argc, char *argv[])
{
    std::cout << "Project 3" << std::endl;
    std::cout << "Training Eigenfaces" << std::endl;
    Eigenface eigenface("./Faces_FA_FB/fa_H/");
    std::cout << "Outputting Average Face" << std::endl;
    Image<GreyScale> averageFace = eigenface.GetAverageImage();
    averageFace.WriteToFile("averageFace.pgm");
    std::vector<Image<GreyScale>> eigenFaces = eigenface.GetEigenfaceImages(0, 10);
    for(uint i = 0; i < eigenFaces.size(); i++)
    {
        eigenFaces[i].WriteToFile((char *)("eigenface_good" + std::to_string(i) + ".pgm").c_str());
    }

    eigenFaces = eigenface.GetEigenfaceImages(eigenface.eigenfaces.cols() - 10, eigenface.eigenfaces.cols());
    for(uint i = 0; i < eigenFaces.size(); i++)
    {
        eigenFaces[i].WriteToFile((char *)("eigenface_bad" + std::to_string(i) + ".pgm").c_str());
    }

    return 0;
}