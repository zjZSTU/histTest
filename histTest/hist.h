#ifndef HIST_H_
#define HIST_H_

#include "commonDef.h"

//灰度直方图
void grayHist(void);

//单通道直方图
void singleHist(void);

//批量处理
void batchHist(void);

//共有六类图片，样本库里各类图片各有一张，利用直方图匹配判断输入图片属于那一类
//
void imageDetect_GrayHist(void);

//共有六类图片，样本库里各类图片各有一张，利用单通道直方图匹配并加权求和来判断输入图片属于那一类
//
void imageDetect_MultiHist(void);

#endif