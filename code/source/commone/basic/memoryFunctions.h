/*------------- memoryFunctions.h
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.01
*
*/ 
/***************************************************************
* �ڴ洦����
***************************************************************/
#pragma once
#include <iostream>
#include <stdio.h>
#include "basic/basicTypes.h"
/*************************************************************/
//------------------------------------------------------
//--- ����ڴ�
extern void*	dMemset(void * _Dst,int _Val,size_t _Size);
//------------------------------------------------------
//--- �ڴ�Ƚ�
extern int32	dMemcmp(const void * _Buf1,const void * _Buf2,size_t _Size);

#ifdef WIN32
//------------------------------------------------------
//--- �ڴ濽��
extern int32	dMemcpy(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);

//------------------------------------------------------
//--- �ڴ��ƶ�
extern int32	dMemmove(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);
#else//WIN32
//------------------------------------------------------
//--- �ڴ濽��
extern void*	dMemcpy(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);

//------------------------------------------------------
//--- �ڴ��ƶ�
extern void*	dMemmove(void* _Dst,size_t _DstSize,const void* _Src,size_t _MaxCount);
#endif//WIN32
