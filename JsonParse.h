/**	@file       JsonParse.h
 *	@note       Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *	@brief		json����
 *
 *	@author     lipengfei
 *	@date       2015/12/09
 *	@note       ��ʷ��¼��
 *	@note       V1.0.0  
 *	@warning	
 */
#ifndef __JSONPARSE_H__
#define __JSONPARSE_H__

#include "stdio.h"
#include "stdlib.h"
#include "cJSON.h"


/** @fn     int JSONGetValue(const cJSON *pJson, const char *szJsonName, int *pValue, int nNullValue )
 *  @brief	��ȡ�ڵ�ֵ
 *	@param  -I   - const cJSON * pJson          ���ڵ�
 *	@param  -I   - const char * szJsonName      �ڵ�����
 *	@param  -I   - int * pValue                 �ڵ�ֵ
 *	@param  -I   - int nNullValue               Ĭ��ֵ
 *	@return int                                 0��ʾ�ɹ�������ʧ��
 */
int JSONGetValue(cJSON *pJson, const char *szJsonName, bool bString, int *pValue, int nNullValue = 0);

/** @fn     int JSONGetValue(const cJSON *pJson, const char *szJsonName, char *szValue, int nLen)
 *  @brief	��ȡ�ڵ�ֵ
 *	@param  -I   - const cJSON * pJson      ���ڵ�
 *	@param  -I   - const char * szJsonName  �ڵ�����
 *	@param  -I   - char * szValue           Ĭ��ֵ
 *	@param  -I   - int nLen                 ����
 *	@return int                             0��ʾ�ɹ�������ʧ��
 */
int JSONGetValue(cJSON *pJson, const char *szJsonName, char *szValue, int nLen);

/** @fn     int JSONGetValue(const cJSON *pJson, const char *szJsonName, float *pValue, float fNullValue )
 *  @brief	��ȡ�ڵ�ֵ
 *	@param  -I   - const cJSON * pJson      ���ڵ�
 *	@param  -I   - const char * szJsonName  �ڵ�����
 *	@param  -I   - float * pValue           ֵ
 *	@param  -I   - float fNullValue         Ĭ��ֵ
 *	@return int                             0��ʾ�ɹ�������ʧ��
 */
int JSONGetValue(cJSON *pJson, const char *szJsonName, bool bString,  double *pValue, double fNullValue = 0.0);



/** @fn     int JSONSetValue(cJSON *pJson, const char *szJsonName, char *szValue)
 *  @brief	
 *	@param  -I   - cJSON * pJson
 *	@param  -I   - const char * szJsonName
 *	@param  -I   - char * szValue
 *	@return int
 */
int JSONSetValue(cJSON *pJson, const char *szJsonName, char *szValue);
#endif
