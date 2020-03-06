#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "ClassifierBase.h"
#include "ClassifierCase1.h"
#include "ClassifierCase3.h"
#include "Eigen/Dense"
#include "BoxMuller.h"
#include "gnuplot.h"
#include "Plot.h"
#include "Image.h"
#include "MLEstimation.h"
#include "ROC.h"

using namespace Eigen;

void EvaluateData(std::string name, const std::vector<Data> &data, ClassifierBase *classifier);
void ComputeSampleStatistics(std::vector<float> sampleRatios, const std::vector<Data> &data1, const std::vector<Data> &data2);
void TrainImageClassifier(std::string imageName, std::string maskFileName, Vector2f& mean1, Matrix2f& covariance1, Vector2f& mean2, Matrix2f& covariance2);
void CreateImageOutputs(std::string title,
                        std::string imageName,
                        std::string maskName,
                        std::string outputName_NormalRGB,
                        std::string outputName_CrCb,
                        std::string outputName_Masked,
                        ClassifierCase3Thresholded & classifier_NormalRGB,
                        ClassifierCase3Thresholded & classifier_CrCb);

int main(int argc, char *argv[])
{
    if(argc >= 4)
    {
        Image<RGB> image1(argv[1]);
        image1.WriteToFile(argv[2]);

        Image<RGB> mask(argv[3]);
        image1.ExtractSkinPixels(mask);
    } else
    {
        std::cout << "Please enter input and output file as cmdline args" << std::endl;
    }
    
    //Data set 1
    std::vector<VectorXf> mean1;
    mean1.push_back(VectorXf(2));
    mean1[0] << 1.0f, 1.0f;
    mean1.push_back(VectorXf(2));
    mean1[1] << 4.0f, 4.0f;

    std::vector<MatrixXf> covariance1;
    covariance1.push_back(MatrixXf(2, 2));
    covariance1.push_back(MatrixXf(2, 2));
    covariance1[0] << 1, 0,
                      0, 1;
    covariance1[1] << 1, 0,
                      0, 1;

    //Data set 2
    std::vector<VectorXf> mean2;
    mean2.push_back(VectorXf(2));
    mean2[0] << 1.0f, 1.0f;
    mean2.push_back(VectorXf(2));
    mean2[1] << 4.0f, 4.0f;

    std::vector<MatrixXf> covariance2;
    covariance2.push_back(MatrixXf(2, 2));
    covariance2[0] << 1, 0,
                      0, 1;
    covariance2.push_back(MatrixXf(2, 2));
    covariance2[1] << 4, 0,
                      0, 8;

    //Prior probabilities
    /*
    std::vector<float> priorProb1;
    priorProb1.push_back(0.5f);
    priorProb1.push_back(0.5f);

    std::vector<float> priorProb2;
    priorProb2.push_back(0.2f);
    priorProb2.push_back(0.8f);
    */

    long numberOfPoints = 200000;
    std::vector<std::vector<Data>> points1(2);
    points1[0].reserve(numberOfPoints);
    points1[1].reserve(numberOfPoints);

    std::vector<std::vector<Data>> points2(2);
    points2[0].reserve(numberOfPoints);
    points2[1].reserve(numberOfPoints);

    for(long int i = 0; i < numberOfPoints / 2; ++i)
    {
        for(int j = 0; j < 2; ++j){
            Data d(2);
            float x = box_muller(mean1[j](0, 0), sqrt(covariance1[j](0, 0)));
            float y = box_muller(mean1[j](1, 0), sqrt(covariance1[j](1, 1)));
            d.feature(0, 0) = x;
            d.feature(1, 0) = y;
            d.label = j;
            points1[j].push_back(d);
        }

        for(int j = 0; j < 2; ++j){
            Data d(2);
            float x = box_muller(mean2[j](0, 0), sqrt(covariance2[j](0, 0)));
            float y = box_muller(mean2[j](1, 0), sqrt(covariance2[j](1, 1)));
            d.feature(0, 0) = x;
            d.feature(1, 0) = y;
            d.label = j;
            points2[j].push_back(d);
        }
    }



    Vector2f mean = GetSampleMean(points2[1]);
    std::cout << mean << std::endl;

    Matrix2f covariance = GetSampleCovariance(points2[1]);
    std::cout << covariance << std::endl;

    std::vector<Data> sample = GetRandomSample(points2[1], 0.001);
    std::cout << std::endl << "Data size: " << sample.size() << std::endl;

    mean = GetSampleMean(sample);
    std::cout << mean << std::endl;

    covariance = GetSampleCovariance(sample);
    std::cout << covariance << std::endl;

    std::cout << "-----------------\n\n\n\n-----------------" << std::endl;

    std::vector<float> sampleRatios({1, 0.1, 0.01, 0.001, 0.0001});
    std::cout << "Problem 1" << std::endl;
    ComputeSampleStatistics(sampleRatios, points1[0], points1[1]);
    std::cout << "Problem 2" << std::endl;
    ComputeSampleStatistics(sampleRatios, points2[0], points2[1]);


    Vector2f mean_CrCb;
    Matrix2f covariance_CrCb;
    Vector2f mean_NormalRGB;
    Matrix2f covariance_NormalRGB;

    TrainImageClassifier("./Data_Prog2/Training_1.ppm", "./Data_Prog2/ref1.ppm", mean_CrCb, covariance_CrCb, mean_NormalRGB, covariance_NormalRGB);
    ClassifierCase3Thresholded classifier_NormalRGB({mean_NormalRGB}, {covariance_NormalRGB}, {0});
    ClassifierCase3Thresholded classifier_CrCb({mean_CrCb}, {covariance_CrCb}, {0});

    CreateImageOutputs("Training1",
                       "./Data_Prog2/Training_1.ppm",
                       "./Data_Prog2/ref1.ppm",
                       "./Data_Prog2/output1_NRGB.ppm",
                       "./Data_Prog2/output1_CrCb.ppm",
                       "./Data_Prog2/_output1_Masked.ppm",
                       classifier_NormalRGB,
                       classifier_CrCb);

    CreateImageOutputs("Training3",
                       "./Data_Prog2/Training_3.ppm",
                       "./Data_Prog2/ref3.ppm",
                       "./Data_Prog2/output3_NRGB.ppm",
                       "./Data_Prog2/output3_CrCb.ppm",
                       "./Data_Prog2/_output3_Masked.ppm",
                       classifier_NormalRGB,
                       classifier_CrCb);

    CreateImageOutputs("Training6",
                       "./Data_Prog2/Training_6.ppm",
                       "./Data_Prog2/ref6.ppm",
                       "./Data_Prog2/output6_NRGB.ppm",
                       "./Data_Prog2/output6_CrCb.ppm",
                       "./Data_Prog2/_output6_Masked.ppm",
                       classifier_NormalRGB,
                       classifier_CrCb);
    return 0;
}

void EvaluateData(std::string name, const std::vector<Data> &data, ClassifierBase *classifier){
    //PlotParams plotParams = classifier->GetPlotParams();
    std::cout << name << std::endl;
    std::vector<MisclassificationData> misclassifications = classifier->GetMisclassification(data);
    int totalMisclassified = 0;
    for(unsigned int i = 0; i < misclassifications.size(); ++i){
        totalMisclassified += misclassifications[i].falsePositives;
        std::cout << "  Class " << i << ":" << std::endl;
        std::cout << "      False positives: " << misclassifications[i].falsePositives << " - (" << 200.0f * misclassifications[i].falsePositives / data.size() << "%)" << std::endl;
        std::cout << "      False negatives: " << misclassifications[i].falseNegatives << " - (" << 200.0f * misclassifications[i].falseNegatives / data.size() << "%)" << std::endl;
    }
    std::cout << "  Total model misclassifications: " << totalMisclassified << " - (" << 100.0f * totalMisclassified / data.size() << "%)" << std::endl;
    std::cout << "  Bhattacharyya error bound: " << 100.0f * classifier->GetErrorBound() << "%" << std::endl << std::endl;
    //plotCompare(name, data, plotParams, false);
}

void ComputeSampleStatistics(std::vector<float> sampleRatios, const std::vector<Data> &data1, const std::vector<Data> &data2)
{
    for(uint i = 0; i < sampleRatios.size(); i++)
    {
        std::vector<Data> sample1 = GetRandomSample(data1, sampleRatios[i]);
        Vector2f mean1 = GetSampleMean(sample1);
        Matrix2f covariance1 = GetSampleCovariance(sample1);

        std::vector<Data> sample2 = GetRandomSample(data2, sampleRatios[i]);
        Vector2f mean2 = GetSampleMean(sample2);
        Matrix2f covariance2 = GetSampleCovariance(sample2);

        ClassifierCase3 classifier({mean1, mean2}, {covariance1, covariance2}, {0.5f, 0.5f});

        std::vector<Data> combinedData(data1);
        combinedData.insert(combinedData.end(), data2.begin(), data2.end());

        EvaluateData("Sample: " + std::to_string(sampleRatios[i] * 100) + "%", combinedData, &classifier);
    }
}

void TrainImageClassifier(std::string imageName, std::string maskFileName, Vector2f& mean1, Matrix2f& covariance1, Vector2f& mean2, Matrix2f& covariance2){
    Image<RGB> inputImage((char*)imageName.c_str());
    Image<RGB> maskImage((char*)maskFileName.c_str());
    std::vector<RGB> skinPixels = inputImage.ExtractSkinPixels(maskImage);

    std::vector<CrCb> pixels_CrCb = RGB::ToCrCb(skinPixels);
    std::vector<NormalRGB> pixels_NormalRGB = RGB::ToNormalRGB(skinPixels);

    std::vector<Data> features_CrCb = CrCb::ToDataVector(pixels_CrCb, {}, 1);
    std::vector<Data> features_NormalRGB = NormalRGB::ToDataVector(pixels_NormalRGB, {}, 1);

    mean1 = GetSampleMean(features_CrCb);
    covariance1 = GetSampleCovariance(features_CrCb);
    mean2 = GetSampleMean(features_NormalRGB);
    covariance2 = GetSampleCovariance(features_NormalRGB);
}

// Output ROC curves and classified images
// Input: training image name, mask name, output image name, classifier
void CreateImageOutputs(std::string title,
                        std::string imageName,
                        std::string maskName,
                        std::string outputName_NormalRGB,
                        std::string outputName_CrCb,
                        std::string outputName_Masked,
                        ClassifierCase3Thresholded & classifier_NormalRGB,
                        ClassifierCase3Thresholded & classifier_CrCb)
{
    // Import image
    Image<RGB> image((char*)(imageName).c_str());
    Image<RGB> mask((char*)maskName.c_str());

    // Create NRGB and CrCb vectors
    std::vector<int> classMask = mask.GetFlattenedMask();
    std::vector<Data> features_NormalRGB = NormalRGB::ToDataVector(RGB::ToNormalRGB(image.FlattenedPixels()), classMask);
    std::vector<Data> features_CrCb = CrCb::ToDataVector(RGB::ToCrCb(image.FlattenedPixels()), classMask);

    // Get ROC values for both data sets using classifier
    std::cout << "Calculating discriminants..." << std::endl;
    vector<float> discriminants_NormalRGB = GetDiscriminants(features_NormalRGB, classifier_NormalRGB);
    vector<float> discriminants_CrCb = GetDiscriminants(features_CrCb, classifier_CrCb);

    const int numThresholds = 100;
    ThresholdRange threshold_NormalRGB = GetThresholdRange(discriminants_NormalRGB, numThresholds);
    ThresholdRange threshold_CrCb = GetThresholdRange(discriminants_CrCb, numThresholds);

    std::vector<MisclassificationData> roc_NormalRGB = GetROCValues(threshold_NormalRGB, discriminants_NormalRGB, features_NormalRGB);
    std::vector<MisclassificationData> roc_CrCb = GetROCValues(threshold_CrCb, discriminants_CrCb, features_CrCb);

    // Get BestThreshold for both data sets
    const float FNtoFPRatio = 0.30;
    float bestThresh_NormalRGB = GetBestThreshold(threshold_NormalRGB, roc_NormalRGB, FNtoFPRatio);
    float bestThresh_CrCb = GetBestThreshold(threshold_CrCb, roc_CrCb, FNtoFPRatio);

    // Plot ROC curves for both data sets
    PlotROC(title, {roc_NormalRGB, roc_CrCb}, {"Normal_RGB", "YCrCb"});

    // Get classified pixels using best threshold
    std::vector<int> classified_NormalRGB = ClassfyPixels(bestThresh_NormalRGB, discriminants_NormalRGB);
    std::vector<int> classified_CrCb = ClassfyPixels(bestThresh_CrCb, discriminants_CrCb);

    // Generate ouput image
    Image<RGB> output_NormalRGB = image.GetClassifiedImage(classified_NormalRGB);
    Image<RGB> output_CrCb = image.GetClassifiedImage(classified_CrCb);
    Image<RGB> output_Masked = image.GetClassifiedImage(classMask);

    output_NormalRGB.WriteToFile((char*)outputName_NormalRGB.c_str());
    output_CrCb.WriteToFile((char*)outputName_CrCb.c_str());
    output_Masked.WriteToFile((char*)outputName_Masked.c_str());
}