#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;
using namespace std;
typedef Point3_<uint8_t> Pixel;

Point consineSimilarity(Mat& img, Mat& templ) {
	//image is W*H, template is w*h
	int H = img.rows, W = img.cols;
	int h = templ.rows, w = templ.cols;

	//Converting the Images to vectors
	vector<int> imgPix;
	vector<int> templPix;
	vector<double> similarity;
	for (Pixel& p : Mat_<Pixel>(img)) {
		imgPix.push_back((int)p.x);
		imgPix.push_back((int)p.y);
		imgPix.push_back((int)p.z);
	}
	for (Pixel& p : Mat_<Pixel>(templ)) {
		templPix.push_back((int)p.x);
		templPix.push_back((int)p.y);
		templPix.push_back((int)p.z);
	}

	for (int i = 0; i < H - h + 1; i++) {
		for (int j = 0; j < W - w + 1; j++) {

			//Cosine Similarity formula
			double dot = 0, x = 0, y = 0;
			for (int k = 0; k < h; k++) {
				for (int l = 0; l < w; l++) {
					for (int p = 0; p < 3; p++) {
						double imgVal = imgPix[(k + i) * 3 * W + (j + l) * 3 + p];
						double templVal = templPix[k * 3 * w + l * 3 + p];
						dot += imgVal * templVal;
						x += imgVal * imgVal;
						y += templVal * templVal;
					}

				}
			}
			
			similarity.push_back((double)dot / (sqrt(x) * sqrt(y)));
		}
	}

	//Find the position with highest similarity score
	int index = 0;

	double highest_score = -1;
	for (unsigned i = 0; i < similarity.size(); i++) {
		if (highest_score < similarity[i]) {
			highest_score = similarity[i];
			index = i;
		}
	}

	Point p(index % (W - w + 1), index / (W - w + 1));
	return p;

}


int main() {
	VideoCapture videoCapture;
	Mat frame, croppedImg, img, crop, cloneImg;
	bool isCrop = false;

	//Open camera
	videoCapture.open(0);

	//Check if camera is opened
	if (!videoCapture.isOpened()) {
		cout << "Cannot open camera" << endl;
	}
	else {
		while (true) {
			//Read video frame from camera 
			videoCapture.read(frame);
			//copy to a clone image as we won't touch the source image
			frame.copyTo(cloneImg);


			Point p1(200, 250);
			Point p2(350, 350);


			int thickness = 2;

			if (isCrop) {
				//resize for faster calculation
				resize(cloneImg, img, Size(), 0.125, 0.125);
				resize(croppedImg, crop, Size(), 0.125, 0.125);
	
				Point p3 = consineSimilarity(img, crop);
	
				p3.x = p3.x * 8;
				p3.y = p3.y * 8;

				Point p4 = p3 + (p2 - p1);
				//captured
				rectangle(cloneImg, p3, p4, Scalar(255, 0, 0), thickness);
			}
			else {
				//not captured
				rectangle(cloneImg, p1, p2, Scalar(255, 0, 0), thickness);
			}

			imshow("VidCap", cloneImg);
			int key = (int)waitKey(1);
			//press esc to end program
			if (key == 27)
				break;
			//press c to capture
			if (key == 99 && !isCrop) {

				croppedImg = frame.clone();
				croppedImg = croppedImg(Range(250, 350), Range(200, 350));


				imshow("crop templ", croppedImg);
				isCrop = true;
			}
		}
		videoCapture.release();
		destroyAllWindows();
	}
	return 0;
}
