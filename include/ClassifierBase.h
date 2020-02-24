#ifndef CLASSIFIER_BASE_H_
#define CLASSIFIER_BASE_H_

#include <vector>
#include <Eigen/Dense>

using namespace Eigen;

class Data
{
public:
    Data(int featureSize)
    {
        feature.resize(featureSize);
    }
    VectorXf feature;
    int label;
};

class PlotParams{
public:
    float a;
    float b;
    float c;
};

class MisclassificationData{
public:
    MisclassificationData(){
        falsePositives = 0;
        falseNegatives = 0;
    }
    int falsePositives;
    int falseNegatives;
};


class ClassifierBase{
public:
    ClassifierBase(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb){
        means = _means;
        covariances = _covariances;
        priorProb = _priorProb;
        numberOfClasses = means.size();
    }

    virtual ~ClassifierBase(){

    }

    //Returns the number of missclassified data-points 
    std::vector<MisclassificationData> GetMisclassification(const std::vector<Data> &data){
        std::vector<MisclassificationData> errors(numberOfClasses, MisclassificationData());
        for(auto dataPoint : data){
            int classification = Classify(dataPoint.feature);
            if(classification != dataPoint.label){
                errors[classification].falsePositives++;
                errors[dataPoint.label].falseNegatives++;
            }
        }
        return errors;
    }

    //Returns the index of the class the input belongs to
    virtual int Classify(const VectorXf &input) = 0;

    //Returns the parameters for the decision boundary
    virtual PlotParams GetPlotParams() = 0;

    //Returns the maximum error of the classifier
    float GetErrorBound()
    {
        float beta = 0.5f;
        float term1 = 0.5f * beta * (1 - beta) * ((means[0] - means[1]).transpose() * ((1 - beta) * covariances[0] + beta * covariances[1]).inverse() * (means[0] - means[1]))(0, 0);
        float term2 = 0.5f * logf(((1 - beta) * covariances[0] + beta * covariances[1]).determinant() / (powf(covariances[0].determinant(), 1 - beta) * powf(covariances[1].determinant(), beta)));
        float k = term1 + term2;
        float ret = pow(priorProb[0], beta) * pow(priorProb[1], 1 - beta) * exp(-k);
        return ret;
    }

protected:
    std::vector<VectorXf> means;
    std::vector<MatrixXf> covariances;
    std::vector<float> priorProb;

    int numberOfClasses;
};





#endif
