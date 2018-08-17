// read_image_create_SVM_features.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
#include<iostream>
#include<windows.h>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<cmath>
#include<iomanip>
#include<io.h>
#include<string>
#include<vector>
#include<string.h>


using namespace std;
unsigned char *pBmpBuf;//读入图像数据的指针

int bmpWidth;//图像的宽
int bmpHeight;//图像的高
RGBQUAD *pColorTable;//颜色表指针

int biBitCount;//图像类型，每像素位数
//给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息,将其写到指定文件中


//首先获取文件夹下的每个图片的路径
void getAllFiles(string path, vector<string>&files, string fileType) {

	//文件句柄
	long hFile = 0;
	struct _finddata_t  fileInfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*" + fileType).c_str(), &fileInfo)) != -1) {
		do {
			//files.push_back(p.assign(path).append("\\").append(fileInfo.name));
			files.push_back(fileInfo.name);
			printf("%s\n", fileInfo.name);
		} while (_findnext(hFile, &fileInfo) == 0);

		_findclose(hFile);//关闭句柄

	}

}




BITMAPINFOHEADER  readBmp(char *bmpName)
{
	FILE *fp = fopen(bmpName, "rb");//二进制读方式打开指定的图像文件

	if (fp == 0) {
		printf("Can't open the file\n");
	}

		

	//跳过位图文件头结构BITMAPFILEHEADER

	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中

	BITMAPINFOHEADER head;
	

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //获取图像宽、高、每像素所占位数等信息

	bmpWidth = head.biWidth;

	bmpHeight = head.biHeight;

	biBitCount = head.biBitCount;//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）

	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;//灰度图像有颜色表，且颜色表表项为256

	if (biBitCount == 8)
	{

		//申请颜色表所需要的空间，读颜色表进内存

		pColorTable = new RGBQUAD[256];

		fread(pColorTable, sizeof(RGBQUAD), 256, fp);

	}

	//申请位图数据所需要的空间，读位图数据进内存

	pBmpBuf = new unsigned char[lineByte * bmpHeight];

	fread(pBmpBuf, 1, lineByte * bmpHeight, fp);

	fclose(fp);//关闭文件

  //读取文件成功
	return head;
}

bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int biBitCount, RGBQUAD *pColorTable)
{

	//如果位图数据指针为0，则没有数据传入，函数返回

	if (!imgBuf)
		return 0;

	//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0

	int colorTablesize = 0;

	if (biBitCount == 8)
		colorTablesize = 1024;

	//待存储图像数据每行字节数为4的倍数

	int lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	//以二进制写的方式打开文件

	FILE *fp = fopen(bmpName, "wb");

	if (fp == 0)
		return 0;

	//申请位图文件头结构变量，填写文件头信息

	BITMAPFILEHEADER fileHead;

	fileHead.bfType = 0x4D42;//bmp类型

							 //bfSize是图像文件4个组成部分之和

	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte*height;

	fileHead.bfReserved1 = 0;

	fileHead.bfReserved2 = 0;

	//bfOffBits是图像文件前3个部分所需空间之和

	fileHead.bfOffBits = 54 + colorTablesize;

	//写文件头进文件

	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	//申请位图信息头结构变量，填写信息头信息

	BITMAPINFOHEADER head;

	head.biBitCount = biBitCount;

	head.biClrImportant = 0;

	head.biClrUsed = 0;

	head.biCompression = 0;

	head.biHeight = height;

	head.biPlanes = 1;

	head.biSize = 40;

	head.biSizeImage = lineByte*height;

	head.biWidth = width;

	head.biXPelsPerMeter = 0;

	head.biYPelsPerMeter = 0;

	//写位图信息头进内存

	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	//如果灰度图像，有颜色表，写入文件 

	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);

	//写位图数据进文件

	fwrite(imgBuf, height*lineByte, 1, fp);

	//关闭文件

	fclose(fp);

	return 1;

}
void doIt(const char *image_file_path,string writePath,string mFileName)
{
	char *imagepath="";
	imagepath=const_cast<char*>(image_file_path);	
	BITMAPINFOHEADER head;
	head=readBmp(imagepath);
	// 输出整体图像信息
	cout << "width=" << bmpWidth << ",height=" << bmpHeight << ",biBitCount=" << biBitCount << endl;
	// 图像的字节数
	int linebyte1 = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	int n = 0, m = 0, count_xiang_su = 0;

	double TotalPixelColor[3] = { 0.0 };	//bmp图像颜色存储顺序（低地址到高地址）为BGR
	//int total_R = 0, total_G = 0, total_B = 0;

	// ofstream outfile("图像像素.txt",ios::in|ios::trunc);将像素数据存入TXT文件。
	//初始化原始像素的数组。

	if (biBitCount == 8)
	{
		for (int i = 0; i<bmpHeight / 2; i++)
		{
			for (int j = 0; j<bmpWidth / 2; i++)
				*(pBmpBuf + i*linebyte1 + j) = 0;
		}
	}

	if (biBitCount == 24)
	{
		for (int i = 0; i<bmpHeight; i++)
		{
			for (int j = 0; j<bmpWidth; j++)
			{
				for (int k = 0; k<3; k++)//每像素RGB三个分量分别置0才变成黑色,RGB存储顺序为BGR
				{
					
					 TotalPixelColor[k] += *(pBmpBuf + i*linebyte1 + j * 3 + k);
					
				}
				n++;
			}


		}
		for (int k = 2; k>=0; k--)
		{

			TotalPixelColor[k] /= n;
			printf("%lf ,", TotalPixelColor[k]);
		}
		
		cout << endl;
		cout << "总的像素个素为:" << n << endl;
		cout << "----------------------------------------------------" << endl;


		FILE *outfp;
		writePath = writePath + mFileName + ".txt";
		const char *temp_writePath;
		temp_writePath = writePath.c_str();
		outfp = fopen(temp_writePath,"w+");
		if (outfp == 0) {
			printf("Can't write the file %s\n", writePath);
		}
		else {

			for (int i = 0; i < bmpHeight; i++)
			{
				for (int j = 0; j < bmpWidth; j++)
				{
					for (int k = 2; k >= 0; k--)//每像素RGB三个分量分别置0才变成黑色,RGB存储顺序为BGR,输出顺序为R G B Px Py
					{

						fprintf(outfp,"%lf ",(*(pBmpBuf + i * linebyte1 + j * 3 + k) - TotalPixelColor[k]));

					}
					fprintf(outfp, "%d %d\n", j, i);
				}


			}
		}
		fclose(outfp);

	}

	// //jiang tuxiang shuju cunpan .
	//char ch[8]; //最多10^8帧
	//_itoa(Num,ch,10);
	//char writePath[300] = "C:\\Users\\Administrator\\Desktop\\matrix\\";
	//strcat(writePath,ch);
	//strcat(writePath, ".bmp");
	//saveBmp(writePath, pBmpBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

	//清除缓冲区，pBmpBuf和pColorTable是全局变量，在文件读入时申请的空间

	delete[]pBmpBuf;
	if (biBitCount == 8)
		delete[]pColorTable;

}

int main()
{
	vector<string> temp;
	string ImgPath = "C:\\Users\\Administrator\\Desktop\\read_image_create_SVM_features\\image-301-cut";
	string writePath = "C:\\Users\\Administrator\\Desktop\\matrix\\";
	getAllFiles(ImgPath,temp,".bmp");//"E:\\潘老师参与项目\\项目数据\\实验视频\\实验数据\\SVM_Train_sequence\\Train_Original_image\\ch1_video_01", temp, ".bmp");
	const char *temp_Imgpath;
	string string_Imgpath ="";
	for (int i = 0; i < temp.size(); i++) 
	{

		cout << temp[i] << endl;
		string_Imgpath = ImgPath + "\\" +temp[i];
		temp_Imgpath = string_Imgpath.c_str();
		string filename = temp[i].substr(0,temp[i].rfind("."));
		/*printf("This is String-char*\n");
		printf("%s\n", temp_file_path);*/
		
		doIt(temp_Imgpath, writePath , filename);
	}//输出每个图片的路径结果

	

	system("pause");
	return 0; 
	
}

