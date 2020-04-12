#include "Eigenface.h"

// Initializes the averageFace, eigenfaces, eigenvalues, eigenspaceTrainingValues
Eigenface::Eigenface(std::string trainingDirectory)
{

}

// testingImages - (N^2 x K)-matrix of testing images
// Returns the image ID of the closest topMatches matches for each column of testingImages
std::vector<std::vector<int>> Eigenface::GetClosestMatches(MatrixXf testingImages, int topMatches) const
{

}

// testingImages - (N^2 x K)-matrix of testing images
// Returns the error/distance between the reconstructed and normalized image for each column of testingImages
std::vector<float> Eigenface::GetDetectionError(MatrixXf testingImages) const
{

}

// Returns the averageFace in image format
Image<GreyScale> Eigenface::GetAverageImage() const
{

}

// Returns the eigenfaces/vectors in image format on range [start, end)
std::vector<Image<GreyScale>> Eigenface::GetEigenfaceImages(int start, int end) const
{

}

// Performs the work of the constructor
void Eigenface::GetTrainingData(std::string trainingDirectory)
{

}

// Returns the (N^2)-vector of the average of the colums of (N^2 x M)-matrix images
VectorXf Eigenface::GetAverageFace(MatrixXf images) const
{

}

// normalizedImages - (N^2 x M)-matrix aka A in which the columns are the normalized images
// Sets the eigenvalues and eigenfaces members
// Computes A'A and uses this to find the eigenvalues/vectors of AA'
void Eigenface::SetEigenvaluesEigenvectors(MatrixXf normalizedImages)
{

}

// images - (N^2 x M, K, 1)-matrix
// Returns an (M x M, K, 1)-matrix in which the columns are the eigenspace representation of the columns of images
MatrixXf Eigenface::ComputeEigenSpaceValues(MatrixXf images) const
{

}

// images - (N^2 x M, K, 1)-matrix
// Subtracts the average face from each column of images and returns the resulting value
MatrixXf Eigenface::NormalizeImages(MatrixXf images) const
{

}

// eigenspaceValues - (M x M, K, 1)-matrix
// eigenCount - number of eigenvectors used in the reconstruction
    // Must be less than or equal to M
// Reconstructs the columns of eigenspaceValues using the first eigenCount eigenfaces
MatrixXf Eigenface::ReconstructImages(MatrixXf eigenspaceValues, int eigenCount) const
{

}

// Outputs the averageFace, eigenFaces, and the eigenspaceTrainingValues to a file
void Eigenface::OutputTrainingData(std::string outputFileName) const
{

}

// Returns the MahalanobisDistance between the eigenspace representations of the two images
// eigenspaceImage - (M)-vector
// eigenCount - number of eigenvectors considered in the calculation
    // Must be less than or equal to M
float Eigenface::MahalanobisDistance(VectorXf eigenspaceImage1, VectorXf eigenspaceImage2, int eigenCount) const
{

}

// Returns the EuclideanDistance between the eigenspace representations of the two images
// eigenspaceImage - (M)-vector
// eigenCount - number of eigenvectors considered in the calculation
    // Must be less than or equal to M
float Eigenface::EuclideanDistance(VectorXf eigenspaceImage1, VectorXf eigenspaceImage2, int eigenCount) const
{

}