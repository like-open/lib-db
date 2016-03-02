/*----------------- dbMySqlStatement.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
** ���Ԥ����
*ִ�в���
*1.��msyql_stmt_init()����Ԥ�����������Ҫ���ڷ�������׼��Ԥ������䣬�ɵ���mysql_stmt_prepare()����Ϊ�䴫�ݰ���SQL�����ַ�����
*2.�����������˽����������mysql_stmt_result_metadata()�Ի�ý����Ԫ���ݡ���Ȼ�������ѯ�����еĽ������ͬ����Ԫ���ݱ���Ҳ�����˽��������ʽ��Ԫ���ݽ����ָ���˽���а��������У�������ÿһ�е���Ϣ��
*3.ʹ��mysql_stmt_bind_param()�����κβ�����ֵ�������������в������������ִ�н����ش��󣬻������޷�Ԥ�ϵĽ����
*4.����mysql_stmt_execute()ִ����䡣
*5.�����������˽�������������ݻ��壬ͨ������mysql_stmt_bind_result()��������ֵ��
*6.ͨ���ظ�����mysql_stmt_fetch()�����н�������ȡ����������ֱ��δ���ָ�����Ϊֹ��
*7.ͨ�����Ĳ���ֵ���ٴ�ִ����䣬�ظ�����3������6��
*------------------------------------------------------------*/
#include <my_global.h>
#include "dbMySql.h"
/*************************************************************/
/*
**
*/
CMySQLStatement::CMySQLStatement(MYSQL_STMT*pStatement)
{
	m_pStatement	= pStatement;

	m_pParamLength	= NULL;
	m_pParamBind	= NULL;
	m_ulParamCount	= 0;

	m_ulRowCount	= 0;
	m_pResultBind	= NULL;
	m_ulResultCount	= 0;
}
/*
**
*/
CMySQLStatement::~CMySQLStatement()
{
	close();
}

/*
**
*/
CMySQLStatement&	CMySQLStatement::operator=	(CMySQLStatement& clMySQLStatement)
{
	close();
	m_pStatement					= clMySQLStatement.m_pStatement;
	m_pParamLength					= clMySQLStatement.m_pParamLength;
	m_pParamBind					= clMySQLStatement.m_pParamBind;
	m_ulParamCount					= clMySQLStatement.m_ulParamCount;

	m_ulRowCount					= clMySQLStatement.m_ulRowCount;
	m_pResultBind					= clMySQLStatement.m_pResultBind;
	m_ulResultCount					= clMySQLStatement.m_ulResultCount;

	/*��Ϊ��Ҫ�����رձ����ÿ�*/ 
	clMySQLStatement.m_pParamLength	= NULL;
	clMySQLStatement.m_pStatement	= NULL;
	clMySQLStatement.m_pParamBind	= NULL;
	clMySQLStatement.m_ulParamCount	= 0;

	clMySQLStatement.m_ulRowCount	= 0;
	clMySQLStatement.m_pResultBind	= NULL;
	clMySQLStatement.m_ulResultCount= 0;

	return *this;
}

/*
**�ͷŰ󶨻���
*/
void	CMySQLStatement::freeBind()
{
	if(m_pParamLength)
		delete[] m_pParamLength;

	if(m_pParamBind)
		delete[] m_pParamBind;
	m_pParamBind	= NULL;
	m_ulParamCount	= 0;

	if(m_pResultBind)
		delete[] m_pResultBind;
	m_pResultBind	= NULL;
	m_ulResultCount	= 0;

	m_ulRowCount	= 0;
}

/*
**�ر�Ԥ�������
*/
bool	CMySQLStatement::close	()
{
	freeBind();
	freeResult();

	if(!m_pStatement)
		return false;

	if(mysql_stmt_close(m_pStatement) != 0)
		return false;

	m_pStatement = NULL;
	return true;
}

/*
**�ͷ���ִ��Ԥ����������ɵĽ�����йص��ڴ�
*/
bool	CMySQLStatement::freeResult			()
{
	if(!m_pStatement)
		return false;

	if(mysql_stmt_free_result(m_pStatement) == 0)
		return true;

	return false;
}


/*
**��ʽ�����ݿ�ָ�׼��
*/
bool	CMySQLStatement::cmd		(const char*pCmd,...)
{
	freeBind();
	if (!pCmd)
		return false;

	unsigned long	ulCmdLength	= 0;
	char			szCommand[16 * 1024];
	dMemset(szCommand,0,sizeof(szCommand));

	va_list	argptr;
	va_start(argptr,pCmd);
	ulCmdLength = dVsprintf(szCommand,sizeof(szCommand),pCmd,argptr);
	va_end(argptr);	

	if(!m_pStatement)
		return false;

	/*Ϊִ�в���׼��SQL�ַ���*/ 
	if(mysql_stmt_prepare(m_pStatement,szCommand,ulCmdLength) != 0)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"mysql_stmt_prepare()");
#endif
		return false;
	}

	/*����*/
	/*��ò�������*/ 
	m_ulParamCount = mysql_stmt_param_count(m_pStatement);
	/*���仺����*/ 
	if(m_ulParamCount)
	{
		m_pParamBind = new MYSQL_BIND[m_ulParamCount];
		m_pParamLength=new unsigned long[m_ulParamCount];
		if(!m_pParamBind)
		{
#ifdef _MySQL_throw
			throw CMySQLException(m_pStatement,0,"mysql_stmt_param_count()�������������ʧ��!");
#endif
			return false;
		}
		dMemset(m_pParamBind,0,sizeof(MYSQL_BIND) * m_ulParamCount);
		dMemset(m_pParamLength,0,sizeof(unsigned long) * m_ulParamCount);
	}

	/*���*/
	if(1)
	{
		/*���ع��������������*/ 
		m_ulResultCount = mysql_stmt_field_count(m_pStatement);

	}
	else
	{
		/*ȡ�����������Ϣ*/ 
		MYSQL_RES* pResultMetadata = mysql_stmt_result_metadata(m_pStatement);
		if (pResultMetadata)
		{
			/*��ò�ѯ���������*/ 
			m_ulResultCount	= mysql_num_fields(pResultMetadata);
			mysql_free_result(pResultMetadata);
		}
	}

	/*���仺����*/ 
	if(m_ulResultCount)
	{
		m_pResultBind = new MYSQL_BIND[m_ulResultCount];
		if(!m_pResultBind)
		{
#ifdef _MySQL_throw
				throw CMySQLException(m_pStatement,0,"������������ʧ��!");
#endif
				return false;
		}
		dMemset(m_pResultBind,0,sizeof(MYSQL_BIND) * m_ulResultCount);
	}

	return true;
}

/*
**ִ��(execute)
*/
bool	CMySQLStatement::execute				()
{
	if(!m_pStatement)
		return false;

	if (mysql_stmt_execute(m_pStatement))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"mysql_stmt_execute()");
#endif
		return false;
	}

	/*���÷��ؽ�������ֶ���󳤶�ֵ*/ 
	my_bool bUpdateMaxLength = 1;
	if(mysql_stmt_attr_get(m_pStatement,STMT_ATTR_UPDATE_MAX_LENGTH,&bUpdateMaxLength) == 0)
	{
		if (bUpdateMaxLength != 1)
		{
			bUpdateMaxLength = 1;
			/*�����Ϊ1������mysql_stmt_store_result()�е�Ԫ����MYSQL_FIELD->max_length*/ 
			if(mysql_stmt_attr_set(m_pStatement,STMT_ATTR_UPDATE_MAX_LENGTH,&bUpdateMaxLength))
			{
				/*���ѡ��δ֪�����ط�0ֵ*/ 
			}
		}
	}
	
	/*��ý�����嵽�ͻ���*/ 
	if (mysql_stmt_store_result(m_pStatement))
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"mysql_stmt_store_result()");
#endif
		return false;
	}
	/*��ý������*/ 
	m_ulRowCount = mysql_stmt_num_rows(m_pStatement);	

	return true;
}

/*
**Ԥ������临λΪ���׼�����״̬
*/
bool	CMySQLStatement::reset	()
{
	if(!m_pStatement)
		return false;

	if(mysql_stmt_reset(m_pStatement) == 0)
		return true;

#ifdef _MySQL_throw
	throw CMySQLException(m_pStatement,0,"mysql_stmt_reset()");
#endif
	return false;
}

/*
**���Ӱ��/�ı�����
*/
my_ulonglong	CMySQLStatement::getRowAffected		()
{
	if(!m_pStatement)
		return 0;

	/*������Ԥ�������UPDATE��DELETE��INSERT�����ɾ������������Ŀ��*/ 
	return mysql_stmt_affected_rows(m_pStatement);
}

/*
**���AUTO_INCREMENT�����ɵ�ID
*/
my_ulonglong	CMySQLStatement::getInsertIncrement	()
{
	if(!m_pStatement)
		return 0;

	/*����Ԥ��������AUTO_INCREMENT�У��������ɵ�ID*/ 
	return mysql_stmt_insert_id(m_pStatement);
}

/*
**�󶨲���
*/
bool	CMySQLStatement::bindParams			()
{
	if(!m_pStatement)
		return false;

	/*�󶨻���*/ 
	if (mysql_stmt_bind_param(m_pStatement, m_pParamBind) == 0)
		return true;

#ifdef _MySQL_throw
	throw CMySQLException(m_pStatement,0,"mysql_stmt_bind_param()");
#endif
	return false;
}

/*
**���ͳ����ݵ�������
*/
bool	CMySQLStatement::sendLongData		(unsigned int parameter_number,const char*data,unsigned long length)
{
	if(!m_pStatement)
		return false;

	if(parameter_number >= m_ulParamCount)
		return false;

	if (mysql_stmt_send_long_data(m_pStatement,parameter_number,data,length) == 0)
		return true;

#ifdef _MySQL_throw
	throw CMySQLException(m_pStatement,0,"mysql_stmt_send_long_data()");
#endif
	return false;
}

/*
**�󶨽����
*/
bool	CMySQLStatement::bindResults			()
{
	if(!m_pStatement || !m_pResultBind)
		return false;

	return (mysql_stmt_bind_result(m_pStatement,m_pResultBind) == 0);
}

/*
**����������
*/
bool	CMySQLStatement::rowMore				()
{
	if(!m_pStatement)
		return false;

	int nRes = mysql_stmt_fetch(m_pStatement);
	if(nRes == MYSQL_NO_DATA)
		return false;

	if(nRes != 0 && mysql_stmt_errno(m_pStatement) != 0)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"mysql_stmt_fetch()");
#endif
		return false;
	}

	return true;
}

/*
**ȡ���ڼ�������
*/
bool	CMySQLStatement::dataSeek			(my_ulonglong offset)
{
	if(!m_pStatement)
		return false;

	mysql_stmt_data_seek(m_pStatement,offset);

	return rowMore();
}

/*
**�ӵ�ǰ������л�ȡ1��
*/
bool	CMySQLStatement::getColume			 (MYSQL_BIND *bind, unsigned int column, unsigned long offset)
{
	if(!m_pStatement || !bind)
		return false;

	int nRes = mysql_stmt_fetch_column(m_pStatement,bind,column,offset);
	if(nRes == 0)
		return true;

	if(nRes == CR_NO_DATA)
		return false;

#ifdef _MySQL_throw
	throw CMySQLException(m_pStatement,0,"mysql_stmt_fetch_column()����������!");/*CR_INVALID_PARAMETER_NO*/ 
#endif

	return false;
}

/*
**�󶨲���
*/
bool	CMySQLStatement::bindParam			(unsigned int uField,enum_field_types buffer_type,void*buffer,unsigned long length,my_bool*is_null)
{
	if (uField >= m_ulParamCount || !m_pStatement)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"CMySQLStatement::bindParam()");
#endif
		return false;
	}

	m_pParamLength[uField]				= length;

	m_pParamBind[uField].buffer_type	= buffer_type;
	m_pParamBind[uField].buffer			= buffer;
	m_pParamBind[uField].length			= &m_pParamLength[uField];
	m_pParamBind[uField].is_null		= is_null;		/*���ֵΪNULL���ñ���Ϊ���桱*/ 

	return true;
}

/*
**
*/
bool	CMySQLStatement::bindParam_Int			(unsigned int uField,enum_field_types buffer_type,my_bool is_unsigned,void*buffer,my_bool*is_null)
{
	if (uField >= m_ulParamCount || !m_pStatement)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"�󶨲������!");
#endif
		return false;
	}

	m_pParamBind[uField].buffer_type	= buffer_type;
	m_pParamBind[uField].is_unsigned	= is_unsigned;	/*�Ƿ�Ϊ�޷���*/ 
	m_pParamBind[uField].buffer			= buffer;
	m_pParamBind[uField].is_null		= is_null;		/*���ֵΪNULL���ñ���Ϊ���桱*/ 

	return true;
}


/*
**�󶨽��
*/
bool	CMySQLStatement::bindResult			(unsigned int uField,enum_field_types buffer_type,void*buffer,unsigned long buffer_length,my_bool*is_null)
{
	if (uField >= m_ulResultCount || !m_pStatement)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"�󶨽�����!");
#endif
		return false;
	}

	m_pResultBind[uField].buffer_type	= buffer_type;
	m_pResultBind[uField].buffer_length	= buffer_length;
	m_pResultBind[uField].buffer		= buffer;
	m_pResultBind[uField].is_null		= is_null;		/*���ֵΪNULL���ñ���Ϊ���桱*/ 

	return true;
}

/*
**�󶨽������
*/
bool	CMySQLStatement::bindResult_Int			(unsigned int uField,enum_field_types buffer_type,my_bool is_unsigned,void*buffer,my_bool*is_null)
{
	if (uField >= m_ulResultCount || !m_pStatement)
	{
#ifdef _MySQL_throw
		throw CMySQLException(m_pStatement,0,"CMySQLStatement::bindResult_Int()");
#endif
		return false;
	}

	m_pResultBind[uField].buffer_type	= buffer_type;
	m_pResultBind[uField].is_unsigned	= is_unsigned;	/*�Ƿ�Ϊ�޷���*/ 
	m_pResultBind[uField].buffer		= buffer;
	m_pResultBind[uField].is_null		= is_null;		/*���ֵΪNULL���ñ���Ϊ���桱*/ 

	return true;
}
