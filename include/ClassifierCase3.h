#ifndef CLASSIFIER_CASE_3_H_
#define CLASSIFIER_CASE_3_H_

#include "ClassifierCase3Thresholded.h"

class ClassifierCase3 : public ClassifierCase3Thresholded{
public:
    ClassifierCase3(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb);
    
    //Returns the index of the class the input belongs to
    int Classify(const VectorXf &input) const;

    //Returns the parameters for the decision boundary
    Plot::PlotParams GetPlotParams();

protected:
    MatrixXf W0;
    MatrixXf W1;
    VectorXf w0;
    VectorXf w1;
    float w00;
    float w10;
};

#endif
