/*----------------- dbMySql.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*CDBMySql��ʵ��
*------------------------------------------------------------*/
#include <my_global.h>
#include "dbMySql.h"
/*#pragma comment(lib, "libmysql.lib")*/
/*************************************************************/
/*
**
*/
CDBMySql::CDBMySql()
{
	m_pMySQL			= NULL;
	m_bConnected	= false;

	mysql_server_init(0,NULL,NULL); /*�ɽ���mysql_init()���Զ�����*/ 
	initialize(true,3);
}
/*
**
*/
CDBMySql::~CDBMySql()
{
	close();
	m_pMySQL	= NULL;
	mysql_server_end();
}

/*
**��ʼ�����ݿ�����
*/
void	CDBMySql::initialize		(my_bool bReConnect,unsigned int uConnectTimeout,const char*pCharsetName)
{
	m_bReConnect		= bReConnect;
	m_uConnectTimeout	= uConnectTimeout;

	dMemset(m_szCharsetName,0,sizeof(m_szCharsetName));

#ifdef WIN32
	dSprintf(m_szCharsetName,sizeof(m_szCharsetName),"%s",pCharsetName ? pCharsetName : "gbk");
#else//WIN32
	dSprintf(m_szCharsetName,sizeof(m_szCharsetName),"%s",pCharsetName ? pCharsetName : "utf8");
#endif//WIN32

	m_ulCmdLength		= 0;
	dMemset(m_szCommand,0,sizeof(m_szCommand));
}

/*
**���������ݿ�
*/
bool	CDBMySql::open(const char*host,const char*user,const char*passwd,const char*db,unsigned int port,const char*unixSocket,unsigned long clientFlag)
{
	m_bConnected	= false;
	/*������ʼ��(MYSQL)����*/ 
	m_pMySQL = mysql_init(m_pMySQL);
	if (!m_pMySQL)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_init()");
#endif
		return false;
	}
	/*mysql_options:����enum mysql_option
	|ѡ��								|��������			|����																							|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_INIT_COMMAND					|char*				|���ӵ�MySQL������ʱ��ִ�е�����ٴ�����ʱ���Զ����ٴ�ִ�С�									|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_COMPRESS					|δʹ��				|ʹ��ѹ���ͻ��ˣ�������Э��																		|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_CONNECT_TIMEOUT			|unsigned int*		|����Ϊ��λ�����ӳ�ʱ��																			|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_GUESS_CONNECTION			|δʹ��				|������libmysqld���ӵ�Ӧ�ó�������⡰�²⡱�Ƿ�ʹ��Ƕ��ʽ��������Զ�̷�������				|
	|									|					|���²⡱��ʾ����������������������Ǳ�����������ʹ��Զ�̷�����������Ϊ��Ĭ����Ϊ��				|
	|									|					|��ʹ��MYSQL_OPT_USE_EMBEDDED_CONNECTION ��MYSQL_OPT_USE_REMOTE_CONNECTION��������				|
	|									|					|������libmysqlclient���ӵ�Ӧ�ó��򣬸�ѡ������ԡ�											|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_LOCAL_INFILE				|ָ��Ԫ�Ŀ�ѡָ��	|���δ����ָ�룬��ָ��ָ��unsigned int != 0��������������LOAD LOCAL INFILE��					|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_NAMED_PIPE				|δʹ��				|ʹ�������ܵ����ӵ�NTƽ̨�ϵ�MySQL��������														|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_PROTOCOL					|unsigned int*		|Ҫʹ�õ�Э�����͡�Ӧ��mysql.h�ж����mysql_protocol_type��ö��ֵ֮һ��							|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_READ_TIMEOUT				|unsigned int*		|�ӷ�������ȡ��Ϣ�ĳ�ʱ��Ŀǰ����Windowsƽ̨��TCP/IP��������Ч����								|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_RECONNECT				|my_bool*			|����������Ӷ�ʧ���������ֹ����������Զ������ӡ���MySQL 5.0.3��ʼ��							|
	|									|					|Ĭ������½�ֹ�����ӣ�����5.0.13�е���ѡ��ṩ��һ������ʽ��ʽ������������Ϊ�ķ�����			|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_SET_CLIENT_IP			|char*				|������libmysqld���ӵ�Ӧ�ó��򣨾߱�����֧�����Ե��ѱ���libmysqld��������ζ�ţ����ڼ���Ŀ�ģ�	|
	|									|					|�û�������Ϊ��ָ����IP��ַ��ָ��Ϊ�ַ������������ӡ�������libmysqlclient���ӵ�Ӧ�ó���		|
	|									|					|��ѡ������ԡ�																				|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_USE_EMBEDDED_CONNECTION	|δʹ��				|������libmysqld���ӵ�Ӧ�ó��򣬶���������˵������ǿ��ʹ��Ƕ��ʽ��������						|
	|									|					|������libmysqlclient���ӵ�Ӧ�ó��򣬸�ѡ������ԡ�											|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_USE_REMOTE_CONNECTION	|δʹ��				|������libmysqld���ӵ�Ӧ�ó��򣬶���������˵������ǿ��ʹ��Զ�̷�������							|
	|									|					|������libmysqlclient���ӵ�Ӧ�ó��򣬸�ѡ������ԡ�											|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_USE_RESULT				|δʹ��				|��ʹ�ø�ѡ�																					|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_OPT_WRITE_TIMEOUT			|unsigned int*		|д��������ĳ�ʱ��Ŀǰ����Windowsƽ̨��TCP/IP��������Ч����									|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_READ_DEFAULT_FILE			|char*				|������ѡ���ļ������Ǵ�my.cnf��ȡѡ�															|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_READ_DEFAULT_GROUP			|char*				|��my.cnf����MYSQL_READ_DEFAULT_FILEָ�����ļ��е��������ȡѡ�								|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_REPORT_DATA_TRUNCATION		|my_bool*			|ͨ��MYSQL_BIND.error������Ԥ������䣬������ֹͨ�����ݽضϴ���Ĭ��Ϊ��ֹ����				|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_SECURE_AUTH					|my_bool*			|�Ƿ����ӵ���֧�������๦�ܵķ���������MySQL 4.1.1�͸��߰汾�У�ʹ���������๦�ܡ�			|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_SET_CHARSET_DIR				|char*				|ָ������ַ��������ļ���Ŀ¼��·������															|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_SET_CHARSET_NAME				|char*				|����Ĭ���ַ������ַ��������ơ�																	|
	|-----------------------------------|-------------------|-----------------------------------------------------------------------------------------------|
	|MYSQL_SHARED_MEMORY_BASE_NAME		|char*				|����Ϊ�����������ͨ�ŵĹ����ڴ����Ӧ����������ӵ�mysqld������ʹ�õ�ѡ��					|
	|									|					|��-shared-memory-base-name����ͬ��																|
	-------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�������ӳ�ʱΪ3��*/ 
	if(mysql_options(m_pMySQL,MYSQL_OPT_CONNECT_TIMEOUT,(char*)&m_uConnectTimeout))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
#endif
	}
	
	unsigned int uTimeout = 10;
	if(mysql_options(m_pMySQL,MYSQL_OPT_READ_TIMEOUT,(char*)&uTimeout))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
#endif
	}
	if(mysql_options(m_pMySQL,MYSQL_OPT_WRITE_TIMEOUT,(char*)&uTimeout))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_OPT_CONNECT_TIMEOUT)");
#endif
	}

	/*�Զ�����*/ 
	if(mysql_options(m_pMySQL,MYSQL_OPT_RECONNECT,&m_bReConnect))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_OPT_RECONNECT)");
#endif
	}
	/*�����ַ���*/ 
	if(!m_szCharsetName[0])
	{
		dSprintf(m_szCharsetName,sizeof(m_szCharsetName),"%s","gbk");
	}

	if(mysql_options(m_pMySQL, MYSQL_SET_CHARSET_NAME,m_szCharsetName))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_SET_CHARSET_NAME)");
#endif
	}
	/*ʹ��ѹ��Э��*/ 
	if(mysql_options(m_pMySQL,MYSQL_OPT_COMPRESS,NULL))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_OPT_COMPRESS)");
#endif
	}
	/*����ͨ�����ݽضϴ���*/ 
	my_bool bReportTruncation = true;
	if(mysql_options(m_pMySQL,MYSQL_REPORT_DATA_TRUNCATION,&bReportTruncation))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_REPORT_DATA_TRUNCATION)");
#endif
	}

	/*Э������MYSQL_OPT_PROTOCOL(mysql_protocol_type)*/ 
	/*��ȡ��ʱMYSQL_OPT_READ_TIMEOUT(unsigned int*)*/ 
	/*д�볬ʱMYSQL_OPT_WRITE_TIMEOUT(unsigned int*)*/ 

	/*host == NULL | "localhost"[�뱾����������]*/ 
	/*user == NULL | ""			[��ǰ�û� UNIX�ǵ�ǰ�ĵ�¼�� Windows ODBC������ȷ��ǰ�û���]*/ 
	/*passwd == NULL			[���ض���ʽ����MySQLȨ��,�û��Ƿ�������,�û�����ò�ͬ��Ȩ��]*/ 
	/*db == NULL				[���ӻὫĬ�ϵ����ݿ���Ϊ��ֵ]*/ 
	/*port != 0					��ֵ������TCP/IP���ӵĶ˿ں�*/ 
	/*unixSocket != NULL		���ַ���������Ӧʹ�õ��׽��ֻ������ܵ�*/ 
	/*clientFlag */ 
	clientFlag |= CLIENT_MULTI_STATEMENTS;	/*�������ַ����ڷ��Ͷ������(��';'����)*/ 
	clientFlag |= CLIENT_MULTI_RESULTS;		/*���������*/ 
	clientFlag |= CLIENT_LOCAL_FILES;		/*����LOAD DATA LOCAL������*/ 

	if(!mysql_real_connect(m_pMySQL,host,user,passwd,db,port,unixSocket,clientFlag))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_real_connect");
#endif
		close();
		return false;
	}

	/*��������䴦��*/ 
	if (mysql_set_server_option(m_pMySQL,MYSQL_OPTION_MULTI_STATEMENTS_ON))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_options(MYSQL_OPTION_MULTI_STATEMENTS_ON)");
#endif
	}

	m_bConnected	= true;
	return true;
}

/*
**�ر����ݿ�����
*/
void	CDBMySql::close()
{
	if (!m_pMySQL)
		return;

	m_bConnected	= false;
	mysql_close(m_pMySQL);
	m_pMySQL	= NULL;
}

/*
**�Ƿ�����SQL(��Ͽ����Զ�����)
*/
bool	CDBMySql::ping()
{
	if (!m_pMySQL)
		return false;

	int32 nRes = mysql_ping(m_pMySQL);
	if (nRes == 0)
		return true;

#ifdef _MySQL_throw
	static char szMsg[128]= {0};
	dSprintf(szMsg,sizeof(szMsg),"mysql_ping(%d)",nRes);

	throw CMySQLException(m_pMySQL,0,szMsg);
#endif
}

//-------------------------------------------------------------
//------------------------------ ɱ����ǰ���ݿ�����
bool	CDBMySql::disconnect			()
{
	if (!m_pMySQL)
		return false;

	unsigned long uThreadID = mysql_thread_id(m_pMySQL);
	if(!uThreadID)
		return true;

	int nRes = mysql_kill(m_pMySQL,uThreadID);
	if(nRes == 0)
		return true;

	switch(nRes)
	{
	case CR_SERVER_GONE_ERROR:	//MySQL������������
		{
			return true;
		}
		break;
	case CR_SERVER_LOST:		//�ڲ�ѯ�����У�������������Ӷ�ʧ
		{
			return true;
		}
		break;
	}

#ifdef _MySQL_throw
	static char szMsg[128]= {0};
	dSprintf(szMsg,sizeof(szMsg),"mysql_kill(%d)",nRes);

	throw CMySQLException(m_pMySQL,0,szMsg);
#endif

	return true;
}

/*
**�Ƿ���������䴦��
*/
bool	CDBMySql::multiSentence(bool bMore)
{
	if (!m_pMySQL)
		return false;

	if (!mysql_set_server_option(m_pMySQL,bMore ? MYSQL_OPTION_MULTI_STATEMENTS_ON : MYSQL_OPTION_MULTI_STATEMENTS_OFF))
		return true;

#ifdef _MySQL_throw
	throw CMySQLException(m_pMySQL,0,"mysql_set_server_option()");
#endif
	return false;
}

/*
**���õ�ǰ�����ַ���
*/
bool	CDBMySql::setConnectCharacter(const char*charsetName)
{
	if (!charsetName || !*charsetName || !ping())
		return false;

	return (mysql_set_character_set(m_pMySQL,charsetName) == 0);
}
/*
**���õ�ǰ�ַ���
*/
bool	CDBMySql::setCharacter		(const char*charsetName)
{
	if(!charsetName)
		return false;

	try
	{
		CMySQLMultiFree	clMultiFree(this);

		this->cmd("SET names '%s';SET CHARACTER_SET '%s'",charsetName,charsetName);
		if(this->execute())
			return true;
	}
	catch(CMySQLException & /*e*/)
	{
		//e;
	}

	return false;
}

/*
**��ǰ����Ĭ���ַ���
*/
const char*	CDBMySql::getConnectCharacter()
{
	if (!ping())
		return "";

	return mysql_character_set_name(m_pMySQL);
}
bool	CDBMySql::getServerInfo()
{
	if (!ping())
		return false;

	dPrintf("mysql_get_host_info(%s)\n",mysql_get_host_info(m_pMySQL));
	dPrintf("��������ʽ���ط������İ汾��(%d)\n",mysql_get_server_version(m_pMySQL));
	dPrintf("���ط������İ汾��(%s)\n",mysql_get_server_info(m_pMySQL));
	dPrintf("���ַ�����ʽ���ط�����״̬(%s)\n",mysql_stat(m_pMySQL));
	dPrintf("����ѯ��Ϣ(%s)\n",mysql_info(m_pMySQL));

	return true;
}

bool	CDBMySql::getClientInfo()
{
	if (m_pMySQL)
		dPrintf("����������ʹ�õ�Э��汾��(%d)\n",mysql_get_proto_info(m_pMySQL));

	dPrintf("���ַ�����ʽ���ؿͻ��˰汾��Ϣ��(%s)\n",mysql_get_client_info());
	dPrintf("��������ʽ���ؿͻ��˰汾��Ϣ(%d)\n",mysql_get_client_version());

	return true;
}

/*
**��ʾ��ǰ�������̵߳��б�
*/
CMySQLResult	CDBMySql::showProcessesList()
{
	if(!ping())
		return CMySQLResult();

	MYSQL_RES*pResult = mysql_list_processes(m_pMySQL);
	if(!pResult)
	{
		/*����*/ 
		/*CR_COMMANDS_OUT_OF_SYNC	�Բ�ǡ����˳��ִ������� 
		CR_SERVER_GONE_ERROR		MySQL�����������á� 
		CR_SERVER_LOST				�ڲ�ѯ�����У�������������Ӷ�ʧ�� 
		CR_UNKNOWN_ERROR			����δ֪���� */
	}

	return CMySQLResult(m_pMySQL,pResult);
}
/*
**��õ�ǰ���ӵ��߳�ID
*/
unsigned long	CDBMySql::getConnectThreadID		()
{
	if(!ping())
		return 0;

	return mysql_thread_id(m_pMySQL);
}

/*
**������һ��SQL������ɵĸ澯��
*/
unsigned int	CDBMySql::getLastWarningCount	()
{
	if(!m_pMySQL)
		return 0;

	return mysql_warning_count(m_pMySQL);
}

/*
**���ִ�в�ѯ���ַ�����Ϣ
*/
const char*	CDBMySql::getExecuteInfo		()
{
	if (!m_pMySQL)
		return NULL;

	return mysql_info(m_pMySQL);
}

/*
**��ǰ����Ĭ���ַ�����Ϣ
*/
bool	CDBMySql::getConnectCharacter(MY_CHARSET_INFO& sInfo)
{
	dMemset(&sInfo,0,sizeof(sInfo));
	if (!m_pMySQL)
		return false;

	mysql_get_character_set_info(m_pMySQL,&sInfo);

	dPrintf("�ַ�����Ϣ:\n");
	dPrintf("�ַ�����: %s\n", sInfo.name);
	dPrintf("У����: %s\n", sInfo.csname);
	dPrintf("ע��: %s\n", sInfo.comment);
	dPrintf("Ŀ¼: %s\n", sInfo.dir);
	dPrintf("���ֽ��ַ���С����: %d\n", sInfo.mbminlen);
	dPrintf("���ֽ��ַ���󳤶�: %d\n", sInfo.mbmaxlen);

	return true;
}

/*
**���������ϵ��û������ݿ�
*/
bool	CDBMySql::changeConnectUser	(const char*user,const char*passwd, const char*db)
{
	my_bool	bRes = mysql_change_user(m_pMySQL,user,passwd,db);
	if(bRes == 0)
		return true;

	return false;
}
/*
**���ݿ�ѡ��
*/
bool	CDBMySql::dbSelect(const char*db)
{
	if (!db || !*db || !ping())
		return false;

	return (mysql_select_db(m_pMySQL,db) == 0);
}

/*
**���ݿⴴ��
*/
bool	CDBMySql::dbCreate(const char*db,const char*charsetName,const char*collationName,bool exist)
{
	if (!db || !*db || !ping())
		return false;

	/*�ú����Ѳ��ٱ����ӣ�ʹ��SQL���CREATE DATABASEȡ����֮��*/ 
	/*return (mysql_create_db(m_pMySQL,db) == 0);*/ 

	dSprintf(m_szCommand,sizeof(m_szCommand),"CREATE DATABASE");

	if (exist)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s IF NOT EXISTS",m_szCommand);

	dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s",m_szCommand,db);

	if (charsetName)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s CHARACTER SET ",m_szCommand,charsetName);

	if (collationName)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s %sCOLLATE %s",m_szCommand,(charsetName && *charsetName) ? "," : "",collationName);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**���ݿ�ɾ��
*/
bool	CDBMySql::dbDrop(const char*db,bool exist)
{
	if (!db || !*db || !ping())
		return false;

	/*�ú����Ѳ��ٱ����ӣ�ʹ��SQL���DROP DATABASEȡ����֮��*/ 
	/*return (mysql_drop_db(m_pMySQL,db) == 0);*/ 

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	dSprintf(m_szCommand,sizeof(m_szCommand),"DROP DATABASE");

	if (exist)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s IF EXISTS",m_szCommand);

	dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s",m_szCommand,db);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**���ݿ��޸�
*/
bool	CDBMySql::dbAlter(const char*db,const char*charsetName,const char*collationName)
{
	if (!charsetName && !collationName)
		return false;

	if (!ping())
		return false;

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	dSprintf(m_szCommand,sizeof(m_szCommand),"ALTER DATABASE");

	if (db && *db)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s",m_szCommand,db);

	if (charsetName)/*�ַ���*/ 
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s CHARACTER SET %s",m_szCommand,charsetName);

	if (collationName)/*����*/ 
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s %sCOLLATE %s",m_szCommand,(charsetName && *charsetName) ? "," : "",collationName);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

//-------------------------------------------------------------
//------------------------------ ���Ƿ����
bool	CDBMySql::tableExists(const char* pTableName)
{
	if (!pTableName)
		return false;

	try
	{
		CMySQLMultiFree	clMultiFree(this);

		this->cmd("SHOW TABLES LIKE '%s'",pTableName);
		if(!this->execute())
			return false;

		CMySQLResult clResult = this->getResult();
		if (clResult.rowMore())
			return true;
	}
	catch(CMySQLException & /*e*/)
	{
		//e;
	}

	return false;
}

/*
**���ݱ���
*/
bool	CDBMySql::tableCreate(const char*tblName,bool temp,bool exist,const char*definition,...)
{
	if (!tblName || !*tblName || !definition || !*definition)
		return false;

	if (!ping())
		return false;

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	if (temp)
		dSprintf(m_szCommand,sizeof(m_szCommand),"CREATE TEMPORARY TABLE");
	else
		dSprintf(m_szCommand,sizeof(m_szCommand),"CREATE TABLE");

	if (exist)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s IF NOT EXISTS",m_szCommand);

	dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s (",m_szCommand,tblName);

	char szFormat[16*1024] = {0};
	va_list	argptr;
	va_start(argptr,definition);
	dVsprintf(szFormat,sizeof(szFormat),definition,argptr);
	va_end(argptr);

	dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s);",m_szCommand,szFormat);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**���ݱ�ɾ��
*/
bool	CDBMySql::tableDrop(bool temp,bool exist,const char*tblName,...)
{
	if (!tblName || !*tblName || !ping())
		return false;

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	if (temp)
		dSprintf(m_szCommand,sizeof(m_szCommand),"DROP TEMPORARY TABLE");
	else
		dSprintf(m_szCommand,sizeof(m_szCommand),"DROP TABLE");

	if (exist)
		dSprintf(m_szCommand,sizeof(m_szCommand),"%s IF EXISTS ",m_szCommand);

	char szFormat[16*1024] = {0};
	va_list	argptr;
	va_start(argptr,tblName);
	dVsprintf(szFormat,sizeof(szFormat),tblName,argptr);
	va_end(argptr);

	dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s",m_szCommand,szFormat);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**���ݱ��޸�
*/
bool	CDBMySql::tableAlter(const char*tblName,const char*command,...)
{
	if (!tblName || !*tblName || !command || !*command || !ping())
		return false;

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	dSprintf(m_szCommand,sizeof(m_szCommand),"ALTER TABLE %s",tblName);

	char szFormat[16*1024] = {0};
	va_list	argptr;
	va_start(argptr,command);
	dVsprintf(szFormat,sizeof(szFormat),command,argptr);
	va_end(argptr);

	dSprintf(m_szCommand,sizeof(m_szCommand),"%s %s",m_szCommand,szFormat);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**���ݱ�������
*/
bool	CDBMySql::tableRename(const char*tblName,const char*newName)
{
	if (!tblName || !*tblName || !newName || !*newName || !ping())
		return false;

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	dSprintf(m_szCommand,sizeof(m_szCommand),"RENAME TABLE %s TO %s",tblName,newName);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**���ݱ�������
*/
bool	CDBMySql::tableRename(const char*tblName,...)
{
	if (!tblName || !*tblName || !ping())
		return false;

	dMemset(m_szCommand,0,sizeof(m_szCommand));
	char szFormat[16*1024] = {0};
	va_list	argptr;
	va_start(argptr,tblName);
	dVsprintf(szFormat,sizeof(szFormat),tblName,argptr);
	va_end(argptr);

	dSprintf(m_szCommand,sizeof(m_szCommand),"RENAME TABLE %s",m_szCommand,szFormat);

	return (mysql_query(m_pMySQL,m_szCommand) == 0);
}

/*
**�������ݿ���
*/
CMySQLResult	CDBMySql::showDatabase(const char*wild)
{
	if (!ping())
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"CDBMySql::showDatabase()");
#endif
		return CMySQLResult(m_pMySQL);
	}

	MYSQL_RES* pResult = mysql_list_dbs(m_pMySQL,wild);
	if (!pResult)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_list_dbs()");
#endif
		return CMySQLResult(m_pMySQL);
	}

	return CMySQLResult(m_pMySQL,pResult);
}

/*
**������ǰ���ݿ��ڱ���
*/
CMySQLResult	CDBMySql::showTables(const char*wild)
{
	if (!ping())
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"CDBMySql::showTables()");
#endif
		return CMySQLResult(m_pMySQL);
	}

	/*SHOW tables [LIKE wild]*/ 
	MYSQL_RES* pResult = mysql_list_tables(m_pMySQL,wild);
	if (!pResult)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_list_tables()");
#endif
		return CMySQLResult(m_pMySQL);
	}

	return CMySQLResult(m_pMySQL,pResult);
}

/*
**������ǰ���ݿ��б��ֶ���
*/
CMySQLResult	CDBMySql::showFields(const char*table,const char*wild)
{
	if (!ping() || !table || !*table)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"CDBMySql::showFields");
#endif
		return CMySQLResult(m_pMySQL);
	}

	MYSQL_RES*pResult = NULL;
	if(0)
	{
		pResult = mysql_list_fields(m_pMySQL,table,wild);
		/*SHOW COLUMNS FROM tbl_name [LIKE wild]*/ /*����ʹ�����*/ 
		if(!pResult)
		{
			/*����*/ 
			/*CR_COMMANDS_OUT_OF_SYNC	�Բ�ǡ����˳��ִ������� 
			CR_SERVER_GONE_ERROR		MySQL�����������á� 
			CR_SERVER_LOST				�ڲ�ѯ�����У�������������Ӷ�ʧ�� 
			CR_UNKNOWN_ERROR			����δ֪���� */
		}
	}
	else
	{
		char	szCmd[2 * 1024] = {0};
		dSprintf(szCmd,sizeof(szCmd),"SHOW COLUMNS FROM %s",table);
		if (wild)
			dSprintf(szCmd,sizeof(szCmd),"%s %s",szCmd,wild);

		if (mysql_query(m_pMySQL,szCmd) != 0)
		{
#ifdef _MySQL_throw
			throw CMySQLException(m_pMySQL,0,"mysql_query()");
#endif
			return CMySQLResult(m_pMySQL);
		}

		pResult = mysql_store_result(m_pMySQL);
	}
	
	if (!pResult)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL);
#endif
		return CMySQLResult(m_pMySQL);
	}

	return CMySQLResult(m_pMySQL,pResult);
}

/*
**��ʽ�����ݿ�ָ��
*/
ulong	CDBMySql::cmd			(const char*pCmd,...)
{
	if (!pCmd)
		return 0;

	dMemset(m_szCommand,0,sizeof(m_szCommand));

	va_list	argptr;
	va_start(argptr,pCmd);
	m_ulCmdLength = dVsprintf(m_szCommand,sizeof(m_szCommand),pCmd,argptr);
	va_end(argptr);	

	return m_ulCmdLength;
}
//-------------------------------------------------------------
//------------------------------ 
ulong	CDBMySql::cmd_				(const char*pCmd)
{
	if (!pCmd)
		return 0;

	dMemset(m_szCommand,0,sizeof(m_szCommand));

	m_ulCmdLength = dStrlen(pCmd);
	if(m_ulCmdLength > sizeof(m_szCommand))
		m_ulCmdLength = sizeof(m_szCommand);

	dStrncpy(m_szCommand,sizeof(m_szCommand),pCmd,m_ulCmdLength);

	return m_ulCmdLength;
}

/*
**ִ��(execute)
*/
bool	CDBMySql::execute		(bool real)
{
	if (!m_szCommand[0] || !ping())
		return false;

	//������
	if(m_pMySQL)
	{
		MYSQL_RES* pDBResult = NULL;
		do
		{
			pDBResult = mysql_store_result(m_pMySQL);
			if(pDBResult)
				mysql_free_result(pDBResult);
		}
		while(mysql_next_result(m_pMySQL) == 0);
	}

	int nRes = 0;
	if (real)
		nRes = mysql_real_query(m_pMySQL,m_szCommand,m_ulCmdLength);	/*�����ڰ������������ݵĲ�ѯ*/ 
	else
		nRes = mysql_query(m_pMySQL,m_szCommand);						/*�������ڰ������������ݵĲ�ѯ*/ 

	if (nRes != 0)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"CDBMySql::execute()");
#endif
		return false;
	}

	return true;
}

/*
**���AUTO_INCREMENT�����ɵ�ID
*/
my_ulonglong	CDBMySql::getInsertIncrement	()
{
	if (!m_pMySQL)
		return 0;

	return	mysql_insert_id(m_pMySQL);
}

/*
**Ӱ��/�ı�����
*/
my_ulonglong	CDBMySql::getRowAffected()
{
	if(!m_pMySQL)
		return 0;

	return mysql_affected_rows(m_pMySQL);
}
/*
**���ִ�н������(�޽����ʱ����)
*/
unsigned int	CDBMySql::getResultFieldCount	()
{
	if(!m_pMySQL)
		return 0;

	return mysql_field_count(m_pMySQL);
}

/*
**���ִ�н��
*/
CMySQLResult	CDBMySql::getResult			()
{
	if(!m_pMySQL)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"CDBMySql::getResult()");
#endif
		return CMySQLResult(m_pMySQL);
	}
	if(!mysql_field_count(m_pMySQL))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_field_count()");
#endif
		return CMySQLResult(m_pMySQL);
	}

	/*��ʼ�����еĽ����������ֱ�Ӵӷ�������ȡ����������Ὣ�䱣������ʱ��򱾵ػ�������*/ 
	/*m_pDBResult = mysql_use_result(m_pMySql);*/ 

	/*���������Ľ�������ͻ��ˡ�*/ 
	MYSQL_RES* pDBResult = mysql_store_result(m_pMySQL);
	if (!pDBResult)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_store_result()");
#endif
		return CMySQLResult(m_pMySQL);
	}

	return CMySQLResult(m_pMySQL,pDBResult);
}

/*
**�Ƿ�������������
*/
bool	CDBMySql::resultMore			()
{
	if (!m_pMySQL)
		return false;

	if(mysql_more_results(m_pMySQL) == 1)
		return false;

	/*�ڶ����ִ�й����з���/��ʼ����һ�������
	����ֵ0 �ɹ����ж�������-1 �ɹ���û�ж������� > 0 ����*/ 
	return (mysql_next_result(m_pMySQL) == 0);
}

/*
**���Ԥ����
*/
CMySQLStatement	CDBMySql::createStatement()
{
	if (!ping())
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"CDBMySql::createStatement()");
#endif
		return CMySQLStatement();
	}

	//������
	if(m_pMySQL)
	{
		MYSQL_RES* pDBResult = NULL;
		do
		{
			pDBResult = mysql_store_result(m_pMySQL);
			if(pDBResult)
				mysql_free_result(pDBResult);
		}
		while(mysql_next_result(m_pMySQL) == 0);
	}
	/*����MYSQL_STMT���*/ 
	MYSQL_STMT* pStmt = mysql_stmt_init(m_pMySQL);
	if(!pStmt)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pMySQL,0,"mysql_stmt_init()");
#endif
		return CMySQLStatement();
	}

	return CMySQLStatement(pStmt);
}
/*************************************************************
* ˵�� : ���ѯ�ͷ�
*************************************************************/
/*
**
*/
CMySQLMultiFree::CMySQLMultiFree()
{
	m_pMySQL = NULL;
}
/*
**
*/
CMySQLMultiFree::CMySQLMultiFree(MYSQL*pMySQL)
{
	m_pMySQL = pMySQL;
}
/*
**
*/
CMySQLMultiFree::CMySQLMultiFree(CDBMySql*pMySQL)
{
	if(!pMySQL)
		pMySQL	= NULL;
	else
		m_pMySQL= pMySQL->m_pMySQL;
}
/*
**
*/
CMySQLMultiFree::~CMySQLMultiFree()
{
	if(m_pMySQL)
	{
		MYSQL_RES* pDBResult = NULL;
		do
		{
			pDBResult = mysql_store_result(m_pMySQL);
			if(pDBResult)
				mysql_free_result(pDBResult);
		}
		while(mysql_next_result(m_pMySQL) == 0);
	}
}
