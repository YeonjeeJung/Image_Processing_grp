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

double dist(pair<int, int> point1, pair<int, int> point2);
void HarrisCorner(int width, int height, unsigned char** ptr, std::vector<std::pair<int, int>>& corners, double th);

vector<pair<int, int> > findCorner(Mat img)
{
	// image.rows * image.cols 크기의 unsigned char 배열 생성 (이미지의 grayscale값 저장할 것임)
	uchar** immat = new uchar*[img.rows];
	for (int j = 0; j < img.rows; j++) {
		immat[j] = new uchar[img.cols];
	}

	// immat 배열 안에 grayscale값 저장
	for (int x = 0; x < img.rows; x++) {
		for (int y = 0; y < img.cols; y++) {
			immat[x][y] = (img.at<Vec3b>(x, y)[0] + img.at<Vec3b>(x, y)[1] + img.at<Vec3b>(x, y)[2]) / 3;
		}
	}

	//cout << img.rows << endl;
	//cout << img.cols << endl;

	int nrow = img.rows;
	int ncol = img.cols;

	// 코너로 인식된 픽셀의 좌표를 저장할 벡터
	// first가 row 인덱스, second가 col 인덱스
	vector<pair<int, int>> corners;

	//cout << "width : " << ncol << ", height : " << nrow << endl;

	// 해리스 코너 인식
	int th = 50000;
	//int th = 10;
	HarrisCorner(img.cols, img.rows, immat, corners, th);
	// 코너로 검출된 점의 개수
	int nCorners = corners.size();
	//cout << "corners : " << nCorners << endl;

	// 전체 코너 출력

	/*
	vector<pair<int, int>>::iterator iter;
	for (iter = corners.begin(); iter != corners.end(); iter++) {
		cout << "[" << iter->first << "," << iter->second << "]" << endl;
	}
	*/

	// 최종 코너 좌표 저장 //
	vector<pair<int, int>> dstCorner(4);
	dstCorner[0] = make_pair(-1, -1);
	dstCorner[1] = make_pair(-1, -1);
	dstCorner[2] = make_pair(-1, -1);
	dstCorner[3] = make_pair(-1, -1);

	// 원본이미지의 끝점들을 저장
	vector<pair<int, int> > limitPoints(4);
	limitPoints[0] = make_pair(0, 0);
	limitPoints[1] = make_pair(ncol, 0);
	limitPoints[2] = make_pair(0, nrow);
	limitPoints[3] = make_pair(ncol, nrow);

	vector<double> mindists(4);
	mindists[0] = 50000;
	mindists[1] = 50000;
	mindists[2] = 50000;
	mindists[3] = 50000;

	int n_corners = corners.size();
	for (int i = 0; i < n_corners; i++) {
		for (int k = 0; k < 4; k++) {
			// 모든 limitPoints와 비교함
			// 어떤 limitPoint와 내 거리가 mindist보다 작으면 그 값을 dstcorner에 저장
			int pointsDist = dist(corners[i], limitPoints[k]);
			if (pointsDist < mindists[k]) {
				mindists[k] = pointsDist;
				dstCorner[k] = corners[i];
			}
		}
	}

	int n_dstcorners = dstCorner.size();

	for (int k = 0; k < n_dstcorners; k++) {
		assert(("corner not detected", (dstCorner[k].first != -1) && (dstCorner[k].second != -1)));
	}

	/*
	for (int k = 0; k < n_dstcorners; k++) {
		for (int i = -2; i <= 2; i++) {
			for (int j = -2; j <= 2; j++) {
				img.at<cv::Vec3b>(dstCorner[k].second + j, dstCorner[k].first + i)[0] = 0;
				img.at<cv::Vec3b>(dstCorner[k].second + j, dstCorner[k].first + i)[1] = 0;
				img.at<cv::Vec3b>(dstCorner[k].second + j, dstCorner[k].first + i)[2] = 255;
			}
		}
	}
	*/

	// 사진에서 가로 세로 길이 
	int width = abs(dstCorner[0].first - dstCorner[1].first);
	int height = abs(dstCorner[0].second - dstCorner[2].second);

	// 가로가 세로보다 길시 저장할 코너 
	vector<pair<int, int>> dstCorner2(4);
	dstCorner2[0] = make_pair(-1, -1);
	dstCorner2[1] = make_pair(-1, -1);
	dstCorner2[2] = make_pair(-1, -1);
	dstCorner2[3] = make_pair(-1, -1);

	// 만약 가로가 세로보다 길다면
	if (width > height) {
		dstCorner2[0] = dstCorner[2];
		dstCorner2[1] = dstCorner[0];
		dstCorner2[2] = dstCorner[3];
		dstCorner2[3] = dstCorner[1];
		return dstCorner2;
	}
	else {
		return dstCorner;
	}


}

double dist(pair<int, int> point1, pair<int, int> point2)
{
	return abs((double)point1.first - point2.first) + abs((double)point1.second - point2.second);
}


// 해리스 코너 검출 함수
// width, height : 이미지의 가로, 세로 픽셀수
// ptr : 이미지 픽셀값 배열의 포인터
// corners : 여기에 코너로 검출된 픽셀의 좌표가 들어가게 됨
// th : threshold 값
void HarrisCorner(int width, int height, unsigned char** ptr, std::vector<std::pair<int, int>>& corners, double th)
{
	register int i, j, x, y;

	int w = width;
	int h = height;

	//-------------------------------------------------------------------------
	// (fx)*(fx), (fx)*(fy), (fy)*(fy) 계산
	//-------------------------------------------------------------------------

	float** dx2 = new float*[h];
	for (int j = 0; j < h; j++) {
		dx2[j] = new float[w];
	}
	float** dy2 = new float*[h];
	for (int j = 0; j < h; j++) {
		dy2[j] = new float[w];
	}
	float** dxy = new float*[h];
	for (int j = 0; j < h; j++) {
		dxy[j] = new float[w];
	}

	float tx, ty;
	for (j = 1; j < h - 1; j++)
		for (i = 1; i < w - 1; i++)
		{
			tx = (ptr[j - 1][i + 1] + ptr[j][i + 1] + ptr[j + 1][i + 1]
				- ptr[j - 1][i - 1] - ptr[j][i - 1] - ptr[j + 1][i - 1]) / 6.f;
			ty = (ptr[j + 1][i - 1] + ptr[j + 1][i] + ptr[j + 1][i + 1]
				- ptr[j - 1][i - 1] - ptr[j - 1][i] - ptr[j - 1][i + 1]) / 6.f;

			dx2[j][i] = tx * tx;
			dy2[j][i] = ty * ty;
			dxy[j][i] = tx * ty;
		}

	//-------------------------------------------------------------------------
	// 가우시안 필터링
	//-------------------------------------------------------------------------

	float** gdx2 = new float*[h];
	for (int j = 0; j < h; j++) {
		gdx2[j] = new float[w];
	}
	float** gdy2 = new float*[h];
	for (int j = 0; j < h; j++) {
		gdy2[j] = new float[w];
	}
	float** gdxy = new float*[h];
	for (int j = 0; j < h; j++) {
		gdxy[j] = new float[w];
	}

	float g[5][5] = { { 1, 4, 6, 4, 1 },{ 4, 16, 24, 16, 4 },
	{ 6, 24, 36, 24, 6 },{ 4, 16, 24, 16, 4 },{ 1, 4, 6, 4, 1 } };

	for (y = 0; y < 5; y++)
		for (x = 0; x < 5; x++)
		{
			g[y][x] /= 256.f;
		}

	float tx2, ty2, txy;
	for (j = 2; j < h - 2; j++)
		for (i = 2; i < w - 2; i++)
		{
			tx2 = ty2 = txy = 0;
			for (y = 0; y < 5; y++)
				for (x = 0; x < 5; x++)
				{
					tx2 += (dx2[j + y - 2][i + x - 2] * g[y][x]);
					ty2 += (dy2[j + y - 2][i + x - 2] * g[y][x]);
					txy += (dxy[j + y - 2][i + x - 2] * g[y][x]);
				}

			gdx2[j][i] = tx2;
			gdy2[j][i] = ty2;
			gdxy[j][i] = txy;
		}

	//-------------------------------------------------------------------------
	// 코너 응답 함수 생성
	//-------------------------------------------------------------------------

	float** crf = new float*[h];
	for (int j = 0; j < h; j++) {
		crf[j] = new float[w];
	}

	float k = 0.04f;
	for (j = 2; j < h - 2; j++)
		for (i = 2; i < w - 2; i++)
		{
			crf[j][i] = (gdx2[j][i] * gdy2[j][i] - gdxy[j][i] * gdxy[j][i]) - k * (gdx2[j][i] + gdy2[j][i])*(gdx2[j][i] + gdy2[j][i]);
			//crf[j][i] = (dx2[j][i] * dy2[j][i] - dxy[j][i] * dxy[j][i]) - k * (dx2[j][i] + dy2[j][i])*(dx2[j][i] + dy2[j][i]);
		}

	//-------------------------------------------------------------------------
	// 임계값보다 큰 지역 최댓값을 찾아 코너로 결정
	//-------------------------------------------------------------------------

	corners.clear();
	float cvf_value;

	for (j = 2; j < h - 2; j++)
		for (i = 2; i < w - 2; i++)
		{
			cvf_value = crf[j][i];
			if (cvf_value > th)
			{

				if (cvf_value > crf[j - 1][i] && cvf_value > crf[j - 1][i + 1] &&
					cvf_value > crf[j][i + 1] && cvf_value > crf[j + 1][i + 1] &&
					cvf_value > crf[j + 1][i] && cvf_value > crf[j + 1][i - 1] &&
					cvf_value > crf[j][i - 1] && cvf_value > crf[j - 1][i - 1])
				{
					corners.push_back(std::make_pair(i, j));
				}
			}
		}

	//-------------------------------------------------------------------------
	// 메모리 해제
	//-------------------------------------------------------------------------

	for (int i = 0; i < h; i++) delete[] dx2[i];
	for (int i = 0; i < h; i++) delete[] dy2[i];
	for (int i = 0; i < h; i++) delete[] dxy[i];
	delete dx2;
	delete dy2;
	delete dxy;

	for (int i = 0; i < h; i++) delete[] gdx2[i];
	for (int i = 0; i < h; i++) delete[] gdy2[i];
	for (int i = 0; i < h; i++) delete[] gdxy[i];
	delete gdx2;
	delete gdy2;
	delete gdxy;

	for (int i = 0; i < h; i++) delete[] crf[i];
	delete crf;

}


