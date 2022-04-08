#include "bmp.h"		//	Simple .bmp library
#include<iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

#define Baseline 30.0
#define Focal_Length 100
#define Image_Width 35.0
#define Image_Height 35.0
#define Resolution_Row 512
#define Resolution_Col 512
#define View_Grid_Row 9
#define View_Grid_Col 9
#define MAX_SHADES 8

char shades[MAX_SHADES] = {'@','#','%','*','|','-','.',' '};

struct Point3d
{
	double x;
	double y;
	double z;
	Point3d(double x_, double y_, double z_) :x(x_), y(y_), z(z_) {}
};

struct Point2d
{
	double x;
	double y;
	Point2d(double x_, double y_) :x(x_), y(y_) {}
};


int main(int argc, char** argv)
{
	if(argc < 5 || argc > 6)
	{
		cout << "Arguments prompt: viewSynthesis.exe <LF_dir> <X Y Z> OR: viewSynthesis.exe <LF_dir> <X Y Z> <focal_length>" << endl;
		return 0;
	}
	string LFDir = argv[1];
	double Vx = stod(argv[2]), Vy = stod(argv[3]), Vz = stod(argv[4]);
	double targetFocalLen = 100; // default focal length for "basic requirement" part
	if(argc == 6)
	{
		targetFocalLen = stod(argv[5]);
	}


	vector<Bitmap> viewImageList;
	//! loading light field views
	for (int i = 0; i < View_Grid_Col*View_Grid_Row; i++)
	{
		char name[128];
		sprintf(name, "/cam%03d.bmp", i);
		string filePath = LFDir + name;
		Bitmap view_i(filePath.c_str());
		viewImageList.push_back(view_i);
	
	}


	Bitmap targetView(Resolution_Col, Resolution_Row);
	cout << "Synthesizing image from viewpoint: (" << Vx << "," << Vy << "," << Vz << ") with focal length: " << targetFocalLen << endl;
	//! resample pixels of the target view one by one
	int skip=1;
	int crop=Vz;
	if(targetFocalLen<100){
		crop=100/targetFocalLen;
	}
	if(targetFocalLen>100){
		skip=targetFocalLen/100;
	}

	int img1,img2,img3,img4;

	int x1=(Vx+120)/(Baseline);
	int y1=(Vy+120)/(Baseline);
		img1=((y1)*9+(x1));
		img2=((y1+1)*9+(x1));
		img3=((y1)*9+(x1+1));
		img4=((y1+1)*9+(x1+1));
	if(x1<8&&y1<8){
		img1=((y1)*9+(x1));
		img2=((y1+1)*9+(x1));
		img3=((y1)*9+(x1+1));
		img4=((y1+1)*9+(x1+1));
	}else if(x1==8&& y1<8){
		img1=((y1)*9+(x1-1));
		img2=((y1+1)*9+(x1-1));
		img3=((y1)*9+(x1));
		img4=((y1+1)*9+(x1));

	}else if(x1<8&&y1==8){
		img1=((y1-1)*9+(x1));
		img2=((y1)*9+(x1));
		img3=((y1-1)*9+(x1+1));
		img4=((y1)*9+(x1+1));

	}else if(x1	==8&&y1	==8){
		img1=((y1-1)*9+(x1-1));
		img2=((y1)*9+(x1-1));
		img3=((y1-1)*9+(x1));
		img4=((y1)*9+(x1));
	}	
	double alp=(Vx+120)/30-x1;
	double beta=(Vy+120)/30-y1;
	double tmp3=(Resolution_Row-1)/2;
	
	for (int r = 0; r < Resolution_Row; r++)
	{
		for (int c = 0; c < Resolution_Col; c++)
		{
			Point3d rayRGB(0, 0, 0);
			int tmp1=(c-tmp3)/Resolution_Col*Image_Width;
			int tmp2=(r-tmp3)/Resolution_Row*Image_Height;
			int ax=Vx+120+Vz/targetFocalLen*tmp1;
			int bx=Vy+120+Vz/targetFocalLen*tmp2;


			int m =(c-tmp3)/targetFocalLen*100+Resolution_Col/2;
			int n =(r-tmp3)/targetFocalLen*100+Resolution_Row/2;
			
			unsigned char red[4], green[4], blue[4];
			int x,y,z;
			if(m>0&&n>0&&m<512&&n<512){
			viewImageList[img1].getColor(m, n, red[0],green[0], blue[0]);
			viewImageList[img2].getColor(m, n, red[1],green[1], blue[1]);
			viewImageList[img3].getColor(m, n, red[2],green[2], blue[2]);
			viewImageList[img4].getColor(m, n, red[3],green[3], blue[3]);
			x=(1-beta)* ((1-alp)*red[0]+alp*red[1]) + beta * ((1-alp)*red[2]+alp*red[3]);
			 y=(1-beta)* ((1-alp)*green[0]+alp*green[1]) + beta * ((1-alp)*green[2]+alp*green[3]);
			z=(1-beta)* ((1-alp)*blue[0]+alp*blue[1]) + beta * ((1-alp)*blue[2]+alp*blue[3]);
			}else{
				x=0;
				y=0;
				z=0;
			}
			if(Vz<0){
				if(c>Resolution_Col/2&&r>Resolution_Row/2){
					rayRGB.x=0;
					rayRGB.y=0;
					rayRGB.z=z;
				}else if(c<Resolution_Col/2&&r>Resolution_Row/2){
					rayRGB.x=0;
					rayRGB.y=y;
					rayRGB.z=0;
				}else if(c>Resolution_Col/2&&r<Resolution_Row/2){
					rayRGB.x=x;
					rayRGB.y=0;
					rayRGB.z=0;
				}else {
					rayRGB.x=x;
					rayRGB.y=y;
					rayRGB.z=0;
				}
			}else{
				if((ax<240&&0<ax)&&(bx<240&&0<bx)&&(m<Resolution_Row&&0<m)&&(n<Resolution_Col&&0<n)){
					rayRGB.x=x;
					rayRGB.y=y;
					rayRGB.z=z;
				}else{
					rayRGB.x=0;
					rayRGB.y=0;
					rayRGB.z=0;
				}
				
			}
			//! record the resampled pixel value
			targetView.setColor(c, r, (unsigned char) rayRGB.x, (unsigned char) rayRGB.y, (unsigned char) rayRGB.z);
		}
	}
	

	string savePath = "newView.bmp";
	targetView.save(savePath.c_str());
	cout << "Result saved!" << endl;
	return 0;
}