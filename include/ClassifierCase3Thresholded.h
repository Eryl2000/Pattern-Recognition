#ifndef CLASSIFIER_CASE_3_THRESHOLDED_H_
#define CLASSIFIER_CASE_3_THRESHOLDED_H_

#include <vector>
#include <Eigen/Dense>
#include "ClassifierBase.h"

using namespace Eigen;

class ClassifierCase3Thresholded : public ClassifierBase{
public:
    ClassifierCase3Thresholded(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb);
    
    virtual int Classify(const VectorXf &input) const;
    float GetDiscriminantValue(const VectorXf &input) const;
    void SetThreshold(float threshold);
    virtual PlotParams GetPlotParams();
};

#endif
