#ifndef corner_hpp
#define corner_hpp

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <time.h>

using namespace std;
using namespace cv;

vector<pair<int, int> > findCorner(Mat img);

#endif