#include <iostream>
#include <vector>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"
#include "CMC.h"

void TestReconstruction(const Eigenface & eigenface, std::string outputImagePath);

int main(int argc, char *argv[])
{
    std::string outputImagePath("output_images/");
    std::string trainedModelsPath("trained_models/");

    std::cout << "Project 3" << std::endl;
    Eigenface eigenface(trainedModelsPath + "model1.txt", "./Faces_FA_FB/fa_H/");

    std::cout << "Outputting Average Face..." << std::endl;
    Image<GreyScale> averageFace = eigenface.GetAverageImage();
    averageFace.WriteToFile(outputImagePath + "averageFace.pgm");

    std::cout << "Outputting Eigenfaces..." << std::endl;
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

    float infoRatio = 80.0f;
    std::cout << "Generating CMC plot for " << infoRatio << "% infoRatio..." << std::endl;
    CMC::GenerateCMC("./Faces_FA_FB/fb_H/", eigenface, infoRatio);

    infoRatio = 90.0f;
    std::cout << "Generating CMC plot for " << infoRatio << "% infoRatio..." << std::endl;
    CMC::GenerateCMC("./Faces_FA_FB/fb_H/", eigenface, infoRatio);

    infoRatio = 95.0f;
    std::cout << "Generating CMC plot for " << infoRatio << "% infoRatio..." << std::endl;
    CMC::GenerateCMC("./Faces_FA_FB/fb_H/", eigenface, infoRatio);

    infoRatio = 5.0f;
    std::cout << "Generating CMC plot for " << infoRatio << "% infoRatio..." << std::endl;
    CMC::GenerateCMC("./Faces_FA_FB/fb_H/", eigenface, infoRatio);

    return 0;
}

void TestReconstruction(const Eigenface & eigenface, std::string outputImagePath)
{
    VectorXf reconstructedVector = eigenface.ReconstructImages(eigenface.eigenspaceTrainingValues.col(0), 100);
    Image<GreyScale> reconstructedImage = eigenface.GetImage(reconstructedVector);
    reconstructedImage.WriteToFile(outputImagePath + "reconstructed.pgm");
    std::cout << "Reconstructed image: " << eigenface.imageNames[0] << std::endl;

    VectorXf originalImage = Image<GreyScale>("./Faces_FA_FB/fa_H/" + eigenface.imageNames[0]).FlattenedVector();

    std::cout << "Reconstruction error: " << (reconstructedVector - originalImage).norm() << std::endl;
}