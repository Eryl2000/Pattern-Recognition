#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"
#include "CMC.h"

void TestReconstruction(const Eigenface & eigenface, std::string outputImagePath);
void GenerateROCCurve(const Eigenface & eigenface, std::string plotName, std::string testingImageDirectory);
void ExperimentA(std::string trainDirectory, std::string testDirectory, std::string modelPath, std::string outputImagePath);
void ExperimentB(std::string trainDirectory, std::string testDirectory, std::string modelPath, std::string outputImagePath);

int main(int argc, char *argv[])
{
    const std::string inputImagePath("Faces_FA_FB/");
    const std::string outputImagePath("output_images/");
    const std::string trainedModelsPath("trained_models/");

    std::cout << "Project 3" << std::endl;
    
    std::cout << "Part A" << std::endl;
    ExperimentA(inputImagePath + "fa_H/", inputImagePath + "fb_H/", trainedModelsPath + "modela.txt", outputImagePath);

    std::cout << "Part B" << std::endl;
    ExperimentB(inputImagePath + "fa_H/", inputImagePath + "fb_H/", trainedModelsPath + "modelb.txt", outputImagePath);
    
    std::cout << "Part C" << std::endl;
    ExperimentA(inputImagePath + "fa_L/", inputImagePath + "fb_L/", trainedModelsPath + "modelca.txt", outputImagePath);
    ExperimentB(inputImagePath + "fa_L/", inputImagePath + "fb_L/", trainedModelsPath + "modelcb.txt", outputImagePath);

    return 0;
}

void ExperimentA(std::string trainDirectory, std::string testDirectory, std::string modelPath, std::string outputImageDirectory){
    std::string suffix = "_" + trainDirectory.substr(trainDirectory.length() - 2, 1);

    //Create model
    Eigenface eigenface(modelPath, trainDirectory);

    //Average face
    std::cout << "Outputting Average Face..." << std::endl;
    Image<GreyScale> averageFace = eigenface.GetAverageImage();
    averageFace.WriteToFile(outputImageDirectory + "averageFace" + suffix + ".pgm");

    //Highest eigenfaces
    std::cout << "Outputting Eigenfaces..." << std::endl;
    std::vector<Image<GreyScale>> eigenFaces = eigenface.GetEigenfaceImages(0, 10);
    for(unsigned int i = 0; i < eigenFaces.size(); i++)
    {
        eigenFaces[i].WriteToFile(outputImageDirectory + "eigenface_good" + std::to_string(i) + suffix + ".pgm");
    }

    //Lowest eigenfaces
    eigenFaces = eigenface.GetEigenfaceImages(eigenface.eigenfaces.cols() - 10, eigenface.eigenfaces.cols());
    for(unsigned int i = 0; i < eigenFaces.size(); i++)
    {
        eigenFaces[i].WriteToFile(outputImageDirectory + "eigenface_bad" + std::to_string(i) + suffix + ".pgm");
    }

    //CMC data
    std::cout << "Generating CMC data..." << std::endl;
    const std::vector<float> infoRatios = {80.0f, 90.0f, 95.0f};
    std::vector<std::vector<std::pair<float, float>>> dataPoints;
    dataPoints.reserve(infoRatios.size());
    for(unsigned int i = 0; i < infoRatios.size(); ++i){
        std::cout << "    Generating data for " << infoRatios[i] << "% infoRatio..." << std::endl;
        dataPoints.push_back(CMC::ConvertToPoints(CMC::GenerateCMC(testDirectory, eigenface, infoRatios[i])));
    }

    std::vector<std::string> infoRatioStrings(infoRatios.size());
    for(unsigned int i = 0; i < infoRatioStrings.size(); i++)
    {
        infoRatioStrings[i] = std::to_string(infoRatios[i]) + "%";
    }

    std::cout << "Plotting combined CMC curve..." << std::endl;
    Plot::PlotPairs("Eigenspace CMC" + suffix, {"N", "Accuracy"}, infoRatioStrings, dataPoints);

    std::cout << std::endl;
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

    std::cout << "Positive count: " << positiveCount << ", Negative count: " << negativeCount << std::endl;
    ROC::PlotROC(plotName, {rocValues}, {"MahalanobisDistance"}, {positiveCount, negativeCount}, {false, true}, true);
}

void ExperimentB(std::string trainDirectory, std::string testDirectory, std::string modelPath, std::string outputImagePath)
{
    std::string suffix = "_" + trainDirectory.substr(trainDirectory.length() - 2, 1);

    std::cout << "Training intruder eigenfaces" << std::endl;
    const int intruderSubjectCount = 50;
    Eigenface eigenfaceIntruder(modelPath, trainDirectory, intruderSubjectCount);

    std::cout << "Generating ROC curve" << std::endl;
    GenerateROCCurve(eigenfaceIntruder, "ROC Eigenfaces" + suffix, testDirectory);
}