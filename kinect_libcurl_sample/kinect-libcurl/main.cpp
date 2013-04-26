#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <opencv2\opencv.hpp>
#include <OpenNI.h>
#include <locale>
#include <curl/curl.h>

#include "pointArea.hpp"
#include "researchArea.hpp"

template<class T> struct curlData{
	char* postthis;
};

// ヒートマップ用カラー作成
cv::Vec3b changeColor(uchar color) {
	cv::Vec3b ret;

	uchar red = 0;
	uchar green = 0;
	uchar blue = 0;

	if(color < 64) {
		red = 0;
		green = 0;
		blue = color * 255 / 63;
	} else if(color < 128) {
		red = 0;
		green = (color - 64) * 255 / 63;
		blue = 255;
	} else if (color < 192) {
		red = 255;
		green = 255 - ((color - 128) * 255 / 63);
		blue = 0;
	} else {
		red = 255;
		green = 0;
		blue = 0;
	}

	ret[0] = blue;
	ret[1] = green;
	ret[2] = red;

	return ret;
}


// depht用カラー作成
cv::Vec3b makeColor(int color) {
	cv::Vec3b ret;

	uchar red = 0;
	uchar green = 0;
	uchar blue = 0;

	if(color < 64) {
		red = 0;
		green = color * 255 / 63;
		blue = 255;
	} else if(color < 128) {
		red = 0;
		green = 255;
		blue = (color - 64) * 255 / 63;
	} else if (color < 192) {
		red = (color - 128) * 255 / 63;
		green = 255;
		blue = 0;
	} else if (color <= 255){
		red = 255;
		green = 255 - ((color - 192) * 255 / 63);
		blue = 0;
	}

	ret[0] = blue;
	ret[1] = green;
	ret[2] = red;

	return ret;
}


// 2次元ポイントエリア作成
pointArea** makePointArea(int areaWidth, int areaHeight, int areaPointX, int areaPointY, int rowNum, int colNum, int intervalX, int intervalY, int minDepth, int maxDepth) {

	int colSize = (areaHeight - (colNum * intervalY)) / colNum ;
	int rowSize = (areaWidth - (rowNum * intervalX)) / rowNum ;

	// エリアの2次元配列を準備
	pointArea** newPointArea = new pointArea*[colNum];

	for (int i = 0; i < colNum; i++) {
		newPointArea[i] = new pointArea[rowNum];
	}

	// 2次元配列のデータを設定
	for (int i = 0; i < colNum; i++) {
		for(int j = 0; j < rowNum; j++) {
			newPointArea[i][j].setAllData(areaPointX + (j * (rowSize + intervalX)),
										  areaPointX + ((j + 1) * rowSize + (j * intervalX)),
										  areaPointY + (i * (colSize + intervalY)), 
										  areaPointY + ((i + 1) * colSize + (i * intervalY)),
										  minDepth,
										  maxDepth,
										  0);
		
		}
	}
	return newPointArea;
}

// 3次元ポイントエリア作成
pointArea*** makePointArea3D(int areaWidth, 
	                         int areaHeight,
							 int areaDepth,
							 int areaPointX, 
							 int areaPointY,
							 int areaPointZ,
							 int rowNum, 
							 int colNum,
							 int depthNum,
							 int intervalX, 
							 int intervalY,
							 int intervalZ) {
	
	int depthSize = (areaDepth - (depthNum * intervalZ)) / depthNum;
	
	pointArea*** newPointArea3D = new pointArea**[depthNum];
	
	for (int i = 0; i < depthNum; i++) {
		newPointArea3D[i] = makePointArea(areaWidth, areaHeight, areaPointX, areaPointY, rowNum, colNum, intervalX, intervalY,
										  areaPointZ + (i * (depthSize + intervalZ)),
										  areaPointZ + ((i + 1) * depthSize + (i * intervalZ)));
	}
	return newPointArea3D;
}


// 2次元フロント用ポイントエリア作成
pointArea** makeFrontPointArea(int areaWidth, 
						       int areaHeight, 
						       int areaPointX, 
						       int areaPointY, 
						       int intervalX, 
						       int intervalY,
						       float areaWX,
						       float areaWY,
						       float areaWZ,
						       float areaPointWX,
						       float areaPointWY,
						       float areaPointWZ,
						       int rowNum, 
						       int colNum,
							   int depthNum) {

	// 正面の縦数はdepthsの数を使用する
	int colSize = (areaHeight - (depthNum * intervalY)) / depthNum ;
	int rowSize = (areaWidth - (rowNum * intervalX)) / rowNum ;

	float sizeWX = areaWX / rowNum;
	float sizeWY = areaWY / colNum;
	float sizeWZ = areaWZ / depthNum;

	// エリアの2次元配列を準備
	pointArea** newPointArea = new pointArea*[depthNum];

	for (int i = 0; i < depthNum; i++) {
		newPointArea[i] = new pointArea[rowNum];
	}

	// 2次元配列のデータを設定
	for (int i = 0; i < depthNum; i++) {
		for(int j = 0; j < rowNum; j++) {
			newPointArea[i][j].setPointData(areaPointX + (j * (rowSize + intervalX)),
										  areaPointX + ((j + 1) * rowSize + (j * intervalX)),
										  areaPointY + areaHeight - (i * (colSize + intervalY)), 
										  areaPointY + areaHeight - ((i + 1) * colSize + (i * intervalY)),
										  areaPointWX + j * sizeWX,
										  areaPointWX + (j + 1) * sizeWX,
										  areaPointWY,
										  areaPointWY + sizeWY,
										  areaPointWZ + i * sizeWZ,
										  areaPointWZ + (i + 1) * sizeWZ
										  );		
		}
	}
	return newPointArea;
}


// 正面データカウント用ポイントエリア作成
researchArea** makeResearchArea(int rows, int cols, int dividesAreaX, int dividesAreaY) {

	int rowNum = (int) rows / dividesAreaX;
	int colNum = (int) cols / dividesAreaY;

	int rowNotMuch = rows % dividesAreaX;
	int colNotMuch = cols % dividesAreaY;

	researchArea** newResearchArea;

	// エリアの2次元配列を準備
	newResearchArea = new researchArea*[cols];

	for (int i = 0; i < colNum; i++) {
		newResearchArea[i] = new researchArea[rows];
	}

	std::ostringstream oss;
	int num;
	std::string nam;

	for (int i = 0; i < dividesAreaY; i++) {
		for (int j = 0; j < dividesAreaX; j++) {
			nam = "";
			num = (i * 3) + j + 1;
			oss.str("");
			oss << "zone" << num;
			nam = oss.str();
			newResearchArea[i][j].setName(nam);
			if ((i == dividesAreaY -1) && (j == dividesAreaX -1 )) {
				newResearchArea[dividesAreaY - 1 - i][j].setMinRow(j * rowNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxRow((j + 1) * rowNum + rowNotMuch - 1);
				newResearchArea[dividesAreaY - 1 - i][j].setMinCol(i * colNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxCol((i + 1) * colNum + colNotMuch - 1);
			}
			else if (i == dividesAreaY - 1) {
				newResearchArea[dividesAreaY - 1 - i][j].setMinRow(j * rowNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxRow((j + 1) * rowNum - 1);
				newResearchArea[dividesAreaY - 1 - i][j].setMinCol(i * colNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxCol((i + 1) * colNum + colNotMuch - 1);
			} else if (j == dividesAreaX -1) {
				newResearchArea[dividesAreaY - 1 - i][j].setMinRow(j * rowNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxRow((j + 1) * rowNum + rowNotMuch - 1);
				newResearchArea[dividesAreaY - 1 - i][j].setMinCol(i * colNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxCol((i + 1) * colNum - 1);
			} else {
				newResearchArea[dividesAreaY - 1 - i][j].setMinRow(j * rowNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxRow((j + 1) * rowNum - 1);
				newResearchArea[dividesAreaY - 1 - i][j].setMinCol(i * colNum);
				newResearchArea[dividesAreaY - 1 - i][j].setMaxCol((i + 1) * colNum - 1);
			}
		}
	}
	return newResearchArea;
}

// ポイントエリア開放
void releasePointArea(pointArea** pointArea, int colNum) {
	for(int i = 0; i < colNum; i++) {
		delete pointArea[i];
	}
	
	delete [] pointArea;
}

// float > String
std::string float2string(float f){
	std::string st;
	std::stringstream ss;
	ss << f;
	ss >> st;
	return st;
}

// int > String
std::string int2string(int i){
	std::string st;
	std::stringstream ss;
	ss << i;
	ss >> st;
	return st;
}

void drawRect(cv::Mat rectImage, pointArea pointArea, int count, int colorNum, int maxColorNum) {
	if (count > 500) {

		int colorUchar = colorNum * 255 / maxColorNum ;
		cv::Vec3b color = makeColor(colorUchar);

		//描画
		// Blue，塗りつぶし，アンチエイリアス
		cv::rectangle(rectImage,
			          cv::Point(pointArea.getMinX(),pointArea.getMinY()),
			          cv::Point(pointArea.getMaxX(),pointArea.getMaxY()),
					  cv::Scalar(color[0],color[1],color[2]),
					  -1,
					  CV_AA);
	}
}

void enterCount(pointArea** newPointAreaFront, researchArea** searchArea, int dividesAreaX, int dividesAreaY) {
	
	int minRowNum = 0;
	int maxRowNum = 0;
	int minColNum = 0;
	int maxColNum = 0;
	
	int count = 0;

	for (int i = 0; i < dividesAreaY; i++) {
		for (int j = 0; j < dividesAreaX; j++) {
			minRowNum = searchArea[i][j].getMinRow();
			maxRowNum = searchArea[i][j].getMaxRow();
			minColNum = searchArea[i][j].getMinCol();
			maxColNum = searchArea[i][j].getMaxCol();

			count = 0;

			bool checkflag = false;
			for (int m = minColNum; m <= maxColNum; m++) {
				for (int n = minRowNum; n <= maxRowNum; n++) {
					count += newPointAreaFront[m][n].getTouchCount();
				}
			}
			searchArea[i][j].setCount(count);
		}
	}
}

void decrementColorCount(pointArea** newPointAreaFront, int rows, int depths) {
	uchar colorCount;

	for (int i = 0; i < depths; i++) {
		for(int j = 0; j < rows; j++) {
			colorCount = newPointAreaFront[i][j].getColorCount();
			if (colorCount >= 2) {
				colorCount -= 2;
				newPointAreaFront[i][j].setColorCount(colorCount);
			}
		}
	}
}

cv::Point2d searchArea(cv::Point3f worldPoint, pointArea** area, int rows, int depths) {

	cv::Point2d ret(0,0);

	for (int i = 0; i < depths; i++) {
		for (int j = 0; j < rows; j++) {
			if( worldPoint.x > area[i][j].getMinWX() && worldPoint.x <= area[i][j].getMaxWX()) {
				if( worldPoint.z > area[i][j].getMinWZ() && worldPoint.z <= area[i][j].getMaxWZ()) {
						ret.x = i;
						ret.y = j;
						return ret;
					}
				}
		}
	}

	ret.x = -1;
	ret.y = -1;
	return ret;
}

void drawRectFront(cv::Mat frontImage, pointArea** newPointAreaFront, int rows, int depths) {

	uchar colorNum = 0;
	cv::Vec3b vec;

	for (int i = 0; i < depths; i++) {
		for(int j = 0; j < rows; j++) {

			colorNum = newPointAreaFront[i][j].getColorCount();

			if(colorNum > 0) {
				vec = changeColor(colorNum);
				cv::rectangle(frontImage,
							  cv::Point(newPointAreaFront[i][j].getMinX(),newPointAreaFront[i][j].getMinY()),
							  cv::Point(newPointAreaFront[i][j].getMaxX(),newPointAreaFront[i][j].getMaxY()),
							  cv::Scalar(vec[0],vec[1],vec[2]),
							  -1,
							  CV_AA);					
			}
		}
	}
}

//画面準備
void cameraInit() {
		cv::namedWindow("Color Camera", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
		cv::namedWindow("Front Camera", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
}

int main(int argc, char *argv[])
{
    try {

		// 色々な変数の設定

		//pointArea用の配列
		int rows = 64;
		int cols = 1;
		int depths = 20;
		int areaWidth = 640;
		int areaHeight= 100;
		int areaDepth = 500;
		int areaPointX = 0;
		int areaPointY = 300;
		int areaPointZ = 500;
		int intervalX = 0;
		int intervalY = 0;
		int intervalZ = 0;

		int areaFrontX = 640;
		int areaFrontY = 480;

		float areaWX = 800;
		float areaWY = 800;
		float areaWZ = 500;
		float areaPointWX = -400;
		float areaPointWY = -400;
		float areaPointWZ = 500;

		int dividesAreaX = 3;
		int dividesAreaY = 3;

		//count
		int countNum = 500;

		//websocketcount
		int wscount = 0;

		//bool curlflag
		int curlflag = 1;

		if(argc != 25) {
			;
		} else {
			rows = atoi(argv[1]);
			cols = atoi(argv[2]);
			depths = atoi(argv[3]);
			areaWidth = atoi(argv[4]);
			areaHeight= atoi(argv[5]);
			areaDepth = atoi(argv[6]);
			areaPointX = atoi(argv[7]);
			areaPointY = atoi(argv[8]);
			areaPointZ = atoi(argv[9]);
			intervalX = atoi(argv[10]);
			intervalY = atoi(argv[11]);
			intervalZ = atoi(argv[12]);
			areaFrontX = atoi(argv[13]);
			areaFrontY = atoi(argv[14]);
			areaWX = (float) atof(argv[15]);
			areaWY = (float) atof(argv[16]);
			areaWZ = (float) atof(argv[17]);
			areaPointWX = (float) atof(argv[18]);
			areaPointWY = (float) atof(argv[19]);
			areaPointWZ = (float) atof(argv[20]);

			//count
			countNum = atoi(argv[21]);
			dividesAreaX = atoi(argv[22]);
			dividesAreaY = atoi(argv[23]);

			//curl flag
			curlflag = atoi(argv[24]);
		}


		//curl
		CURL *curl;
		CURLcode res;

		if(curlflag == 0) { 	
			curl = curl_easy_init();

			if(curl) {
				curl_easy_setopt(curl, CURLOPT_PROXY, "XXX.XXX.XXX.XXX:8080");
				curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD,"user:password");
				curl_easy_setopt(curl, CURLOPT_URL, "http://example.com:8888/connections/");
				curl_easy_setopt(curl, CURLOPT_POST, 1L);
			}
		}

		bool enterFlag = false;
		bool captureFlag = false;

		cv::Mat image;

		pointArea*** newPointArea3D = new pointArea**[depths];
		newPointArea3D = makePointArea3D(areaWidth, areaHeight, areaDepth,
			                             areaPointX, areaPointY, areaPointZ,
										 rows, cols, depths,
										 intervalX, intervalY, intervalZ);


		pointArea** newPointAreaFront = new pointArea*[cols];
		newPointAreaFront = makeFrontPointArea(areaFrontX, areaFrontY, 0, 0, 0, 0, 
                                               areaWX, areaWY, areaWZ, areaPointWX, areaPointWY, areaPointWZ, 
											   rows, cols, depths);

		researchArea** newResearchArea = new researchArea*[dividesAreaX];		
		newResearchArea = makeResearchArea(rows, depths, dividesAreaX, dividesAreaY);

		// OpenNIの初期化
		openni::OpenNI::initialize();
        openni::Device device;

		auto ret = device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error( "" );
        }

		openni::VideoStream colorStream;
		colorStream.create( device, openni::SensorType::SENSOR_COLOR );

		colorStream.start();
		openni::VideoStream depthStream;
        depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
        depthStream.start();

		device.setDepthColorSyncEnabled(true);
		device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
		cv::Mat colorImage;
        cv::Mat depthImage;
		cv::Mat pointImage = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
		cv::Mat NewImage = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
		cv::Mat captureImage = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

		int flag = 0;

		// メインループ
	    while (1) {

			//正面画像の読み込み
			image = cv::imread("front.png", 1);

			cv::Mat rectImage = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

			// OpenNIのカラー情報を読み込む
			openni::VideoFrameRef colorFrame;
            colorStream.readFrame( &colorFrame );

			// OpenNIのdepth情報を読み込む
			openni::VideoFrameRef depthFrame;
			depthStream.readFrame( &depthFrame );


			if ( colorFrame.isValid() ) {

					// カラー情報を取得
                    colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                                          colorStream.getVideoMode().getResolutionX(),
                                          CV_8UC3, (char*)colorFrame.getData() );

					// RGB → BGR形式に変換する。
					cv::cvtColor( colorImage, colorImage, CV_RGB2BGR );

					// Depth情報を取得
                    depthImage = cv::Mat( depthStream.getVideoMode().getResolutionY(),
                                          depthStream.getVideoMode().getResolutionX(),
                                          CV_16U, (char*)depthFrame.getData() );


					// カメラ上のdepth調査エリアを描画する。
					for (int i = 0; i < cols; i++) {
						for (int j = 0; j < rows; j++) {
							newPointArea3D[0][i][j].drawArea(colorImage);
						}
					}

					// flagをfalseに戻す。
					for (int i = 0; i < depths; i++) {
						for (int j = 0; j < rows; j++) {
							newPointAreaFront[i][j].setTouchFlag(false);
						}
					}

					// enterFlagをfalseに戻す。
					enterFlag = false;

					int count = 0;
					
					auto videoMode = depthStream.getVideoMode();
					short* depth = (short*)depthFrame.getData();

					// 調査対象範囲のdepth値の調査
					for (int i = 0; i < cols; i++) {
						for(int j = 0; j < rows; j++) {
							// 調査座標分のdepth配列を準備
							int *vec;

							for(int k = depths - 1 ; k >= 0; k--) {
								int colorCount = 0;
								int touchCount = 0;

								vec = new int[newPointArea3D[k][i][j].getHeight() * newPointArea3D[k][i][j].getWidth()];
								count = newPointArea3D[k][i][j].innerDepthCount(videoMode.getResolutionX(), depth, vec);

								colorCount = newPointArea3D[k][i][j].getColorCount();
								touchCount = newPointArea3D[k][i][j].getTouchCount();

								touchCount += 1;
								newPointArea3D[k][i][j].setTouchCount(touchCount);

								if (count > countNum) {
									enterFlag = true;

									if(colorCount < 200) {
										colorCount = newPointArea3D[k][i][j].getColorCount();
										colorCount = colorCount + 50;
										newPointArea3D[k][i][j].setColorCount(colorCount);
									}
								}
								
								if (count > countNum) {
									float wx, wy, wz;
									cv::Point3f point = newPointArea3D[k][i][j].getCenter();
									openni::CoordinateConverter::convertDepthToWorld(depthStream,
															 point.x,
															 point.y,
															 point.z,
															 &wx,
															 &wy,
															 &wz);

									cv::Point3f wpoint(wx, wy, wz);
									cv::Point2d wpoint2d;

									wpoint2d = searchArea(wpoint, newPointAreaFront, rows, depths);
									int x = (int)wpoint2d.x;
									int y = (int)wpoint2d.y;

									if (x >= 0 && y >= 0) {
										newPointAreaFront[x][y].setColorCount(newPointArea3D[k][i][j].getColorCount());
										newPointAreaFront[x][y].setTouchCount(newPointAreaFront[x][y].getTouchCount() + 1);

										// カウント上昇エリアの淵を描画する
										cv::rectangle(image,
											          cv::Point(newPointAreaFront[x][y].getMinX(), newPointAreaFront[x][y].getMinY()),
										              cv::Point(newPointAreaFront[x][y].getMaxX(), newPointAreaFront[x][y].getMaxY()),
										              cv::Scalar(0,255,255),
										              2,
										              CV_AA);
										
										newPointAreaFront[x][y].setTouchFlag(true);
										
										cv::Point3f point2 = newPointArea3D[1][0][1].getCenter();
										float wx2, wy2, wz2;
										openni::CoordinateConverter::convertDepthToWorld(depthStream,
															 point2.x,
															 point2.y,
															 point2.z,
															 &wx2,
															 &wy2,
															 &wz2);
									}									
								}

								drawRect(rectImage, newPointArea3D[k][i][j], count, k, depths);					
								delete [] vec;
							}
						}
					}

					decrementColorCount(newPointAreaFront, rows, depths);
					drawRectFront(NewImage, newPointAreaFront, rows, depths);
					
					// カメラ画像とヒートマップを合成する。
					cv::add(NewImage, image, image);
					cv::add(rectImage, colorImage, colorImage);

                    cv::imshow( "Color Camera", colorImage );
                    cv::imshow( "Front Camera", image );

					wscount++;
					if(curlflag == 0) { 
						if (wscount > 120) {
							std::string stX, stY, stZ;
							cv::Point3f retPoint;

							// Heatmap用データを作成する
							std::string temp = "connection={\"max\": \"30\", \"data\":[";
							for (int i = 0; i < depths; i++) {
								for (int j = 0; j < rows; j++) {
									if(newPointAreaFront[i][j].getTouchCount() > 0) {

										temp += "{";
										retPoint = newPointAreaFront[i][j].getCenter();

										stX = float2string(retPoint.x);
											stY = float2string(retPoint.y);
										stZ = float2string(retPoint.z);

										temp = temp + "\"x\": \"";
										temp += stX; 
										temp += "\", \"y\": \"";
										temp += stY; 
										temp += "\" , \"count\" : \"";
										temp += int2string(newPointAreaFront[i][j].getTouchCount());
										temp += "\"},";
									}
								}
							}

							int length = temp.length();
							temp.erase(length - 1);
							temp += "]}";
							int len = temp.length();
							char* postthis = new char[len+1];
							memcpy(postthis, temp.c_str(), len+1);

							if(curl) {
								curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
								curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
								res = curl_easy_perform(curl);
							}
						wscount = 0;
						}	
					}
			}
			int key = cv::waitKey( 10 );
            if ( key == 'q' ) {
				break;
            }
		}
	}
    catch ( std::exception& ) {
        std::cout << openni::OpenNI::getExtendedError() << std::endl;
		return 1;
    }
	return 0;
}