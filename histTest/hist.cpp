#include "hist.h"

//灰度直方图
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

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};   
    CvHistogram *hist=NULL;//创建直方图的空指针结构  
  
    hist=cvCreateHist(dims, sizes, type, ranges, 1);//创建直方图  

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

		cvCalcHist(&img, hist, 0, NULL); //统计直方图  
		cvNormalizeHist(hist, 1.0); //归一化直方图 

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

//单通道直方图
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

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};   
    CvHistogram *hist=NULL;//创建直方图的空指针结构  
  
    hist=cvCreateHist(dims, sizes, type, ranges, 1);//创建直方图  

	IplImage *img=NULL;
	IplImage *bImg=NULL;
	IplImage *gImg=NULL;
	IplImage *rImg=NULL;
	int count=0; //计数器，判断处理的图片总数
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
		//计算bImg的直方图
		cvCalcHist(&bImg, hist, 0, NULL); //统计直方图  
		cvNormalizeHist(hist, 1.0); //归一化直方图 

		for (int i=0; i<256; i++) {
			val = cvQueryHistValue_1D(hist, i);
			fout<<val<<", ";

			if ((i+1) % 10 == 0)
				fout<<endl;
		}
		fout<<endl<<endl;

		cvClearHist(hist);
		//计算gImg的直方图
		cvCalcHist(&gImg, hist, 0, NULL); //统计直方图  
		cvNormalizeHist(hist, 1.0); //归一化直方图 

		for (int i=0; i<256; i++) {
			val = cvQueryHistValue_1D(hist, i);
			fout<<val<<", ";

			if ((i+1) % 10 == 0)
				fout<<endl;
		}
		fout<<endl<<endl;

		cvClearHist(hist);
		//计算rImg的直方图
		cvCalcHist(&rImg, hist, 0, NULL); //统计直方图  
		cvNormalizeHist(hist, 1.0); //归一化直方图 

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

//批量处理
//一次性处理多张图片，将每张图片分为单通道图像后建立直方图，然后将直方图写入.txt文件中
void batchHist(void)
{
	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};   

	const char *d="\\."; //分隔符

	char *p;	
	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the list.txt"<<endl;
		cin.get();
		exit(0);
	}

	int count=0;//计数器
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

//共有六类图片，样本库里各类图片各有一张，利用灰度直方图匹配判断输入图片属于那一类
//
void imageDetect_GrayHist(void)
{
	const int NUM = 5;
	/*
		1 宫爆鸡丁 gbjd
		2 番茄炒蛋 fqcd
		3 蒜泥生菜 snsc
		4 鱼头豆腐汤 ytdft
		5 酸辣土豆丝 sltds 
	*/
	int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
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
	//提取样本图片的直方图信息
	IplImage *samp;
	char buf[1024];
	int count = 0;//计数器
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
	//提取输入图片的直方图信息
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

	//批量处理输入图片
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
	double val[NUM]; //存储输入图像直方图与六个样本库图像直方图对比的结果
	int maxNum=-1; //存储直方图对比之后值最小的哪个类别
	double maxVal=1; //存储直方图对比最小的值
	while (fin.getline(buf, 1024)!=NULL) {
		src = cvLoadImage(buf, CV_LOAD_IMAGE_GRAYSCALE);
		if (src == NULL) {
			cout<<"can not load image "<<buf<<endl;
			cin.get();
			exit(0);
		}

		cvCalcHist(&src, hist, 0, NULL);//计算直方图
		cvNormalizeHist(hist, 1.0); //归一化直方图
		for (int i=0; i<NUM; i++) { //对比输入图像直方图和样本库直方图
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
			//输出
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

//共有六类图片，样本库里各类图片各有一张，利用单通道直方图匹配并加权求和来判断输入图片属于那一类
//
void imageDetect_MultiHist(void)
{
	const int NUM = 5;
	/*
		1 宫爆鸡丁 gbjd
		2 番茄炒蛋 fqcd
		3 蒜泥生菜 snsc
		4 鱼头豆腐汤 ytdft
		5 酸辣土豆丝 sltds 
	*/
	int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
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

	IplImage *Img[3]; //单通道直方图

	//提取样本图片的直方图信息
	IplImage *samp;
	char buf[1024];
	int count = 0;//计数器
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

		cvCalcHist(&Img[0], sample_hist[count][0], 0, NULL); //b通道
		cvNormalizeHist(sample_hist[count][0], 1.0); //直方图归一化

		cvCalcHist(&Img[1], sample_hist[count][1], 0, NULL); //g通道
		cvNormalizeHist(sample_hist[count][1], 1.0);

		cvCalcHist(&Img[2], sample_hist[count][2], 0, NULL);//r通道
		cvNormalizeHist(sample_hist[count][2], 1.0);
		count++;

		cvReleaseImage(&samp);
		for (int i=0; i<3; i++) {
		cvReleaseImage(&Img[i]);
		}
	}

	//批量处理输入图片
	fin.close();
	fin.open("snsc\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the input.txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("snsc\\output.txt"); //将信息写入output.txt中
	if (!fout.is_open()) {
		cout<<"can not create the output.txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(hist[3]);
	for (int i=0; i<3; i++) { //新建直方图
		hist[i] = cvCreateHist(dims, sizes, type, ranges, 1);
	}
	IplImage *src=NULL;
	double val[NUM][3]; //存储输入图像直方图与六个样本库图像直方图对比的结果
	int maxNum=-1; //存储直方图对比之后值最小的哪个类别
	double maxVal=1; //存储直方图对比最小的值
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

		for (int i=0; i<NUM; i++) { //对比输入图像直方图和样本库直方图
			for (int j=0; j<3; j++) {
				val[i][j] = cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}
		//加权求和
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
		maxVal = 1; //重置初始值

		for (int j=0; j<3; j++) { 
			for (int i=0; i<NUM; i++) { //从每个通道中比较得出最匹配的类别
				if (val[i][j] < maxVal) {
					maxNum = i+1;
					maxVal = val[i][j];
				}

				fout<<val[i][j]<<"  ";
			}
			fout<<maxVal<<"  "<<maxNum<<endl;
			maxVal = 1;//重置初始值
		}
		fout<<endl<<endl;

		cout<<buf<<endl;

		maxNum = -1;
		maxVal = 1;
		for (int i=0; i<3; i++) {
			cvClearHist(hist[i]); //直方图清零操作
			cvReleaseImage(&Img[i]); //销毁单通道图像 
		}
		cvReleaseImage(&src);
	}

	fin.close();
	fout.close();
}