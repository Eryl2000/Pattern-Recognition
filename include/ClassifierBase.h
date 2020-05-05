#ifndef CLASSIFIER_BASE_H_
#define CLASSIFIER_BASE_H_

#include <vector>
#include <Eigen/Dense>

#include "Data.h"
#include "Plot.h"

using namespace Eigen;

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
    std::vector<MisclassificationData> GetMisclassification(const std::vector<Data> &data) const{
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

    float GetErrorRate(const std::vector<Data> & data) const
    {
        int errorCount = 0;
        for(unsigned int i = 0; i < data.size(); i++)
        {
            int classification = Classify(data[i].feature);
            if(data[i].label != classification)
            {
                errorCount++;
            }
        }

        return (float)errorCount / data.size();
    }

    //Returns the index of the class the input belongs to
    virtual int Classify(const VectorXf &input) const = 0;

    //Returns the parameters for the decision boundary
    virtual Plot::PlotParams GetPlotParams() = 0;

    //Returns the maximum error of the classifier
    float GetErrorBound()
    {
        float beta = 0.5f; // Bhattacharyya bound
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
