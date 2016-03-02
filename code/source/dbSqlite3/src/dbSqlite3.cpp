/*----------------- dbSqlite3.cpp
*
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.0
*--------------------------------------------------------------
*
*------------------------------------------------------------*/
#include "dbSqlite3.h"
#include <memory.h>
#include <vector>
#include <string>
/*************************************************************/
//-------------------------------------------------------------
//------------------------------ 
CSqlite3::CSqlite3()
{
	//m_pSQL			= NULL;
	m_pConnection	= NULL;
	memset(m_szSQL,0,sizeof(m_szSQL));
	memset(m_szDBName,0,sizeof(m_szDBName));

	/*二进制读写*/
	m_pBuf			= NULL;
	m_bEncoded		= false;
	m_nBinaryLen	= 0;
	m_nBufferLen	= 0;
	m_nEncodedLen	= 0;
}
//-------------------------------------------------------------
//------------------------------ 
CSqlite3::~CSqlite3()
{
	close();
	/*二进制读写*/
	clear();
}

//-------------------------------------------------------------
//------------------------------ 打开数据库(内存数据库":memory:")
bool	CSqlite3::open(const char* pDBName)
{
	if (!pDBName || !*pDBName)
		return false;

	close();
#ifdef WIN32
	sprintf_s(m_szDBName,sizeof(m_szDBName),pDBName);
#else//WIN32
	sprintf(m_szDBName,pDBName);
#endif//WIN32

	int	nCode = sqlite3_open(pDBName, &m_pConnection);
	if (nCode == SQLITE_OK)
		return true;

#ifdef	sqlite_throw
	throw CSqlite3Exception(m_pConnection,nCode);
#endif	/*sqlite_throw*/ 

	return false;
}
//-------------------------------------------------------------
//------------------------------ 打开数据库(内存数据库":memory:")
bool	CSqlite3::open16(const void* pDBName)
{
	if (!pDBName)
		return false;

	close();
	int	nCode = sqlite3_open16(pDBName, &m_pConnection);
	if (nCode == SQLITE_OK)
		return true;

#ifdef	sqlite_throw
	throw CSqlite3Exception(m_pConnection,nCode);
#endif	/*sqlite_throw*/ 

	return false;
}
//-------------------------------------------------------------
//------------------------------ 关闭数据库
void	CSqlite3::close()
{
	if (m_pConnection)
		sqlite3_close(m_pConnection);
	m_pConnection	= NULL;

	clearSQL();
	memset(m_szDBName,0,sizeof(m_szDBName));
}

//-------------------------------------------------------------
//------------------------------ 数据库复制(覆盖)
bool	CSqlite3::copy		(CSqlite3&clSqlite)
{
	if(!isOpen() || !clSqlite.isOpen())
		return false;

	std::vector<std::string>	vecTable;
	//1 检查表(把已有表删除并重建)
	clSqlite.cmd("SELECT name,sql FROM sqlite_master WHERE type = 'table'");
	CSqlite3Query clQuery = clSqlite.query();
	for(;!clQuery.isEof();clQuery.nextRow())
	{
		const unsigned char*pTableName = clQuery.getString(0);
		if(!pTableName)
			continue;

		//清理原数据表
		if(this->tableExists((const char*)pTableName))
		{
			this->cmd("DROP TABLE %s",(const char*)pTableName);
			if (this->exec() == -1)
				return false;
		}
		//添加新数据
		this->cmd("%s",(const char*)clQuery.getString(1));
		if (this->exec() == -1)
			return false;

		vecTable.push_back((const char*)pTableName);
	}

	//2 
	for (unsigned long i = 0;i < vecTable.size();++i)
	{
		std::string&strTable = vecTable[i];
		if(strTable.empty())
			continue;

		CSqlite3Transaction clTransaction	= this->getTransaction();
		clTransaction.begin();

		clSqlite.cmd("SELECT * FROM %s",strTable.c_str());
		CSqlite3Query clQuery = clSqlite.query();
		for(;!clQuery.isEof();clQuery.nextRow())
		{
			if(clQuery.getColumn() <= 0)
				continue;

			static char szSQL[10 * 1048] = {0};
			memset(szSQL,0,sizeof(szSQL));
#ifdef WIN32
			sprintf_s(szSQL,sizeof(szSQL)
#else//WIN32
			sprintf(szSQL
#endif//WIN32
			,"INSERT INTO %s VALUES (",strTable.c_str());

			bool bFirst = true;
			for (int j = 0;j < clQuery.getColumn();++j)
			{
#ifdef WIN32
				sprintf_s(szSQL,sizeof(szSQL)
#else//WIN32
				sprintf(szSQL
#endif//WIN32
				,"%s%s?",szSQL,bFirst ? "" : ",");
				bFirst = false;
			}
#ifdef WIN32
			sprintf_s(szSQL,sizeof(szSQL)
#else//WIN32
			sprintf(szSQL
#endif//WIN32
				,"%s);",szSQL);

			this->cmd(szSQL);
			CSqlite3Statement clStatement = this->compileStatement();
			for (int j = 0;j < clQuery.getColumn();++j)
			{
				int nType = clQuery.getType(j);
				switch(nType)
				{
				case SQLITE_INTEGER://整数
					clStatement.bind_int64(j + 1,clQuery.getInt64(j));
					break;
				case SQLITE_FLOAT:	//浮点
					clStatement.bind_double(j + 1,clQuery.getDouble(j));
					break;
				case SQLITE_TEXT:	//文本
					clStatement.bind_text(j + 1,(const char*)clQuery.getString(j));
					break;
				case SQLITE_BLOB:	//二进制
				case SQLITE_NULL:	//自定义
					clStatement.bind_blob(j + 1,clQuery.getBlob(j),clQuery.getBytes(j));
					break;
				}
			}
			int nRet = clStatement.exec();
			if(nRet <= 0)
				return false;
		}
		clTransaction.commit();
	}

	return true;
}

//-------------------------------------------------------------
//------------------------------ 设置超时时间(毫秒)
void	CSqlite3::setBusyTimeout(int nMillisecs)
{
	sqlite3_busy_timeout(m_pConnection, nMillisecs);
}
//-------------------------------------------------------------
//------------------------------ 表是否存在
bool	CSqlite3::tableExists(const char* pTableName)
{
	if (!pTableName)
		return false;

	cmd("select count(*) from sqlite_master where type='table' and name='%s'",pTableName);

	CSqlite3Query	quit = query();
	return (!quit.isEof() && quit.getColumn() && quit.getInt(0) > 0);
}

//-------------------------------------------------------------
//------------------------------ 
void	CSqlite3::clearSQL()
{
	/*if (m_pSQL)
	sqlite3_free(m_pSQL);
	m_pSQL	= NULL;*/

	memset(m_szSQL,0,sizeof(m_szSQL));
}

//-------------------------------------------------------------
//------------------------------ 格式化SQL语句(二进制用%Q格式化)
void	CSqlite3::cmd(const char* pCmd,...)
{
	if (!pCmd)
		return;

	va_list	argptr;
	va_start(argptr,pCmd);
#ifdef WIN32
	vsprintf_s(m_szSQL,sizeof(m_szSQL),pCmd,argptr);
#else//WIN32
	vsprintf(m_szSQL,pCmd,argptr);
#endif//WIN32
	//m_pSQL = sqlite3_vmprintf(pCmd, argptr);
	va_end(argptr);
}

//-------------------------------------------------------------
//------------------------------ 执行语句(返回影响行数字)
int	CSqlite3::exec()
{
	char* pError = NULL;
	int nCode = sqlite3_exec(m_pConnection,
		m_szSQL,
		//m_pSQL,
		NULL,		/*callback函数(每条数据调用一次)*/ /*int (*callback)(void*,int[列数],char**[列数据],char**[列名])*/ 
		NULL,		/*callback函数*/ 
		&pError);	/*出错信息*/ 
	/*执行行清空,避免重复执行*/ 
	clearSQL();

	if (nCode == SQLITE_OK)
		return sqlite3_changes(m_pConnection);/*返回影响行数*/ 

#ifdef	sqlite_throw
	throw CSqlite3Exception(m_pConnection,nCode,pError);
#endif	/*sqlite_throw*/ 
	//nRet = sqlite3_reset(mpVM);

	return -1;
}

//-------------------------------------------------------------
//------------------------------ 查询
CSqlite3Query	CSqlite3::query()
{
	sqlite3_stmt*	pVM;
	const char*		pzTail=NULL;
	int nRet = sqlite3_prepare(m_pConnection, m_szSQL/*m_pSQL*/, -1, &pVM, &pzTail);
	clearSQL();

	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pConnection,nRet,pzTail);
#endif	/*sqlite_throw*/
	}

	return CSqlite3Query(pVM);
}

//-------------------------------------------------------------
//------------------------------ 获得表
CSqlite3Table	CSqlite3::getTable()
{
	char*	pError		= NULL;
	char**	ppszResults	= NULL;
	int		nRows		= 0;
	int		nCols		= 0;
	int nRet = sqlite3_get_table(m_pConnection, m_szSQL/*m_pSQL*/, &ppszResults, &nRows, &nCols, &pError);
	clearSQL();
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pConnection,nRet,pError);
#endif	/*sqlite_throw*/
	}
	return CSqlite3Table(ppszResults, nRows, nCols);
}
//-------------------------------------------------------------
//------------------------------ 获得一个声明
CSqlite3Statement	CSqlite3::compileStatement()
{
	const char* pzTail=NULL;
	sqlite3_stmt* pVM = NULL;
	int nRet = sqlite3_prepare(m_pConnection, m_szSQL/*m_pSQL*/, -1, &pVM, &pzTail);
	clearSQL();
	if (nRet != SQLITE_OK)
	{
#ifdef	sqlite_throw
		throw CSqlite3Exception(m_pConnection,nRet,pzTail);
#endif	/*sqlite_throw*/
	}

	return CSqlite3Statement(m_pConnection,pVM);
}

//-------------------------------------------------------------
//------------------------------ 产生一个新事务处理
CSqlite3Transaction	CSqlite3::getTransaction()
{
	return CSqlite3Transaction(m_pConnection);
}
//-------------------------------------------------------------
//------------------------------ 最后插入行ID
long long	CSqlite3::lastInsertRowID()
{
	return sqlite3_last_insert_rowid(m_pConnection);
}

//-------------------------------------------------------------
//------------------------------ 
void CSqlite3::clear()
{
	if (m_pBuf)
	{
		m_nBinaryLen = 0;
		m_nBufferLen = 0;
		delete[] m_pBuf;
		m_pBuf = NULL;
	}
}

//-------------------------------------------------------------
//------------------------------ 
const unsigned char* CSqlite3::getEncoded(const unsigned char* pBuf, int nLen)
{
	if (pBuf && nLen)
	{
		clear();
		// Allow extra space for encoded binary as per comments in
		// SQLite encode.c See bottom of this file for implementation
		// of SQLite functions use 3 instead of 2 just to be sure ;-)
		m_nBinaryLen = nLen;
		m_nBufferLen = 3 + (257*nLen)/254;

		m_pBuf = new unsigned char[m_nBufferLen];
		if (!m_pBuf)
			return NULL;

		memcpy(m_pBuf, pBuf, nLen);
		m_bEncoded = false;		
	}

	if (!m_bEncoded)
	{
		unsigned char* ptmp = new unsigned char[m_nBinaryLen];
		memcpy(ptmp, m_pBuf, m_nBinaryLen);
		m_nEncodedLen = encode_binary(ptmp, m_nBinaryLen, m_pBuf,m_nBufferLen);
		delete[] ptmp;
		m_bEncoded = true;
	}

	return m_pBuf;
}

//-------------------------------------------------------------
//------------------------------ 
const unsigned char* CSqlite3::getBinary(const unsigned char* pBuf,int nLen)
{
	if (pBuf && nLen)
	{
		clear();
		m_nEncodedLen	= nLen;
		m_nBufferLen	= m_nEncodedLen + 1; /*Allow for NULL terminator*/ 
		m_pBuf = new unsigned char[m_nBufferLen];
		if (!m_pBuf)
		{
			return NULL;
		}
		memcpy(m_pBuf, pBuf, nLen);
		m_bEncoded = true;		
	}

	if (m_bEncoded)
	{
		/*in/out buffers can be the same*/ 
		m_nBinaryLen = decode_binary(m_pBuf,m_nBufferLen, m_pBuf,m_nBufferLen);

		if (m_nBinaryLen == 0)
		{
		}

		m_bEncoded = false;
	}

	return m_pBuf;
}

//-------------------------------------------------------------
//------------------------------ 
int CSqlite3::getBinaryLength()
{
	return m_nBinaryLen;
}

//-------------------------------------------------------------
//------------------------------ 二进制编码
unsigned int CSqlite3::encode_binary(const unsigned char *pIn,unsigned int uIn, unsigned char *pOut,unsigned int uOut)
{
	if (!pOut || uOut < 2)	/*输出不能为空或长度小于2*/ 
		return 0;

	/*memset(pOut, 0, uOut);*/ 
	if(!uIn)
	{
		pOut[0] = 'x';
		pOut[1] = 0;
		return 1;
	}

	if (!pIn || uOut < uIn + 1)/*输入不能为空输出长度不能小于输入长度*/ 
		return 0;

	int e = 0,	/*启始码*/ 
		m = 0;
	int cnt[256];
	memset(cnt, 0, sizeof(cnt));
	for(int i = uIn - 1; i >= 0; i--)
	{/*计算编码总值*/ 
		cnt[pIn[i]]++;
	}
	m = uIn;
	for(int i = 1; i < 256; i++)
	{
		int sum;
		if(i=='\'')
			continue;
		sum = cnt[i] + cnt[(i+1)&0xff] + cnt[(i+'\'')&0xff];
		if(sum < m)
		{
			m = sum;
			e = i;
			if(m == 0)
				break;
		}
	}
	pOut[0] = e;

	unsigned int uLen = 1;
	for(unsigned int i = 0; i < uIn; i++)
	{
		int c = (pIn[i] - e)&0xff;
		if(c == 0)
		{
			pOut[uLen++] = 1;
			pOut[uLen++] = 1;
		}
		else if(c == 1)
		{
			pOut[uLen++] = 1;
			pOut[uLen++] = 2;
		}
		else if(c == '\'')
		{
			pOut[uLen++] = 1;
			pOut[uLen++] = 3;
		}
		else
		{
			pOut[uLen++] = c;
		}
	}
	pOut[uLen] = 0;
	return uLen;
}

//-------------------------------------------------------------
//------------------------------ 二进制解码
unsigned int CSqlite3::decode_binary(const unsigned char *pIn,unsigned int uIn,unsigned char *pOut,unsigned int uOut)
{
	if (!pOut || !uOut)
		return 0;

	/*memset(pOut, 0, uOut);*/ 
	if (!pIn || !uIn)
		return 0;

	unsigned int uLen = 0;
	int c, e;
	e = *(pIn++);/*跳过第一个字节*/ 
	while((c = *(pIn++)) != 0 && uIn > uLen)
	{
		if (uLen >= uOut)/*长度不足时跳出*/ 
			break;

		if(c == 1)
		{
			c = *(pIn++);
			if( c==1 )
			{
				c = 0;
			}
			else if( c==2 )
			{
				c = 1;
			}
			else if( c==3 )
			{
				c = '\'';
			}
			else
			{
				return -1;
			}
		}
		pOut[uLen++] = (c + e) & 0xff;
	}
	return uLen;
}
