/*----------------- dbSqlite3Statement.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*
*------------------------------------------------------------*/
#include "dbSqlite3.h"
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Statement::CSqlite3Statement(sqlite3*pDBCon,sqlite3_stmt*pStmt)
{
	m_pDBCon	= pDBCon;
	m_pStatement= pStmt;

}
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Statement::~CSqlite3Statement()
{
	finalize();
}
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Statement& CSqlite3Statement::operator=(CSqlite3Statement& statement)
{
	m_pDBCon	= statement.m_pDBCon;
	m_pStatement= statement.m_pStatement;
	statement.m_pStatement = NULL;

	return *this;
}

//-------------------------------------------------------------
//------------------------------ 销毁SQL
void	CSqlite3Statement::finalize()
{
	sqlite3_finalize(m_pStatement);
	m_pStatement = NULL;
}

//-------------------------------------------------------------
//------------------------------ 重置
void	CSqlite3Statement::reset()
{
	if (m_pStatement)
	{
		int nRet = sqlite3_reset(m_pStatement);
		if (nRet != SQLITE_OK)
		{
#ifdef	sqlite_throw
			throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
		}
	}
}

//-------------------------------------------------------------
//------------------------------ 执行语句(返回影响行数字)
int	CSqlite3Statement::exec()
{
	int nRowsChanged = 0;
	int nRet = sqlite3_step(m_pStatement);
	if (nRet == SQLITE_DONE)
		nRowsChanged = sqlite3_changes(m_pDBCon);

	nRet = sqlite3_reset(m_pStatement);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}

	return nRowsChanged;
}
//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_null(int nParam)
{
	int nRet = sqlite3_bind_null(m_pStatement, nParam);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_double(int nParam,double dValue)
{
	int nRet = sqlite3_bind_double(m_pStatement, nParam, dValue);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}
//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_int(int nParam,int nValue)
{
	int nRet = sqlite3_bind_int(m_pStatement, nParam, nValue);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_int64(int nParam,long long int nValue)
{
	int nRet = sqlite3_bind_int64(m_pStatement, nParam, nValue);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}
//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_blob(int nParam,const void* pValue,int nLen)
{
	int nRet = sqlite3_bind_blob(m_pStatement, nParam, pValue,nLen, SQLITE_TRANSIENT);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_text(int nParam,const char* pValue)
{
	int nRet = sqlite3_bind_text(m_pStatement, nParam, pValue, -1, SQLITE_TRANSIENT);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Statement::bind_text16(int nParam,const void* pValue)
{
	int nRet = sqlite3_bind_text16(m_pStatement, nParam,pValue, -1, SQLITE_TRANSIENT);
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pDBCon,nRet);
#endif	/*sqlite_throw*/ 
	}
	return nRet;
}
