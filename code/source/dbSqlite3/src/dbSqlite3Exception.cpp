/*----------------- dbSqlite3Exception.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*异常抛出信息
*------------------------------------------------------------*/
#include "dbSqlite3.h"
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Exception::CSqlite3Exception(sqlite3* pDB,int nCode,const char* pMsg)
{
	if (nCode)
		this->nCode = nCode;
	else
		this->nCode = sqlite3_errcode(pDB);

#ifdef WIN32
	sprintf_s(szMsg,sizeof(szMsg),sqlite3_errmsg(pDB));
	if (pMsg)
		sprintf_s(szMsg,sizeof(szMsg),"%s[%s]",szMsg,pMsg);
#else//WIN32
	sprintf(szMsg,sqlite3_errmsg(pDB));
	if (pMsg)
		sprintf(szMsg,"%s[%s]",szMsg,pMsg);
#endif//WIN32
}
