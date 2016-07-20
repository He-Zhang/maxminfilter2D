/*
Author：Wincoder
Site：http://herzhang.github.io/
Data：2016,07,20

Reference:
Van Herk, M. 1992. A fast algorithm for local minimum and maximum filters on rectangular and octagonal kernels. Pattern Recognition Letters 13, 7, 517–521.
*/

using namespace cv;
using namespace std;

enum
{
	VANHERK_MAX = 0,
	VANHERK_MIN = 1
};

enum
{
	VANHERK_ROW = 0,
	VANHERK_COL = 1
};


// 最小值滤波
cv::Mat MinFilt2(const cv::Mat& Img, cv::Size S);
// 最大值滤波
cv::Mat MaxFilt2(const cv::Mat& Img, cv::Size S);

Mat Vanherk(Mat& I, int N, int type, int direc)
{
	CV_Assert(!I.empty());

	if (direc == VANHERK_COL)
		I = I.t();

	int height = I.rows;
	int width  = I.cols;

	/// Correcting I size
	bool fixsize = false;
	int  addel   = 0;

	Mat f;
	if (width % N != 0)
	{
		fixsize = true;
		addel = N - width % N;

		if (type == VANHERK_MAX)
			copyMakeBorder(I, f, 0, 0, 0, addel, BORDER_CONSTANT, Scalar(0, 0, 0));
		else
			copyMakeBorder(I, f, 0, 0, 0, addel, BORDER_REPLICATE);
	}
	else
	{
		f = I;
	}

	int lf = f.cols;
	int li = I.cols;
	I.release();

	/// Declaring aux.mat.
	Mat g = f;
	Mat h = g;

	/// Filling g & h(aux.mat.)
	vector<int> ig;
	vector<int> ih;
	
	for (int i = 0; i < width; i = i + N)
	{
		ig.push_back(i);
		ih.push_back(i + N - 1);
	}

	vector<int> igold;
	vector<int> ihold;
	if (type == VANHERK_MAX)
	{
		for (int i = 1; i < N; i++)
		{
			igold = ig;
			ihold = ih;

			Mat gtemp = g.clone();
			Mat htemp = h.clone();
			for (int j = 0; j < ig.size(); j++)
			{
				ig[j] = ig[j] + 1;
				ih[j] = ih[j] - 1;

				Mat_<uchar>(max(f.col(ig[j]), g.col(igold[j]))).copyTo(gtemp.col(ig[j]));
				Mat_<uchar>(max(f.col(ih[j]), h.col(ihold[j]))).copyTo(htemp.col(ih[j]));
			}
			g = gtemp;
			h = htemp;
		}
	}
	else
	{
		for (int i = 1; i < N; i++)
		{
			igold = ig;
			ihold = ih;

			Mat gtemp = g.clone();
			Mat htemp = h.clone();
			for (int j = 0; j < ig.size(); j++)
			{
				ig[j] = ig[j] + 1;
				ih[j] = ih[j] - 1;

				Mat_<uchar>(min(f.col(ig[j]), g.col(igold[j]))).copyTo(gtemp.col(ig[j]));
				Mat_<uchar>(min(f.col(ih[j]), h.col(ihold[j]))).copyTo(htemp.col(ih[j]));
			}
			g = gtemp;
			h = htemp;
		}
	}

	f.release();


	/// Comparing g & h
	Mat dst = Mat::zeros(height,width,CV_8U);

	if (fixsize)
	{
		if (addel > (N - 1) / 2)
		{
			Rect roil(0, 0, floor((N - 1) / 2.0) , height);
			Rect roir(floor((N - 1) / 2.0) , 0, width - (floor((N - 1) / 2.0) ), height);

			if (type == VANHERK_MAX)
			{
				g.colRange(ceil((N - 1) / 2.0), N - 1).copyTo(dst(roil));
				Mat_<uchar>(max(g.colRange(N - 1, lf - addel + ceil((N - 1) / 2.0)), h.colRange(0, lf - N + 1 - addel + ceil((N - 1) / 2.0)))).copyTo(dst(roir));
			}
			else
			{
				g.colRange(ceil((N - 1) / 2.0), N - 1).copyTo(dst(roil));
				Mat_<uchar>(min(g.colRange(N - 1, lf - addel + ceil((N - 1) / 2.0)), h.colRange(0, lf - N + 1 - addel + ceil((N - 1) / 2.0)))).copyTo(dst(roir));
			}
		}
		else
		{ 
			Rect roil(0, 0, floor((N - 1) / 2.0), height);
			Rect roim(floor((N - 1) / 2.0), 0, lf - N + 1, height);
			Rect roir(lf - ceil((N - 1) / 2.0) , 0, width -(lf - ceil((N - 1) / 2.0)), height);

			if (type == VANHERK_MAX)
			{
				if (roil.area() != 0)
					g.colRange(ceil((N - 1) / 2.0), N - 1).copyTo(dst(roil));
				
				Mat_<uchar>(max(g.colRange(N - 1, lf), h.colRange(0, lf - N + 1))).copyTo(dst(roim));
				
				if (roir.area() != 0)
					h.colRange(lf - N + 1, lf - N + 1 + ceil((N - 1) / 2.0) - addel).copyTo(dst(roir));
			}
			else
			{
				if (roil.area() != 0)
					g.colRange(ceil((N - 1) / 2.0), N - 1).copyTo(dst(roil));
				
				Mat_<uchar>(min(g.colRange(N - 1, lf), h.colRange(0, lf - N + 1))).copyTo(dst(roim));
				
				if(roir.area() != 0)
					h.colRange(lf - N + 1, lf - N + 1 + ceil((N - 1) / 2.0) - addel).copyTo(dst(roir));
			}
		}
	}
	else // not fixsize(addel = 0, lf = lx)
	{
		Rect roil(0, 0, ceil((N - 1) / 2.0) , height);
		Rect roim(ceil((N - 1) / 2.0) , 0, li - N + 1, height);
		Rect roir(width - ceil((N - 1) / 2.0), 0, N - ceil((N - 1) / 2.0) - 1, height);

		if (type == VANHERK_MAX)
		{
			if (roil.area() != 0)
				g.colRange(N - ceil((N - 1) / 2.0) - 1, N - 1).copyTo(dst(roil));
			
			Mat_<uchar>(max(g.colRange(N - 1, li), h.colRange(0, li - N + 1))).copyTo(dst(roim));
			
			if (roir.area() != 0)
				h.colRange(li - N + 1, li - N + 1 + floor((N - 1) / 2.0)).copyTo(dst(roir));
		}
		else
		{
			if (roil.area() != 0)
				g.colRange(N - ceil((N - 1) / 2.0) - 1, N - 1).copyTo(dst(roil));

			Mat_<uchar>(min(g.colRange(N - 1, li), h.colRange(0, li - N + 1))).copyTo(dst(roim));
			
			if (roir.area() != 0)
				h.colRange(li - N + 1, li - N + 1 + floor((N - 1) / 2.0)).copyTo(dst(roir));
		}
	}
	
	if(direc ==VANHERK_COL)
		dst = dst.t();
	
	return dst;
}

cv::Mat MaxFilt2(const cv::Mat& Img, Size S)
{
	Mat I = Img.clone();
	
	/// filtering
	Mat ImgX = Vanherk(I,    S.height, VANHERK_MAX, VANHERK_ROW);
	Mat dst  = Vanherk(ImgX, S.width,  VANHERK_MAX, VANHERK_COL);

	return dst;
}

cv::Mat MinFilt2(const cv::Mat& Img, Size S)
{
	Mat I = Img.clone();

	/// filtering
	Mat ImgX = Vanherk(I,    S.height, VANHERK_MIN, VANHERK_ROW);
	Mat dst  = Vanherk(ImgX, S.width,  VANHERK_MIN, VANHERK_COL);

	return dst;
}


