#include "pointArea.hpp"
#include <iostream>
#include <vector>
#include <opencv2\opencv.hpp>

pointArea::pointArea()
{
	minX = 0;
	maxX = 0;
	minY = 0;
	maxY = 0;
	minDepth = 0;
	maxDepth = 0;
}


pointArea::pointArea(int minx, int maxx, int miny, int maxy)
{
	minX = minx;
	maxX = maxx;
	minY = miny;
	maxY = maxy;
	minDepth = 700;
	maxDepth = 1000;
}

pointArea::pointArea(int minX, int maxX, int minY, int maxY, int minDepth, int maxDepth)
{
	this->minX = minX;
	this->maxX = maxX;
	this->minY = minY;
	this->maxY = maxY;
	this->minDepth = minDepth;
	this->maxDepth = maxDepth;
}

cv::Mat pointArea::drawArea(cv::Mat image) {
	cv::line(image, cv::Point(minX, minY), cv::Point(maxX, minY), cv::Scalar(0,0,200), 2, 4);  
	cv::line(image, cv::Point(maxX, minY), cv::Point(maxX, maxY), cv::Scalar(0,0,200), 2, 4);  
	cv::line(image, cv::Point(maxX, maxY), cv::Point(minX, maxY), cv::Scalar(0,0,200), 2, 4);  
	cv::line(image, cv::Point(minX, maxY), cv::Point(minX, minY), cv::Scalar(0,0,200), 2, 4); 

	return image;
}

int pointArea::innerDepthCount(int videoModeX, short* depth, int* vec) {

	int count = 0;
	int depthIndex;

	int num = getWidth() * getHeight(); 

	// init
	for(int k=0; k < num; k++) {
		vec[k] = 0;
	}

	for(int i = minX; i < maxX; i++) {
		for(int j = minY; j < maxY; j++) {
			depthIndex = (j * videoModeX) + i;

			if(depth[depthIndex] < maxDepth && depth[depthIndex] > minDepth) {
					count++;
				
				vec[(i - minX) * (j - minY)] = depth[depthIndex];
			}
		}
	}

	return count;
}


cv::Point3f pointArea::getCenter() {
	cv::Point3f point;

	point.x = (float)pointArea::getMinX() + (pointArea::getMaxX() - pointArea::getMinX()) / 2;
	point.y = (float)pointArea::getMinY() + (pointArea::getMaxY() - pointArea::getMinY()) / 2;
	point.z = (float)pointArea::getMinDepth() + (pointArea::getMaxDepth() - pointArea::getMinDepth()) / 2;

	return point;
}