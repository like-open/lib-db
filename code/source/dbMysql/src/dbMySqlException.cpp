/*----------------- dbMySqlException.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*MySQL异常处理
*------------------------------------------------------------*/
#include <my_global.h>
#include "dbMySql.h"
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CMySQLException::CMySQLException(MYSQL* mysql,unsigned int uCode,const char* pMsg)
{
	this->uCode			= uCode;
	dMemset(this->szMsg,0,sizeof(this->szMsg));
	dMemset(this->szError,0,sizeof(this->szError));
	dMemset(this->szSqlState,0,sizeof(this->szSqlState));

	if(pMsg)
		dStrcpy(this->szMsg,sizeof(this->szMsg),pMsg);

	if(mysql)
	{
		/*返回上次调用的MySQL函数的错误编号*/ 
		if(!uCode)
			this->uCode		= mysql_errno(mysql);	

		/*返回上次调用的MySQL函数的错误消息*/ 
		dStrcpy(this->szError,sizeof(this->szError),mysql_error(mysql));
		/*返回关于上一个错误的SQLSTATE错误代码*/ 
		dStrcpy(this->szSqlState,sizeof(this->szSqlState),mysql_sqlstate(mysql));
	}
}

//-------------------------------------------------------------
//------------------------------ 
CMySQLException::CMySQLException(MYSQL_STMT* stmt,unsigned int uCode,const char* pMsg)
{
	this->uCode			= uCode;
	dMemset(this->szMsg,0,sizeof(this->szMsg));
	dMemset(this->szError,0,sizeof(this->szError));
	dMemset(this->szSqlState,0,sizeof(this->szSqlState));

	if(pMsg)
		dStrcpy(this->szMsg,sizeof(this->szMsg),pMsg);

	if(stmt)
	{
		if(!uCode)
			this->uCode		= mysql_stmt_errno(stmt);/*返回上次语句执行的错误编号*/ 

		/*返回上次语句执行的错误消息*/ 
		dStrcpy(this->szError,sizeof(this->szError),mysql_stmt_error(stmt));
		/*返回关于上次语句执行的SQLSTATE错误代码*/ 
		dStrcpy(this->szSqlState,sizeof(this->szSqlState),mysql_stmt_sqlstate(stmt));
	}
}
