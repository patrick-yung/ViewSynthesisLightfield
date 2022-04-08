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

	int x=(Vx+120)/(Baseline);
	int y=(Vy+120)/(Baseline);
	int img1,img2,img3,img4;
	if(x<8&&y<8){
		img1=((y)*9+(x));
		img2=((y+1)*9+(x));
		img3=((y)*9+(x+1));
		img4=((y+1)*9+(x+1));
	}else if(x==8&& y<8){
		img1=((y)*9+(x-1));
		img2=((y+1)*9+(x-1));
		img3=((y)*9+(x));
		img4=((y+1)*9+(x));

	}else if(x<8&&y==8){
		img1=((y-1)*9+(x));
		img2=((y)*9+(x));
		img3=((y-1)*9+(x+1));
		img4=((y)*9+(x+1));

	}else if(x==8&&y==8){
		img1=((y-1)*9+(x-1));
		img2=((y)*9+(x-1));
		img3=((y-1)*9+(x));
		img4=((y)*9+(x));
	}


	Bitmap tmp =viewImageList[3];
	if(targetFocalLen==100){
		for (int r = 0; r < Resolution_Row; r++)
		{
			for (int c = 0; c < Resolution_Col; c++)
			{
				Point3d rayRGB(0, 0, 0);
					unsigned char red, green, blue;
				//! resample the pixel value of this ray: TODO
				if(r<=Resolution_Row/2&&c<=Resolution_Col/2){
					//Image 1//
					if(r-Vz>0&&c-Vz>0){
						viewImageList[img1].getColor(c, r, red,green, blue);
					}else{
						red=0;
						green=0;
						blue=0;
					}
						
				}else if(r>Resolution_Row/2&&c<=Resolution_Col/2){
					//Image 2//
					if(r+Vz<Resolution_Row&&c-Vz>0){
						viewImageList[img2].getColor(c, r, red,green, blue);
					}else{
						red=0;
						green=0;
						blue=0;
					}
				}else if(r<=Resolution_Row/2&&c>=Resolution_Col/2){
					//Image 3//
					if(r-Vz>0&&c+Vz<Resolution_Col){

						viewImageList[img3].getColor(c, r, red,green, blue);
					}else{
						red=0;
						green=0;
						blue=0;
					}
				}else if(r>=Resolution_Row/2&&c>=Resolution_Col/2){
					//Image 4//
					if(r+Vz<Resolution_Row&&c+Vz<Resolution_Col){
						viewImageList[img4].getColor(c, r, red,green, blue);
					}else{
						red=0;
						green=0;
						blue=0;
					}


				}else{
						red=0;
						green=0;
						blue=0;
				}


				//! record the resampled pixel value
				targetView.setColor(c, r, (unsigned char) rayRGB.x, (unsigned char) rayRGB.y, (unsigned char) rayRGB.z);
			}
		}
	}else if(targetFocalLen<100){
		int end=Resolution_Row/(2*crop);
		for (int r = 0; r < Resolution_Row; r+=1)
		{
			int start=Resolution_Row/(2*crop);
			for (int c = 0; c < Resolution_Col; c+=1)
			{
				Point3d rayRGB(0, 0, 0);
				unsigned char red, green, blue;
				//! resample the pixel value of this ray: TODO
				if(end>0&&r<end){
					end--;
					red=0;
					green=0;
					blue=0;
					end=Resolution_Row/(2*crop);
				}else if(end>0&&Resolution_Row-r<end){
					red=0;
					green=0;
					blue=0;
					
				}else if(start>0&&c<start){
					start--;
					red=0;
					green=0;
					blue=0;
					start=Resolution_Row/(2*crop);
				}else if(start>0&&Resolution_Row-c<start){
					red=0;
					green=0;
					blue=0;
				}else if(r<=Resolution_Row/2&&c<=Resolution_Col/2){
					//Image 1//
						viewImageList[img1].getColor((c-start)*crop, (r-end)*crop, red,green, blue);

				}else if(r>Resolution_Row/2&&c<=Resolution_Col/2){
					//Image 2//
						viewImageList[img2].getColor((c-start)*crop, (r-end)*crop, red,green, blue);

				}else if(r<=Resolution_Row/2&&c>=Resolution_Col/2){
					//Image 3//
						viewImageList[img3].getColor((c-start)*crop, (r-end)*crop, red,green, blue);
				}else if(r>=Resolution_Row/2&&c>=Resolution_Col/2){
					//Image 4//
						viewImageList[img4].getColor((c-start)*crop, (r-end)*crop, red,green, blue);

				}else{
					red=0;
					green=0;
					blue=0;
				}
				rayRGB.x=red;
				rayRGB.y=green;
				rayRGB.z=blue;

				//! record the resampled pixel value
				targetView.setColor(c, r, (unsigned char) rayRGB.x, (unsigned char) rayRGB.y, (unsigned char) rayRGB.z);
			}
		}
	}else if(targetFocalLen>100){
		int start=Resolution_Row/2-Resolution_Row/(2*skip);
		for (int r = 0; r < Resolution_Row; r+=1)
		{
			for (int c = 0; c < Resolution_Col; c+=1)
			{
				Point3d rayRGB(0, 0, 0);
				unsigned char red, green, blue;
				//! resample the pixel value of this ray: TODO
				 if(r<=Resolution_Row/2&&c<=Resolution_Col/2){
					//Image 1//
						viewImageList[img1].getColor(start+(c)/skip,start+(r)/skip, red,green, blue);

				}else if(r>Resolution_Row/2&&c<=Resolution_Col/2){
					//Image 2//
						viewImageList[img1].getColor(start+(c)/skip, start+(r)/skip, red,green, blue);

				}else if(r<=Resolution_Row/2&&c>=Resolution_Col/2){
					//Image 3//
						viewImageList[img1].getColor(start+(c)/skip, start+(r)/skip, red,green, blue);
				}else if(r>=Resolution_Row/2&&c>=Resolution_Col/2){
					//Image 4//
					viewImageList[img1].getColor(start+(c)/skip, start+(r)/skip, red,green, blue);

				}else{
					red=0;
					green=0;
					blue=0;
				}
				rayRGB.x=red;
				rayRGB.y=green;
				rayRGB.z=blue;

				//! record the resampled pixel value
				targetView.setColor(c, r, (unsigned char) rayRGB.x, (unsigned char) rayRGB.y, (unsigned char) rayRGB.z);
			}
		}
	}
	string savePath = "newView.bmp";
	targetView.save(savePath.c_str());
	cout << "Result saved!" << endl;
	return 0;
}