#include "Eigenface.h"

#include "Eigen/Eigenvalues"

#if defined(unix) || defined(__unix__) || defined(__unix)
    #include "dirent.h"
#else
    #include "direntWindows.h"
#endif

#include <algorithm>
#include <numeric>
#include <regex>

#include "Image.h"

// Initializes the averageFace, eigenfaces, eigenvalues, eigenspaceTrainingValues
Eigenface::Eigenface(std::string trainingDirectory)
{
    std::cout << "Loading training image files..." << std::endl;
    GetTrainingData(trainingDirectory);
}

// testingImages - (N^2 x K)-matrix of testing images
// infoRatio - percent information perserved / percent eigenvalues used
// Returns the image index of the closest topMatches matches for each column of testingImages
std::vector<std::vector<int>> Eigenface::GetClosestMatches(MatrixXf testingImages, int topMatches, float infoRatio) const
{
    // Subtract mean face
    MatrixXf normalizedTestingImages = NormalizeImages(testingImages);
    // Compute eigenspace representation
    MatrixXf testingEigenspaceValues = ComputeEigenSpaceValues(normalizedTestingImages);

    std::vector<std::vector<int>> topMatchesAllImages(testingImages.cols());
    for(int i = 0; i < testingEigenspaceValues.cols(); i++)
    {
        std::vector<float> currentTopMatches(eigenspaceTrainingValues.cols());
        VectorXf currentTestingEigenspaceValues = testingEigenspaceValues.col(i);

        // For every training eigenspace representation
        for(int j = 0; j < eigenspaceTrainingValues.cols(); j++)
        {
            VectorXf currentTrainingEigenspaceValues = eigenspaceTrainingValues.col(j);

            // Compute Mahalanobis distance between eigenspace coefficients
            currentTopMatches[j] = MahalanobisDistance(currentTestingEigenspaceValues, currentTrainingEigenspaceValues, (int) (eigenfaces.cols() * infoRatio));
        }

        // Find the top N faces with the lowest distance
        std::vector<int> currentIndices(currentTopMatches.size());
        std::iota(currentIndices.begin(), currentIndices.end(), 0);

        std::sort(currentIndices.begin(), currentIndices.end(), [&currentTopMatches] (int lhs, int rhs) {
            return currentTopMatches[lhs] < currentTopMatches[rhs];
        });

        topMatchesAllImages[i] = std::vector<int>(currentIndices.begin(), currentIndices.begin() + topMatches);
    }

    return topMatchesAllImages;
}

// testingImages - (N^2 x K)-matrix of testing images
// infoRatio - percent information perserved / percent eigenvalues used
// Returns the error/distance between the reconstructed and normalized image for each column of testingImages
std::vector<float> Eigenface::GetDetectionError(MatrixXf testingImages, float infoRatio) const
{

}

// Returns the averageFace in image format
Image<GreyScale> Eigenface::GetAverageImage() const
{
    return Image<GreyScale>(averageFace, imageRows, imageCols, imageRange);
}

// Returns the eigenfaces/vectors in image format on range [start, end)
std::vector<Image<GreyScale>> Eigenface::GetEigenfaceImages(int start, int end) const
{
    std::vector<Image<GreyScale>> eigenfaceImages(end - start);
    for(int i = start; i < end; i++)
    {
        // TODO: Take normalized eigenface and convert back to 255 scale
        eigenfaceImages[i - start] = Image<GreyScale>(VectorXf(eigenfaces.col(i) * imageRange), imageRows, imageCols, imageRange);
    }

    return eigenfaceImages;
}

// Performs the work of the constructor
void Eigenface::GetTrainingData(std::string trainingDirectory)
{
    DIR *dir = opendir (trainingDirectory.c_str());
    if (dir == NULL)
    {
        std::cerr << "Could not open training directory!" << std::endl;
        return;
    }
    struct dirent *ent;

    std::vector<VectorXf> images;
    bool imageInfoSet = false;
    while ((ent = readdir(dir)) != NULL)
    {
        if(!std::regex_match(ent->d_name, std::regex(".*\.pgm$")))
        {
            continue;
        }

        // TODO: Sort/Label images by their ID
        // TODO: Remove images from training set (first 50 of problem B)

        //std::cout << "Opening: " << ent->d_name << std::endl;

        Image<GreyScale> currentTrainingImage((char *)(trainingDirectory + std::string(ent->d_name)).c_str());
        images.push_back(currentTrainingImage.FlattenedVector());

        if(!imageInfoSet)
        {
            imageCols = currentTrainingImage.Cols;
            imageRows = currentTrainingImage.Rows;
            imageRange = currentTrainingImage.PixelValueRange;
            imageInfoSet = true;
        }
    }

    closedir (dir);

    if(images.size() == 0)
    {
        std::cerr << "No images were found in the provided training directory!" << std::endl;
        return;
    }

    MatrixXf imagesMatrix(images[0].size(), images.size());
    for(unsigned int i = 0; i < images.size(); i++)
    {
        imagesMatrix.col(i) = images[i];
    }

    std::cout << "Training Eigenfaces..." << std::endl;
    std::cout << "    Finding average face..." << std::endl;
    averageFace = GetAverageFace(imagesMatrix);
    std::cout << "    Normalizing images..." << std::endl;
    MatrixXf normalizedImages = NormalizeImages(imagesMatrix);
    std::cout << "    Finding eigenvectors..." << std::endl;
    SetEigenvaluesEigenvectors(normalizedImages);
}

// Returns the (N^2)-vector of the average of the colums of (N^2 x M)-matrix images
VectorXf Eigenface::GetAverageFace(MatrixXf images) const
{
    return images.rowwise().mean();
}

// normalizedImages - (N^2 x M)-matrix aka A in which the columns are the normalized images
// Sets the eigenvalues and eigenfaces members
// Computes A'A and uses this to find the eigenvalues/vectors of AA'
void Eigenface::SetEigenvaluesEigenvectors(MatrixXf normalizedImages)
{
    MatrixXf dotNormalized = normalizedImages.transpose() * normalizedImages;
    EigenSolver<MatrixXf> solver(dotNormalized);
    eigenvalues = solver.eigenvalues().real();

    // TODO: Normalize correctly, currently getting magnitude of 34.696
        // Also there are values out of range initially which make sense that they appear but how to represent?
    eigenfaces = (normalizedImages * solver.eigenvectors().real()).colwise().normalized();
}

// images - (N^2 x M, K, 1)-matrix
// Returns an (M x M, K, 1)-matrix in which the columns are the eigenspace representation of the columns of images
MatrixXf Eigenface::ComputeEigenSpaceValues(MatrixXf images) const
{
    return eigenfaces.transpose() * images;
}

// images - (N^2 x M, K, 1)-matrix
// Subtracts the average face from each column of images and returns the resulting value
MatrixXf Eigenface::NormalizeImages(MatrixXf images) const
{
    return images.colwise() - averageFace;
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

// TODO: Remove function, based on reading closer ek is said to be the Mahalanobis Distance between test image and closest training image
// Returns the EuclideanDistance between the eigenspace representations of the two images
// eigenspaceImage - (M)-vector
// eigenCount - number of eigenvectors considered in the calculation
    // Must be less than or equal to M
float Eigenface::EuclideanDistance(VectorXf eigenspaceImage1, VectorXf eigenspaceImage2, int eigenCount) const
{

}