CMAKE_MINIMUM_REQUIRED (VERSION 2.8.3)

##############################################################
### ���б�
##############################################################
SET(_Library_List 
	dbMysql
	dbSqlite3
	)

##############################################################
### װ������
##############################################################
# ����
foreach(_lib_ ${_Library_List})
	INCLUDE(${CMAKE_CURRENT_LIST_DIR}/${_lib_}/file_list.cmake)
endforeach(_lib_)
