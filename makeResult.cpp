#include "makeResult.hpp"

// nearest neighbor interpolation은 내 점 좌표에서 가장 가까운 점(반올림한 점)으로 내 색 결정
vector<double> nearestNeighbor(cv::Mat image, double originx, double originy)
{
	vector<double> retColor(3);

	int x = (int)(originx + 0.5);
	int y = (int)(originy + 0.5);

	for (int k = 0; k < 3; k++) retColor[k] = image.at<cv::Vec3b>(y, x)[k];

	return retColor;
}

// bilinear interpolation은 주변 네 점의 색으로 내 색 결정
// * . *
//   @
// * . *
vector<double> bilinear(cv::Mat image, double originx, double originy)
{
	vector<double> retColor(3);

	//int width = image.cols;
	//int height = image.rows;

	int x = (int)originx;
	int y = (int)originy;

	double delx = abs(originx - x); // 버려진 부분
	double dely = abs(originy - y); // 버려진 부분

	//double ColorChannel[3];

	for (int k = 0; k < 3; k++) {

		double Xtop = (1 - delx) * image.at<cv::Vec3b>(y, x)[k] + delx * image.at<cv::Vec3b>(y, x + 1)[k];
		double Xbottom = (1 - delx) * image.at<cv::Vec3b>(y + 1, x)[k] + delx * image.at<cv::Vec3b>(y + 1, x + 1)[k];
		double FinalColor = (1 - dely) * Xtop + dely * Xbottom;

		if (FinalColor > 255) FinalColor = 255;
		if (FinalColor < 0) FinalColor = 0;

		retColor[k] = FinalColor;
	}

	return retColor;
}

double weight(double a, double del)
{
	double absdel = abs(del);

	if (0 <= absdel && absdel < 1) return (a + 2)*pow(absdel, 3.0) - (a + 3)*pow(absdel, 2.0) + 1;
	else if (1 <= absdel && absdel < 2) return a * pow(absdel, 3.0) - 5 * a*pow(absdel, 2.0) + 8 * a*absdel - 4 * a;
	else return 0;
}

// bicubic interpolation은 주변 네 점의 색으로 내 색 결정
// *   * . *   *
//
// *   * . *   *
//       @
// *   * . *   *
//
// *   * . *   *
vector<double> bicubic(cv::Mat image, double originx, double originy)
{
	//double a = -0.5;
	//double a = -0.75;
	double a = -1.0;

	int x = (int)originx - 1;
	int y = (int)originy - 1;

	vector<double> retColor(3);
	for (int i = 0; i < 3; i++) retColor[i] = 0;

	double colorArr[4][3];

	for (int j = 0; j < 4; j++) {
		for (int k = 0; k < 3; k++) colorArr[j][k] = 0;

		for (int i = 0; i < 4; i++) {
			double delx = abs(originx - (x + i));

			for (int k = 0; k < 3; k++) {
				colorArr[j][k] += weight(a, delx)*image.at<cv::Vec3b>(y + j, x + i)[k];
			}
		}
	}

	for (int j = 0; j < 4; j++) {
		double dely = abs(originy - (y + j));

		for (int k = 0; k < 3; k++) {
			retColor[k] += colorArr[j][k] * weight(a, dely);
		}
	}

	for (int k = 0; k < 3; k++) {
		if (retColor[k] > 255) retColor[k] = 255;
		if (retColor[k] < 0) retColor[k] = 0;
	}

	return retColor;
}

// return형은 이미지, 메인문에서 매개변수로 받아오는 건 원본 이미지
// int interpolateOpt 변수가 0이면 nearest neighbor interpolation
// int interpolateOpt 변수가 1이면 bilinear interpolation
// int interpolateOpt 변수가 2이면 bicubic interpolation
cv::Mat MakeResult(cv::Mat image, vector<vector<double> > TransformMat, int interpolateOpt)
{
	// 세로, 가로 --> a4비율
	cv::Mat newimage(580, 420, CV_8UC3);

	int width = newimage.cols;
	int height = newimage.rows;

	vector<vector<double>> InverseTransformMat = inverse(TransformMat);

	double originx = 0;
	double originy = 0;
	double ref = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// 변환 행렬의 역행렬을 이용해 각 좌표의 원좌표 구하기
			originx = InverseTransformMat[0][0] * j + InverseTransformMat[0][1] * i + InverseTransformMat[0][2];
			originy = InverseTransformMat[1][0] * j + InverseTransformMat[1][1] * i + InverseTransformMat[1][2];
			ref = InverseTransformMat[2][0] * j + InverseTransformMat[2][1] * i + InverseTransformMat[2][2];

			if (ref != 1) {
				originx /= ref;
				originy /= ref;
				ref = 1;
			}

			vector<double> ColorChannel;

			// 옵션에 따라 보간법 수행
			if (interpolateOpt == 0) ColorChannel = nearestNeighbor(image, originx, originy);
			else if (interpolateOpt == 1) ColorChannel = bilinear(image, originx, originy);
			else if (interpolateOpt == 2) ColorChannel = bicubic(image, originx, originy);

			for (int k = 0; k < 3; k++) newimage.at<cv::Vec3b>(i, j)[k] = ColorChannel[k];
		}
	}

	return newimage;
}