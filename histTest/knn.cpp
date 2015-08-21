#include "commonDef.h"
#include "histData.h"
#include "knn.h"

//KNN算法
//先计算50个样本图片的三通道直方图信息（分开计算各通道直方图），然后将输入图片与样本图像进行比较，计算出各单通道的对比值后进行加权融合，
//从计算出来的加权值中进行从小到大的排序，取前K个加权值（已知各加权值属于样本库中哪一个类别），判断哪个类别的数值较大，则输入图片属于那一类
void knn_train(void) {
	const int NUM = 50; //样本库总数
	const int K = 5;   //取前k个样本进行最后比较

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the filelist.txt"<<endl;
		cin.get();
		exit(0);
	}


	CvHistogram *(sample_hist[NUM][3]);  //存储样本图像分离单通道后取得的直方图

	//样本直方图
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//获取样本直方图信息
	int count=0; //计数器
	char buf[1024]={'\0'};
	IplImage *src = NULL; //原始图像
	IplImage *(Img[3]); //分离后的单通道图像

	while (fin.getline(buf, 1024)!=NULL) {
		src = cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load sample image "<<buf<<endl;
			cin.get();
			exit(0);
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], sample_hist[count][i], 0, NULL); //计算直方图
			cvNormalizeHist(sample_hist[count][i], 1.0);  //归一化直方图
			
			cvReleaseImage(&Img[i]); //销毁单通道图像
		}

		count++; //统计样本图像总数
		cvReleaseImage(&src); //销毁源图像
	}

	//获取输入图像直方图信息
	fin.close();
	fin.open("input\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("output.txt");
	if (!fout.is_open()) {
		cout<<"can not creat the txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(hist[3]); //存放输入图像分离通道后得到的单通道直方图
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}
	count = 0; //计数器清零
	float val[NUM][3]={0}; //用于存放单通道图像匹配值
	float resu[NUM][2]={0};   //用于存储加权求和后的匹配值 第一行存储加权求和的值 第二行区分结果类别

#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif

	while (fin.getline(buf, 1024)!=NULL) {
		//数组清零
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0; //单通道直方图匹配值
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0; //加权求和值
			}
		}
		
		src=cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load input image "<<buf<<endl;
			cin.get();
			exit(0);		
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL); //计算单通道直方图信息
			cvNormalizeHist(hist[i], 1.0); //归一化直方图

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //直方图匹配
			for (int i=0; i<NUM; i++) {
				val[i][j]=(float)cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //加权求和
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10; //根据样本库filelist.txt文件中不同类别图像的顺序
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //属于番茄炒蛋
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //属于宫爆鸡丁
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //属于酸辣土豆丝
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //属于蒜泥生菜
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //属于鱼头豆腐汤
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //冒泡排序 从小到大
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					float temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //如果两个属于不同类别，则交换
						temp = resu[i][1];
						resu[i][1] = resu[j][1];
						resu[j][1] = temp;
					}
				}
			}
		}
#define DEBUG  
#ifdef DEBUG

		for (int i=0; i<K; i++) {
			cout<<resu[i][0]<<"  "; 
		}
		cout<<endl;
		for (int i=0; i<K; i++) {
			cout<<resu[i][1]<<"  ";
		}
		cout<<endl;

#endif

		int num[5]={0}; //共5类图像
		for (int i=0; i<K; i++) { //统计匹配值最小的前K个值是哪一类
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) { //计算5个类别中哪两个类别图像数量最高
			if (num[i] > max[0][0]) {
				max[1][0] = max[0][0];
				max[1][1] = max[0][1];
				max[0][0] = num[i];
				max[0][1] = i+1;
			} else if (num[i] > max[1][0]) {
				max[1][0] = num[i];
				max[1][1] = i+1;
			}
		}
		if (resu[0][1] == resu[1][1]) { //匹配值最好的前两位均为同一类别
			fout<<"属于"<<resu[0][1]<<endl;
		} else if (resu[0][0] <= 0.11 && resu[1][0] > 0.1) { //仅有最好的匹配值<=0.1
			fout<<"属于"<<resu[0][1]<<endl;
		} else if (max[0][0] == max[1][0])
			fout<<"属于"<<max[1][1]<<endl;
		else
			fout<<"属于"<<max[0][1]<<endl;
		
		count++;
		if (count == 10) {
			fout<<"#####################################################"<<endl;
		} else if (count == 20) {
			fout<<"####################################################"<<endl;
		} else if (count == 30) {
			fout<<"####################################################"<<endl;
		} else if (count == 40) {
			fout<<"###################################################"<<endl;
		} else if (count == 50) {
			fout<<"###################################################"<<endl;
		}
		
		cout<<buf<<endl;

		for (int i=0; i<3; i++) { //清除输入图像直方图的值
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //销毁源图像
	}

	fin.close();
	fout.close();
	cout<<"Done."<<endl;
}

//KNN算法
//先计算50个样本图片的三通道直方图信息（分开计算各通道直方图），然后将输入图片与样本图像进行比较，计算出各单通道的对比值后进行加权融合，
//从计算出来的加权值中进行从小到大的排序，取前K个加权值（已知各加权值属于样本库中哪一个类别），判断哪个类别的数值较大，则输入图片属于那一类
void knn_train_DetailedOutput(void) {
	const int NUM = 50; //样本库总数
	const int K = 5;   //取前k个样本进行最后比较

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the filelist.txt"<<endl;
		cin.get();
		exit(0);
	}


	CvHistogram *(sample_hist[NUM][3]);  //存储样本图像分离单通道后取得的直方图

	//样本直方图
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//获取样本直方图信息
	int count=0; //计数器
	char buf[1024]={'\0'};
	IplImage *src = NULL; //原始图像
	IplImage *(Img[3]); //分离后的单通道图像

	while (fin.getline(buf, 1024)!=NULL) {
		src = cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load sample image "<<buf<<endl;
			cin.get();
			exit(0);
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], sample_hist[count][i], 0, NULL); //计算直方图
			cvNormalizeHist(sample_hist[count][i], 1.0);  //归一化直方图
			
			cvReleaseImage(&Img[i]); //销毁单通道图像
		}

		count++; //统计样本图像总数
		cvReleaseImage(&src); //销毁源图像
	}

	//获取输入图像直方图信息
	fin.close();
	fin.open("input\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("output.txt");
	if (!fout.is_open()) {
		cout<<"can not creat the txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(hist[3]); //存放输入图像分离通道后得到的单通道直方图
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}
	count = 0; //计数器清零
	float val[NUM][3]={0}; //用于存放单通道图像匹配值
	float resu[NUM][2]={0};   //用于存储加权求和后的匹配值 第一行存储加权求和的值 第二行区分结果类别

#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif

	while (fin.getline(buf, 1024)!=NULL) {
		//数组清零
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0; //单通道直方图匹配值
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0; //加权求和值
			}
		}
		
		src=cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load input image "<<buf<<endl;
			cin.get();
			exit(0);		
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL); //计算单通道直方图信息
			cvNormalizeHist(hist[i], 1.0); //归一化直方图

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //直方图匹配
			for (int i=0; i<NUM; i++) {
				val[i][j]=(float)cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //加权求和
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10; //根据样本库filelist.txt文件中不同类别图像的顺序
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //属于番茄炒蛋
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //属于宫爆鸡丁
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //属于酸辣土豆丝
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //属于蒜泥生菜
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //属于鱼头豆腐汤
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //冒泡排序 从小到大
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					float temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //如果两个属于不同类别，则交换
						temp = resu[i][1];
						resu[i][1] = resu[j][1];
						resu[j][1] = temp;
					}
				}
			}
		}
#define DEBUG  
#ifdef DEBUG

		for (int i=0; i<K; i++) {
			cout<<resu[i][0]<<"  "; 
		}
		cout<<endl;
		for (int i=0; i<K; i++) {
			cout<<resu[i][1]<<"  ";
		}
		cout<<endl;

#endif

		int num[5]={0}; //共5类图像
		for (int i=0; i<K; i++) { //统计匹配值最小的前K个值是哪一类
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) { //计算5个类别中哪两个类别图像数量最高
			if (num[i] > max[0][0]) {
				max[1][0] = max[0][0];
				max[1][1] = max[0][1];
				max[0][0] = num[i];
				max[0][1] = i+1;
			} else if (num[i] > max[1][0]) {
				max[1][0] = num[i];
				max[1][1] = i+1;
			}
		}

		count++;
		if (count == 1) {
			fout<<"**************************属于番茄炒蛋 fqcd 编号2"<<endl;
		} else if (count == 11) {
			fout<<"**************************属于宫保鸡丁 gbjd 编号1"<<endl;
		} else if (count == 21) {
			fout<<"**************************属于酸辣土豆丝 sltds 编号5"<<endl;
		} else if (count == 31) {
			fout<<"**************************属于蒜泥生菜 snsc 编号3"<<endl;
		} else if (count == 41) {
			fout<<"**************************属于鱼头豆腐汤 ytdft 编号4"<<endl;
		}

		fout<<"B value:";
		for (int i=0; i<5; i++) {
			fout<<val[i][0]<<"  ";
		}
		fout<<endl;
		fout<<"G value:";
		for (int i=0; i<5; i++) {
			fout<<val[i][1]<<"  ";
		}
		fout<<endl;
		fout<<"R value:";
		for (int i=0; i<5; i++) {
			fout<<val[i][2]<<"  ";
		}
		fout<<endl;
		fout<<"BGR ：";
		for (int i=0; i<5; i++) {
			fout<<resu[i][0]<<"  ";
		}
		fout<<endl;
		fout<<"类别 ：";
		for (int i=0; i<5; i++) {
			fout<<resu[i][1]<<"  ";
		}
		fout<<endl;
		fout<<"判别 ：";

		if (max[0][0] == max[1][0])
			fout<<"属于"<<max[1][1]<<endl;
		else
			fout<<"属于"<<max[0][1]<<endl;

		if (count == 10) {
			fout<<"#####################################################"<<endl;
		} else if (count == 20) {
			fout<<"####################################################"<<endl;
		} else if (count == 30) {
			fout<<"####################################################"<<endl;
		} else if (count == 40) {
			fout<<"###################################################"<<endl;
		} else if (count == 50) {
			fout<<"###################################################"<<endl;
		}

		cout<<buf<<endl;

		for (int i=0; i<3; i++) { //清除输入图像直方图的值
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //销毁源图像
	}

	fin.close();
	fout.close();
	cout<<"Done."<<endl;
}

//计算样本sample中5类10张共50张图片的三通道直方图，并存入一个txt文件中
//
void calHist(void) {
	const int NUM = 50; //样本库总数
	const int K = 5;   //取前k个样本进行最后比较

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the filelist.txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(sample_hist[NUM][3]);  //存储样本图像分离单通道后取得的直方图

	//新建直方图
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//获取样本直方图信息
	int count=0; //计数器
	char buf[1024]={'\0'};
	IplImage *src = NULL; //原始图像
	IplImage *(Img[3]); //分离后的单通道图像

	while (fin.getline(buf, 1024)!=NULL && count<256) {
		src = cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load sample image "<<buf<<endl;
			cin.get();
			exit(0);
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], sample_hist[count][i], 0, NULL); //计算直方图
			cvNormalizeHist(sample_hist[count][i], 1.0);  //归一化直方图
			
			cvReleaseImage(&Img[i]);
		}

		count++;
		cvReleaseImage(&src);
	}
	
	//将直方图信息存入txt文件中
	ofstream fout("sample.txt");
	if (!fout.is_open()) {
		cout<<"can not create sample.txt"<<endl;
		cin.get();
		exit(0);
	}

	float val=0;
	for (int i=0; i<NUM; i++) { //共有50张图片
		for (int j=0; j<3; j++) { //每张图片有3个通道
			fout<<"{ ";
			for (int k=0; k<256; k++) { //每个通道共有256（0-255）个数值
				val = (float)cvQueryHistValue_1D(sample_hist[i][j], k);
				
				if (k != 255)
					fout<<val<<",  ";
				else
					fout<<val<<"  ";
				if ((k+1) % 10 == 0)
					fout<<endl;
			} 
			fout<<" },"<<endl;
		}
	}

	fout.close();
	cout<<"Done."<<endl;
}

//KNN算法
//从存储样本直方图信息的txt文件中提取50个3通道样本图像提取的150个直方图，计算输入图片的3个通道的直方图，计算出各单通道的对比值后进行加权融合，
//从计算出来的加权值中进行从小到大的排序，取前K个加权值（已知各加权值属于样本库中哪一个类别），判断哪个类别的数值较大，则输入图片属于那一类
void knn_train2(void) {
	const int NUM = 50; //样本库总数
	const int K = 5;   //取前k个样本进行最后比较

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};

	IplImage *src = NULL; //原始图像
	IplImage *(Img[3]); //分离后的单通道图像
	CvHistogram *(sample_hist[NUM][3]);  //存储样本图像分离单通道后取得的直方图    

	//获取样本直方图信息
	ifstream fin("sample.txt");
	if (!fin.is_open()) {
		cout<<"can not open the sample.txt"<<endl;
		cin.get();
		exit(0);
	}
	//提取直方图数据
	float array[NUM][3][256];
	const char *d=" "; //分割模式符
	char buf[1024]="\0";
	int count = 0;
	char *p=NULL;
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			count = 0;
			while (fin.getline(buf, 1024)!=NULL && count<256) {
				p=NULL;
				p=strtok(buf, d);
				while (p) {
					array[i][j][count] = (float)atof(p);
					p=strtok(NULL, d);
					count++;
				}
			}
			fin.getline(buf, 1024);//一幅图像的单通道直方图数据之间相隔一行
		}
		fin.getline(buf, 1024); //不同图像的直方图数据之间相隔两行
	}

	// CvHistogram *hist_test=cvCreateHist(dims, sizes, type, ranges, 1);
	CvHistogram *(hist_test[NUM][3]);

	//创建sample直方图
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			hist_test[i][j] = cvCreateHist(dims, sizes, type, ranges, 1);

			sample_hist[i][j] = cvMakeHistHeaderForArray(dims, sizes, hist_test[i][j], array[i][j], NULL);
		}
	}

	//获取输入图像直方图信息
	fin.close();
	fin.open("input\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the input\\filelist.txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("output.txt");
	if (!fout.is_open()) {
		cout<<"can not creat the output.txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(hist[3]); //用于存储输入图像3个通道的直方图数据
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}

	double val[NUM][3]={0}; //用于存放单通道图像匹配值
	double resu[NUM][2]={0};   //用于存储加权求和后的匹配值 第一行存储加权求和的值 第二行区分结果类别
/*
#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif
*/

	while (fin.getline(buf, 1024)!=NULL) {
		//数组清零
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0;
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0;
			}
		}
		
		src=cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load input image "<<buf<<endl;
			cin.get();
			exit(0);		
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL);
			cvNormalizeHist(hist[i], 1.0);

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //直方图匹配
			for (int i=0; i<NUM; i++) {
				val[i][j]=cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //加权求和
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10;
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //属于番茄炒蛋
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //属于宫爆鸡丁
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //属于酸辣土豆丝
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //属于蒜泥生菜
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //属于鱼头豆腐汤
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //冒泡排序 从小到大
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					double temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //如果两个属于不同类别，则交换
						temp = resu[i][1];
						resu[i][1] = resu[j][1];
						resu[j][1] = temp;
					}
				}
			}
		}
#define DEBUG
#ifdef DEBUG

		for (int i=0; i<K; i++) {
			cout<<resu[i][0]<<"  "; 
		}
		cout<<endl;
		for (int i=0; i<K; i++) {
			cout<<resu[i][1]<<"  ";
		}
		cout<<endl;

#endif

		int num[5]={0};
		for (int i=0; i<K; i++) { //统计匹配值最小的前K个值是哪一类
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) {
			if (num[i] > max[0][0]) {
				max[1][0] = max[0][0];
				max[1][1] = max[0][1];
				max[0][0] = num[i];
				max[0][1] = i+1;
			} else if (num[i] > max[1][0]) {
				max[1][0] = num[i];
				max[1][1] = i+1;
			}
		}

		if (max[0][0] == max[1][0])
			fout<<"属于"<<max[1][1]<<endl;
		else
			fout<<"属于"<<max[0][1]<<endl;

		cout<<buf<<endl;

		for (int i=0; i<3; i++) { //清除输入图像直方图的值
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //销毁源图像
	}

	fin.close();
	fout.close();
}

//KNN算法
//以及样本库中5类10张图片共150个单通道直方图数据，创建样本库直方图，然后根据输入图像得到输入图像单通道直方图，计算出各单通道的对比值后进行加权融合，
//从计算出来的加权值中进行从小到大的排序，取前K个加权值（已知各加权值属于样本库中哪一个类别），判断哪个类别的数值较大，则输入图片属于那一类
void knn_train3(void) {
	const int NUM = 50; //样本库总数
	const int K = 5;   //取前k个样本进行最后比较

	//直方图参数
    int dims=1; //创建一维直方图  
    int sizes[]={256}; //共有256个取值范围  
    int type=CV_HIST_ARRAY; //表示使用密集多维矩阵结构  
    float range[]={0, 255}; //取值范围为0-255  
    float *ranges[]={range};

	CvHistogram *(sample_hist[NUM][3]);  //存储样本图像分离单通道后取得的直方图

	//样本直方图
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//获取样本直方图信息
	int count=0; //计数器

	CvHistogram *(sample_test[NUM][3]); //cvMakeHistHeaderForArray()函数中用到
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_test[i][j] = cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	for (int i=0; i<NUM; i++) { //根据已有的样本库数据得到直方图
		sample_hist[i][0] = cvMakeHistHeaderForArray(dims, sizes, sample_hist[i][0], data[i*3], NULL);
		sample_hist[i][1] = cvMakeHistHeaderForArray(dims, sizes, sample_hist[i][1], data[i*3+1], NULL);
		sample_hist[i][2] = cvMakeHistHeaderForArray(dims, sizes, sample_hist[i][2], data[i*3+2], NULL);
	}

	//获取输入图像直方图信息
	ifstream fin("input\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the txt"<<endl;
		cin.get();
		exit(0);
	}

	ofstream fout("output.txt");
	if (!fout.is_open()) {
		cout<<"can not creat the txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(hist[3]); //存放输入图像分离通道后得到的单通道直方图
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}
	count = 0; //计数器清零
	float val[NUM][3]={0}; //用于存放单通道图像匹配值
	float resu[NUM][2]={0};   //用于存储加权求和后的匹配值 第一行存储加权求和的值 第二行区分结果类别

#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif
	char buf[1024]={'\0'};
	IplImage *src = NULL; //原始图像
	IplImage *(Img[3]); //分离后的单通道图像
	while (fin.getline(buf, 1024)!=NULL) {
		//数组清零
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0; //单通道直方图匹配值
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0; //加权求和值
			}
		}
		
		src=cvLoadImage(buf);
		if (src == NULL) {
			cout<<"can not load input image "<<buf<<endl;
			cin.get();
			exit(0);		
		}

		for (int i=0; i<3; i++) {
			Img[i]=cvCreateImage(cvGetSize(src), src->depth, 1);
		}
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //将多通道图像分离为单通道图像

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL); //计算单通道直方图信息
			cvNormalizeHist(hist[i], 1.0); //归一化直方图

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //直方图匹配
			for (int i=0; i<NUM; i++) {
				val[i][j]=(float)cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //加权求和
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10; //根据样本库filelist.txt文件中不同类别图像的顺序
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //属于番茄炒蛋
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //属于宫爆鸡丁
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //属于酸辣土豆丝
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //属于蒜泥生菜
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //属于鱼头豆腐汤
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //冒泡排序 从小到大
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					float temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //如果两个属于不同类别，则交换
						temp = resu[i][1];
						resu[i][1] = resu[j][1];
						resu[j][1] = temp;
					}
				}
			}
		}
#define DEBUG  
#ifdef DEBUG

		for (int i=0; i<K; i++) {
			cout<<resu[i][0]<<"  "; 
		}
		cout<<endl;
		for (int i=0; i<K; i++) {
			cout<<resu[i][1]<<"  ";
		}
		cout<<endl;

#endif

		int num[5]={0}; //共5类图像
		for (int i=0; i<K; i++) { //统计匹配值最小的前K个值是哪一类
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) { //计算5个类别中哪两个类别图像数量最高
			if (num[i] > max[0][0]) {
				max[1][0] = max[0][0];
				max[1][1] = max[0][1];
				max[0][0] = num[i];
				max[0][1] = i+1;
			} else if (num[i] > max[1][0]) {
				max[1][0] = num[i];
				max[1][1] = i+1;
			}
		}
		if (resu[0][1] == resu[1][1]) { //匹配值最好的前两位均为同一类别
			fout<<"属于"<<resu[0][1]<<endl;
		} else if (resu[0][0] <= 0.11 && resu[1][0] > 0.1) { //仅有最好的匹配值<=0.1
			fout<<"属于"<<resu[0][1]<<endl;
		} else if (max[0][0] == max[1][0])
			fout<<"属于"<<max[1][1]<<endl;
		else
			fout<<"属于"<<max[0][1]<<endl;
		
		count++;
		if (count == 10) {
			fout<<"#####################################################"<<endl;
		} else if (count == 20) {
			fout<<"####################################################"<<endl;
		} else if (count == 30) {
			fout<<"####################################################"<<endl;
		} else if (count == 40) {
			fout<<"###################################################"<<endl;
		} else if (count == 50) {
			fout<<"###################################################"<<endl;
		}
		
		cout<<buf<<endl;

		for (int i=0; i<3; i++) { //清除输入图像直方图的值
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //销毁源图像
	}

	fin.close();
	fout.close();
	cout<<"Done."<<endl;
}