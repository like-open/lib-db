/*----------------- dbSqlite3Table.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*
*------------------------------------------------------------*/
#include "dbSqlite3.h"
#include <stdlib.h>
#include <string.h>
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CSqlite3Table::CSqlite3Table(char** ppszResults, int nRows, int nCols)
{
	m_nRows			= nRows;
	m_nCols			= nCols;
	m_nCurrentRow	= 0;
	m_ppszResults	= ppszResults;
}
/*
**
*/
CSqlite3Table::~CSqlite3Table()
{
	finalize();
}
/*
**
*/
CSqlite3Table& CSqlite3Table::operator=(CSqlite3Table& table)
{
	finalize();
	m_nRows			= table.m_nRows;
	m_nCols			= table.m_nCols;
	m_nCurrentRow	= table.m_nCurrentRow;
	m_ppszResults	= table.m_ppszResults;
	table.m_ppszResults	= NULL;

	return *this;
}

//-------------------------------------------------------------
//------------------------------ 
void	CSqlite3Table::finalize()
{
	if (m_ppszResults)
	{
		sqlite3_free_table(m_ppszResults);
		m_ppszResults = NULL;
	}
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Table::getIndex(int nCol)
{
	if (nCol < 0 || nCol > m_nCols - 1)
		return 0;

	return ((m_nCurrentRow * m_nCols) + m_nCols + nCol);
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Table::getField(const char* pField)
{
	if (!pField || !*pField)
		return -1;

	for (int nField = 0; nField < m_nCols; nField++)
	{
		if (strcmp(pField, m_ppszResults[nField]) == 0)
		{
			return nField;
		}
	}
	return -1;
}

//-------------------------------------------------------------
//------------------------------ 数据结果列数
int	CSqlite3Table::getColumn()
{
	return m_nCols;
}

//-------------------------------------------------------------
//------------------------------ 数据结果行数
int	CSqlite3Table::getRow()
{
	return m_nRows;
}

//-------------------------------------------------------------
//------------------------------ 设置当前行
bool	CSqlite3Table::setRow(int nRow)
{
	if (nRow < 0 || nRow > m_nRows - 1)
	{
		return false;
	}

	m_nCurrentRow = nRow;
	return true;
}

//-------------------------------------------------------------
//------------------------------ 第N列的字段名
const char*	CSqlite3Table::getName(int nCol)
{
	if (nCol < 0 || nCol > m_nCols - 1)
	{
		return "";
	}

	return m_ppszResults[nCol];
}

//-------------------------------------------------------------
//------------------------------ 
bool	CSqlite3Table::isNull(int nCol)
{
	int nIndex = getIndex(nCol);
	if (nIndex <= 0)
		return true;

	return (m_ppszResults[nIndex] == NULL);
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Table::getInt(int nCol,int nNullValue)
{
	const char*pValue = getValue(nCol);
	if (pValue)
	{
#ifdef WIN32
		return static_cast<int>(_atoi64(pValue));
#else//WIN32
		return static_cast<int>(atoll(pValue));
#endif//WIN32
	}

	return nNullValue;
}

//-------------------------------------------------------------
//------------------------------ 
double	CSqlite3Table::getDouble(int nCol,double dNullValue)
{
	const char*pValue = getValue(nCol);
	if (pValue)
		return atof(pValue);

	return dNullValue;
}

//-------------------------------------------------------------
//------------------------------ 
const char* CSqlite3Table::getValue(int nCol)
{
	int nIndex = getIndex(nCol);
	if (nIndex <= 0)
		return NULL;

	return m_ppszResults[nIndex];
}
/*
**
*/
const char* CSqlite3Table::getString(int nCol,const char* pNullValue)
{
	const char*pValue = getValue(nCol);
	if (pValue)
		return pValue;

	return pNullValue;
}

//-------------------------------------------------------------
//------------------------------ 
bool	CSqlite3Table::isNull(const char* pField)
{
	return isNull(getField(pField));
}

//-------------------------------------------------------------
//------------------------------ 
int	CSqlite3Table::getInt(const char* pField,int nNullValue)
{
	return getInt(getField(pField));
}

//-------------------------------------------------------------
//------------------------------ 
double	CSqlite3Table::getDouble(const char* pField,double dNullValue)
{
	return getDouble(getField(pField));
}

//-------------------------------------------------------------
//------------------------------ 
const char* CSqlite3Table::getValue(const char* pField)
{
	return getValue(getField(pField));
}

//-------------------------------------------------------------
//------------------------------ 
const char* CSqlite3Table::getString(const char* pField,const char* pNullValue)
{
	return getString(getField(pField));
}
