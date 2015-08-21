#include "hist.h"

//�Ҷ�ֱ��ͼ
void grayHist(void)
{
	ifstream fin("1//filelist.txt");
	if (!fin.is_open())
	{
		cout<<"can not open "<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("hist.txt");
	if (!fout.is_open()) {
		cout<<"can not create "<<endl;
		cin.get();
		exit(0);
	}

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};   
    CvHistogram *hist=NULL;//����ֱ��ͼ�Ŀ�ָ��ṹ  
  
    hist=cvCreateHist(dims, sizes, type, ranges, 1);//����ֱ��ͼ  

	IplImage *img = NULL;

	int count=0;
	char buf[1024];
	double val=0;
	while (fin.getline(buf, 1024) != NULL) {
		count ++;

		img = cvLoadImage(buf, CV_LOAD_IMAGE_GRAYSCALE);
		if (img == NULL)
		{
			cout<<"can not load "<<endl;
			cin.get();
			exit(0);
		}

		cvCalcHist(&img, hist, 0, NULL); //ͳ��ֱ��ͼ  
		cvNormalizeHist(hist, 1.0); //��һ��ֱ��ͼ 

		for (int i=0; i<256; i++) {
			val = cvQueryHistValue_1D(hist, i);
			fout<<val<<", ";

			if ((i+1) % 10 == 0)
				fout<<endl;
		}

		cvReleaseImage(&img);
		cvClearHist(hist);
		if (count == 1)
			break;
	}
}

//��ͨ��ֱ��ͼ
void singleHist(void)
{
	ifstream fin("1//filelist.txt");
	if (!fin.is_open())
	{
		cout<<"can not open "<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("hist.txt");
	if (!fout.is_open()) {
		cout<<"can not create "<<endl;
		cin.get();
		exit(0);
	}

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};   
    CvHistogram *hist=NULL;//����ֱ��ͼ�Ŀ�ָ��ṹ  
  
    hist=cvCreateHist(dims, sizes, type, ranges, 1);//����ֱ��ͼ  

	IplImage *img=NULL;
	IplImage *bImg=NULL;
	IplImage *gImg=NULL;
	IplImage *rImg=NULL;
	int count=0; //���������жϴ����ͼƬ����
	char buf[1024];
	double val=0;
	while (fin.getline(buf, 1024) != NULL) {
		count ++;

		img = cvLoadImage(buf);
		if (img == NULL)
		{
			cout<<"can not load "<<endl;
			cin.get();
			exit(0);
		}

		bImg=cvCreateImage(cvGetSize(img), img->depth, 1);
		gImg=cvCreateImage(cvGetSize(img), img->depth, 1);
		rImg=cvCreateImage(cvGetSize(img), img->depth, 1);
		if (bImg == NULL || gImg == NULL || rImg == NULL) {
			cout<<"can not create "<<endl;
			cin.get();
			exit(0);
		}
		cvSplit(img, bImg, gImg, rImg, NULL);
		//����bImg��ֱ��ͼ
		cvCalcHist(&bImg, hist, 0, NULL); //ͳ��ֱ��ͼ  
		cvNormalizeHist(hist, 1.0); //��һ��ֱ��ͼ 

		for (int i=0; i<256; i++) {
			val = cvQueryHistValue_1D(hist, i);
			fout<<val<<", ";

			if ((i+1) % 10 == 0)
				fout<<endl;
		}
		fout<<endl<<endl;

		cvClearHist(hist);
		//����gImg��ֱ��ͼ
		cvCalcHist(&gImg, hist, 0, NULL); //ͳ��ֱ��ͼ  
		cvNormalizeHist(hist, 1.0); //��һ��ֱ��ͼ 

		for (int i=0; i<256; i++) {
			val = cvQueryHistValue_1D(hist, i);
			fout<<val<<", ";

			if ((i+1) % 10 == 0)
				fout<<endl;
		}
		fout<<endl<<endl;

		cvClearHist(hist);
		//����rImg��ֱ��ͼ
		cvCalcHist(&rImg, hist, 0, NULL); //ͳ��ֱ��ͼ  
		cvNormalizeHist(hist, 1.0); //��һ��ֱ��ͼ 

		for (int i=0; i<256; i++) {
			val = cvQueryHistValue_1D(hist, i);
			fout<<val<<", ";

			if ((i+1) % 10 == 0)
				fout<<endl;
		}
		fout<<endl<<endl;

		cvReleaseImage(&img);
		cvClearHist(hist);
		if (count == 1)
			break;
	}	
}

//��������
//һ���Դ������ͼƬ����ÿ��ͼƬ��Ϊ��ͨ��ͼ�����ֱ��ͼ��Ȼ��ֱ��ͼд��.txt�ļ���
void batchHist(void)
{
	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};   

	const char *d="\\."; //�ָ���

	char *p;	
	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the list.txt"<<endl;
		cin.get();
		exit(0);
	}

	int count=0;//������
	char buf[1024];
	while (fin.getline(buf, 1024) != NULL) {
		count++;

		IplImage *img=cvLoadImage(buf);
		if (img == NULL) {
			cout<<"can not load "<<buf<<endl;
			cin.get();
			exit(0);
		}

		char *p=strtok(buf, d);
		char name[1024]={'\0'};
		char name1[1024]={'\0'};
		while(p){
			strcpy(name1, name);
			strcpy(name, p);
			p=strtok(NULL, d);
		}

		strcat(name1, ".txt");
		ofstream fout(name1);
		if (!fout.is_open()) {
			cout<<"can not create the "<<name1<<endl;
			cin.get();
			exit(0);
		}

		IplImage *Img[3];
		CvHistogram *hist[3];

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(img), img->depth, 1);
			hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
		cvSplit(img, Img[0], Img[1], Img[2], NULL);

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, 0);
			cvNormalizeHist(hist[i], 1.0);
		
			for (int j=0; j<256; j++) {
				double val=cvQueryHistValue_1D(hist[i], j);
				fout<<val<<", ";
				if ((j+1) % 10 == 0)
					fout<<endl;
			}
			fout<<endl<<endl;
		}

		cvReleaseImage(&img);
		for (int i=0; i<3; i++) {
			cvReleaseImage(&Img[i]);
			cvReleaseHist(&hist[i]);
		}
		fout.close();
	}

	fin.close();
}

//��������ͼƬ�������������ͼƬ����һ�ţ����ûҶ�ֱ��ͼƥ���ж�����ͼƬ������һ��
//
void imageDetect_GrayHist(void)
{
	const int NUM = 5;
	/*
		1 �������� gbjd
		2 ���ѳ��� fqcd
		3 �������� snsc
		4 ��ͷ������ ytdft
		5 ��������˿ sltds 
	*/
	int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range}; 

	CvHistogram *sample_hist[NUM];

	for (int i=0; i<NUM; i++) {
		sample_hist[i] = cvCreateHist(dims, sizes, type, ranges, 1);
	}

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the sample.txt"<<endl;
		cin.get();
		exit(0);
	}
	//��ȡ����ͼƬ��ֱ��ͼ��Ϣ
	IplImage *samp;
	char buf[1024];
	int count = 0;//������
	while (fin.getline(buf, 1024)!=NULL && count < NUM) {
		samp = cvLoadImage(buf, CV_LOAD_IMAGE_GRAYSCALE);
		if (samp == NULL) {
			cout<<"can not open the "<<buf<<endl;
			cin.get();
			exit(0);
		}

		cvCalcHist(&samp, sample_hist[count], 0, NULL);
		cvNormalizeHist(sample_hist[count], 1.0);
		count++;

		cvReleaseImage(&samp);
	}

	/*
	//��ȡ����ͼƬ��ֱ��ͼ��Ϣ
	CvHistogram *hist = cvCreateHist(dims, sizes, type, ranges, 1);
	IplImage *src = cvLoadImage("input.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cvCalcHist(&src, hist, 0, NULL);

	double val=0;
	for (int i=0; i<NUM; i++) {
		val = cvCompareHist(hist, sample_hist[i], CV_COMP_BHATTACHARYYA);
		cout<<val<<" ";
	}
	cout<<endl;
	*/

	//������������ͼƬ
	fin.close();
	fin.open("gbjd\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the input.txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("gbjd\\output.txt");
	if (!fout.is_open()) {
		cout<<"can not create the output.txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *hist=cvCreateHist(dims, sizes, type, ranges, 1);
	IplImage *src=NULL;
	double val[NUM]; //�洢����ͼ��ֱ��ͼ������������ͼ��ֱ��ͼ�ԱȵĽ��
	int maxNum=-1; //�洢ֱ��ͼ�Ա�֮��ֵ��С���ĸ����
	double maxVal=1; //�洢ֱ��ͼ�Ա���С��ֵ
	while (fin.getline(buf, 1024)!=NULL) {
		src = cvLoadImage(buf, CV_LOAD_IMAGE_GRAYSCALE);
		if (src == NULL) {
			cout<<"can not load image "<<buf<<endl;
			cin.get();
			exit(0);
		}

		cvCalcHist(&src, hist, 0, NULL);//����ֱ��ͼ
		cvNormalizeHist(hist, 1.0); //��һ��ֱ��ͼ
		for (int i=0; i<NUM; i++) { //�Ա�����ͼ��ֱ��ͼ��������ֱ��ͼ
			val[i] = cvCompareHist(hist, sample_hist[i], CV_COMP_BHATTACHARYYA);
		}

		for (int i=0; i<NUM-1; i++) {
			if (val[i] < maxVal)
			{
				maxNum = i+1;
				maxVal = val[i];
			}
		}

		for (int i=0; i<NUM; i++) {
			//���
			fout<<val[i]<<"  "; 
		}
		fout<<maxVal<<"  "<<maxNum<<endl;

		cout<<buf<<endl;

		maxNum = -1;
		maxVal = 1;
		cvClearHist(hist);
		cvReleaseImage(&src);
	}

	fin.close();
	fout.close();
}

//��������ͼƬ�������������ͼƬ����һ�ţ����õ�ͨ��ֱ��ͼƥ�䲢��Ȩ������ж�����ͼƬ������һ��
//
void imageDetect_MultiHist(void)
{
	const int NUM = 5;
	/*
		1 �������� gbjd
		2 ���ѳ��� fqcd
		3 �������� snsc
		4 ��ͷ������ ytdft
		5 ��������˿ sltds 
	*/
	int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range}; 

	CvHistogram *(sample_hist[NUM][3]);

	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j] = cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the sample.txt"<<endl;
		cin.get();
		exit(0);
	}

	IplImage *Img[3]; //��ͨ��ֱ��ͼ

	//��ȡ����ͼƬ��ֱ��ͼ��Ϣ
	IplImage *samp;
	char buf[1024];
	int count = 0;//������
	while (fin.getline(buf, 1024)!=NULL && count < NUM) {
		samp = cvLoadImage(buf);
		if (samp == NULL) {
			cout<<"can not open the "<<buf<<endl;
			cin.get();
			exit(0);
		}

		for (int i=0; i<3; i++) {
			Img[i] = cvCreateImage(cvGetSize(samp), samp->depth, 1);
		}
		cvSplit(samp, Img[0], Img[1], Img[2], NULL);

		cvCalcHist(&Img[0], sample_hist[count][0], 0, NULL); //bͨ��
		cvNormalizeHist(sample_hist[count][0], 1.0); //ֱ��ͼ��һ��

		cvCalcHist(&Img[1], sample_hist[count][1], 0, NULL); //gͨ��
		cvNormalizeHist(sample_hist[count][1], 1.0);

		cvCalcHist(&Img[2], sample_hist[count][2], 0, NULL);//rͨ��
		cvNormalizeHist(sample_hist[count][2], 1.0);
		count++;

		cvReleaseImage(&samp);
		for (int i=0; i<3; i++) {
		cvReleaseImage(&Img[i]);
		}
	}

	//������������ͼƬ
	fin.close();
	fin.open("snsc\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the input.txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("snsc\\output.txt"); //����Ϣд��output.txt��
	if (!fout.is_open()) {
		cout<<"can not create the output.txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(hist[3]);
	for (int i=0; i<3; i++) { //�½�ֱ��ͼ
		hist[i] = cvCreateHist(dims, sizes, type, ranges, 1);
	}
	IplImage *src=NULL;
	double val[NUM][3]; //�洢����ͼ��ֱ��ͼ������������ͼ��ֱ��ͼ�ԱȵĽ��
	int maxNum=-1; //�洢ֱ��ͼ�Ա�֮��ֵ��С���ĸ����
	double maxVal=1; //�洢ֱ��ͼ�Ա���С��ֵ
	while (fin.getline(buf, 1024)!=NULL) {
		src = cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load image "<<buf<<endl;
			cin.get();
			exit(0);
		}

		for (int i=0; i<3; i++) {
			Img[i] = cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL);

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL);
			cvNormalizeHist(hist[i], 1.0);
		}

		for (int i=0; i<NUM; i++) { //�Ա�����ͼ��ֱ��ͼ��������ֱ��ͼ
			for (int j=0; j<3; j++) {
				val[i][j] = cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}
		//��Ȩ���
		double valll[NUM]={0};
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				valll[i] += val[i][j] * 0.3;
			}
		}

		for (int i=0; i<NUM; i++) {
			fout<<valll[i]<<"  ";
			if (valll[i] < maxVal) {
				maxVal = valll[i];
				maxNum = i+1;
			}
		}
		fout<<maxVal<<"  "<<maxNum<<endl;
		maxVal = 1; //���ó�ʼֵ

		for (int j=0; j<3; j++) { 
			for (int i=0; i<NUM; i++) { //��ÿ��ͨ���бȽϵó���ƥ������
				if (val[i][j] < maxVal) {
					maxNum = i+1;
					maxVal = val[i][j];
				}

				fout<<val[i][j]<<"  ";
			}
			fout<<maxVal<<"  "<<maxNum<<endl;
			maxVal = 1;//���ó�ʼֵ
		}
		fout<<endl<<endl;

		cout<<buf<<endl;

		maxNum = -1;
		maxVal = 1;
		for (int i=0; i<3; i++) {
			cvClearHist(hist[i]); //ֱ��ͼ�������
			cvReleaseImage(&Img[i]); //���ٵ�ͨ��ͼ�� 
		}
		cvReleaseImage(&src);
	}

	fin.close();
	fout.close();
}