#ifndef HIST_H_
#define HIST_H_

#include "commonDef.h"

//�Ҷ�ֱ��ͼ
void grayHist(void);

//��ͨ��ֱ��ͼ
void singleHist(void);

//��������
void batchHist(void);

//��������ͼƬ�������������ͼƬ����һ�ţ�����ֱ��ͼƥ���ж�����ͼƬ������һ��
//
void imageDetect_GrayHist(void);

//��������ͼƬ�������������ͼƬ����һ�ţ����õ�ͨ��ֱ��ͼƥ�䲢��Ȩ������ж�����ͼƬ������һ��
//
void imageDetect_MultiHist(void);

#endif