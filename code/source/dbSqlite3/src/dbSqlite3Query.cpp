/*----------------- dbSqlite3Query.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*SQL结果声明
*------------------------------------------------------------*/
#include "dbSqlite3.h"
#include <string.h>
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Query::CSqlite3Query(sqlite3_stmt* pStmt)
{
	m_bEof			= true;
	m_nColumnCount	= 0;
	m_pStatement	= pStmt;
	if (m_pStatement)
	{
		m_nColumnCount = sqlite3_column_count(m_pStatement);
		nextRow();
	}
}

//-------------------------------------------------------------
//------------------------------ 
CSqlite3Query::~CSqlite3Query()
{
	finalize();
}

//-------------------------------------------------------------
//------------------------------ 
CSqlite3Query& CSqlite3Query::operator=(CSqlite3Query& query)
{
	finalize();

	m_bEof			= query.m_bEof;
	m_pStatement	= query.m_pStatement;
	m_nColumnCount	= query.m_nColumnCount;

	query.m_pStatement	= NULL;

	return *this;
}

//-------------------------------------------------------------
//------------------------------ 销毁SQL声明
int	CSqlite3Query::finalize()
{
	if (!m_pStatement)
		return 0;

	int nCode = sqlite3_finalize(m_pStatement);
	m_pStatement = NULL;
	return nCode;
}

//-------------------------------------------------------------
//------------------------------ 数据到下一行
bool	CSqlite3Query::nextRow()
{
	m_bEof	= false;
	int nRet = sqlite3_step(m_pStatement);	/*执行一个SQL声明的状态*/ 
	if (nRet == SQLITE_DONE)
	{/*行尾*/ 
		m_bEof	= true;
		/*no rows*/ 
	}
	else if (nRet == SQLITE_ROW)
	{/*还有多行*/ 
		/*more rows, nothing to do*/ 
		return true;
	}
	else
	{/*无数据*/ 
		m_bEof			= true;
		m_nColumnCount	= 0;
		//nRet = sqlite3_reset(m_pStatement);
		finalize();
	}

	return false;
}

/*
**数据结果列数
**
**以在执行了 sqlite3_prepare()之后的任何时刻调用
*/
int	CSqlite3Query::getColumn()
{
	return m_nColumnCount;
	/*if (!m_pStatement)
	return 0;

	return sqlite3_column_count(m_pStatement);*/
	/*必需要在sqlite3_step()之后调用
	如果调用sqlite3_step()返回值是 SQLITE_DONE 或者一个错误代码,则此时调用sqlite3_data_count() 将返回 0*/
	/*return sqlite3_data_count(m_pStatement);*/
}


//-------------------------------------------------------------
//------------------------------ 
bool	CSqlite3Query::isColumn(int nCol)
{
	if (m_pStatement && nCol >= 0 && nCol < m_nColumnCount)
		return true;

	return false;
}

/*-------------------------------------------------------------
**第N列的值的数据类型
**
**#define SQLITE_INTEGER  1	//整数
**#define SQLITE_FLOAT    2	//浮点
**#define SQLITE_TEXT     3	//文本
**#define SQLITE_BLOB     4	//二进制
**#define SQLITE_NULL     5	//自定义类型(用sqlite3_column_decltype获取)
*/
int	CSqlite3Query::getType	(int nCol)
{
	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_type(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ 字段名列ID
int	CSqlite3Query::getField(const char* pField)
{
	if (!pField || !*pField)
	{
		return -1;
	}
	for (int nField = 0; nField < m_nColumnCount; nField++)
	{
		const char* szTemp = sqlite3_column_name(m_pStatement, nField);

		if (strcmp(pField, szTemp) == 0)
		{
			return nField;
		}
	}

	return -1;
}

//-------------------------------------------------------------
//------------------------------ 第N列的字段名
const char*	CSqlite3Query::getName(int nCol)
{
	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_name(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ 返回该列在 CREATE TABLE 语句中声明的类型,它可以用在当返回类型是空字符串的时候
const char*	CSqlite3Query::getDecltype(int nCol)
{
	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_decltype(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ 返回双字节数据
double	CSqlite3Query::getDouble(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;	

	return sqlite3_column_double(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ 以本地主机的整数格式返回一个整数值
int	CSqlite3Query::getInt(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_int(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ 返回64位的整数
long long	CSqlite3Query::getInt64(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_int64(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ BLOBs列的字节数或者TEXT字符串的字节数
int	CSqlite3Query::getBytes(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_bytes(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ 返回BLOB数据
const void*	CSqlite3Query::getBlob(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_blob(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ 返回TEXT数据
const unsigned char*CSqlite3Query::getString(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;	

	return sqlite3_column_text(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ 获取二进制数据
bool	CSqlite3Query::getBlob(int nCol,void* pBuf,int nBufLen)
{
	if(m_bEof)
		return false;

	if (!pBuf || nBufLen <= 0)
		return false;

	if (!isColumn(nCol))
		return false;

	int nLen = sqlite3_column_bytes(m_pStatement,nCol);
	if (nLen > nBufLen)
		return false;

#ifdef WIN32
	memcpy_s(pBuf,nBufLen,sqlite3_column_blob(m_pStatement,nCol),nLen);
#else//WIN32
	memcpy(pBuf,sqlite3_column_blob(m_pStatement,nCol),nLen);
#endif//WIN32
	return true;
}

//-------------------------------------------------------------
//------------------------------ 第N列的UTF-16编码字段名
const void*	CSqlite3Query::getName16(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_name16(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ 用来返回该列在 CREATE TABLE 语句中声明的类型,它可以用在当返回类型是空字符串的时候 UTF-16编码
const void*	CSqlite3Query::getDecltype16(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_decltype16(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ 返回UTF-16编码的BLOBs列的字节数或者TEXT字符串的字节数
int	CSqlite3Query::getBytes16(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_bytes16(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ 返回UTF-16编码TEXT数据
const void*	CSqlite3Query::getText16(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_text16(m_pStatement,nCol);
}
