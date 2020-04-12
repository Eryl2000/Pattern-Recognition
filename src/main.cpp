#include <iostream>
#include <vector>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"

int main(int argc, char *argv[])
{
    std::string outputImagePath("output_images/");

    std::cout << "Project 3" << std::endl;
    Eigenface eigenface("./Faces_FA_FB/fa_H/");
    std::cout << "Outputting Average Face" << std::endl;
    Image<GreyScale> averageFace = eigenface.GetAverageImage();
    averageFace.WriteToFile(outputImagePath + "averageFace.pgm");

    std::cout << "Outputting Eigenfaces" << std::endl;
    std::vector<Image<GreyScale>> eigenFaces = eigenface.GetEigenfaceImages(0, 10);
    for(unsigned int i = 0; i < eigenFaces.size(); i++)
    {
        eigenFaces[i].WriteToFile(outputImagePath + std::string("eigenface_good" + std::to_string(i) + ".pgm"));
    }

    eigenFaces = eigenface.GetEigenfaceImages(eigenface.eigenfaces.cols() - 10, eigenface.eigenfaces.cols());
    for(unsigned int i = 0; i < eigenFaces.size(); i++)
    {
        eigenFaces[i].WriteToFile(outputImagePath + std::string("eigenface_bad" + std::to_string(i) + ".pgm"));
    }

    Image<GreyScale> reconstructedImage = eigenface.GetImage(VectorXf(eigenface.ReconstructImages(eigenface.eigenspaceTrainingValues.col(0), 100)));
    reconstructedImage.WriteToFile(outputImagePath + "reconstructed.pgm");
    std::cout << "Reconstructed image: " << eigenface.imageNames[0] << std::endl;

    return 0;
}