/*----------------- dbSqlite3Transaction.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*事务
*------------------------------------------------------------*/
#include "dbSqlite3.h"
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Transaction::CSqlite3Transaction(sqlite3* pDBCon)
{
	m_bEnd		= true;
	m_pDBCon	= pDBCon;
}
/*
*/
CSqlite3Transaction::~CSqlite3Transaction()
{
	rollback();
}
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Transaction& CSqlite3Transaction::operator=(CSqlite3Transaction& transaction)
{
	rollback();
	m_bEnd		= transaction.m_bEnd;
	m_pDBCon	= transaction.m_pDBCon;
	transaction.m_pDBCon	= NULL;

	return *this;
}
//-------------------------------------------------------------
//------------------------------ 开始事务
bool	CSqlite3Transaction::begin()
{
	rollback();

	char szSQL[128] = {"begin transaction;"};
	char* pError = NULL;
	int nRet = sqlite3_exec(m_pDBCon, szSQL, 0, 0, &pError);
	if (nRet == SQLITE_OK && sqlite3_changes(m_pDBCon) != -1)
	{
		m_bEnd = false;
		return true;
	}

#ifdef	sqlite_throw
	throw CSqlite3Exception(m_pDBCon,nRet,pError);
#endif	/*sqlite_throw*/ 

	return false;
}

//-------------------------------------------------------------
//------------------------------ 提交事务
bool	CSqlite3Transaction::commit()
{
	if (m_bEnd)
		return false;

	char szSQL[128] = {"commit transaction;"};
	char* pError = NULL;
	int nRet = sqlite3_exec(m_pDBCon, szSQL, 0, 0, &pError);
	if (nRet == SQLITE_OK && sqlite3_changes(m_pDBCon) != -1)
	{
		m_bEnd = true;
		return true;
	}

#ifdef	sqlite_throw
	throw CSqlite3Exception(m_pDBCon,nRet,pError);
#endif	/*sqlite_throw*/ 

	return false;
}

//-------------------------------------------------------------
//------------------------------ 回滚事务
bool	CSqlite3Transaction::rollback()
{
	if (m_bEnd)
		return false;

	char szSQL[128] = {"rollback transaction;"};
	char* pError = NULL;
	int nRet = sqlite3_exec(m_pDBCon, szSQL, 0, 0, &pError);
	if (nRet == SQLITE_OK && sqlite3_changes(m_pDBCon) != -1)
	{
		m_bEnd = true;
		return true;
	}

#ifdef	sqlite_throw
	throw CSqlite3Exception(m_pDBCon,nRet,pError);
#endif	/*sqlite_throw*/ 

	return false;
}
