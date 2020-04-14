#include "Eigenface.h"
#include "CMC.h"

namespace CMC{


std::vector<float> GenerateCMC(std::string testingImagesDirectory, const Eigenface &eigenface, float infoRatio){
	std::vector<float> ret;
	ret.reserve(50);

	std::vector<std::string> testImageNames;
	Image<GreyScale> exampleImage;
	int intruderCount;
	MatrixXf testingImages = eigenface.GetImageMatrix(testingImagesDirectory, testImageNames, exampleImage, intruderCount);
	std::vector<std::vector<EigenfaceError>> matches = eigenface.GetClosestMatches(testingImages, 50, infoRatio);
	int numCorrectlyFound = 0;
	int numIncorrectlyFound = 0;
	for(int n = 1; n <= 50; ++n){
		float numMatches = 0;
		for(unsigned int testIndex = 0; testIndex < matches.size(); ++testIndex){
			bool found = false;
			std::string trainingName("");
			std::string testName = testImageNames[testIndex];
			for(int trainingIndex = 0; trainingIndex < n; ++ trainingIndex){
				trainingName = eigenface.imageNames[ matches[testIndex][trainingIndex].index ];
				if(eigenface.IsSameSubject(testName, trainingName)){
					found = true;
					break;
				}
			}
			if(found){
				numMatches++;
			}
			if(found && numCorrectlyFound < 3){
				numCorrectlyFound++;
				std::cout << "        Correct match. Query: " << testImageNames[testIndex] << ".pgm ==> " << trainingName << ".pgm" << std::endl;
			} else if(!found && numIncorrectlyFound < 3){
				numIncorrectlyFound++;
				std::cout << "        Incorrect match. Query: " << testImageNames[testIndex] << ".pgm ==> " << trainingName << ".pgm" << std::endl;
			}
		}
		float accuracy = numMatches / matches.size();
		//std::cout << n << ", " << accuracy << std::endl;
		ret.push_back(accuracy);
	}
	return ret;
}





}