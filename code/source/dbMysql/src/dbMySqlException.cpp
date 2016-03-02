/*----------------- dbMySqlException.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*MySQL�쳣����
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
		/*�����ϴε��õ�MySQL�����Ĵ�����*/ 
		if(!uCode)
			this->uCode		= mysql_errno(mysql);	

		/*�����ϴε��õ�MySQL�����Ĵ�����Ϣ*/ 
		dStrcpy(this->szError,sizeof(this->szError),mysql_error(mysql));
		/*���ع�����һ�������SQLSTATE�������*/ 
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
			this->uCode		= mysql_stmt_errno(stmt);/*�����ϴ����ִ�еĴ�����*/ 

		/*�����ϴ����ִ�еĴ�����Ϣ*/ 
		dStrcpy(this->szError,sizeof(this->szError),mysql_stmt_error(stmt));
		/*���ع����ϴ����ִ�е�SQLSTATE�������*/ 
		dStrcpy(this->szSqlState,sizeof(this->szSqlState),mysql_stmt_sqlstate(stmt));
	}
}
