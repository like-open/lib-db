/*----------------- dbMySqlResult.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*���ݿ��ѯ�������ʵ��
*------------------------------------------------------------*/
#include <my_global.h>
#include "dbMySql.h"
/*************************************************************/
/*
**
*/
CMySQLResult::CMySQLResult(MYSQL* mysql,MYSQL_RES*pResult)
{
	m_pMySQL	= mysql;
	m_pResult	= NULL;

	initialize(pResult);
}
/*
**
*/
CMySQLResult::~CMySQLResult()
{
	freeResult();
}

/*
**
*/
CMySQLResult& CMySQLResult::operator=(CMySQLResult& clMySQLResult)
{
	freeResult();

	m_pMySQL			= clMySQLResult.m_pMySQL;
	m_pResult			= clMySQLResult.m_pResult;
	m_pRow				= clMySQLResult.m_pRow;
	m_ulRowCount		= clMySQLResult.m_ulRowCount;
	m_uFiledCount		= clMySQLResult.m_uFiledCount;
	m_pulFiledsLength	= clMySQLResult.m_pulFiledsLength;

	/*��Ϊ��Ҫ�����ͷű����ÿ�*/ 
	clMySQLResult.m_pMySQL			= NULL;
	clMySQLResult.m_pResult			= NULL;
	clMySQLResult.m_pRow			= NULL;
	clMySQLResult.m_ulRowCount		= 0;
	clMySQLResult.m_uFiledCount		= 0;
	clMySQLResult.m_pulFiledsLength	= NULL;

	return *this;
}
/*
**��ʼ�������
*/
void	CMySQLResult::initialize		(MYSQL_RES*pResult)
{
	freeResult();
	m_pResult	= pResult;

	if(m_pResult)
	{
		/*���������*/ 
		m_ulRowCount = mysql_num_rows(m_pResult);
		/*���������*/ 
		m_uFiledCount= mysql_num_fields(m_pResult);
	}
}
/*
**�ͷŽ����ʹ�õ��ڴ�
*/
void	CMySQLResult::freeResult		()
{
	if (m_pResult)
		mysql_free_result(m_pResult);

	m_pResult			= NULL;

	m_pRow				= NULL;
	m_ulRowCount		= 0;
	m_uFiledCount		= 0;
	m_pulFiledsLength	= NULL;
}
/*
**�����ϴ��ֶι���λ��
*/
MYSQL_FIELD_OFFSET	CMySQLResult::fieldTell()
{
	if (!m_pResult)
		return 0;

	return mysql_field_tell(m_pResult);
}
/*
**���й������ָ������(��ʼΪ0��FieldCount)
*/
MYSQL_FIELD_OFFSET	CMySQLResult::fieldSeek(MYSQL_FIELD_OFFSET offset)
{ 
	if (!m_pResult)
		return false;

	if (offset > m_uFiledCount - 1)
		return false;

	return mysql_field_seek(m_pResult,offset);
}

/*
**�������ڽ�����������е���Ϣ(��һ���ֶ�)
*/
MYSQL_FIELD*CMySQLResult::fieldFetch()
{
	if (!m_pResult)
		return NULL;

	//pField->db;			/*�ֶ�Դ�Ե�����*/ 
	//pField->table;		/*�������*/ 
	//pField->name;			/*�ֶ�����*/ 
	//pField->length;		/*�ֶεĿ��*/ 
	//pField->max_length;	/*�ֶ������*/ 
	//pField->flags;		/*�ֶα�־(IS_NOT_NULL(pField->flags)),IS_BLOB(pField->flags)*/ 
	//pField->type;			/*�ֶ�����*/ 

	return mysql_fetch_field(m_pResult);
}

/*
**�������ĳ�е���Ϣ(fieldnr = 0��FieldCount-1)
*/
MYSQL_FIELD*CMySQLResult::fieldFetchDirect(unsigned int fieldnr)
{
	if (!m_pResult)
		return NULL;

	if (fieldnr > m_uFiledCount - 1)
		return NULL;

	return mysql_fetch_field_direct(m_pResult,fieldnr);
}

/*
**���������ֶνṹ������
*/
MYSQL_FIELD*CMySQLResult::fieldFetchs()
{
	if (!m_pResult)
		return NULL;

	return mysql_fetch_fields(m_pResult);
}

/*
**��������ݳ���
*/
unsigned long	CMySQLResult::getFiledDataLength(unsigned int uField)
{
	if (!m_pResult)
		return 0;

	if (uField > m_uFiledCount - 1)
		return 0;

	if (!m_pulFiledsLength || !m_pulFiledsLength[uField])
		return 0;

	return m_pulFiledsLength[uField];
}

/*
**����һ������
*/
bool	CMySQLResult::rowMore()
{/*�ӽ�����л�ȡ��һ��(row[i])*/ 
	if (!m_pResult)
		return false;

	m_pulFiledsLength = NULL;
	m_pRow = mysql_fetch_row(m_pResult);
	if (!m_pRow)
		return false;

	m_pulFiledsLength = mysql_fetch_lengths(m_pResult);

	return true;
}

/*
**�Ƿ��ѵ������β
*/
bool	CMySQLResult::rowEof			()
{
	if (!m_pResult)
		return true;

	/*�ú����Ѳ��ٱ����ӣ�����ʹ��mysql_errno()��mysql_error()ȡ����֮��*/ 
	return (mysql_eof(m_pResult) == 0);
}
/*
**ȡ���ڼ�������
*/
bool	CMySQLResult::dataSeek(my_ulonglong offset)
{
	if (!m_pResult)
		return false;

	if (offset > m_ulRowCount - 1)
		return false;

	/*�ڲ�ѯ������в��������б�š�
	��Ӧ��mysql_store_result()����ʹ��,��Χ��0��mysql_num_rows(result)-1*/ 
	mysql_data_seek(m_pResult,offset);

	return rowMore();
}

/*
**��ǰ�й��
*/
MYSQL_ROWS*	CMySQLResult::rowTell()
{/*�����й��λ��*/ 
	if (!m_pResult)
		return NULL;;

	return mysql_row_tell(m_pResult);
}

/*
**�ƶ��й��
*/
MYSQL_ROWS*	CMySQLResult::rowSeek(MYSQL_ROWS*offset)
{
	if (!m_pResult || !offset)
		return NULL;

	/*ʹ�ô�mysql_row_tell()���ص�ֵ�����ҽ�����е���ƫ��*/ 
	return mysql_row_seek(m_pResult,offset);
}

/*
**��ȡ��ǰ���ֶ�����/�ַ�������
*/
const char*	CMySQLResult::getData(unsigned int uField,const char*pDefault)
{
	if (!m_pResult)
		return NULL;

	if (uField > m_uFiledCount - 1)
		return NULL;

	if (!m_pRow && !rowMore())
		return NULL;

	if(!m_pRow[uField])
		return pDefault;

	return m_pRow[uField];
}
