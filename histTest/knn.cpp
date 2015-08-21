#include "commonDef.h"
#include "histData.h"
#include "knn.h"

//KNN�㷨
//�ȼ���50������ͼƬ����ͨ��ֱ��ͼ��Ϣ���ֿ������ͨ��ֱ��ͼ����Ȼ������ͼƬ������ͼ����бȽϣ����������ͨ���ĶԱ�ֵ����м�Ȩ�ںϣ�
//�Ӽ�������ļ�Ȩֵ�н��д�С���������ȡǰK����Ȩֵ����֪����Ȩֵ��������������һ����𣩣��ж��ĸ�������ֵ�ϴ�������ͼƬ������һ��
void knn_train(void) {
	const int NUM = 50; //����������
	const int K = 5;   //ȡǰk�������������Ƚ�

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the filelist.txt"<<endl;
		cin.get();
		exit(0);
	}


	CvHistogram *(sample_hist[NUM][3]);  //�洢����ͼ����뵥ͨ����ȡ�õ�ֱ��ͼ

	//����ֱ��ͼ
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//��ȡ����ֱ��ͼ��Ϣ
	int count=0; //������
	char buf[1024]={'\0'};
	IplImage *src = NULL; //ԭʼͼ��
	IplImage *(Img[3]); //�����ĵ�ͨ��ͼ��

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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], sample_hist[count][i], 0, NULL); //����ֱ��ͼ
			cvNormalizeHist(sample_hist[count][i], 1.0);  //��һ��ֱ��ͼ
			
			cvReleaseImage(&Img[i]); //���ٵ�ͨ��ͼ��
		}

		count++; //ͳ������ͼ������
		cvReleaseImage(&src); //����Դͼ��
	}

	//��ȡ����ͼ��ֱ��ͼ��Ϣ
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

	CvHistogram *(hist[3]); //�������ͼ�����ͨ����õ��ĵ�ͨ��ֱ��ͼ
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}
	count = 0; //����������
	float val[NUM][3]={0}; //���ڴ�ŵ�ͨ��ͼ��ƥ��ֵ
	float resu[NUM][2]={0};   //���ڴ洢��Ȩ��ͺ��ƥ��ֵ ��һ�д洢��Ȩ��͵�ֵ �ڶ������ֽ�����

#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif

	while (fin.getline(buf, 1024)!=NULL) {
		//��������
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0; //��ͨ��ֱ��ͼƥ��ֵ
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0; //��Ȩ���ֵ
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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL); //���㵥ͨ��ֱ��ͼ��Ϣ
			cvNormalizeHist(hist[i], 1.0); //��һ��ֱ��ͼ

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //ֱ��ͼƥ��
			for (int i=0; i<NUM; i++) {
				val[i][j]=(float)cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //��Ȩ���
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10; //����������filelist.txt�ļ��в�ͬ���ͼ���˳��
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //���ڷ��ѳ���
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //���ڹ�������
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //������������˿
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //������������
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //������ͷ������
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //ð������ ��С����
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					float temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //����������ڲ�ͬ����򽻻�
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

		int num[5]={0}; //��5��ͼ��
		for (int i=0; i<K; i++) { //ͳ��ƥ��ֵ��С��ǰK��ֵ����һ��
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) { //����5����������������ͼ���������
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
		if (resu[0][1] == resu[1][1]) { //ƥ��ֵ��õ�ǰ��λ��Ϊͬһ���
			fout<<"����"<<resu[0][1]<<endl;
		} else if (resu[0][0] <= 0.11 && resu[1][0] > 0.1) { //������õ�ƥ��ֵ<=0.1
			fout<<"����"<<resu[0][1]<<endl;
		} else if (max[0][0] == max[1][0])
			fout<<"����"<<max[1][1]<<endl;
		else
			fout<<"����"<<max[0][1]<<endl;
		
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

		for (int i=0; i<3; i++) { //�������ͼ��ֱ��ͼ��ֵ
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //����Դͼ��
	}

	fin.close();
	fout.close();
	cout<<"Done."<<endl;
}

//KNN�㷨
//�ȼ���50������ͼƬ����ͨ��ֱ��ͼ��Ϣ���ֿ������ͨ��ֱ��ͼ����Ȼ������ͼƬ������ͼ����бȽϣ����������ͨ���ĶԱ�ֵ����м�Ȩ�ںϣ�
//�Ӽ�������ļ�Ȩֵ�н��д�С���������ȡǰK����Ȩֵ����֪����Ȩֵ��������������һ����𣩣��ж��ĸ�������ֵ�ϴ�������ͼƬ������һ��
void knn_train_DetailedOutput(void) {
	const int NUM = 50; //����������
	const int K = 5;   //ȡǰk�������������Ƚ�

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the filelist.txt"<<endl;
		cin.get();
		exit(0);
	}


	CvHistogram *(sample_hist[NUM][3]);  //�洢����ͼ����뵥ͨ����ȡ�õ�ֱ��ͼ

	//����ֱ��ͼ
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//��ȡ����ֱ��ͼ��Ϣ
	int count=0; //������
	char buf[1024]={'\0'};
	IplImage *src = NULL; //ԭʼͼ��
	IplImage *(Img[3]); //�����ĵ�ͨ��ͼ��

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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], sample_hist[count][i], 0, NULL); //����ֱ��ͼ
			cvNormalizeHist(sample_hist[count][i], 1.0);  //��һ��ֱ��ͼ
			
			cvReleaseImage(&Img[i]); //���ٵ�ͨ��ͼ��
		}

		count++; //ͳ������ͼ������
		cvReleaseImage(&src); //����Դͼ��
	}

	//��ȡ����ͼ��ֱ��ͼ��Ϣ
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

	CvHistogram *(hist[3]); //�������ͼ�����ͨ����õ��ĵ�ͨ��ֱ��ͼ
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}
	count = 0; //����������
	float val[NUM][3]={0}; //���ڴ�ŵ�ͨ��ͼ��ƥ��ֵ
	float resu[NUM][2]={0};   //���ڴ洢��Ȩ��ͺ��ƥ��ֵ ��һ�д洢��Ȩ��͵�ֵ �ڶ������ֽ�����

#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif

	while (fin.getline(buf, 1024)!=NULL) {
		//��������
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0; //��ͨ��ֱ��ͼƥ��ֵ
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0; //��Ȩ���ֵ
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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL); //���㵥ͨ��ֱ��ͼ��Ϣ
			cvNormalizeHist(hist[i], 1.0); //��һ��ֱ��ͼ

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //ֱ��ͼƥ��
			for (int i=0; i<NUM; i++) {
				val[i][j]=(float)cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //��Ȩ���
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10; //����������filelist.txt�ļ��в�ͬ���ͼ���˳��
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //���ڷ��ѳ���
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //���ڹ�������
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //������������˿
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //������������
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //������ͷ������
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //ð������ ��С����
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					float temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //����������ڲ�ͬ����򽻻�
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

		int num[5]={0}; //��5��ͼ��
		for (int i=0; i<K; i++) { //ͳ��ƥ��ֵ��С��ǰK��ֵ����һ��
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) { //����5����������������ͼ���������
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
			fout<<"**************************���ڷ��ѳ��� fqcd ���2"<<endl;
		} else if (count == 11) {
			fout<<"**************************���ڹ������� gbjd ���1"<<endl;
		} else if (count == 21) {
			fout<<"**************************������������˿ sltds ���5"<<endl;
		} else if (count == 31) {
			fout<<"**************************������������ snsc ���3"<<endl;
		} else if (count == 41) {
			fout<<"**************************������ͷ������ ytdft ���4"<<endl;
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
		fout<<"BGR ��";
		for (int i=0; i<5; i++) {
			fout<<resu[i][0]<<"  ";
		}
		fout<<endl;
		fout<<"��� ��";
		for (int i=0; i<5; i++) {
			fout<<resu[i][1]<<"  ";
		}
		fout<<endl;
		fout<<"�б� ��";

		if (max[0][0] == max[1][0])
			fout<<"����"<<max[1][1]<<endl;
		else
			fout<<"����"<<max[0][1]<<endl;

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

		for (int i=0; i<3; i++) { //�������ͼ��ֱ��ͼ��ֵ
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //����Դͼ��
	}

	fin.close();
	fout.close();
	cout<<"Done."<<endl;
}

//��������sample��5��10�Ź�50��ͼƬ����ͨ��ֱ��ͼ��������һ��txt�ļ���
//
void calHist(void) {
	const int NUM = 50; //����������
	const int K = 5;   //ȡǰk�������������Ƚ�

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};

	ifstream fin("sample\\filelist.txt");
	if (!fin.is_open()) {
		cout<<"can not open the filelist.txt"<<endl;
		cin.get();
		exit(0);
	}

	CvHistogram *(sample_hist[NUM][3]);  //�洢����ͼ����뵥ͨ����ȡ�õ�ֱ��ͼ

	//�½�ֱ��ͼ
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//��ȡ����ֱ��ͼ��Ϣ
	int count=0; //������
	char buf[1024]={'\0'};
	IplImage *src = NULL; //ԭʼͼ��
	IplImage *(Img[3]); //�����ĵ�ͨ��ͼ��

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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], sample_hist[count][i], 0, NULL); //����ֱ��ͼ
			cvNormalizeHist(sample_hist[count][i], 1.0);  //��һ��ֱ��ͼ
			
			cvReleaseImage(&Img[i]);
		}

		count++;
		cvReleaseImage(&src);
	}
	
	//��ֱ��ͼ��Ϣ����txt�ļ���
	ofstream fout("sample.txt");
	if (!fout.is_open()) {
		cout<<"can not create sample.txt"<<endl;
		cin.get();
		exit(0);
	}

	float val=0;
	for (int i=0; i<NUM; i++) { //����50��ͼƬ
		for (int j=0; j<3; j++) { //ÿ��ͼƬ��3��ͨ��
			fout<<"{ ";
			for (int k=0; k<256; k++) { //ÿ��ͨ������256��0-255������ֵ
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

//KNN�㷨
//�Ӵ洢����ֱ��ͼ��Ϣ��txt�ļ�����ȡ50��3ͨ������ͼ����ȡ��150��ֱ��ͼ����������ͼƬ��3��ͨ����ֱ��ͼ�����������ͨ���ĶԱ�ֵ����м�Ȩ�ںϣ�
//�Ӽ�������ļ�Ȩֵ�н��д�С���������ȡǰK����Ȩֵ����֪����Ȩֵ��������������һ����𣩣��ж��ĸ�������ֵ�ϴ�������ͼƬ������һ��
void knn_train2(void) {
	const int NUM = 50; //����������
	const int K = 5;   //ȡǰk�������������Ƚ�

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};

	IplImage *src = NULL; //ԭʼͼ��
	IplImage *(Img[3]); //�����ĵ�ͨ��ͼ��
	CvHistogram *(sample_hist[NUM][3]);  //�洢����ͼ����뵥ͨ����ȡ�õ�ֱ��ͼ    

	//��ȡ����ֱ��ͼ��Ϣ
	ifstream fin("sample.txt");
	if (!fin.is_open()) {
		cout<<"can not open the sample.txt"<<endl;
		cin.get();
		exit(0);
	}
	//��ȡֱ��ͼ����
	float array[NUM][3][256];
	const char *d=" "; //�ָ�ģʽ��
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
			fin.getline(buf, 1024);//һ��ͼ��ĵ�ͨ��ֱ��ͼ����֮�����һ��
		}
		fin.getline(buf, 1024); //��ͬͼ���ֱ��ͼ����֮���������
	}

	// CvHistogram *hist_test=cvCreateHist(dims, sizes, type, ranges, 1);
	CvHistogram *(hist_test[NUM][3]);

	//����sampleֱ��ͼ
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			hist_test[i][j] = cvCreateHist(dims, sizes, type, ranges, 1);

			sample_hist[i][j] = cvMakeHistHeaderForArray(dims, sizes, hist_test[i][j], array[i][j], NULL);
		}
	}

	//��ȡ����ͼ��ֱ��ͼ��Ϣ
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

	CvHistogram *(hist[3]); //���ڴ洢����ͼ��3��ͨ����ֱ��ͼ����
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}

	double val[NUM][3]={0}; //���ڴ�ŵ�ͨ��ͼ��ƥ��ֵ
	double resu[NUM][2]={0};   //���ڴ洢��Ȩ��ͺ��ƥ��ֵ ��һ�д洢��Ȩ��͵�ֵ �ڶ������ֽ�����
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
		//��������
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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL);
			cvNormalizeHist(hist[i], 1.0);

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //ֱ��ͼƥ��
			for (int i=0; i<NUM; i++) {
				val[i][j]=cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //��Ȩ���
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10;
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //���ڷ��ѳ���
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //���ڹ�������
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //������������˿
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //������������
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //������ͷ������
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //ð������ ��С����
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					double temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //����������ڲ�ͬ����򽻻�
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
		for (int i=0; i<K; i++) { //ͳ��ƥ��ֵ��С��ǰK��ֵ����һ��
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
			fout<<"����"<<max[1][1]<<endl;
		else
			fout<<"����"<<max[0][1]<<endl;

		cout<<buf<<endl;

		for (int i=0; i<3; i++) { //�������ͼ��ֱ��ͼ��ֵ
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //����Դͼ��
	}

	fin.close();
	fout.close();
}

//KNN�㷨
//�Լ���������5��10��ͼƬ��150����ͨ��ֱ��ͼ���ݣ�����������ֱ��ͼ��Ȼ���������ͼ��õ�����ͼ��ͨ��ֱ��ͼ�����������ͨ���ĶԱ�ֵ����м�Ȩ�ںϣ�
//�Ӽ�������ļ�Ȩֵ�н��д�С���������ȡǰK����Ȩֵ����֪����Ȩֵ��������������һ����𣩣��ж��ĸ�������ֵ�ϴ�������ͼƬ������һ��
void knn_train3(void) {
	const int NUM = 50; //����������
	const int K = 5;   //ȡǰk�������������Ƚ�

	//ֱ��ͼ����
    int dims=1; //����һάֱ��ͼ  
    int sizes[]={256}; //����256��ȡֵ��Χ  
    int type=CV_HIST_ARRAY; //��ʾʹ���ܼ���ά����ṹ  
    float range[]={0, 255}; //ȡֵ��ΧΪ0-255  
    float *ranges[]={range};

	CvHistogram *(sample_hist[NUM][3]);  //�洢����ͼ����뵥ͨ����ȡ�õ�ֱ��ͼ

	//����ֱ��ͼ
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_hist[i][j]=cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	//��ȡ����ֱ��ͼ��Ϣ
	int count=0; //������

	CvHistogram *(sample_test[NUM][3]); //cvMakeHistHeaderForArray()�������õ�
	for (int i=0; i<NUM; i++) {
		for (int j=0; j<3; j++) {
			sample_test[i][j] = cvCreateHist(dims, sizes, type, ranges, 1);
		}
	}
	for (int i=0; i<NUM; i++) { //�������е����������ݵõ�ֱ��ͼ
		sample_hist[i][0] = cvMakeHistHeaderForArray(dims, sizes, sample_hist[i][0], data[i*3], NULL);
		sample_hist[i][1] = cvMakeHistHeaderForArray(dims, sizes, sample_hist[i][1], data[i*3+1], NULL);
		sample_hist[i][2] = cvMakeHistHeaderForArray(dims, sizes, sample_hist[i][2], data[i*3+2], NULL);
	}

	//��ȡ����ͼ��ֱ��ͼ��Ϣ
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

	CvHistogram *(hist[3]); //�������ͼ�����ͨ����õ��ĵ�ͨ��ֱ��ͼ
	for (int i=0; i<3; i++) {
		hist[i]=cvCreateHist(dims, sizes, type, ranges, 1);
	}
	count = 0; //����������
	float val[NUM][3]={0}; //���ڴ�ŵ�ͨ��ͼ��ƥ��ֵ
	float resu[NUM][2]={0};   //���ڴ洢��Ȩ��ͺ��ƥ��ֵ ��һ�д洢��Ȩ��͵�ֵ �ڶ������ֽ�����

#ifdef DEBUG
	for (int i=0; i<K; i++) {
		cout<<val[i][0]<<" "<<val[i][1]<<" "<<val[i][2]<<endl; 
	}
	for (int i=0; i<K; i++) {
		cout<<resu[i][0]<<"  "<<resu[i][1]<<endl;
	}
#endif
	char buf[1024]={'\0'};
	IplImage *src = NULL; //ԭʼͼ��
	IplImage *(Img[3]); //�����ĵ�ͨ��ͼ��
	while (fin.getline(buf, 1024)!=NULL) {
		//��������
		for (int i=0; i<NUM; i++) {
			for (int j=0; j<3; j++) {
				val[i][j] = 0; //��ͨ��ֱ��ͼƥ��ֵ
			}

			for (int j=0; j<2; j++) {
				resu[i][j] = 0; //��Ȩ���ֵ
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
		cvSplit(src, Img[0], Img[1], Img[2], NULL); //����ͨ��ͼ�����Ϊ��ͨ��ͼ��

		for (int i=0; i<3; i++) {
			cvCalcHist(&Img[i], hist[i], 0, NULL); //���㵥ͨ��ֱ��ͼ��Ϣ
			cvNormalizeHist(hist[i], 1.0); //��һ��ֱ��ͼ

			cvReleaseImage(&Img[i]);
		}
	
		for (int j=0; j<3; j++) { //ֱ��ͼƥ��
			for (int i=0; i<NUM; i++) {
				val[i][j]=(float)cvCompareHist(hist[j], sample_hist[i][j], CV_COMP_BHATTACHARYYA);
			}
		}

		for (int i=0; i<NUM; i++) { //��Ȩ���
			for (int j=0; j<3; j++) {
				resu[i][0] += val[i][j]*0.3;
			}
			int option = i/10; //����������filelist.txt�ļ��в�ͬ���ͼ���˳��
			switch (option) {
			case 0:
				//0-9
				resu[i][1] = 2; //���ڷ��ѳ���
				break;
			case 1:
				//10-19
				resu[i][1] = 1; //���ڹ�������
				break;
			case 2:
				//20-29
				resu[i][1] = 5; //������������˿
				break;
			case 3:
				//30-39
				resu[i][1] = 3; //������������
				break;
			case 4:
				//40-49
				resu[i][1] = 4; //������ͷ������
				break;
			default:
				break;
			}
		}

		for (int i=0; i<NUM-1; i++) { //ð������ ��С����
 			for (int j=i+1; j<NUM; j++) {
				if (resu[i][0] > resu[j][0]) {
					float temp = resu[i][0];
					resu[i][0] = resu[j][0];
					resu[j][0] = temp;

					if (resu[i][1] != resu[j][1]) { //����������ڲ�ͬ����򽻻�
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

		int num[5]={0}; //��5��ͼ��
		for (int i=0; i<K; i++) { //ͳ��ƥ��ֵ��С��ǰK��ֵ����һ��
			num[(int)resu[i][1]-1]++;
		}

		int max[2][2]={0};
		for (int i=0; i<5; i++) { //����5����������������ͼ���������
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
		if (resu[0][1] == resu[1][1]) { //ƥ��ֵ��õ�ǰ��λ��Ϊͬһ���
			fout<<"����"<<resu[0][1]<<endl;
		} else if (resu[0][0] <= 0.11 && resu[1][0] > 0.1) { //������õ�ƥ��ֵ<=0.1
			fout<<"����"<<resu[0][1]<<endl;
		} else if (max[0][0] == max[1][0])
			fout<<"����"<<max[1][1]<<endl;
		else
			fout<<"����"<<max[0][1]<<endl;
		
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

		for (int i=0; i<3; i++) { //�������ͼ��ֱ��ͼ��ֵ
			cvClearHist(hist[i]);
		}
		cvReleaseImage(&src); //����Դͼ��
	}

	fin.close();
	fout.close();
	cout<<"Done."<<endl;
}