#ifndef CLASSIFIER_BASE_H_
#define CLASSIFIER_BASE_H_

#include <vector>
#include <Eigen/Dense>

using namespace Eigen;

class ClassifierResults
{
public:
    ClassifierResults(){
        classificationIndex = -1;
    }
    std::vector<float> discriminants;
    int classificationIndex;
};

class Data
{
public:
    VectorXf feature;
    int label;
};





class ClassifierBase{
public:
    ClassifierBase(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProbabilities){
        means = _means;
        covariances = _covariances;
        priorProbabilities = _priorProbabilities;
    }

    ClassifierResults Classify(const VectorXf &input){
        ClassifierResults ret;
        float highestDiscriminant = 0.0f;
        for(unsigned int i = 0; i < means.size(); ++i){
            ret.discriminants.push_back(discriminant(input, i));
            if(ret.discriminants[i] > highestDiscriminant){
                highestDiscriminant = ret.discriminants[i];
                ret.classificationIndex = i;
            }
        }
        return ret;
    }

    float GetErrorBound(){
        return 0.0f; //TODO: Implement this
    }

    std::vector<int> GetMisclassification(const std::vector<Data> &data){
        std::vector<int> errors(means.size(), 0);
        for(auto dataPoint : data){
            if(dataPoint.label != Classify(dataPoint.feature).classificationIndex){
                errors[dataPoint.label]++;
            }
        }
        return errors;
    }

protected:
    std::vector<VectorXf> means;
    std::vector<MatrixXf> covariances;
    std::vector<float> priorProbabilities;

    virtual float discriminant(const VectorXf &input, int classIndex) = 0;

};





#endif
