#ifndef EIGENFACE_H_
#define EIGENFACE_H_

#include <string>
#include <vector>
#include "Eigen/Dense"

using namespace Eigen;

// N^2 - number of pixels in an image
// M - number of images in the training set
// K - number of images in the testing set

class Eigenface
{
    public:
        // (N^2)-vector
        VectorXf averageFace;

        // (N^2 x M)-matrix with eigenvectors/faces are the columns
        MatrixXf eigenfaces;

        // (M)-vector of eigenvalues that correspond to the eigenfaces
        VectorXf eigenvalues;

        // (M x M)-matrix with columns of the eigenspace representation of each training image
        MatrixXf eigenspaceTrainingValues;

        // Initializes the averageFace, eigenfaces, eigenvalues, eigenspaceTrainingValues
        Eigenface(std::string trainingDirectory);

        // testingImages - (N^2 x K)-matrix of testing images
        // Returns the image ID of the closest topMatches matches for each column of testingImages
        std::vector<std::vector<int>> GetClosestMatches(MatrixXf testingImages, int topMatches);

        // testingImages - (N^2 x K)-matrix of testing images
        // Returns the error/distance between the reconstructed and normalized image for each column of testingImages
        std::vector<float> GetDetectionError(MatrixXf testingImages);
    

    private:
        // Performs the work of the constructor
        void GetTrainingData(std::string trainingDirectory);

        // Returns the (N^2)-vector of the average of the colums of (N^2 x M)-matrix images
        VectorXf GetAverageFace(MatrixXf images);

        // normalizedImages - (N^2 x M)-matrix aka A in which the columns are the normalized images
        // Sets the eigenvalues and eigenfaces members
        // Computes A'A and uses this to find the eigenvalues/vectors of AA'
        void SetEigenvaluesEigenvectors(MatrixXf normalizedImages);

        // images - (N^2 x M, K, 1)-matrix
        // Returns an (M x M, K, 1)-matrix in which the columns are the eigenspace representation of the columns of images
        MatrixXf ComputeEigenSpaceValues(MatrixXf images);

        // images - (N^2 x M, K, 1)-matrix
        // Subtracts the average face from each column of images and returns the resulting value
        MatrixXf NormalizeImages(MatrixXf images);

        // eigenspaceValues - (M x M, K, 1)-matrix
        // eigenCount - number of eigenvectors used in the reconstruction
            // Must be less than or equal to M
        // Reconstructs the columns of eigenspaceValues using the first eigenCount eigenfaces
        MatrixXf ReconstructImages(MatrixXf eigenspaceValues, int eigenCount);

        // Outputs the averageFace, eigenFaces, and the eigenspaceTrainingValues to a file
        void OutputTrainingData(std::string outputFileName);

        // Returns the MahalanobisDistance between the eigenspace representations of the two images
        // eigenspaceImage - (M)-vector
        // eigenCount - number of eigenvectors considered in the calculation
            // Must be less than or equal to M
        float MahalanobisDistance(VectorXf eigenspaceImage1, VectorXf eigenspaceImage2, int eigenCount);

        // Returns the EuclideanDistance between the eigenspace representations of the two images
        // eigenspaceImage - (M)-vector
        // eigenCount - number of eigenvectors considered in the calculation
            // Must be less than or equal to M
        float EuclideanDistance(VectorXf eigenspaceImage1, VectorXf eigenspaceImage2, int eigenCount);
};


#endif