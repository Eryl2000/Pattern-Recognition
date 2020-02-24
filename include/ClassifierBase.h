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
    virtual float GetErrorBound() = 0;

protected:
    std::vector<VectorXf> means;
    std::vector<MatrixXf> covariances;
    std::vector<float> priorProb;

    int numberOfClasses;
};





#endif
