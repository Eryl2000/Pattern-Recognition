#include <bits/stdc++.h> 
#include "ClassifierCase3.h"


ClassifierCase3::ClassifierCase3(const std::vector<VectorXf> &_means,
      const std::vector<MatrixXf> &_covariances,
      const std::vector<float> &_priorProb)
    : ClassifierBase(_means, _covariances, _priorProb){

}

//Returns the index of the class the input belongs to
int ClassifierCase3::Classify(const VectorXf &input){
    MatrixXf A0 = -0.5f * covariances[0].inverse();
    MatrixXf A1 = -0.5f * covariances[1].inverse();
    VectorXf B0 = covariances[0].inverse() * means[0];
    VectorXf B1 = covariances[1].inverse() * means[1];
    float C0 = -0.5f * means[0].transpose() * covariances[0].inverse() * means[0] - 0.5f * logf(covariances[0].determinant()) + logf(priorProb[0]);
    float C1 = -0.5f * means[1].transpose() * covariances[1].inverse() * means[1] - 0.5f * logf(covariances[1].determinant()) + logf(priorProb[1]);

    float G0 = input.dot(A0 * input) + input.dot(B0) + C0;
    float G1 = input.dot(A1 * input) + input.dot(B1) + C1;
    if(G0 >= G1){
        return 0;
    } else{
        return 1;
    }
}

//Returns the parameters for the decision boundary
PlotParams ClassifierCase3::GetPlotParams(){
    VectorXf W = means[0] - means[1];
    VectorXf X0 = 0.5f * (means[0] + means[1]) - (covariances[0](0, 0) * covariances[0](0, 0)) / W.dot(W.transpose()) * logf(priorProb[0] / priorProb[1])*W;

    PlotParams ret;
    ret.a = 0;
    ret.b = -W(0, 0) / W(1, 0);
    ret.c = X0(1, 0) - ret.b * X0(0, 0);
    return ret;
}