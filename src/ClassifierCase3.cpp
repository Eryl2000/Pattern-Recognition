#include <bits/stdc++.h>
#include "ClassifierCase3.h"


ClassifierCase3::ClassifierCase3(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb)
    : ClassifierBase(_means, _covariances, _priorProb){

    W0 = -0.5f * covariances[0].inverse();
    W1 = -0.5f * covariances[1].inverse();
    w0 = covariances[0].inverse() * means[0];
    w1 = covariances[1].inverse() * means[1];
    w00 = -0.5f * means[0].transpose() * covariances[0].inverse() * means[0] - 0.5f * logf(covariances[0].determinant()) + logf(priorProb[0]);
    w10 = -0.5f * means[1].transpose() * covariances[1].inverse() * means[1] - 0.5f * logf(covariances[1].determinant()) + logf(priorProb[1]);
}

//Returns the index of the class the input belongs to
int ClassifierCase3::Classify(const VectorXf &input){
    float G0 = input.dot(W0 * input) + input.dot(w0) + w00;
    float G1 = input.dot(W1 * input) + input.dot(w1) + w10;
    if(G0 >= G1){
        return 0;
    } else{
        return 1;
    }
}

//Returns the parameters for the decision boundary
PlotParams ClassifierCase3::GetPlotParams(){
    float c = W0(0, 1) - W1(0, 1);
    if(c != 0)
    {
        std::cerr << "NON-DIAGONAL COVARIANCE ENTRIES PER CLASS MUST BE EQUAL FOR THE CURRENT PLOTTING IMPLEMENTATION!" << std::endl;
    }

    float a = W0(1, 1) - W1(1, 1);
    float b = W0(0, 0) - W1(0, 0);
    float d = w0(0, 0) - w1(0, 0);
    float f = w0(1, 0) - w1(1, 0);
    float g = w00 - w10;

    PlotParams ret;
    ret.linear = false;
    ret.p1 = -f / (2 * a);
    ret.p2 = -b / a;
    ret.p3 = -d / a;
    ret.p4 = (f * f) / (4 * a * a) - g / a;
    return ret;
}
