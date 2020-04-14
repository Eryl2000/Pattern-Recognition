#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"
#include "CMC.h"

void TestReconstruction(const Eigenface & eigenface, std::string outputImagePath);
void GenerateROCCurve(const Eigenface & eigenface, std::string plotName, std::string testingImageDirectory);

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

    std::cout << "Training intruder eigenfaces" << std::endl;
    const int intruderSubjectCount = 50;
    Eigenface eigenfaceIntruder(trainedModelsPath + "modelb.txt", "./Faces_FA_FB/fa_H/", intruderSubjectCount);

    std::cout << "Generating ROC curve" << std::endl;
    GenerateROCCurve(eigenfaceIntruder, "ROC Eigenfaces", "./Faces_FA_FB/fb_H/");

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

void GenerateROCCurve(const Eigenface & eigenface, std::string plotName, std::string testingImageDirectory)
{
    const int infoRatio = 95;
    const int numThresholds = 100;

    std::vector<std::string> imageNames;

    // not usefull, not need for image info (unless testing image is different from training for some reason)
    Image<GreyScale> exampleImage;

    // should be 0, not usefull
    int intuderCount;

    MatrixXf testingImages = eigenface.GetImageMatrix(testingImageDirectory, imageNames, exampleImage, intuderCount);
    std::vector<float> errorValues = eigenface.GetDetectionError(testingImages, infoRatio);

    if(errorValues.size() != (unsigned int)testingImages.cols())
    {
        std::cerr << "main GenerateROCCurve called eigenface.GetDetectionError and the count of errorValues did not match the number of columns in the testing images matrix" << std::endl;
        return;
    }

    ROC::ThresholdRange thresh = ROC::GetThresholdRange(errorValues, numThresholds);

    std::vector<bool> classification(errorValues.size());
    std::unordered_set<std::string> intruderNames(eigenface.imageNames.begin(), eigenface.imageNames.begin() + eigenface.intruderCount);
    int positiveCount = 0;
    int negativeCount = 0;

    for(unsigned int i = 0; i < classification.size(); i++)
    {
        std::unordered_set<std::string>::iterator intrusionMatch = std::find_if(intruderNames.begin(), intruderNames.end(), [&imageNames, i] (const std::string & intruderName)
        {
            return Eigenface::IsSameSubject(imageNames[i], intruderName);
        });
        classification[i] = intrusionMatch == intruderNames.end();
        classification[i] ? positiveCount++ : negativeCount++;
    }

    std::vector<MisclassificationData> rocValues = ROC::GetROCValues(thresh, errorValues, classification);

    // TODO: Plot normalized ROC data (divide by total postives/negatives)
    std::cout << "Positive count: " << positiveCount << ", Negative count: " << negativeCount << std::endl;
    ROC::PlotROC(plotName, {rocValues}, {"MahalanobisDistance"}, {positiveCount, negativeCount}, {false, true}, true);
}