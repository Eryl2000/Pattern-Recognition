#ifndef CLASSIFIER_CASE_1_H_
#define CLASSIFIER_CASE_1_H_

#include <vector>
#include <Eigen/Dense>
#include "ClassifierBase.h"

using namespace Eigen;

class ClassifierCase1 : public ClassifierBase{
public:
    ClassifierCase1(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb);
    
    //Returns the index of the class the input belongs to
    int Classify(const VectorXf &input);

    //Returns the parameters for the decision boundary
    PlotParams GetPlotParams();

    //Returns the maximum error of the classifier
    //float GetErrorBound();

protected:
    VectorXf W;
    VectorXf X0;
};

#endif