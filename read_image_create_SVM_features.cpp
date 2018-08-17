// read_image_create_SVM_features.cpp : �������̨Ӧ�ó������ڵ㡣
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
unsigned char *pBmpBuf;//����ͼ�����ݵ�ָ��

int bmpWidth;//ͼ��Ŀ�
int bmpHeight;//ͼ��ĸ�
RGBQUAD *pColorTable;//��ɫ��ָ��

int biBitCount;//ͼ�����ͣ�ÿ����λ��
//����һ��ͼ��λͼ���ݡ����ߡ���ɫ��ָ�뼰ÿ������ռ��λ������Ϣ,����д��ָ���ļ���


//���Ȼ�ȡ�ļ����µ�ÿ��ͼƬ��·��
void getAllFiles(string path, vector<string>&files, string fileType) {

	//�ļ����
	long hFile = 0;
	struct _finddata_t  fileInfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*" + fileType).c_str(), &fileInfo)) != -1) {
		do {
			//files.push_back(p.assign(path).append("\\").append(fileInfo.name));
			files.push_back(fileInfo.name);
			printf("%s\n", fileInfo.name);
		} while (_findnext(hFile, &fileInfo) == 0);

		_findclose(hFile);//�رվ��

	}

}




BITMAPINFOHEADER  readBmp(char *bmpName)
{
	FILE *fp = fopen(bmpName, "rb");//�����ƶ���ʽ��ָ����ͼ���ļ�

	if (fp == 0) {
		printf("Can't open the file\n");
	}

		

	//����λͼ�ļ�ͷ�ṹBITMAPFILEHEADER

	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��

	BITMAPINFOHEADER head;
	

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //��ȡͼ����ߡ�ÿ������ռλ������Ϣ

	bmpWidth = head.biWidth;

	bmpHeight = head.biHeight;

	biBitCount = head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ�����������4�ı�����

	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256

	if (biBitCount == 8)
	{

		//������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�

		pColorTable = new RGBQUAD[256];

		fread(pColorTable, sizeof(RGBQUAD), 256, fp);

	}

	//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�

	pBmpBuf = new unsigned char[lineByte * bmpHeight];

	fread(pBmpBuf, 1, lineByte * bmpHeight, fp);

	fclose(fp);//�ر��ļ�

  //��ȡ�ļ��ɹ�
	return head;
}

bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int biBitCount, RGBQUAD *pColorTable)
{

	//���λͼ����ָ��Ϊ0����û�����ݴ��룬��������

	if (!imgBuf)
		return 0;

	//��ɫ���С�����ֽ�Ϊ��λ���Ҷ�ͼ����ɫ��Ϊ1024�ֽڣ���ɫͼ����ɫ���СΪ0

	int colorTablesize = 0;

	if (biBitCount == 8)
		colorTablesize = 1024;

	//���洢ͼ������ÿ���ֽ���Ϊ4�ı���

	int lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	//�Զ�����д�ķ�ʽ���ļ�

	FILE *fp = fopen(bmpName, "wb");

	if (fp == 0)
		return 0;

	//����λͼ�ļ�ͷ�ṹ��������д�ļ�ͷ��Ϣ

	BITMAPFILEHEADER fileHead;

	fileHead.bfType = 0x4D42;//bmp����

							 //bfSize��ͼ���ļ�4����ɲ���֮��

	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte*height;

	fileHead.bfReserved1 = 0;

	fileHead.bfReserved2 = 0;

	//bfOffBits��ͼ���ļ�ǰ3����������ռ�֮��

	fileHead.bfOffBits = 54 + colorTablesize;

	//д�ļ�ͷ���ļ�

	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	//����λͼ��Ϣͷ�ṹ��������д��Ϣͷ��Ϣ

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

	//дλͼ��Ϣͷ���ڴ�

	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	//����Ҷ�ͼ������ɫ��д���ļ� 

	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);

	//дλͼ���ݽ��ļ�

	fwrite(imgBuf, height*lineByte, 1, fp);

	//�ر��ļ�

	fclose(fp);

	return 1;

}
void doIt(const char *image_file_path,string writePath,string mFileName)
{
	char *imagepath="";
	imagepath=const_cast<char*>(image_file_path);	
	BITMAPINFOHEADER head;
	head=readBmp(imagepath);
	// �������ͼ����Ϣ
	cout << "width=" << bmpWidth << ",height=" << bmpHeight << ",biBitCount=" << biBitCount << endl;
	// ͼ����ֽ���
	int linebyte1 = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;
	int n = 0, m = 0, count_xiang_su = 0;

	double TotalPixelColor[3] = { 0.0 };	//bmpͼ����ɫ�洢˳�򣨵͵�ַ���ߵ�ַ��ΪBGR
	//int total_R = 0, total_G = 0, total_B = 0;

	// ofstream outfile("ͼ������.txt",ios::in|ios::trunc);���������ݴ���TXT�ļ���
	//��ʼ��ԭʼ���ص����顣

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
				for (int k = 0; k<3; k++)//ÿ����RGB���������ֱ���0�ű�ɺ�ɫ,RGB�洢˳��ΪBGR
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
		cout << "�ܵ����ظ���Ϊ:" << n << endl;
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
					for (int k = 2; k >= 0; k--)//ÿ����RGB���������ֱ���0�ű�ɺ�ɫ,RGB�洢˳��ΪBGR,���˳��ΪR G B Px Py
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
	//char ch[8]; //���10^8֡
	//_itoa(Num,ch,10);
	//char writePath[300] = "C:\\Users\\Administrator\\Desktop\\matrix\\";
	//strcat(writePath,ch);
	//strcat(writePath, ".bmp");
	//saveBmp(writePath, pBmpBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

	//�����������pBmpBuf��pColorTable��ȫ�ֱ��������ļ�����ʱ����Ŀռ�

	delete[]pBmpBuf;
	if (biBitCount == 8)
		delete[]pColorTable;

}

int main()
{
	vector<string> temp;
	string ImgPath = "C:\\Users\\Administrator\\Desktop\\read_image_create_SVM_features\\image-301-cut";
	string writePath = "C:\\Users\\Administrator\\Desktop\\matrix\\";
	getAllFiles(ImgPath,temp,".bmp");//"E:\\����ʦ������Ŀ\\��Ŀ����\\ʵ����Ƶ\\ʵ������\\SVM_Train_sequence\\Train_Original_image\\ch1_video_01", temp, ".bmp");
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
	}//���ÿ��ͼƬ��·�����

	

	system("pause");
	return 0; 
	
}

