#ifndef makeResult_hpp
#define makeResult_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <math.h>

#include "getMatrix.hpp"

using namespace std;

cv::Mat MakeResult(cv::Mat image, vector<vector<double> > TransformMat, int interpolateOpt);

#endif /* makeResult_hpp */
