/*------------- systemFunction.h
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.01
*
*/ 
/***************************************************************
* 
***************************************************************/
#pragma once
#include "std/std_vector.h"
#include "basic/basicTypes.h"
#include "stackChecker/stackChecker.h"
/*************************************************************/
//-------------------------------------------------------------
//--- 获得模型文件路径名
extern void		getModuleFileName(char*pFilename,uint32 uSize);

//------------------------------------------------------
//--- 设置工作目录
extern void		setWorkDirectory(bool bModuleFile = true);

//------------------------------------------------------
//--- 产生一个随机数
extern int32	randRange(int32 nMax,int32 nMin  = 0);
extern uint32	randRange(uint32 uMax,uint32 uMin  = 0);
//-------------------------------------------------------------
//--- 获得随机索引
extern int32	getRandIndex(stl_vector<uint32>&_arr);

//-------------------------------------------------------------
//--- 是否随机成功
extern bool		randRatio	(uint32 uMin,uint32 uMax,uint32 uCount,uint32 uRatio,uint32 uRatioMax = 10000);
//--- 是否随机成功
extern bool		randRatio	(uint32 uRatio,uint32 uRatioMax = 10000);
