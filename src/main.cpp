#include <iostream>
#include "Plot.h"
#include "ROC.h"
#include "MLEstimation.h"
#include "Image.h"
#include "Eigenface.h"

int main(int argc, char *argv[])
{
    std::cout << "Project 3" << std::endl;

    Image<GreyScale> testImage("Faces_FA_FB/fa_H/00001_930831_fa_a.pgm");
    VectorXf flattened = testImage.FlattenedVector();
    Image<GreyScale> fromFlattened(flattened, testImage.Rows, testImage.Cols);

    return 0;
}