/**	@file       mian.cpp
 *	@note       Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *	@brief		 
 *
 *	@author     lipengfei
 *	@date       2018/05/10
 *	@note       历史记录：
 *	@note       V1.0.0  
 *	@warning	
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "OSSocket.h"
#include "JsonParse.h"
#include "CmdParse.h"
#include "AntColonyAlgorithm.h"
#include <stdexcept>
#include <iostream>
#include <vector>

#define MAX_SOCKET_BUFFER       (1024 * 1024 * 4)       /// 发送接受数据最大4M
//extern int nNumofFightPlane;


/** @fn     int RecvJuderData(OS_SOCKET hSocket, char *pBuffer)
 *  @brief	接受数据
 *	@param  -I   - OS_SOCKET hSocket
 *	@param  -I   - char * pBuffer
 *	@return int
 */
int RecvJuderData(OS_SOCKET hSocket, char *pBuffer)
{
    int         nRecvLen = 0;
    int         nLen = 0;

    while (1)
    {
        // 接受头部长度
        nLen = OSRecv(hSocket, pBuffer + nRecvLen, MAX_SOCKET_BUFFER);
        if (nLen <= 0)
        {
            printf("recv error\n");
            return nLen;
        }

        nRecvLen += nLen;

        if (nRecvLen >= SOCKET_HEAD_LEN)
        {
            break;
        }
    }

    int         nJsonLen = 0;
    char        szLen[10] = { 0 };

    memcpy(szLen, pBuffer, SOCKET_HEAD_LEN);

    nJsonLen = atoi(szLen);

    while (nRecvLen < (SOCKET_HEAD_LEN + nJsonLen))
    {
        // 说明数据还没接受完
		#ifdef OS_WINDOWS
			Sleep(300);
		#endif
        nLen = OSRecv(hSocket, pBuffer + nRecvLen, MAX_SOCKET_BUFFER);
        if (nLen <= 0)
        {
            printf("recv error\n");
            return nLen;
        }

        nRecvLen += nLen;
    }

    return 0;
}

/** @fn     int SendJuderData(OS_SOCKET hSocket, char *pBuffer, int nLen)
 *  @brief	发送数据
 *	@param  -I   - OS_SOCKET hSocket
 *	@param  -I   - char * pBuffer
 *	@param  -I   - int nLen
 *	@return int
 */
int SendJuderData(OS_SOCKET hSocket, char *pBuffer, int nLen)
{
    int     nSendLen = 0;
    int     nLenTmp = 0;

    while (nSendLen < nLen)
    {
        nLenTmp = OSSend(hSocket, pBuffer + nSendLen, nLen - nSendLen);
        if (nLenTmp < 0)
        {
            return -1;
        }

        nSendLen += nLenTmp;
    }

    return 0;
}
int main(int argc, char *argv[])
{
#ifdef OS_WINDOWS
    // windows下，需要进行初始化操作
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
        return false;
    }

#endif

    char        szIp[64] = { 0 };
    int         nPort = 0;
    char        szToken[128] = { 0 };
    int         nRet = 0;
    OS_SOCKET   hSocket;
    char        *pRecvBuffer = NULL;
    char        *pSendBuffer = NULL;
    int         nLen = 0;
   
        // 本地调试去掉这个
    if (argc != 4)
    {
        printf("error arg num\n");
        return -1;
    }

    // 解析参数
    strcpy(szIp, argv[1]);
    nPort = atoi(argv[2]);
    strcpy(szToken, argv[3]);

  //   strcpy(szIp, "47.95.243.246");
	 //nPort = 32521;
  //   strcpy(szToken, "1440ec23-a828-417f-b547-4e7fa77a21c7");

    printf("server ip %s, prot %d, token %s\n", szIp, nPort, szToken);

    // 开始连接服务器
    nRet = OSCreateSocket(szIp, (unsigned int)nPort, &hSocket);
    if (nRet != 0)
    {
        printf("connect server error\n");
        return nRet;
    }

    // 分配接受发送内存
    pRecvBuffer = (char*)malloc(MAX_SOCKET_BUFFER);
    if (pRecvBuffer == NULL)
    {
        return -1;
    }

    pSendBuffer = (char*)malloc(MAX_SOCKET_BUFFER);
    if (pSendBuffer == NULL)
    {
        free(pRecvBuffer);
        return -1;
    }

    memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);

    // 接受数据  连接成功后，Judger会返回一条消息：
    nRet = RecvJuderData(hSocket, pRecvBuffer);
    if (nRet != 0)
    {
        return nRet;
    }

    // json 解析
    // 获取头部
    CONNECT_NOTICE  stNotice;

    nRet = ParserConnect(pRecvBuffer + SOCKET_HEAD_LEN, &stNotice);
    if (nRet != 0)
    {
        return nRet;
    }

    // 生成表明身份的json
    TOKEN_INFO  stToken;

    strcpy(stToken.szToken, szToken);  // 如果是调试阶段，请输入你调试的token，在我的对战中获取，
                                       // 实际比赛，不要传入调试的，按照demo写的，有服务器调用传入。
    strcpy(stToken.szAction, "sendtoken");

    memset(pSendBuffer, 0, MAX_SOCKET_BUFFER);
    nRet = CreateTokenInfo(&stToken, pSendBuffer, &nLen);
    if (nRet != 0)
    {
        return nRet;
    }
    // 选手向裁判服务器表明身份(Player -> Judger)
    nRet = SendJuderData(hSocket, pSendBuffer, nLen);
    if (nRet != 0)
    {
        return nRet;
    }

    //身份验证结果(Judger -> Player)　
    memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);
    nRet = RecvJuderData(hSocket, pRecvBuffer);
    if (nRet != 0)
    {
        return nRet;
    }

    // 解析验证结果的json
    TOKEN_RESULT      stResult;
    nRet = ParserTokenResult(pRecvBuffer + SOCKET_HEAD_LEN, &stResult);
    if (nRet != 0)
    {
        return 0;
    }

    // 是否验证成功
    if (stResult.nResult != 0)
    {
        printf("token check error\n");
        return -1;
    }
    // 选手向裁判服务器表明自己已准备就绪(Player -> Judger)
    READY_PARAM     stReady;

    strcpy(stReady.szToken, szToken);
    strcpy(stReady.szAction, "ready");

    memset(pSendBuffer, 0, MAX_SOCKET_BUFFER);
    nRet = CreateReadyParam(&stReady, pSendBuffer, &nLen);
    if (nRet != 0)
    {
        return nRet;
    }
    nRet = SendJuderData(hSocket, pSendBuffer, nLen);
    if (nRet != 0)
    {
        return nRet;
    }

    //对战开始通知(Judger -> Player)　 
    memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);
    nRet = RecvJuderData(hSocket, pRecvBuffer);
    if (nRet != 0)
    {
        return nRet;
    }
    // 解析数据
    //Mapinfo 结构体可能很大，不太适合放在栈中，可以定义为全局或者内存分配
    MAP_INFO            *pstMapInfo;
    MATCH_STATUS        *pstMatchStatus;
    FLAY_PLANE          *pstFlayPlane;

    pstMapInfo = (MAP_INFO *)malloc(sizeof(MAP_INFO));
    if (pstMapInfo == NULL)
    {
        return -1;
    }

    pstMatchStatus = (MATCH_STATUS *)malloc(sizeof(MATCH_STATUS));
    if (pstMapInfo == NULL)
    {
        return -1;
    }

    pstFlayPlane = (FLAY_PLANE *)malloc(sizeof(FLAY_PLANE));
    if (pstFlayPlane == NULL)
    {
        return -1;
    }

    memset(pstMapInfo, 0, sizeof(MAP_INFO));
    memset(pstMatchStatus, 0, sizeof(MATCH_STATUS));
    memset(pstFlayPlane, 0, sizeof(FLAY_PLANE));

    nRet = ParserMapInfo(pRecvBuffer + SOCKET_HEAD_LEN, pstMapInfo);
    if (nRet != 0)
    {
        return nRet;
    }

    // 第一次把无人机的初始赋值给flayplane
    pstFlayPlane->nPurchaseNum = 0;
    pstFlayPlane->nUavNum = pstMapInfo->nUavNum;
    for (int i = 0; i < pstMapInfo->nUavNum; i++)
    {
        pstFlayPlane->astUav[i] = pstMapInfo->astUav[i];
    }

	int *pMap = (int *)malloc(sizeof(int)*(pstMapInfo->nMapX)*(pstMapInfo->nMapY));
	std::vector<int> *listofweight = new std::vector<int>[1];// 购买用，找到未分配的货物
	memset(pMap, 0, sizeof(int)*(pstMapInfo->nMapX)*(pstMapInfo->nMapY));
	JSONMap22Darray(pMap, pstMapInfo);
	sPlaneStatus *sPlaneStatus1 = new sPlaneStatus;		
	//int *pgoods = (int *)malloc(sizeof(int)*pstMatchStatus->nGoodsNum * 4);// 存储货物与匹配信息 | # | weight | value | is allocated? | 
	PlaneStatusinit(sPlaneStatus1,pstFlayPlane,pstMapInfo);//地图给的无人机种类可能不同
	//GoodsStatusinit(pgoods, pstMapInfo);
    // 根据服务器指令，不停的接受发送数据
    while (1)
    {
        // 进行当前时刻的数据计算, 填充飞行计划结构体，注意：0时刻不能进行移动，即第一次进入该循环时
        if (pstMatchStatus->nTime != 0)
        {
			if (pstMatchStatus->nTime == 83)
			{
				int aaa = 0;
			}
			if (sPlaneStatus1->nEnemyParkingX == -1)
			{
				sPlaneStatus1->nEnemyParkingX = pstMatchStatus->astEnemyUav[0].nX;
				sPlaneStatus1->nEnemyParkingY = pstMatchStatus->astEnemyUav[0].nY;
			}
			//检查无人机是否坠毁,若坠毁删除相应飞机的状态节点
			// 第一次把无人机的初始赋值给flayplane
			FLAY_PLANE *pstFlayPlaneLast = new FLAY_PLANE;
 			memcpy(pstFlayPlaneLast, pstFlayPlane, sizeof(FLAY_PLANE));
			PlaneCheck(sPlaneStatus1, pstFlayPlane, pstMatchStatus, pstMapInfo);//不删除，最后再删除,最后删掉上次购买的东西//每次出发前加上这次充电的电量
			GoodsAllocation(sPlaneStatus1, pstFlayPlane, pstMatchStatus, pstMapInfo, listofweight);
			AlgorithmCalculationFun(sPlaneStatus1, pMap, pstMapInfo, pstMatchStatus, pstFlayPlane);
			DeletePlane(pstFlayPlane);
			PurchasePlane(pstFlayPlane, pstMapInfo, pstMatchStatus, listofweight);

			//增加防撞逻辑，策略为在路线上相撞，价值小的抬起，让价值大的先走，也就是在ssPlaneStatus1相应链表中增加
			//两个节点 抬起 和 下降
			CollisionDectection(pstFlayPlane, pstFlayPlaneLast, pMap, pstMapInfo, pstMatchStatus, sPlaneStatus1);
			//std::cout << pstMatchStatus->nTime << std::endl;

        }

        
        //发送飞行计划结构体
        memset(pSendBuffer, 0, MAX_SOCKET_BUFFER);
        nRet = CreateFlayPlane(pstFlayPlane, szToken, pSendBuffer, &nLen);
        if (nRet != 0)
        {
            return nRet;
        }
        nRet = SendJuderData(hSocket, pSendBuffer, nLen);
        if (nRet != 0)
        {
            return nRet;
        }

        printf("%s\n", pSendBuffer);
		//std::cout << pstMatchStatus->nTime << std::endl;
        // 接受当前比赛状态
        memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);
        nRet = RecvJuderData(hSocket, pRecvBuffer);
        if (nRet != 0)
        {
            return nRet;
        }

        // 解析
        nRet = ParserMatchStatus(pRecvBuffer + SOCKET_HEAD_LEN, pstMatchStatus);
        if (nRet != 0)
        {
            return nRet;
        }

        if (pstMatchStatus->nMacthStatus == 1)
        {
            // 比赛结束
            printf("game over, we value %d, enemy value %d\n", pstMatchStatus->nWeValue, pstMatchStatus->nEnemyValue);
			//std::cout << pstMatchStatus->nTime << std::endl;
			return 0;
        }
    }
	freememory(sPlaneStatus1);
    // 关闭socket
    OSCloseSocket(hSocket);
    // 资源回收
    free(pRecvBuffer);
    free(pSendBuffer);
    free(pstMapInfo);
    free(pstMatchStatus);
    free(pstFlayPlane);

    return 0;
}