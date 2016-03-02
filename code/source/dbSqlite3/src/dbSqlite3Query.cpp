/*----------------- dbSqlite3Query.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*SQL�������
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
//------------------------------ ����SQL����
int	CSqlite3Query::finalize()
{
	if (!m_pStatement)
		return 0;

	int nCode = sqlite3_finalize(m_pStatement);
	m_pStatement = NULL;
	return nCode;
}

//-------------------------------------------------------------
//------------------------------ ���ݵ���һ��
bool	CSqlite3Query::nextRow()
{
	m_bEof	= false;
	int nRet = sqlite3_step(m_pStatement);	/*ִ��һ��SQL������״̬*/ 
	if (nRet == SQLITE_DONE)
	{/*��β*/ 
		m_bEof	= true;
		/*no rows*/ 
	}
	else if (nRet == SQLITE_ROW)
	{/*���ж���*/ 
		/*more rows, nothing to do*/ 
		return true;
	}
	else
	{/*������*/ 
		m_bEof			= true;
		m_nColumnCount	= 0;
		//nRet = sqlite3_reset(m_pStatement);
		finalize();
	}

	return false;
}

/*
**���ݽ������
**
**����ִ���� sqlite3_prepare()֮����κ�ʱ�̵���
*/
int	CSqlite3Query::getColumn()
{
	return m_nColumnCount;
	/*if (!m_pStatement)
	return 0;

	return sqlite3_column_count(m_pStatement);*/
	/*����Ҫ��sqlite3_step()֮�����
	�������sqlite3_step()����ֵ�� SQLITE_DONE ����һ���������,���ʱ����sqlite3_data_count() ������ 0*/
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
**��N�е�ֵ����������
**
**#define SQLITE_INTEGER  1	//����
**#define SQLITE_FLOAT    2	//����
**#define SQLITE_TEXT     3	//�ı�
**#define SQLITE_BLOB     4	//������
**#define SQLITE_NULL     5	//�Զ�������(��sqlite3_column_decltype��ȡ)
*/
int	CSqlite3Query::getType	(int nCol)
{
	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_type(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ �ֶ�����ID
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
//------------------------------ ��N�е��ֶ���
const char*	CSqlite3Query::getName(int nCol)
{
	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_name(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ ���ظ����� CREATE TABLE ���������������,���������ڵ����������ǿ��ַ�����ʱ��
const char*	CSqlite3Query::getDecltype(int nCol)
{
	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_decltype(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ ����˫�ֽ�����
double	CSqlite3Query::getDouble(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;	

	return sqlite3_column_double(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ �Ա���������������ʽ����һ������ֵ
int	CSqlite3Query::getInt(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_int(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ ����64λ������
long long	CSqlite3Query::getInt64(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_int64(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ BLOBs�е��ֽ�������TEXT�ַ������ֽ���
int	CSqlite3Query::getBytes(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_bytes(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ ����BLOB����
const void*	CSqlite3Query::getBlob(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_blob(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ ����TEXT����
const unsigned char*CSqlite3Query::getString(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;	

	return sqlite3_column_text(m_pStatement,nCol);
}

//-------------------------------------------------------------
//------------------------------ ��ȡ����������
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
//------------------------------ ��N�е�UTF-16�����ֶ���
const void*	CSqlite3Query::getName16(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_name16(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ �������ظ����� CREATE TABLE ���������������,���������ڵ����������ǿ��ַ�����ʱ�� UTF-16����
const void*	CSqlite3Query::getDecltype16(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_decltype16(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ ����UTF-16�����BLOBs�е��ֽ�������TEXT�ַ������ֽ���
int	CSqlite3Query::getBytes16(int nCol)
{
	if(m_bEof)
		return 0;

	if (!isColumn(nCol))
		return 0;

	return sqlite3_column_bytes16(m_pStatement,nCol);
}
//-------------------------------------------------------------
//------------------------------ ����UTF-16����TEXT����
const void*	CSqlite3Query::getText16(int nCol)
{
	if(m_bEof)
		return NULL;

	if (!isColumn(nCol))
		return NULL;

	return sqlite3_column_text16(m_pStatement,nCol);
}
