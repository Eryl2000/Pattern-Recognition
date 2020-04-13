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
#include <unistd.h>
#include <stdexcept>

#include "Image.h"

// Initializes the averageFace, eigenfaces, eigenvalues, eigenspaceTrainingValues
Eigenface::Eigenface(std::string modelName, std::string trainingDirectory, int intruderSubjects)
{
    if(access(modelName.c_str(), F_OK) != -1){
        std::cout << "    Loading trained model from file..." << std::endl;
        ReadTrainingData(modelName);
    } else{
        std::cout << "Loading training image files..." << std::endl;

        Image<GreyScale> exampleImage;
        MatrixXf imagesMatrix = GetImageMatrix(trainingDirectory, imageNames, exampleImage, intruderCount, intruderSubjects);

        imageCols = exampleImage.Cols;
        imageRows = exampleImage.Rows;
        imageRange = exampleImage.PixelValueRange;

        if(imagesMatrix.rows() == 0 && imagesMatrix.cols() == 0){
            return;
        }

        std::cout << "Training Eigenfaces..." << std::endl;

        std::cout << "    Finding average face..." << std::endl;
        averageFace = GetAverageFace(imagesMatrix);

        std::cout << "    Normalizing images..." << std::endl;
        MatrixXf normalizedImages = NormalizeImages(imagesMatrix);

        std::cout << "    Finding eigenvectors..." << std::endl;
        SetEigenvaluesEigenvectors(normalizedImages);

        std::cout << "    Computing eigenspace representations for each training image..." << std::endl;
        eigenspaceTrainingValues = ComputeEigenSpaceValues(normalizedImages);

        std::cout << "    Saving trained model to file..." << std::endl;
        OutputTrainingData(modelName);
    }
}


// Copy constructor
Eigenface::Eigenface(const Eigenface &other){
    averageFace = other.averageFace;
    eigenfaces = other.eigenfaces;
    eigenvalues = other.eigenvalues;
    eigenspaceTrainingValues = other.eigenspaceTrainingValues;
    imageNames = other.imageNames;
    imageRows = other.imageRows;
    imageCols = other.imageCols;
    imageRange = other.imageRange;
}

// testingImages - (N^2 x K)-matrix of testing images
// infoRatio - percent information perserved / percent eigenvalues used
// Returns the image index and error of the closest topMatches matches for each column of testingImages
// Returned vector's first index corresponds to the testing image index, the second with the training image index
std::vector<std::vector<EigenfaceError>> Eigenface::GetClosestMatches(const MatrixXf &testingImages, int topMatches, float infoRatio) const
{
    // Subtract mean face
    MatrixXf normalizedTestingImages = NormalizeImages(testingImages);
    // Compute eigenspace representation
    MatrixXf testingEigenspaceValues = ComputeEigenSpaceValues(normalizedTestingImages);

    std::vector<std::vector<EigenfaceError>> topMatchesAllImages(testingImages.cols());
    for(int i = 0; i < testingEigenspaceValues.cols(); i++)
    {
        VectorXf currentTestingEigenspaceValues = testingEigenspaceValues.col(i);
        std::vector<EigenfaceError> currentError;
        currentError.reserve(eigenspaceTrainingValues.cols());
        // For every training eigenspace representation
        for(int j = 0; j < eigenspaceTrainingValues.cols(); j++)
        {
            VectorXf currentTrainingEigenspaceValues = eigenspaceTrainingValues.col(j);

            // Compute Mahalanobis distance between eigenspace coefficients
            float error = MahalanobisDistance(currentTestingEigenspaceValues, currentTrainingEigenspaceValues, infoRatio);
            currentError.push_back(EigenfaceError(j, error));
        }
        std::sort(currentError.begin(), currentError.end());
        topMatchesAllImages[i] = std::vector<EigenfaceError>(currentError.begin(), currentError.begin() + topMatches);
    }

    return topMatchesAllImages;
}


// testingImages - (N^2 x K)-matrix of testing images
// infoRatio - percent information perserved / percent eigenvalues used
// Returns the error/distance between the reconstructed and normalized image for each column of testingImages
std::vector<float> Eigenface::GetDetectionError(const MatrixXf &testingImages, float infoRatio) const
{
    std::vector<float> ret;
    ret.reserve(testingImages.cols());
    std::vector<std::vector<EigenfaceError>> matches = GetClosestMatches(testingImages, 1, infoRatio);
    for(int i = 0; i < testingImages.cols(); ++i){
        ret.push_back(matches[i][0].error);
    }
    return ret;
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
        // Take normalized eigenface and convert back to 255 scale
        VectorXf eigenfaceImageScale = AdjustToImageRange(eigenfaces.col(i), imageRange);

        eigenfaceImages[i - start] = GetImage(eigenfaceImageScale);
    }

    return eigenfaceImages;
}


// eigenspaceValues - (M x M, K, 1)-matrix
// infoRatio - percent information perserved / percent eigenvalues used
    // Must be less than or equal to M
// Reconstructs the columns of eigenspaceValues using the first eigenCount eigenfaces
MatrixXf Eigenface::ReconstructImages(const MatrixXf &eigenspaceValues, float infoRatio) const
{
    int eigenCount = eigenfaces.cols() * (infoRatio / 100.0f);
    return (eigenfaces.block(0, 0, eigenfaces.rows(), eigenCount) * eigenspaceValues.block(0, 0, eigenCount, eigenspaceValues.cols())).colwise() + averageFace;
}

Image<GreyScale> Eigenface::GetImage(const VectorXf & image) const
{
    return Image<GreyScale>(image, imageRows, imageCols, imageRange);
}


// Returns (N^2 x M, K)-matrix in which each column is an image in directory
// _imageNames - populated with the names of each image corresponding with each column
// exampleImage - example image from the directory used to extract image info
// intruders - number of intruder images at the start of the imageNames
// subjectIntruders - number of subjects to exclude from the set
MatrixXf Eigenface::GetImageMatrix(std::string directory,
                                   std::vector<std::string> & _imageNames,
                                   Image<GreyScale> & exampleImage,
                                   int & intruders,
                                   int subjectIntruders) const
{
    if(_imageNames.size() != 0)
    {
        throw std::invalid_argument("Eigenface::GetImageMatrix _imageNames must be empty when the function is called");
    }

    // Get image names
    DIR *dir = opendir (directory.c_str());
    if (dir == NULL)
    {
        std::cerr << "Could not open training directory!" << std::endl;
        return MatrixXf(0, 0);
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        if(std::regex_match(ent->d_name, std::regex(R"(.*\.pgm$)")))
        {
            std::string filename = std::string(ent->d_name);
            _imageNames.push_back(filename.substr(0, filename.size() - 4));
        }
    }

    closedir (dir);

    if(_imageNames.size() == 0)
    {
        std::cerr << "No images were found in the provided training directory!" << std::endl;
        return MatrixXf(0, 0);
    }

    // Sort image names
    std::sort(_imageNames.begin(), _imageNames.end());

    int currentIntruderSubjectCount = 1;
    // Need to set to first subject ID if subjects do not start with 1
    int currentIntruderSubject = 1;
    intruders = -1;
    
    // Terminates when intruders points to the first non-intruder image
    while(currentIntruderSubjectCount <= subjectIntruders)
    {
        intruders++;

        int currentSubjectID = SubjectID(_imageNames[intruders]);

        if(currentSubjectID > currentIntruderSubject)
        {
            currentIntruderSubject = currentSubjectID;
            currentIntruderSubjectCount++;

        } else if(currentSubjectID < currentIntruderSubject)
        {
            std::cerr << "Eigenface GetImageMatrix subjects are not sorted correctly! (subject ID decreased when traversing)" << std::endl;
            return MatrixXf(0, 0);
        }
    }

    if(subjectIntruders == 0)
    {
        intruders = 0;
    }

    // Gets first exampleImage in order to get the matrix row size
    exampleImage = Image<GreyScale>(directory + _imageNames[intruders] + ".pgm");
    VectorXf exampleImageVector = exampleImage.FlattenedVector();
    MatrixXf imagesMatrix(exampleImageVector.size(), _imageNames.size() - intruders);
    imagesMatrix.col(intruders) = exampleImageVector;

    // Opens each image and assigns it to each column
    for(unsigned int i = intruders + 1; i < _imageNames.size(); i++)
    {
        Image<GreyScale> currentTrainingImage(directory + _imageNames[i] + ".pgm");
        imagesMatrix.col(i - intruders) = currentTrainingImage.FlattenedVector();
    }
    return imagesMatrix;
}


// Tranforms the vector to be in range [0, range]
VectorXf Eigenface::AdjustToImageRange(const VectorXf &image, int range) const
{
    VectorXf ret = image;
    ret -= VectorXf::Ones(ret.size()) * ret.minCoeff();
    ret = ret.array() * (VectorXf::Ones(ret.size()) / ret.maxCoeff() * range).array();
    return ret;
}


// Returns the (N^2)-vector of the average of the colums of (N^2 x M)-matrix images
VectorXf Eigenface::GetAverageFace(const MatrixXf &images) const
{
    return images.rowwise().mean();
}


// normalizedImages - (N^2 x M)-matrix aka A in which the columns are the normalized images
// Sets the eigenvalues and eigenfaces members
// Computes A'A and uses this to find the eigenvalues/vectors of AA'
void Eigenface::SetEigenvaluesEigenvectors(const MatrixXf &normalizedImages)
{
    MatrixXf dotNormalized = normalizedImages.transpose() * normalizedImages;
    EigenSolver<MatrixXf> solver(dotNormalized);
    eigenvalues = solver.eigenvalues().real();
    eigenfaces = normalizedImages * solver.eigenvectors().real();
}


// images - (N^2 x M, K, 1)-matrix
// Returns an (M x M, K, 1)-matrix in which the columns are the eigenspace representation of the columns of images
MatrixXf Eigenface::ComputeEigenSpaceValues(const MatrixXf &images) const
{
    return eigenfaces.transpose() * images;
}


// images - (N^2 x M, K, 1)-matrix
// Subtracts the average face from each column of images and returns the resulting value
MatrixXf Eigenface::NormalizeImages(const MatrixXf &images) const
{
    return images.colwise() - averageFace;
}


// Outputs the averageFace, eigenFaces, and the eigenspaceTrainingValues to a file
void Eigenface::OutputTrainingData(std::string outputFileName) const
{
    std::ofstream file(outputFileName);
    if(!file.is_open()){
        std::cout << "Can't open file: " << outputFileName << std::endl;
        exit(1);
    }

    file << imageRows << " " << imageCols << "\n";
    file << imageRange << "\n";
    file << intruderCount << "\n";

    std::cout << "        Writing image names..." << std::endl;
    file << imageNames.size() << "\n";
    for(unsigned int i = 0; i < imageNames.size(); ++i){
        file << imageNames[i] << "\n";
    }
    file << "\n";

    std::cout << "        Writing average face..." << std::endl;
    file << averageFace.size() << "\n";
    file << averageFace << "\n\n";

    std::cout << "        Writing eigenvalues..." << std::endl;
    file << eigenvalues.size() << "\n";
    file << eigenvalues << "\n\n";

    std::cout << "        Writing eigenfaces..." << std::endl;
    file << eigenfaces.rows() << " " << eigenfaces.cols() << "\n";
    file << eigenfaces << "\n\n";

    std::cout << "        Writing eigenspace training values..." << std::endl;
    file << eigenspaceTrainingValues.rows() << " " << eigenspaceTrainingValues.cols() << "\n";
    file << eigenspaceTrainingValues << "\n\n";
    
    file.close();
}


// Reads the averageFace, eigenFaces, and the eigenspaceTrainingValues from a file
void Eigenface::ReadTrainingData(std::string inputFileName){
    // Read numbers from file into buffer.
    std::ifstream infile;
    infile.open(inputFileName);

    infile >> imageRows >> imageCols;
    infile >> imageRange;
    infile >> intruderCount;

    int tempRows, tempCols;

    std::cout << "        Reading image names..." << std::endl;
    infile >> tempRows;
    imageNames.reserve(tempRows);
    std::string line;
    getline(infile, line);
    for(int i = 0; i < tempRows; ++i){
        getline(infile, line);
        imageNames.push_back(line);
    }

    std::cout << "        Reading average face..." << std::endl;
    infile >> tempRows;
    averageFace.resize(tempRows);
    for(int i = 0; i < tempRows; ++i){
        float cur;
        infile >> cur;
        averageFace(i) = cur;
    }

    std::cout << "        Reading eigenvalues..." << std::endl;
    infile >> tempRows;
    eigenvalues.resize(tempRows);
    for(int i = 0; i < tempRows; ++i){
        float cur;
        infile >> cur;
        eigenvalues(i) = cur;
    }

    std::cout << "        Reading eigenfaces..." << std::endl;
    infile >> tempRows >> tempCols;
    eigenfaces.resize(tempRows, tempCols);
    for(int i = 0; i < tempRows * tempCols; ++i){
        float cur;
        infile >> cur;
        eigenfaces(i / tempCols, i % tempCols) = cur;
    }

    std::cout << "        Reading eigenspace training values..." << std::endl;
    infile >> tempRows >> tempCols;
    eigenspaceTrainingValues.resize(tempRows, tempCols);
    for(int i = 0; i < tempRows * tempCols; ++i){
        float cur;
        infile >> cur;
        eigenspaceTrainingValues(i / tempCols, i % tempCols) = cur;
    }

    infile.close();
}


// Returns the MahalanobisDistance between the eigenspace representations of the two images
// eigenspaceImage - (M)-vector
// eigenCount - number of eigenvectors considered in the calculation
    // Must be less than or equal to M
float Eigenface::MahalanobisDistance(const VectorXf &eigenspaceImage1, const VectorXf &eigenspaceImage2, float infoRatio) const
{
    int eigenCount = eigenfaces.cols() * (infoRatio / 100.0f);
    float total = 0;
    for(int i = 0; i < eigenCount; ++i){
        float diff = eigenspaceImage1(i) - eigenspaceImage2(i);
        total += diff * diff / eigenvalues(i);
    }
    return total;
}


// Compares two image names (from Faces_FA_FB naming convention)
// Returns true if the images are of the same person, false if not
bool Eigenface::IsSameSubject(const std::string & name1, const std::string & name2){
    return SubjectID(name1) == SubjectID(name2);
}

// Returns the subject name identifier from the image name
int Eigenface::SubjectID(const std::string & fileName)
{
    std::string delimiter = "_";
    return std::stoi(fileName.substr(0, fileName.find(delimiter)));
}