/**	@file       JsonParse.cpp
 *	@note       Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *	@brief		 
 *
 *	@author     lipengfei
 *	@date       2015/12/09
 *	@note       ��ʷ��¼��
 *	@note       V1.0.0  
 *	@warning	
 */


#include "JsonParse.h"
#include <string.h>


/** @fn     int JSONGetValue(const cJSON *pJson, const char *szJsonName, int *pValue, int nNullValue )
 *  @brief	��ȡ�ڵ�ֵ
 *	@param  -I   - const cJSON * pJson          ���ڵ�
 *	@param  -I   - const char * szJsonName      �ڵ�����
 *	@param  -I   - int * pValue                 �ڵ�ֵ
 *	@param  -I   - int nNullValue               Ĭ��ֵ
 *	@return int                                 0��ʾ�ɹ�������ʧ��
 */
int JSONGetValue(cJSON *pJson, const char *szJsonName, bool bString, int *pValue, int nNullValue)
{
    if (pJson       == NULL ||
        szJsonName  == NULL ||
        pValue      == NULL)
    {
        return -1;
    }

    cJSON       *pJsonValue = cJSON_GetObjectItem(pJson, szJsonName);
    
    *pValue = nNullValue;

    if (pJsonValue != NULL)
    {
        if (bString == true)
        {
            *pValue = atoi(pJsonValue->valuestring);
        }
        else
        {
            *pValue = pJsonValue->valueint;
        }
    }
    else
    {
        return -1;
    }

    return 0;

}

/** @fn     int JSONGetValue(const cJSON *pJson, const char *szJsonName, char *szValue, int nLen)
 *  @brief	��ȡ�ڵ�ֵ
 *	@param  -I   - const cJSON * pJson      ���ڵ�
 *	@param  -I   - const char * szJsonName  �ڵ�����
 *	@param  -I   - char * szValue           Ĭ��ֵ
 *	@param  -I   - int nLen                 ����
 *	@return int                             0��ʾ�ɹ�������ʧ��
 */
int JSONGetValue(cJSON *pJson, const char *szJsonName, char *szValue, int nLen)
{
    if (pJson      == NULL ||
        szJsonName == NULL ||
        szValue    == NULL)
    {
        return -1;
    }

    cJSON   *pJsonValue = cJSON_GetObjectItem(pJson, szJsonName);

    memset(szValue, 0, nLen);

    if (pJsonValue != NULL)
    {
        strcpy(szValue, pJsonValue->valuestring);
    }
    else
    {
        return -1;
    }


    return 0;
}

/** @fn     int JSONGetValue(const cJSON *pJson, const char *szJsonName, float *pValue, float fNullValue )
 *  @brief	��ȡ�ڵ�ֵ
 *	@param  -I   - const cJSON * pJson      ���ڵ�
 *	@param  -I   - const char * szJsonName  �ڵ�����
 *	@param  -I   - float * pValue           ֵ
 *	@param  -I   - float fNullValue         Ĭ��ֵ
 *	@return int                             0��ʾ�ɹ�������ʧ��
 */
int JSONGetValue(cJSON *pJson, const char *szJsonName, bool bString, double *pValue, double fNullValue)
{
    if (pJson      == NULL ||
        szJsonName == NULL ||
        pValue     == NULL)
    {
        return -1;
    }

    cJSON       *pJsonValue = cJSON_GetObjectItem(pJson, szJsonName);

    *pValue = fNullValue;

    if (pJsonValue != NULL)
    {
        if (bString == true)
        {
            *pValue = atof(pJsonValue->valuestring);
        }
        else
        {
            *pValue = pJsonValue->valueint;
        }
    }
    else
    {
        return -1;
    }


    return 0;
}

/** @fn     int JSONSetValue(cJSON *pJson, const char *szJsonName, char *szValue)
*  @brief
*	@param  -I   - cJSON * pJson
*	@param  -I   - const char * szJsonName
*	@param  -I   - char * szValue
*	@return int
*/
int JSONSetValue(cJSON *pJson, const char *szJsonName, char *szValue)
{
    cJSON       *pJsonValue = cJSON_CreateString(szValue);

    cJSON_AddItemToObject(pJson, szJsonName, pJsonValue);

    return 0;
}