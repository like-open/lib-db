/*------------- stringFunctions.h
* Copyright (C): 2016 Lake
* Author       : Lake.Liu
* Version      : V1.01
*/ 
/***************************************************************
* �ַ�������
***************************************************************/
#pragma once
#include "basic/basicTypes.h"
#include <memory.h>
#include <string>
#include <stdarg.h>
/*************************************************************/
extern void		set_locale(pc_str _locale);
extern pc_str	get_locale();
//------------------------------------------------------
//------------------------------ String Basic Functions
//--- �����ַ�������
extern uint32	dStrlen(const char *s);

//--- �ַ�������
#ifdef WIN32
extern int32	dStrcat(char *dst,uint32 _size, const char *src);
#else // WIN32
extern char*	dStrcat(char *dst,uint32 _size, const char *src);
#endif // WIN32

//--- �ַ�������(_count:����������)
#ifdef WIN32
extern int32	dStrncat(char *dst,uint32 _size, const char *src,uint32 _count);
#else // WIN32
extern char*	dStrncat(char *dst,uint32 _size, const char *src,uint32 _count);
#endif // WIN32

//--- �ַ�������
#ifdef WIN32
extern int32	dStrcpy(char *dst,uint32 _size, const char *src);
#else // WIN32
extern char*	dStrcpy(char *dst,uint32 _size, const char *src);
#endif // WIN32

//--- �ַ�������(len:������)
#ifdef WIN32
extern int32	dStrncpy(char *dst,uint32 _size, const char *src, uint32 len);
#else // WIN32
extern char*	dStrncpy(char *dst,uint32 _size, const char *src, uint32 len);
#endif // WIN32

//--- �����ַ���
extern	int32	dStrcpyMax(char *dst,uint32 _size, const char *src, uint32 len);

//--- ��buf��ָ�ڴ������ǰcount���ֽڲ����ַ�chλ��
extern void*	dMemchr(void *buf,char ch,uint32 count);

//--- �ַ����в��ҵ�һ�γ���c��λ��
extern char*	dStrchr(char *s,char c);
extern pc_str 	dStrchr(const char *s,char c);

//--- �ֽ��ַ���Ϊһ���Ǵ�
extern char*	dStrtok(char *s, const char *delim);

//--- ���ַ���ת��ΪСд��ʽ
extern char*	dStrlwr(char *s);

//--- ���ַ���ת��Ϊ��д��ʽ
extern char*	dStrupr(char *s);

//--- �Ƿ���ֽ�
extern int32	ismbblead(char c);

//--- �ַ����Ƚ�(���ִ�Сд)
extern int		dStrncmp(const char *s1,const char * s2,int n);

//--- �ַ����Ƚ�(�����ִ�Сд)
extern int		dStrnicmp(const char *s1,const char * s2,int n);

//--- �ַ������ҵ�һ�γ��ֵ�λ��(���ִ�Сд)
extern char*	dStrstr(char *haystack, char *needle);

//--- �ַ������ҵ�һ�γ��ֵ�λ��(�����ִ�Сд)
extern char*	dStristr(char *haystack, char *needle);

//--- �ַ�����ʽ��
extern int		dSprintf(char *string,size_t sizeInBytes,const char *format, ... );
extern int		dVsprintf(char *string,size_t sizeInBytes,const char *format,va_list _Args);
//--- �ַ�����ӡ
extern int		dPrintf(const char *format, ... );
extern int		dVprintf(const char *format,va_list _Args);
//------------------------------------------------------
//------------------------------ String Types
//--- ���Ҳ�����ֽ��Ƿ�Ϸ�
extern bool		testMultibyte(char* pStr);

//--- ȥ��ָ���ַ�
extern void		wipeOffChar(char* pStr,uint32 uLen,char cChar);

//--- ����ȫ��ǿո�
extern void		filtrationBlank(char* pStr,uint32 uLen);

//--- �Ƿ��������ַ���
extern bool		numeralString(const char* pStr,int32 nLen);

//--- �Ƿ�ӵ���ַ�
extern bool		haveFromString		(std::string&strString,char c);
//--- ����ַ�����
extern void		getSubString		(std::string&strString,std::string&strSubString,char c = ';');
//--- �ض��ַ���
extern void		truncateString		(std::string&strString,char c = ';');
//--- ����ַ�����
extern void		truncateString		(std::string&strString,std::string&strSub,std::string::size_type&_off,std::string::size_type _count);
//--- ��ȡ����(����0Ϊδ��ȡ��)
extern int64	readInt64FromString	(std::string&strString,char c = ';');
//--- ��ȡ����ʱ��(����0Ϊδ��ȡ��)
extern uint64	readDateTimeFromString(std::string&strString);

//--- ������ַ���
extern bool		getSubString		(std::string&strString,std::string&strSubString,const char*pStrtok);


//------------------------------------------------------
//------------------------------ ANSI��UTF8��ת��
//--- ANSIת��UTF8
extern bool	convertANSItoUTF8(char* pAnsiString,int32 nLen);
//--- UTF8תANSI
extern bool	convertUTF8toANSI(char* pUtf8String,int32 nLen);
//--- ANSIת��UTF8
extern char*_convertANSItoUTF8(const char* pAnsiString,int32 nLen);
//--- UTF8תANSI
extern char*_convertUTF8toANSI(const char* pUtf8String,int32 nLen);
//--- ת��Ϊ���ֽ�
extern std::wstring&convertToWString(const char *str);
//--- ���ַ������ַ�ת��
extern std::string&convertToCString(std::wstring&str);