#ifndef _INC_POINT
#define _INC_POINT

#include <iostream>
#include <vector>

#include <opencv2\opencv.hpp>

class pointArea
{
private:
    int minX;
	int maxX;
	int minY;
	int maxY;
	int minDepth;
	int maxDepth;
	uchar colorCount;
	int touchCount;
	bool touchFlag;

	float minWX;
	float maxWX;
	float minWY;
	float maxWY;
	float minWZ;
	float maxWZ;

public:
	pointArea();
	pointArea(int minx, int maxx, int miny, int maxy);
	pointArea(int minx, int maxx, int miny, int maxy, int mindepth, int maxdepth);

	void setPointData (int minX, int maxX, int minY, int maxY, 
		               float minWX, float maxWX, float minWY, float maxWY, float minWZ, float maxWZ) {
		this -> minX = minX;
		this -> maxX = maxX;
		this -> minY = minY;
		this -> maxY = maxY;
		this -> minWX = minWX;
		this -> maxWX = maxWX;
		this -> minWY = minWY;
		this -> maxWY = maxWY;
		this -> minWZ = minWZ;
		this -> maxWZ = maxWZ;
		this -> colorCount = 0;
		this -> touchCount = 0;
		this -> touchFlag = false;
	}

	void setAllData(int minX, int maxX, int minY, int maxY, int minDepth, int maxDepth, int colorCount) {
		this -> minX = minX;
		this -> maxX = maxX;
		this -> minY = minY;
		this -> maxY = maxY;
		this -> minDepth = minDepth; 
		this -> maxDepth = maxDepth;
		this -> colorCount = colorCount;
	};

	void setMinX(int x) { minX = x; }
	void setMaxX(int x) { maxX = x; }
	void setMinY(int y) { minY = y; }
	void setMaxY(int y) { maxY = y; }
	void setMinDepth(int depth) { minDepth = depth; }
	void setMaxDepth(int depth) { maxDepth = depth; }
	void setColorCount(uchar count) { colorCount = count; }
	void setTouchCount(int count) { touchCount = count; }
	void setTouchFlag(bool bol) { touchFlag = bol; }

	void setMinWX(float x) { minWX = x; }
	void setMinWY(float y) { minWY = y; }
	void setMinWZ(float depth) { minWZ = depth; }
	void setMaxWX(float x) { maxWX = x; }
	void setMaxWY(float y) { maxWY = y; }
	void setMaxWZ(float depth) { maxWZ = depth; }


	int getMinX() { return minX; }
    int getMaxX() { return maxX; }
    int getMinY() { return minY; }
    int getMaxY() { return maxY; }
	int getMinDepth() { return minDepth; }
    int getMaxDepth() { return maxDepth; }
	uchar getColorCount() { return colorCount; }
	int getTouchCount() { return touchCount; }
	bool getTouchFlag() { return touchFlag; }


	int getWidth() {return maxX - minX;}
	int getHeight() {return maxY - minY;}
	int getDepth() {return maxDepth - minDepth;}

	float getMinWX() { return minWX; }
    float getMinWY() { return minWY; }
    float getMinWZ() { return minWZ; }
	float getMaxWX() { return maxWX; }
    float getMaxWY() { return maxWY; }
    float getMaxWZ() { return maxWZ; }

	cv::Point3f getCenter();

	// drawArea
	cv::Mat drawArea(cv::Mat image);
	
	// SearchDepthCount
	int innerDepthCount(int videoModeX, short* depth, int* vec);

};

#endif