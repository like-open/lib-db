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
//--- ���ģ���ļ�·����
extern void		getModuleFileName(char*pFilename,uint32 uSize);

//------------------------------------------------------
//--- ���ù���Ŀ¼
extern void		setWorkDirectory(bool bModuleFile = true);

//------------------------------------------------------
//--- ����һ�������
extern int32	randRange(int32 nMax,int32 nMin  = 0);
extern uint32	randRange(uint32 uMax,uint32 uMin  = 0);
//-------------------------------------------------------------
//--- ����������
extern int32	getRandIndex(stl_vector<uint32>&_arr);

//-------------------------------------------------------------
//--- �Ƿ�����ɹ�
extern bool		randRatio	(uint32 uMin,uint32 uMax,uint32 uCount,uint32 uRatio,uint32 uRatioMax = 10000);
//--- �Ƿ�����ɹ�
extern bool		randRatio	(uint32 uRatio,uint32 uRatioMax = 10000);
