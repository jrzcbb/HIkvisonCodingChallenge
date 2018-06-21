/* 先不管动态内存释放问题，最后加一个head指针，循环释放即可*/
//extern int nNumofFightPlane;
#include "AntColonyAlgorithm.h"
int sPlaneStatus::nNumofFightPlane = 0;
int sPlaneStatus::nEnemyParkingX = -1;
int sPlaneStatus::nEnemyParkingY = -1;
int sPlaneStatus::nWePlaneatEnemyParking = 0;
//生成在指定区间start~end之间的随机整数
int Random(int start, int end) {
	int dis = end - start;
	return rand() % dis + start;
}
//删除antPosition链表
void freememoryAntPosition(antPosition *xy)
{
	antPosition *p = xy;
	while (p != NULL)
	{
		antPosition *q = p;
		p = p->next;
		delete q;
	}
	//throw std::logic_error("The method or operation is not implemented.");
}
//删除sAntPath链表
void freememoryAntPath(sAntPath * temp)
{
	sAntPath *p = temp;
	while (p != NULL)
	{
		freememoryAntPosition(temp->xy);
		p = temp->next;
		delete temp;
		temp = p;
	}
	//throw std::logic_error("The method or operation is not implemented.");
}
//删除sPlaneStatus链表
void freememory(sPlaneStatus * temp)
{
	while (temp != NULL)
	{
		freememoryAntPath(temp->AntPath);
		sPlaneStatus *p = temp;
		temp = temp->next;
	}	
	//throw std::logic_error("The method or operation is not implemented.");
}

//std::default_random_engine e(time(0));
//std::uniform_real_distribution<double> d(0, 1);
float aver(int *a, int l)//计算整型数组a，前l个元素的平均值。
{
	float r = 0;
	int i;
	for (i = 0; i < l; i++)
		r += a[i];//累加。
	r /= l;//平均值
	return r;//返回结果。
}
float sum(float *a, int l)//计算整型数组a，前l个元素的平均值。
{
	float r = 0;
	int i;
	for (i = 0; i < l; i++)
		r += a[i];//累加。
	return r;//返回结果。
}
void uniform(float *a, int l)//计算整型数组a，前l个元素的归一化。
{
	float r = sum(a,l);
	for (int i = 0; i < l; i++)
	{
		a[i] /= r;
	}
}
int findmax(float *Q, int param2)
{
	int loc = 0;
	float temp = Q[0];
	for (int i = 1; i < param2;i++)
	{
		loc = temp > Q[i] ? loc : i;
		temp = temp > Q[i] ? temp : Q[i];
	}
	return loc;
}
// 2018年5月24日22:53:34加入新功能：
// 找到每个飞机可以装载的重量
void PlaneStatusinit(sPlaneStatus *sPlaneStatus1, FLAY_PLANE *pstFlayPlane, MAP_INFO * pstMap)
{
	sPlaneStatus *p = sPlaneStatus1;
	for (int i = 0; i < pstFlayPlane->nUavNum; i++)
	{
		p->istakeoff = false;
		p->nID = pstFlayPlane->astUav[i].nNO;
		p->nGoodsStatus = -1;
		p->nisArrival = -1;
		p->nStatus = UAV_NOMAL;
		int type = FindPlanetype(pstFlayPlane->astUav[i], pstMap);
		p->remain_electricity = 0;
		p->capacity = pstMap->astUavPrice[type].capacity;
		p->charge = pstMap->astUavPrice[type].charge;
		p->nbirthTime = 1;
		if (i < pstFlayPlane->nUavNum-1)
		{
			p->next = new sPlaneStatus;
			p = p->next;
		}
	}
}
//删除毁坏飞机节点,插入新买节点
//应该根据match信息更新结构体
void PlaneCheck(sPlaneStatus * sPlaneStatus1, FLAY_PLANE * pstFlayPlane, MATCH_STATUS *pstMatchStatus, MAP_INFO *pstMap)
{
	pstFlayPlane->nUavNum = pstMatchStatus->nUavWeNum;
	for (int i = 0; i < pstMatchStatus->nUavWeNum; i++)
	{
		pstFlayPlane->astUav[i] = pstMatchStatus->astWeUav[i];
	}
	sPlaneStatus *p = sPlaneStatus1;
	for (int i = 0; i < pstFlayPlane->nUavNum; i ++)
	{
		if (pstFlayPlane->astUav[i].nStatus == UAV_CRASH)
		{
			p->nGoodsStatus = -1;
			p->nStatus = UAV_CRASH;
			if (p->isFightPlane)//删掉已坠毁的无人机
			{
				p->isFightPlane = false;
				p->nNumofFightPlane--;
				p->nAttackID = -1;
			}
		}
		//添加节点
		if (p == NULL && pstFlayPlane->astUav[i].nStatus == UAV_NOMAL)
		{
			sPlaneStatus *ptemp = sPlaneStatus1;
			while (ptemp->next != NULL)
			{
				ptemp = ptemp->next;
			}
			ptemp->next = new sPlaneStatus;
			ptemp = ptemp->next;
			//p = p->next;
			ptemp->nID = pstFlayPlane->astUav[i].nNO;
			ptemp->nGoodsStatus = -1;
			ptemp->nisArrival = -1;
			ptemp->nStatus = UAV_NOMAL;
			ptemp->istakeoff = false;
			ptemp->nbirthTime = pstMatchStatus->nTime;
			int type = FindPlanetype(pstMatchStatus->astWeUav[i], pstMap);
			ptemp->capacity = pstMap->astUavPrice[type].capacity;
			ptemp->charge = pstMap->astUavPrice[type].charge;
			ptemp->remain_electricity = 0;
			p = ptemp;
		}
		p = p->next;
	}
	pstFlayPlane->nPurchaseNum = 0;
	for (int i = 0; i < MAX_UAV_PRICE_NUM ;i ++)
	{
		pstFlayPlane->szPurchaseType[i][0] = '\0';
	}
}
//初始化货物状态矩阵，方便分配环节
void GoodsStatusinit(int * pgoods, MAP_INFO * pstMapInfo)
{

}
//初始化内存,内存中每个位置为param2
void Antmemset(float * pPheromone, float param2, int nLength)
{
	if (nLength > 2147483647)
	{
		std::cout << "数据过大" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < nLength;i++)
	{
		*(pPheromone + i) = param2;
	}
}
//将格式地图转化为数组
//地图的索引方式为（X,Y） = *(pMap + X*pstMapInfo->nMapY + Y)
void JSONMap22Darray(int * pMap, MAP_INFO * pstMapInfo)
{
	
	for (int i = 0; i < pstMapInfo->nBuildingNum; i++)
	{
		//Y坐标循环
		for (int k = pstMapInfo->astBuilding[i].nX; k <= pstMapInfo->astBuilding[i].nX + pstMapInfo->astBuilding[i].nL - 1; k++)
		{
			//X坐标循环
			for (int j = pstMapInfo->astBuilding[i].nY; j <= pstMapInfo->astBuilding[i].nY + pstMapInfo->astBuilding[i].nW - 1; j++)
			{
				*(pMap + k*pstMapInfo->nMapY + j) = pstMapInfo->astBuilding[i].nH;// -pstMap->nHLow;应该在搜索时限制高度
			}
		}
	}

	//打印地图做验证
#ifdef PRINT_2D_MAP
	*(pMap + 0 * pstMapInfo->nMapY + 16) = 999;
	*(pMap + 10 * pstMapInfo->nMapY + 0) = 999;
	for (int i = 0; i < pstMapInfo->nMapX; i++)
	{
		for (int j = 0; j < pstMapInfo->nMapY; j++)
		{
			std::cout << '\t' << *(pMap + i*20 + j);
		}
		std::cout << std::endl;
	}
	*(pMap + 0 * pstMapInfo->nMapY + 16) = 0;
	*(pMap + 10 * pstMapInfo->nMapY + 0) = 0;
#endif

}
//增加能力为规划从接货到送货的整条路径
void SearchAntPath(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO *pstMap)
{
	int astUavX = uav.nX, astUavY = uav.nY, astUavZ = uav.nZ;
	int astGoodsX = goods.nStartX, astGoodsY = goods.nStartY;
	// 为每个种群找一条路径
	sAntPath *p = AntPath; 
	if (p->xy != NULL)
	{
		freememoryAntPosition(p->xy);
	}
	p->xy = new antPosition;
	p->npathLenth = 0;
	antPosition *q = FindPath(p, pstMap,astUavX, astUavY, astUavZ, astGoodsX, astGoodsY, pMap, pPheromone);//q->应为空指针 
	//到达货物地点，降落取货
	int ntempx, ntempy, ntempz;
	while (q->z > 0)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z - 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//取到货物，起飞送货
	while (q->z < pstMap->nHLow)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z + 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//规划送货路径
	q->next = new antPosition; //2018年5月23日21:32:33新加入，可能是bug的关键
	astGoodsX = goods.nEndX; astGoodsY = goods.nEndY;
	q = FindPath(p, pstMap, q->x, q->y, q->z, astGoodsX, astGoodsY, pMap, pPheromone);
	//到达货物目的地，降落送货
	while (q->z > 0)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z - 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//已送货，重新起飞
	while (q->z <= pstMap->nHLow)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z + 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//加上最开始上升的一格
	//antPosition * ptemp = AntPath->xy;
	//antPosition * ptemp1 = new antPosition;
	//ptemp1->x = ptemp->x;
	//ptemp1->y = ptemp->y;
	//ptemp1->z = ptemp->z;
	//ptemp1->next = ptemp;
	//AntPath->xy = ptemp1;
}
//搜索单条轨迹
void SearchAntPathPart(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO * pstMap)
{
	int astUavX = uav.nX, astUavY = uav.nY, astUavZ = uav.nZ;
	int astGoodsX = goods.nStartX, astGoodsY = goods.nStartY;
	// 为每个种群找一条路径
	sAntPath *p = AntPath;
	p->npathLenth = 0;
	if (p->xy != NULL)
	{
		freememoryAntPosition(p->xy);
	}
	p->xy = new antPosition;
	FindPath(p,pstMap, astUavX, astUavY, astUavZ, astGoodsX, astGoodsY,pMap,pPheromone);
}
void AntColonyAlgorithm(sPlaneStatus *AntPath, int *pMap, UAV uav, GOODS goods, MAP_INFO * pstMap)
{
	////将地图转换为2D数组，存储x-y-h作为地图使用，雾区暂时认为是空的,这个只做一次，最后将此过程放在mian函数开始，现在调试用
	if (goods.nNO == -1)//没有货物时一定要置-1
	{
		AntPath->fx = uav.nX;
		AntPath->fy = uav.nY;
		AntPath->fz = uav.nZ;
		AntPath->goodsx = goods.nStartX;
		AntPath->goodsy = goods.nStartY;
		AntPath->dstx = goods.nEndX;
		AntPath->dsty = goods.nEndY;
		//信息素浓度，大小为X*Y*H
		//索引方式（X,Y,Z） = X*W + Y + Z *W*L
		float *pPheromone = (float *)malloc(sizeof(float)* pstMap->nMapX * pstMap->nMapY * (pstMap->nHHigh));
		Antmemset(pPheromone, 1.0, pstMap->nMapX * pstMap->nMapY * (pstMap->nHHigh));
		SearchAntPathPart(pPheromone,pMap, AntPath->AntPath, uav, goods, pstMap);
		free(pPheromone);
	} 
	else
	{
		AntPath->fx = uav.nX;
		AntPath->fy = uav.nY;
		AntPath->fz = uav.nZ;
		AntPath->goodsx = goods.nStartX;
		AntPath->goodsy = goods.nStartY;
		AntPath->dstx = goods.nEndX;
		AntPath->dsty = goods.nEndY;
		AntPath->nGoodWeight = goods.nWeight;
		//信息素浓度，大小为X*Y*H
		//索引方式（X,Y,Z） = X*W + Y + Z *W*L
		float *pPheromone = (float *)malloc(sizeof(float)* pstMap->nMapX * pstMap->nMapY * (pstMap->nHHigh));
		Antmemset(pPheromone, 1.0, pstMap->nMapX * pstMap->nMapY * (pstMap->nHHigh));
		SearchAntPath(pPheromone,pMap, AntPath->AntPath, uav, goods, pstMap);
		free(pPheromone);
	}
	
	//throw std::logic_error("The method or operation is not implemented.");
}

void movenext(FLAY_PLANE * pstFlayPlane, int i, sPlaneStatus * q)
{
	antPosition *q1 = q->AntPath->xy;
	q->AntPath->xy = q->AntPath->xy->next;
	q1->next = NULL;//一定要设置终止，否则会把整条路径释放。。。
	freememoryAntPosition(q1);//释放内存
	pstFlayPlane->astUav[i].nX = q->AntPath->xy->x;
	pstFlayPlane->astUav[i].nY = q->AntPath->xy->y;
	pstFlayPlane->astUav[i].nZ = q->AntPath->xy->z;
	q->fx = pstFlayPlane->astUav[i].nX;
	q->fy = pstFlayPlane->astUav[i].nY;
	q->fz = pstFlayPlane->astUav[i].nZ;
	if (pstFlayPlane->astUav[i].nGoodsNo != -1)
	{
		pstFlayPlane->astUav[i].remain_electricity -= q->nGoodWeight;
		q->remain_electricity -= q->nGoodWeight;
	}

}
/** @fn     void AlgorithmCalculationFun()
*  @brief	学生的算法计算， 参数什么的都自己写，
*	@return void
*/
void  AlgorithmCalculationFun(sPlaneStatus *sPlaneStatus1, int *pMap, MAP_INFO *pstMap, MATCH_STATUS * pstMatch, FLAY_PLANE *pstFlayPlane)
{
	//2018年5月22日09:29:53现在的bug应该是送完货物之后，货物不存在，程序直接规划下一个货物，导致错误
	//2018年5月22日10:17:13可以更好QAQ获取货物犯规(1号有丢失货物的现象或者超重现象或者没到点位就能获取货物)，随机游荡陪练机器人(不保证不撞机)获取货物正常
	//货物在pstflayplane中置零，否则会出现错误
	//货物被取走也会出现在货物列表中，所以还要检查敌方飞机是否取走了货物，若取走要从新分配。并且飞完之后
	//检查货物分配状态
	//2018年5月23日20:55:55今天添加了防撞逻辑，还有很多问题，另外将路径规划写成FindPath文件后，出现了飞机撞墙BUG，很奇怪
	sPlaneStatus *q = sPlaneStatus1;
	for (int i = 0; i < pstMatch->nUavWeNum; i++)//序号i与q对应，i每加1，q后移一个就可以了
	{
		//判断飞机是否还在
		if (pstMatch->astWeUav[i].nStatus == UAV_CRASH)//如果不在，跳过处理
		{
			q = q->next;
			continue;
		}
		else
		{
			//首先判断是否起飞
			if (q->istakeoff)
			{
				//////////////////////////////////////////////////////////////////////////
				if (q->AntPath == NULL && !q->isFightPlane)
				{
					if ( q->nGoodsStatus != -1)
					{
						q->AntPath = new sAntPath;
						for (int j = 0; j < pstMatch->nGoodsNum; j++)
						{
							if (pstMatch->astGoods[j].nNO == q->nGoodsStatus)
							{
								AntColonyAlgorithm(q, pMap, pstFlayPlane->astUav[i], pstMatch->astGoods[j], pstMap);
								movenext(pstFlayPlane, i, q);
								break;
							}
						}
					}
					else //若没货，去一个随机的地方
					{
						q->AntPath = new sAntPath;
						int x = Random(0, pstMap->nMapX - 1), y = Random(0, pstMap->nMapY - 1);
						while (*(pMap + x*pstMap->nMapY + y) != 0)
						{
							x = Random(0, pstMap->nMapX - 1); y = Random(0, pstMap->nMapY - 1);
						}
						GOODS astGoodstemp = { -1, x,y ,0,0,0,0,0,0,0 };
						AntColonyAlgorithm(q, pMap, pstFlayPlane->astUav[i], astGoodstemp, pstMap);
						movenext(pstFlayPlane, i, q);
						q->AntPath->npathLenth--;//应该先于放置语句
						//送完货，到达nHLow时，才会触发释放路径
						q->brandomPath = true;
						if (q->AntPath->npathLenth == 0)
						{
							q->nGoodsStatus = -1;
							q->nGoodValue = 0;
							freememoryAntPath(q->AntPath);
							q->AntPath = NULL;
							q->brandomPath = true;
						}
						//break;
					}
				}
				else if(!q->isFightPlane)
				{
					if (q->nGoodsStatus != -1 && q->brandomPath)
					{
						freememoryAntPath(q->AntPath);
						q->AntPath = new sAntPath;
						for (int j = 0; j < pstMatch->nGoodsNum; j++)
						{
							if (pstMatch->astGoods[j].nNO == q->nGoodsStatus)
							{
								AntColonyAlgorithm(q, pMap, pstFlayPlane->astUav[i], pstMatch->astGoods[j], pstMap);
								q->brandomPath = false;
								movenext(pstFlayPlane, i, q);
								q = q->next;
								break;
							}
						}
						continue;
					}
					movenext(pstFlayPlane, i, q);
					//判断是否到达货物地点，并取货
					if (pstFlayPlane->astUav[i].nX == q->goodsx && pstFlayPlane->astUav[i].nY == q->goodsy &&pstFlayPlane->astUav[i].nZ == 0)
					{
						for (int j = 0; j < pstMatch->nGoodsNum; j++)
						{
							if (pstMatch->astGoods[j].nNO == q->nGoodsStatus)
							{
								if (pstMatch->astGoods[j].nState == 0)
								{
									pstFlayPlane->astUav[i].nGoodsNo = q->nGoodsStatus;
									q->nGoodValue = 1;

									pstFlayPlane->astUav[i].remain_electricity -= q->nGoodWeight;
								}
							}
						}
					}

					if (q->AntPath->npathLenth == pstMap->nHLow)//放置货物语句
					{
						//pstFlayPlane->astUav[i].nGoodsNo = -1;
					}
					q->AntPath->npathLenth--;//应该先于放置语句
											 //送完货，到达nHLow时，才会触发释放路径
					if (q->AntPath->npathLenth == 0)
					{
						q->nGoodsStatus = -1;
						q->nGoodValue = 0;
						freememoryAntPath(q->AntPath);
						q->AntPath = NULL;
					}
				}
				else if (q->isFightPlane && q->AntPath == NULL)//是战斗机，并且没路径,之后在这里判断，若目标以拿起货物，
				{
					int itenemy, xx = q->nEnemyParkingX, yy = q->nEnemyParkingY, zz = 0;
					for (int itenemyNum = 0; itenemyNum < pstMatch->nUavEnemyNum;itenemyNum++)
					{
						if (pstMatch->astEnemyUav[itenemyNum].nNO == q->nAttackID)
						{
							if (pstMatch->astEnemyUav[itenemyNum].nGoodsNo != -1)//对方是否拿到货物
							{
								int la = 0,lb = 0;
								if (pstMatch->astEnemyUav[itenemyNum].nStatus == UAV_FOG)
								{
									for (int kkk = 0; kkk < pstMatch->nGoodsNum; kkk++)
									{
										if (pstMatch->astGoods[kkk].nNO == pstMatch->astEnemyUav[itenemyNum].nGoodsNo)
										{
											la = abs(q->fx - pstMatch->astGoods[kkk].nStartX);
											lb = abs(q->fy - pstMatch->astGoods[kkk].nStartY);
											q->goodsx = pstMatch->astGoods[kkk].nStartX;
											q->goodsy = pstMatch->astGoods[kkk].nStartY;
											break;
										}
									}
									//la = abs(q->fx - pstMatch->astEnemyUav[itenemyNum].nX);
									//lb = abs(q->fy - pstMatch->astEnemyUav[itenemyNum].nY);
								}
								else
								{
									la = abs(q->fx - pstMatch->astEnemyUav[itenemyNum].nX);
									lb = abs(q->fy - pstMatch->astEnemyUav[itenemyNum].nY);

								}
								if (la < lb) swap(&la, &lb);
								int HIGH = pstMatch->astEnemyUav[itenemyNum].nZ == -1 ? 0 : pstMatch->astEnemyUav[itenemyNum].nZ;
								if (la<int(pstMap->nHLow / 2))
								{
									if (pstMatch->astEnemyUav[itenemyNum].nStatus==UAV_FOG)
									{
										xx = q->goodsx;
										yy = q->goodsy;
										zz = pstMap->nHLow;
										break;
									}
									xx = pstMatch->astEnemyUav[itenemyNum].nX;
									yy = pstMatch->astEnemyUav[itenemyNum].nY;
									zz = pstMatch->astEnemyUav[itenemyNum].nZ;
									break;
								}
								else
								{
									// 如果距离货物已经拿起来，则直接去货物所在的位置
									for (int kkk = 0; kkk < pstMatch->nGoodsNum; kkk++)
									{
										if (pstMatch->astGoods[kkk].nNO == pstMatch->astEnemyUav[itenemyNum].nGoodsNo)
										{
											xx = pstMatch->astGoods[kkk].nEndX;
											yy = pstMatch->astGoods[kkk].nEndY;
											zz = pstMap->nHLow;
											break;
										}
									}
									break;
								}
							}
							else
							{
								xx = pstMatch->astEnemyUav[itenemyNum].nX;
								yy = pstMatch->astEnemyUav[itenemyNum].nY;
								zz = pstMatch->astEnemyUav[itenemyNum].nZ;
								break;
							}
						}
					}
					if (xx == -1 && yy ==-1)
					{
						q = q->next;
						continue;
					}
					if (xx == pstFlayPlane->astUav[i].nX && yy == pstFlayPlane->astUav[i].nY)
					{
						if (pstFlayPlane->astUav[i].nZ > zz)
						{
							pstFlayPlane->astUav[i].nZ--;
							q->fz--;
							q = q->next;
						}
						else
						{
							pstFlayPlane->astUav[i].nZ++;
							q->fz++;
							q = q->next;
						}
						continue;
					}
					q->AntPath = new sAntPath;
					GOODS astGoodstemp = { -1, xx, yy, 0, 0, 0, 0, 0, 0, 0 };
					AntColonyAlgorithm(q, pMap, pstFlayPlane->astUav[i], astGoodstemp, pstMap);
					movenext(pstFlayPlane, i, q);
					q->AntPath->npathLenth--;//应该先于放置语句
					//送完货，到达nHLow时，才会触发释放路径
					if (q->AntPath->npathLenth == 0)
					{
						q->nGoodsStatus = -1;
						q->nGoodValue = 0;
						freememoryAntPath(q->AntPath);
						q->AntPath = NULL;
						q->brandomPath = true;
					}
				}
				else if (q->isFightPlane && q->AntPath != NULL)//是战斗机，但有路径
				{
					movenext(pstFlayPlane, i, q);
					q->AntPath->npathLenth--;//应该先于放置语句
					//送完货，到达nHLow时，才会触发释放路径
					if (q->AntPath->npathLenth == 0)
					{
						q->nGoodsStatus = -1;
						q->nGoodValue = 0;
						freememoryAntPath(q->AntPath);
						q->AntPath = NULL;
					}
				}
			}
			else//若没起飞，则起飞
			{
				bool flag = true;
				for (int m = 0; m < pstFlayPlane->nUavNum; m++)
				{
					if (pstFlayPlane->astUav[i].nZ + 1 == pstFlayPlane->astUav[m].nZ && pstFlayPlane->astUav[i].nX == pstFlayPlane->astUav[m].nX && pstFlayPlane->astUav[i].nY == pstFlayPlane->astUav[m].nY)
					{
						flag = false;
					}
				}
				if (pstFlayPlane->astUav[i].nZ >= pstMap->nHLow)//将最低高度加1，有助于飞机起飞和划定轨迹
				{
					q->istakeoff = true;
				}
				//if (flag && pstFlayPlane->astUav[i].nZ <pstMap->nHLow - 1 && pstFlayPlane->astUav[i].remain_electricity == q->capacity)
				if (flag && pstFlayPlane->astUav[i].nZ < pstMap->nHLow)
				{
					// 充电还是起飞
					if (pstFlayPlane->astUav[i].nX == pstMap->nParkingX && pstFlayPlane->astUav[i].nY == pstMap->nParkingY && pstFlayPlane->astUav[i].remain_electricity != q->capacity)
					{
						pstFlayPlane->astUav[i].remain_electricity += q->charge;
						if (pstFlayPlane->astUav[i].remain_electricity >= q->capacity)
						{
							pstFlayPlane->astUav[i].remain_electricity = q->capacity;
						}
						q->remain_electricity = pstFlayPlane->astUav[i].remain_electricity;
					}
					else//电之后就可以起飞了
					{
						pstFlayPlane->astUav[i].nZ++;
						q->fz++;
					}
				}
				else//冲了一次电，但是上面有挡着的，所以还要继续充电
				{
					if (pstFlayPlane->astUav[i].nX == pstMap->nParkingX && pstFlayPlane->astUav[i].nY == pstMap->nParkingY && pstFlayPlane->astUav[i].nZ == 0)
					{
						pstFlayPlane->astUav[i].remain_electricity += q->charge;
						if (pstFlayPlane->astUav[i].remain_electricity >= q->capacity)
						{
							pstFlayPlane->astUav[i].remain_electricity = q->capacity;
						}
						q->remain_electricity = pstFlayPlane->astUav[i].remain_electricity;
					}
				}
				if (pstFlayPlane->astUav[i].nZ >= pstMap->nHLow)//将最低高度加1，有助于飞机起飞和划定轨迹
				{
					q->istakeoff = true;
				}
			}
		}
		q = q->next;
	}
}
//删除FLAY_PLANE结构体中撞毁的飞机，给服务器返回正确的飞机数量和位置
void DeletePlane(FLAY_PLANE * pstFlayPlane)
{
	int i = 0;
	while (pstFlayPlane->astUav[i].nLoadWeight != 0)
	{
		if (pstFlayPlane->astUav[i].nStatus == UAV_CRASH)
		{
			int j = i;
			while (pstFlayPlane->astUav[j].nLoadWeight != 0)
			{
				pstFlayPlane->astUav[j] = pstFlayPlane->astUav[j + 1];
				j++;
			}
			pstFlayPlane->nUavNum--;
			i--;
		}
		i++;
	}
}
//暂时只能防止自相撞,现在需要把之前的信息传进来，包括前一帧位置和地图信息
void CollisionDectection(FLAY_PLANE * pstFlayPlane, FLAY_PLANE * pstFlayPlaneLast, int * pMap, MAP_INFO * pstMapInfo, MATCH_STATUS * pstMatchStatus, sPlaneStatus * sPlaneStatus1)
{
	for (int i = 0; i < pstFlayPlane->nUavNum;i++)
	{
		sPlaneStatus *p1 = sPlaneStatus1;
		while (p1->nID != pstFlayPlane->astUav[i].nNO)
		{
			p1 = p1->next;
		}
		for (int j = i+1; j < pstFlayPlane->nUavNum;j++)
		{
			sPlaneStatus *p2 = sPlaneStatus1;
			while (p2->nID != pstFlayPlane->astUav[j].nNO)
			{
				p2 = p2->next;
			}
			//若存在位置相同的，则判定为相撞
			int a = abs(pstFlayPlane->astUav[i].nX - pstFlayPlane->astUav[j].nX), b = abs(pstFlayPlane->astUav[i].nY - pstFlayPlane->astUav[j].nY);
			if ((a == 1 && b == 1 || a == 1 && b == 0 || a == 0 && b == 1 || a == 0 && b == 0)//
				&& (pstFlayPlane->astUav[i].nZ == pstFlayPlane->astUav[j].nZ)
				&& (p1->AntPath != NULL && p2->AntPath != NULL)
				&& (abs(pstFlayPlane->astUav[i].nX - pstMapInfo->nParkingX) > 2 || abs(pstFlayPlane->astUav[i].nY - pstMapInfo->nParkingY) >2)
				&& (abs(pstFlayPlane->astUav[i].nX - sPlaneStatus1->nEnemyParkingX) > 2 || abs(pstFlayPlane->astUav[i].nY - sPlaneStatus1->nEnemyParkingY) >2)
				)//&& (abs(pstFlayPlane->astUav[i].nX - sPlaneStatus1->nEnemyParkingX) > 2 || abs(pstFlayPlane->astUav[i].nY - sPlaneStatus1->nEnemyParkingY) >2) || (!p2->isFightPlane || !p1->isFightPlane)
			{
				//if (p1->nID == 39 && (p2->nID == 40 || p2->nID == 27))
				//{
				//	int aaaa = 0;
				//}
				int ii = i, jj = j;
				if (p1->nID != pstFlayPlane->astUav[i].nNO)// || p2->nID == pstFlayPlane->astUav[j].nNO)
				{
					continue;
				}
				int p1nGoodValue=0, p2nGoodValue=0;
				for (int kkk = 0; kkk < pstMatchStatus->nGoodsNum;kkk++)
				{
					if (pstMatchStatus->astGoods[kkk].nNO = p1->nGoodsStatus)
					{
						p1nGoodValue = pstMatchStatus->astGoods[kkk].nValue;
					}
					if (pstMatchStatus->astGoods[kkk].nNO = p2->nGoodsStatus)
					{
						p2nGoodValue = pstMatchStatus->astGoods[kkk].nValue;
					}
				}
				//if (p1->nID == 39)
				//{
				//	int aaaa = 0;
				//}
				if (p1nGoodValue <= p2nGoodValue || p1->AntPath == NULL )
				{
					int ntemp = ii;
					ii = jj;
					jj = ntemp;
					sPlaneStatus *ptemp = p1;
					p1 = p2;
					p2 = ptemp;
				}
				//Bug:有可能会超过最大高度
				if (p2->AntPath != NULL)
				{
					if (p2->AntPath->xy->z + 1 <= pstMapInfo->nHHigh )
					{
						antPosition *temp = p2->AntPath->xy;
						//先上升,挺两次
						p2->AntPath->npathLenth++;
						temp = p2->AntPath->xy;
						p2->AntPath->xy = new antPosition;
						p2->AntPath->xy->x = pstFlayPlane->astUav[jj].nX;
						p2->AntPath->xy->y = pstFlayPlane->astUav[jj].nY;
						p2->AntPath->xy->z = pstFlayPlane->astUav[jj].nZ + 1;
						p2->AntPath->xy->next = temp;
						p2->AntPath->npathLenth++;
						temp = p2->AntPath->xy;
						p2->AntPath->xy = new antPosition;
						p2->AntPath->xy->x = pstFlayPlane->astUav[jj].nX;
						p2->AntPath->xy->y = pstFlayPlane->astUav[jj].nY;
						p2->AntPath->xy->z = pstFlayPlane->astUav[jj].nZ + 1;
						p2->AntPath->xy->next = temp;
						//再下降
						p2->AntPath->npathLenth++;
						temp = p2->AntPath->xy;
						p2->AntPath->xy = new antPosition;
						int jjj = -1;
						for (int iii = 0; iii < pstFlayPlaneLast->nUavNum;iii++)
						{
							if (pstFlayPlaneLast->astUav[iii].nNO == pstFlayPlane->astUav[jj].nNO)
							{
								jjj = iii;
							}
						}if (jjj == -1)
						{
							std::cout << "error on dectection of collision" << std::endl;
						}
						p2->AntPath->xy->x = pstFlayPlaneLast->astUav[jjj].nX;
						p2->AntPath->xy->y = pstFlayPlaneLast->astUav[jjj].nY;
						p2->AntPath->xy->z = pstFlayPlaneLast->astUav[jjj].nZ + 1;
						p2->AntPath->xy->next = temp;
						pstFlayPlane->astUav[jj].nX = p2->AntPath->xy->x;
						pstFlayPlane->astUav[jj].nY = p2->AntPath->xy->y;
						pstFlayPlane->astUav[jj].nZ = p2->AntPath->xy->z;
					}
					else
					{
						antPosition *temp = p2->AntPath->xy;
						//再下降
						p2->AntPath->npathLenth++;
						temp = p2->AntPath->xy;
						p2->AntPath->xy = new antPosition;
						p2->AntPath->xy->x = pstFlayPlane->astUav[jj].nX;
						p2->AntPath->xy->y = pstFlayPlane->astUav[jj].nY;
						p2->AntPath->xy->z = pstFlayPlane->astUav[jj].nZ - 1;
						p2->AntPath->xy->next = temp;
						p2->AntPath->npathLenth++;
						temp = p2->AntPath->xy;
						p2->AntPath->xy = new antPosition;
						p2->AntPath->xy->x = pstFlayPlane->astUav[jj].nX;
						p2->AntPath->xy->y = pstFlayPlane->astUav[jj].nY;
						p2->AntPath->xy->z = pstFlayPlane->astUav[jj].nZ - 1;
						p2->AntPath->xy->next = temp;
						//先上升
						p2->AntPath->npathLenth++;
						temp = p2->AntPath->xy;
						p2->AntPath->xy = new antPosition;
						int jjj = -1;
						for (int iii = 0; iii < pstFlayPlaneLast->nUavNum; iii++)
						{
							if (pstFlayPlaneLast->astUav[iii].nNO == pstFlayPlane->astUav[jj].nNO)
							{
								jjj = iii;
							}
						}if (jjj == -1)
						{
							std::cout << "error on dectection of collision" << std::endl;
						}
						p2->AntPath->xy->x = pstFlayPlaneLast->astUav[jj].nX;
						p2->AntPath->xy->y = pstFlayPlaneLast->astUav[jj].nY;
						p2->AntPath->xy->z = pstFlayPlaneLast->astUav[jj].nZ - 1;
						p2->AntPath->xy->next = temp;
						pstFlayPlane->astUav[jj].nX = p2->AntPath->xy->x;
						pstFlayPlane->astUav[jj].nY = p2->AntPath->xy->y;
						pstFlayPlane->astUav[jj].nZ = p2->AntPath->xy->z;
					}
				}
				else
				{

				}
			}
		}
	}
}
void PurchasePlane(FLAY_PLANE * pstFlayPlane, MAP_INFO *pstMap, MATCH_STATUS * pstMatch, std::vector<int> *listofweight)
{
	//购买飞机
	//购买策略为这次未分配的货物，在货物选择函数中加入两架虚拟飞机，如果虚拟飞机分配了货物，则购买
	//若未分配，就不购买
	std::vector<int> listofplane;
	for (int i = 0; i < pstMap->nUavPriceNum;i++)
	{
		listofplane.push_back(pstMap->astUavPrice[i].nLoadWeight);
	}
	sort(listofplane.begin(), listofplane.end());
	//for (int i = listofweight->size() - 1; i >= 0;i--)
	for (int i = 0; i < listofweight->size(); i++)
	{
		//找到重量对应的飞机类型(区间)现在改为先买便宜的，看看什么效果
		if ((*listofweight)[i] <= listofplane[4] && (*listofweight)[i] > listofplane[3])
		{
			(*listofweight)[i] = listofplane[4];
		}
		else if ((*listofweight)[i] <= listofplane[3] && (*listofweight)[i] > listofplane[2])
		{
			(*listofweight)[i] = listofplane[3];
		}
		else if ((*listofweight)[i] <= listofplane[2] && (*listofweight)[i] > listofplane[1])
		{
			(*listofweight)[i] = listofplane[2];
		}
		else if ((*listofweight)[i] <= listofplane[1] && (*listofweight)[i] > listofplane[0])
		{
			(*listofweight)[i] = listofplane[1];
		}
		else if ((*listofweight)[i] <= listofplane[0] )
		{
			(*listofweight)[i] = listofplane[0];
		}
	}
	int npur = 0;
	std::vector<int> listofplane1;
	for (int i = 0; i < pstMap->nUavPriceNum; i++)
	{
		listofplane1.push_back(pstMap->astUavPrice[i].nLoadWeight);
	}
	int nWeValue = pstMatch->nWeValue;
	int boughtValue = 0;
	//char type = 'a';
	for (std::vector<int>::iterator iter = listofweight->begin(); iter != listofweight->end();iter++)
	{
		for (int it = 0; it < listofweight->size(); it++)
		{
			int it1 = findindex(listofplane1, (*listofweight)[it]);
			if (nWeValue >= pstMap->astUavPrice[it1 ].nValue)
			{
				boughtValue = pstMap->astUavPrice[it1].nValue;
				nWeValue -= boughtValue;
				pstFlayPlane->szPurchaseType[npur][0] = 'F';
				//_itoa(it1+1, &pstFlayPlane->szPurchaseType[npur++][1], 10);
				std::string aaa = std::to_string(it1 + 1);
				pstFlayPlane->szPurchaseType[npur++][1] = aaa[0];
				//pstFlayPlane->szPurchaseType[npur++][1] = type;
				pstFlayPlane->nPurchaseNum++;
				listofweight->erase(iter);
				iter = listofweight->begin();
				break;
			}
			else break;
		}
		if (listofweight->size() == 0)
		{
			break;
		}
	}
}
void ResetPlaneStatus(FLAY_PLANE * pstFlayPlane, int i, sPlaneStatus * q)
{
	q->istakeoff = false;
	freememoryAntPath(q->AntPath);
	q->AntPath = NULL;
	q->nGoodsStatus = -1;
	q->nGoodValue = 0;

	//pstFlayPlane->astUav[i].nZ++;
}
antPosition* FindPath(sAntPath * p, MAP_INFO *pstMap,int startx, int starty, int startz,int astGoodsX, int astGoodsY, int *pMap,float *pPheromone)
{
	//1 2位置写入飞机初始位置
	antPosition *q = p->xy;
	antPosition *qqqq = q;
	while (q->next != NULL)
	{
		qqqq = q;
		q = q->next;
	}
	if (qqqq->x != startx || qqqq->y != starty || qqqq->z != startz)
	{
		q->x = startx;
		q->y = starty;
		q->z = startz;
	}
	else
	{
		freememoryAntPosition(q->next);
		q = qqqq;
	}
	//为第k个种群找出一个路径
	while (q->y != astGoodsY || q->x != astGoodsX)//当当前的xy没有到达时，找到下一个路径，所以，如果终点就是起点，不会生成轨迹
	{
		//若没有到达货物，则找到下一步
		q->next = new antPosition;
		//计算可到达格子的适应度
		float Q[11] = { 0.0 };
		int nq = 0;
		for (int i = -nMoveXMax; i <= nMoveXMax; i++)//Y
		{
			for (int j = -nMoveYMax; j <= nMoveYMax; j++)//X
			{
				if (q->x + i >= 0 && q->x + i <= pstMap->nMapX - 1 && q->y + j >= 0 && q->y + j <= pstMap->nMapY - 1)//应该-1
				{
					//判断目标点是否就在可视范围内,若在，直接退出循环
					if (q->x + i == astGoodsX &&  q->y + j == astGoodsY)
					{
						int ntempx = q->x + i, ntempy = q->y + j, ntempz = q->z;
						q = q->next;
						q->x = ntempx; q->y = ntempy;
						q->z = ntempz;
						p->npathLenth++;
						break;
					}
					//计算启发值，这里简化模型，Q = S*D，S为下一点是否可到达，也就是是否是bulidings，D为下一点到目标点距离的平方
					if (q->z - *(pMap + (q->x + i)*pstMap->nMapY + q->y + j) > 0)//不能超过9
					{
						Q[nq++] = 50 * *(pPheromone + (q->x + i)*pstMap->nMapY + q->y + j + q->z*pstMap->nMapX*pstMap->nMapY) / sqrtf(float((((q->x + i - astGoodsX) * (q->x + i - astGoodsX))) + ((q->y + j - astGoodsY) * (q->y + j - astGoodsY)) + (q->z * q->z)));//50为权值,大小影响应该不大，存疑
					}
					else
					{
						Q[nq++] = 0;
					}

					//将信息素矩阵改为数组，以便于初始化为1;
					//接下来分几步：1、走过的信息素衰减，使得种群种路径多元化
					//              2、最佳路径的信息素加大，使得最短路径出现概率变大

				}
				else { Q[nq++] = 0; }
			}
			if (q->x == astGoodsX &&  q->y == astGoodsY) break;//判断目标点是否就在可视范围内,若在，直接退出循环
		}
		if (q->x == astGoodsX &&  q->y == astGoodsY) break;//判断目标点是否就在可视范围内,若在，直接退
														   //米子宫格搜索完之后，接下来是上下两个格子，一共11个(!!可以不用搜索下面的格子)
		if (q->z + 1 <= pstMap->nHHigh)
		{
			Q[nq++] = 50 * (*(pPheromone + q->x*pstMap->nMapY + q->y + q->z*pstMap->nMapX*pstMap->nMapY)) / sqrtf(float(((q->x - astGoodsX) * (q->x - astGoodsX)) + (q->y - astGoodsY) * (q->y - astGoodsY )+ (q->z + 1)*(q->z + 1)));//50为权值,大小影响应该不大，存疑
		}
		//出循环
		//uniform(Q, ++nq);
		//生成随机数，准备轮盘赌
		//!!用轮盘赌收敛过慢
		//float froll = float(d(e));
		int   nindex = 0;//表示赌中数的下标
		nindex = findmax(Q, 11);
		int ntempx, ntempy, ntempz;
		if (nindex < 9)//小于9，在米内
		{
			ntempx = q->x + nindex3[(nindex) / 3]; ntempy = q->y + nindex3[(nindex) % 3]; ntempz = q->z;
			q = q->next;
			q->x = ntempx; q->y = ntempy;
			q->z = ntempz;
			p->npathLenth++;
		} //大于9，在上面的那个格子
		else
		{
			ntempx = q->x; ntempy = q->y; ntempz = q->z + 1;
			q = q->next;
			q->x = ntempx; q->y = ntempy;
			q->z = ntempz;
			p->npathLenth++;
		}
		//计算信息素浓度
		*(pPheromone + q->x*pstMap->nMapY + q->y + q->z*pstMap->nMapX*pstMap->nMapY) *= 0.5;
	}
	return q;
}
void GoodsAllocation(sPlaneStatus * sPlaneStatus1, FLAY_PLANE * pstFlayPlane, MATCH_STATUS * pstMatchStatus, MAP_INFO *pstMap, std::vector<int> *listofweight)
{
//#define TEST_ASSIGNMENT
	// 找出未被分配的飞机N和货物M,存放相应的ID号
	std::vector<int> M, N, Allocated;
	std::vector<int> vPlaneNo, vGoodsNo;//按顺序存放所有正常飞机和货物的ID号
	sPlaneStatus *q = sPlaneStatus1;
	while (q != NULL && pstMatchStatus->nTime >=pstMap->nHLow +0)//15
	{
		// 存未被分配的飞机，或分配后货物被敌方拿走的飞机
		//N.push_back(q->nID);
		if (q->nGoodsStatus == -1 && q->nStatus != UAV_CRASH && !q->isFightPlane )					// 没有分配的，直接进入未分配列表
		{
			N.push_back(q->nID); 
		}
		// 存已被分配的货物
		if (q->nGoodsStatus != -1 && q->nStatus != UAV_CRASH && !q->isFightPlane )					// 已分配的，没过5time重新关联一次
		{
			int flag = false;//货物是否消失
			for (int i = 0; i < pstMatchStatus->nGoodsNum; i ++)//当消失时，这个if不会触发
			{
				if (pstMatchStatus->astGoods[i].nNO == q->nGoodsStatus)
				{
					flag = true;//货物还在场
					if (pstMatchStatus->astGoods[i].nState == 0)//货物在场并且没被拿起
					{
						//
						//if (pstMatchStatus->nTime %20 == 0)//每隔5time，重新分配一次
						//{
						//	if (q->nGoodValue != 1)
						//	{
						//		q->nGoodsStatus = -1;
						//		q->istakeoff = false;
						//		freememoryAntPath(q->AntPath);
						//		q->AntPath = NULL;
						//		N.push_back(q->nID);//已分配，但时间到了要重新分配
						//	}
						//}
						//else
							//
						{
							Allocated.push_back(q->nGoodsStatus);
						}
					}
					else//被拿起
					{
						if (q->AntPath != NULL)
						{
							int la = abs(q->AntPath->xy->x - pstMatchStatus->astGoods[i].nStartX), lb = abs(q->AntPath->xy->y - pstMatchStatus->astGoods[i].nStartY);
							if (la < lb) swap(&la, &lb);
							if (la < pstMap->nHLow - 1)
							{
							}
							else
							{
								Allocated.push_back(pstMatchStatus->astGoods[i].nNO);
								if (q->nGoodValue != 1)
								{
									q->nGoodsStatus = -1;
									q->istakeoff = false;
									freememoryAntPath(q->AntPath);
									q->AntPath = NULL;
									N.push_back(q->nID);//已分配，但是被敌方拿走了，或者友方？
								}
							}
						}
						else
						{
							Allocated.push_back(pstMatchStatus->astGoods[i].nNO);
							if (q->nGoodValue != 1)
							{
								q->nGoodsStatus = -1;
								q->istakeoff = false;
								freememoryAntPath(q->AntPath);
								q->AntPath = NULL;
								N.push_back(q->nID);//已分配，但是被敌方拿走了，或者友方？
							}
						}
					}	
				}	
			}
			if (!flag)//已分配没被拿走，但是消失了  超时
			{
				N.push_back(q->nID);
				//重置q
				if (q->AntPath != NULL)
				{
					if (q->AntPath->xy->z < pstMap->nHLow)
					{
						q->istakeoff = false;
					}
				}
				freememoryAntPath(q->AntPath);
				q->istakeoff = false;
				q->AntPath = NULL;
				q->nGoodsStatus = -1;
				q->nGoodValue = 0;
			}
		}
		q = q->next;
	}
	for (int i = 0; i < pstMatchStatus->nGoodsNum; i++)
	{
		if (find(Allocated.begin(), Allocated.end(), pstMatchStatus->astGoods[i].nNO) == Allocated.end())//如果在已分配中没有找见
		{
			M.push_back(pstMatchStatus->astGoods[i].nNO);
		}
		vGoodsNo.push_back(pstMatchStatus->astGoods[i].nNO);
	}
	for (int i = 0; i < pstMatchStatus->nUavWeNum; i++)
	{
		vPlaneNo.push_back(pstMatchStatus->astWeUav[i].nNO);
	}
	// 首先计算距离矩阵，矩阵大小为N*M，其中M为未分配货物的飞机数，N为未被分配的货物数

	double *pCostMat = (double *)malloc(sizeof(double) * M.size() * N.size());
	bool   *pisOverElectricity = (bool *)malloc(sizeof(bool) * M.size() * N.size());
	memset(pisOverElectricity, 0, sizeof(bool) * M.size() * N.size());
	//double pCostMat[] = { 0,1,1,1,1,1, 1,0,1,1,1,1, 1,1,0,1,1,1, 1,1,1,0,1,1, 1,1,1,1,1,0, 1,1,1,1,0,1 };
	// 记录对应位置的下标
	int xa, xb, ya, yb, xc, yc;
	int lx = 0, ly = 0;
	int lax = 0, lay = 0, lbx = 0, lby = 0;
	int it = 0;
	int nWeightplane, nWeightGoods, nValue;
	int remain_electricity;
	double dist,distend;
	for (int i = 0; i < N.size();i++)//i行j列
	{//N[i]
		//飞机信息
		it = findindex(vPlaneNo, N[i]);
		xa = pstMatchStatus->astWeUav[it].nX;
		ya = pstMatchStatus->astWeUav[it].nY;
		nWeightplane = pstMatchStatus->astWeUav[it].nLoadWeight;
		remain_electricity = pstMatchStatus->astWeUav[it].remain_electricity;
		//it = FindPlanetype(pstMatchStatus->astWeUav[i], pstMap);
		//nWeightplane = pstMatchStatus->astWeUav[it].nLoadWeight;
		for (int j = 0; j < M.size(); j++)
		{//M[j]
			//货物信息
			it = findindex(vGoodsNo, M[j]);
			xb = pstMatchStatus->astGoods[it].nStartX;
			yb = pstMatchStatus->astGoods[it].nStartY;
			xc = pstMatchStatus->astGoods[it].nEndX;
			yc = pstMatchStatus->astGoods[it].nEndY;
			nWeightGoods = pstMatchStatus->astGoods[it].nWeight;
			nValue = pstMatchStatus->astGoods[it].nValue;
			//dist = sqrt((xa - xb)*(xa - xb) + (ya - yb)*(ya - yb))
			//	+ sqrt((pstMatchStatus->astGoods[it].nStartX - pstMatchStatus->astGoods[it].nEndX)*(pstMatchStatus->astGoods[it].nStartX - pstMatchStatus->astGoods[it].nEndX) 
			//		+ (pstMatchStatus->astGoods[it].nStartY - pstMatchStatus->astGoods[it].nEndY)*(pstMatchStatus->astGoods[it].nStartY - pstMatchStatus->astGoods[it].nEndY));
			//distend = sqrt((xc - xb)*(xc - xb) + (yc - yb)*(yc - yb)) + pstMap->nHLow * 2 +30;
			lax = abs(xa - xb); lay = abs((ya - yb));
			if (lax < lay) swap(&lax, &lay);// 令x为较大数
			lx = abs(xc - xb); ly = abs(yc - yb);
			if (lx < ly) swap(&lx, &ly);
			dist = lax + 15;
			distend = lx + pstMap->nHLow*2 +10;
			//计算
			//*(pCostMat + i*M.size() + j) = (nWeightplane >= nWeightGoods && (dist + pstMap->nHLow) < pstMatchStatus->astGoods[it].nRemainTime - pstMatchStatus->nTime + pstMatchStatus->astGoods[it].nStartTime) ? (sqrt(dist)*nWeightGoods / nValue) : 99999999;// std::numeric_limits<double>::max();
			//*(pCostMat + i*M.size() + j) = (nWeightplane >= nWeightGoods && (dist + pstMap->nHLow) < pstMatchStatus->astGoods[it].nRemainTime - pstMatchStatus->nTime + pstMatchStatus->astGoods[it].nStartTime) ? ((dist)*nWeightGoods / nValue) : 99999999;// std::numeric_limits<double>::max();
			bool isntoverWeight = nWeightplane >= nWeightGoods;
			bool isntoverTime = ((lax + pstMap->nHLow) < pstMatchStatus->astGoods[it].nRemainTime - pstMatchStatus->nTime + pstMatchStatus->astGoods[it].nStartTime);
			bool isntoverElectricity = ((remain_electricity - distend * nWeightGoods) >= 1e-10);
			*(pisOverElectricity + i*M.size() + j) = isntoverElectricity;
			*(pCostMat + i*M.size() + j) = (isntoverWeight && isntoverTime && isntoverElectricity) ? (dist / nValue*nValue*nValue) : 99999999;// std::numeric_limits<double>::max();
			#ifdef TEST_ASSIGNMENT
				std::cout << std::setw(20) << *(pCostMat + i*M.size() + j);
			#endif
		}
		#ifdef TEST_ASSIGNMENT
			std::cout << std::endl;
		#endif
	}
	double *assignment = (double *)malloc(sizeof(double) * N.size());
	memset(assignment, 0, N.size());
	double Cost = 0;

	////////for test////

#ifdef TEST_ASSIGNMENT
	free(assignment);
	assignment = (double *)malloc(sizeof(double) * 6);
	double pCostMat1[] = { 0, 99999999, 99999999, 99999999, 99999999, 99999999,
						  99999999, 0, 99999999, 99999999, 99999999, 99999999,
						  99999999, 99999999, 0, 99999999, 99999999, 99999999,
						  99999999, 99999999, 99999999, 0, 99999999, 99999999,
						  99999999, 99999999, 99999999, 99999999, 99999999, 0,
						  99999999, 99999999, 99999999, 99999999, 99999999, 99999999 };
	assignmentoptimal(assignment, &Cost, pCostMat1, 6, 6);//有时候看起来不太合理但真的是全局最优
	for (int i = 0; i < 6;i++)
	{
		std::cout << assignment[i] << " ";
	}
#endif
	assignmentoptimal(assignment, &Cost, pCostMat, N.size(), M.size());//有时候看起来不太合理但真的是全局最优
	//生成一个购买列表
	// step1:找出未分配的飞机
	std::vector<int> listofasmgoods;
	// 之后将分配结果给到结构图
	q = sPlaneStatus1;
	int it1, it2;
	while (q != NULL)
	{
		for (int i = 0; i < N.size();i++)
		{
			int j = int(*(assignment + i))-1;// 分配结果应该-1，因为是从1开始的
			it1 = findindex(vPlaneNo, N[i]);
			if (j != -1)
			{
				it2 = findindex(vGoodsNo, M[j]);
				if (q->nID == pstMatchStatus->astWeUav[it1].nNO)
				{
					//这里同样需要判断电量
					if (pstMatchStatus->astWeUav[it1].nLoadWeight >= pstMatchStatus->astGoods[it2].nWeight)
					//if (pstMatchStatus->astWeUav[it1].nLoadWeight >= pstMatchStatus->astGoods[it2].nWeight )
					{
						if (*(pisOverElectricity + M.size()*i + j))
						{
							q->nGoodsStatus = pstMatchStatus->astGoods[it2].nNO;
						}
						listofasmgoods.push_back(j);
						//q->nGoodValue = pstMatchStatus->astGoods[it2].nValue;
					}
				}
			}
		}
		q = q->next;
	}
	// 若分配的数量小于总货物数量，则需要买飞机
	// 找到未分配货物的重量
	if (listofweight->size() == 0)
	{
		listofweight->push_back(10);
		listofweight->push_back(10);
		if (listofasmgoods.size() < M.size())
		{
			for (int i = 0; i < M.size(); i++)
			{
				bool flag = true;
				for (int j = 0; j < listofasmgoods.size(); j++)
				{
					if (listofasmgoods[j] == i)
					{
						flag = false;
						break;
					}
				}
				if (flag)
				{
					int it2 = findindex(vGoodsNo, M[i]);
					listofweight->push_back(pstMatchStatus->astGoods[it2].nWeight);
				}
			}
		}
		sort(listofweight->begin(), listofweight->end());
	}
	free(assignment);
	free(pCostMat);
	free(pisOverElectricity);
	// 检索未分配飞机，调出便宜的作为攻击机
	// 这里应该直接分配好敌方飞机ID
	q = sPlaneStatus1;
	
	while (q->next != NULL && q->nNumofFightPlane <FIGHT_PLANE_NUM)
	{
		if (q->isFightPlane)//如果是攻击机，检查一下原来敌方目标是否在存在
		{
			bool flag = true;
			for (int i = 0; i < pstMatchStatus->nUavEnemyNum;i++)
			{
				if (q->nAttackID == pstMatchStatus->astEnemyUav[i].nNO)
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				q->isFightPlane = false;
				q->nNumofFightPlane--;
				q->nAttackID = -1;
				//if (q->fz < pstMap->nHLow)
				{
					q->istakeoff = false;
				}
			}
		}
		if (!q->isFightPlane && q->capacity<5000 && q->nNumofFightPlane<FIGHT_PLANE_NUM && q->nStatus != UAV_CRASH && q->nGoodsStatus == -1)
		{
			// 找到价值最大敌方未分配的飞机
			// 先找是否还有未分配的F5 - F1 - F2 - F4 - F3
			int nitEnemy = -1;
			bool flag = false;
			std::vector<int> planeType;
			for (int i = 0; i < pstMap->nUavPriceNum; i++) planeType.push_back(pstMap->astUavPrice[i].nValue);
			sort(planeType.begin(), planeType.end());
			for (int j = pstMap->nUavPriceNum-1; j >=0 ; j--)
			{
				char *aaa = NULL;
				for (int k = 0; k < pstMap->nUavPriceNum; k++) { if (pstMap->astUavPrice[k].nValue == planeType[j]) { aaa = pstMap->astUavPrice[k].szType; break; } }
				for (int i = 0; i < pstMatchStatus->nUavEnemyNum; i++)
				{
					//找到第i贵的飞机
					if (strcmp(pstMatchStatus->astEnemyUav[i].szType, aaa) == 0 && pstMatchStatus->astEnemyUav[i].nStatus != UAV_FOG)
					{
						flag = false;
						sPlaneStatus *p = sPlaneStatus1;
						while (p->next != NULL)//这架飞机有没有被分配
						{
							if (p->nAttackID == pstMatchStatus->astEnemyUav[i].nNO)
							{
								flag = true;
								break;
							}
							p = p->next;
						}
					}
					else
					{
						continue;
					}
					if (!flag)//没分配，进入
					{
						nitEnemy = i;
						break;
					}
				}
				if (!flag && nitEnemy != -1)
				{
					break;
				}
			}
			if (!flag && nitEnemy != -1)
			{
				q->isFightPlane = true;
				q->nNumofFightPlane++;
				q->nAttackID = pstMatchStatus->astEnemyUav[nitEnemy].nNO;
				//break;
			}
		}
		q = q->next;
	}
}
int findindex(std::vector<int> a, int b)
{
	for (int i = 0; i < a.size();i++)
	{
		if (a[i] == b)
		{
			return i;
		}
	}
	return -1;
}
int FindPlanetype(UAV astWeUav, MAP_INFO * pstMap)
{
	for (int i = 0; i < pstMap->nUavPriceNum;i++)
	{
		if ((strcmp(astWeUav.szType, pstMap->astUavPrice[i].szType)) == 0)
		{
			return i;
		}
	}
	std::cout << "无此飞机类型！" << std::endl;
	return -1;
}

// 匈牙利算法
void assignmentoptimal(double *assignment, double *cost, double *distMatrixIn, int nOfRows, int nOfColumns)
{
	double *distMatrix, *distMatrixTemp, *distMatrixEnd, *columnEnd, value, minValue;
	bool *coveredColumns, *coveredRows, *starMatrix, *newStarMatrix, *primeMatrix;
	int nOfElements, minDim, row, col;
#ifdef CHECK_FOR_INF
	bool infiniteValueFound;
	double maxFiniteValue, infValue;
#endif

	/* initialization */
	*cost = 0;
	for (row = 0; row < nOfRows; row++)
#ifdef ONE_INDEXING
		assignment[row] = 0.0;
#else
		assignment[row] = -1.0;
#endif

	/* generate working copy of distance Matrix */
	/* check if all matrix elements are positive */
	nOfElements = nOfRows * nOfColumns;

	//distMatrix    = (double *)mxMalloc(nOfElements * sizeof(double));
	distMatrix = (double *)malloc(nOfElements * sizeof(double));

	distMatrixEnd = distMatrix + nOfElements;
	for (row = 0; row < nOfElements; row++)
	{
		value = distMatrixIn[row];
		//if(mxIsFinite(value) && (value < 0))
		//	mexErrMsgTxt("All matrix elements have to be non-negative.");
		if (value < 0)
		{
			printf("All matrix elements have to be non-negative.");
		}
		distMatrix[row] = value;
	}

#ifdef CHECK_FOR_INF
	/* check for infinite values */
	maxFiniteValue = -1;
	infiniteValueFound = false;

	distMatrixTemp = distMatrix;
	while (distMatrixTemp < distMatrixEnd)
	{
		value = *distMatrixTemp++;

		//if(mxIsFinite(value))
		if (MAX_INF == value)
		{
			if (value > maxFiniteValue)
				maxFiniteValue = value;
		}
		else
			infiniteValueFound = true;
	}
	if (infiniteValueFound)
	{
		if (maxFiniteValue == -1) /* all elements are infinite */
			return;

		/* set all infinite elements to big finite value */
		if (maxFiniteValue > 0)
			infValue = 10 * maxFiniteValue * nOfElements;
		else
			infValue = 10;
		distMatrixTemp = distMatrix;
		while (distMatrixTemp < distMatrixEnd)
			//if(mxIsInf(*distMatrixTemp++))
			if (MAX_INF != (*distMatrixTemp++))
			{
				*(distMatrixTemp - 1) = infValue;
			}
	}
#endif

	/* memory allocation */
	//coveredColumns = (bool *)mxCalloc(nOfColumns,  sizeof(bool));
	//coveredRows    = (bool *)mxCalloc(nOfRows,     sizeof(bool));
	//starMatrix     = (bool *)mxCalloc(nOfElements, sizeof(bool));
	//primeMatrix    = (bool *)mxCalloc(nOfElements, sizeof(bool));
	//newStarMatrix  = (bool *)mxCalloc(nOfElements, sizeof(bool)); /* used in step4 */


	coveredColumns = (bool *)malloc(nOfColumns * sizeof(bool));
	memset(coveredColumns, 0, nOfColumns * sizeof(bool));
	coveredRows = (bool *)malloc(nOfRows * sizeof(bool));
	memset(coveredRows, 0, nOfRows * sizeof(bool));
	starMatrix = (bool *)malloc(nOfElements * sizeof(bool));
	memset(starMatrix, 0, nOfElements * sizeof(bool));
	primeMatrix = (bool *)malloc(nOfElements * sizeof(bool));
	memset(primeMatrix, 0, nOfElements * sizeof(bool));
	newStarMatrix = (bool *)malloc(nOfElements * sizeof(bool)); /* used in step4 */
	memset(newStarMatrix, 0, nOfElements * sizeof(bool));
	/* preliminary steps */
	//轨迹到点迹的距离个数 小于 点迹到轨迹的距离个数 
	if (nOfRows <= nOfColumns)
	{
		minDim = nOfRows;
		//遍历轨迹数,求出点迹到轨迹中的最小值（列数最小），并将点迹标记为true
		for (row = 0; row < nOfRows; row++)
		{
			/* find the smallest element in the row */
			//寻找每列最小值
			distMatrixTemp = distMatrix + row;
			minValue = *distMatrixTemp;
			//每列偏移量即行数为：nOfRows
			distMatrixTemp += nOfRows;
			//求出每列最小值，（列 <= 行）
			while (distMatrixTemp < distMatrixEnd)
			{
				value = *distMatrixTemp;
				if (value < minValue)
					minValue = value;
				distMatrixTemp += nOfRows;
			}

			/* subtract the smallest element from each element of the row */
			//减去每列最小值，获得该列最优位置
			distMatrixTemp = distMatrix + row;

			while (distMatrixTemp < distMatrixEnd /*&& MAX_INF != minValue*/)
			{
				//if (MAX_INF != (*distMatrixTemp))
				//{
				*distMatrixTemp -= minValue;
				//}
				distMatrixTemp += nOfRows;
			}
		}

		/* Steps 1 and 2a */
		//标记每列减去最小值的下标信息
		for (row = 0; row < nOfRows; row++)
		{
			for (col = 0; col < nOfColumns; col++)
			{
				//printf("%0.3f      ", distMatrix[row + nOfRows*col]);
				if (distMatrix[row + nOfRows*col] == 0)
					if (!coveredColumns[col])
					{
						starMatrix[row + nOfRows*col] = true; //标识第row列第col行数据最优
						coveredColumns[col] = true; //标识col行有最优解，注：没有计算行的最优解
						break;
					}
			}
			//	printf("\n");
		}


	}
	else /* if(nOfRows > nOfColumns) */ //轨迹到点迹的距离个数 大于 点迹到轨迹的距离个数 
	{
		minDim = nOfColumns;
		//遍历点迹数，求出点迹到轨迹中的最小值（行数最小）
		for (col = 0; col < nOfColumns; col++)
		{
			/* find the smallest element in the column */
			//寻找每行最小值
			//获取每行的首个数据
			distMatrixTemp = distMatrix + nOfRows*col;
			//行的数据长度
			columnEnd = distMatrixTemp + nOfRows;

			minValue = *distMatrixTemp++;
			//获取行中最优解的值
			while (distMatrixTemp < columnEnd)
			{
				value = *distMatrixTemp++;
				if (value < minValue)
					minValue = value;
			}

			/* subtract the smallest element from each element of the column */
			//每行减去最优解的值，标识了该行最优解的位置
			distMatrixTemp = distMatrix + nOfRows*col;
			while (distMatrixTemp < columnEnd)
			{
				//if (MAX_INF != (*distMatrixTemp))
				//{
				*distMatrixTemp++ -= minValue;
				//}
				//distMatrixTemp++;
			}
		}

		/* Steps 1 and 2a */
		//标记每行减去最小值的最优解位置
		for (col = 0; col < nOfColumns; col++)
			for (row = 0; row < nOfRows; row++)
				if (distMatrix[row + nOfRows*col] == 0)
					if (!coveredRows[row])
					{
						starMatrix[row + nOfRows*col] = true; //row列col行最优
						coveredColumns[col] = true; //标识col行有最优解
						coveredRows[row] = true; //标识row列有最优解
						break;
					}
		for (row = 0; row < nOfRows; row++)
			coveredRows[row] = false;  //所有列中均无最优解

	}

	/* move to step 2b */
	step2b(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);

	/* compute cost and remove invalid assignments */
	computeassignmentcost(assignment, cost, distMatrixIn, nOfRows);

	/* free allocated memory */
	//mxFree(distMatrix);
	//mxFree(coveredColumns);
	//mxFree(coveredRows);
	//mxFree(starMatrix);
	//mxFree(primeMatrix);
	//mxFree(newStarMatrix);

	free(distMatrix);
	free(coveredColumns);
	free(coveredRows);
	free(starMatrix);
	free(primeMatrix);
	free(newStarMatrix);


	return;
}

/********************************************************/
//处理轨迹个数小于或等于点迹个数的情况
void buildassignmentvector(double *assignment, bool *starMatrix, int nOfRows, int nOfColumns)
{
	int row, col;
	//找出轨迹到点迹的最优下标
	for (row = 0; row < nOfRows; row++) //轨迹
		for (col = 0; col < nOfColumns; col++) //点迹
			if (starMatrix[row + nOfRows*col]) //轨迹到点迹的最优值
			{
#ifdef ONE_INDEXING
				assignment[row] = col + 1; /* MATLAB-Indexing */
#else
				assignment[row] = col;
#endif
				break;
			}
}

/********************************************************/
void computeassignmentcost(double *assignment, double *cost, double *distMatrix, int nOfRows)
{
	int row, col;
#ifdef CHECK_FOR_INF
	double value;
#endif

	for (row = 0; row < nOfRows; row++)
	{
#ifdef ONE_INDEXING
		col = assignment[row] - 1; /* MATLAB-Indexing */
#else
		col = assignment[row];
#endif

		if (col >= 0)
		{
#ifdef CHECK_FOR_INF
			value = distMatrix[row + nOfRows*col];
			//if(mxIsFinite(value))
			if (MAX_INF == value)
			{
				*cost += value;
			}
			else
#ifdef ONE_INDEXING
				assignment[row] = 0.0;
#else
				assignment[row] = -1.0;
#endif

#else
			*cost += distMatrix[row + nOfRows*col];
#endif
		}
	}
}

/********************************************************/
void step2a(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
	bool *starMatrixTemp, *columnEnd;
	int col;

	/* cover every column containing a starred zero */
	for (col = 0; col < nOfColumns; col++)
	{
		starMatrixTemp = starMatrix + nOfRows*col;
		columnEnd = starMatrixTemp + nOfRows;
		while (starMatrixTemp < columnEnd){
			if (*starMatrixTemp++)
			{
				coveredColumns[col] = true;
				break;
			}
		}
	}

	/* move to step 3 */
	step2b(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

/********************************************************/
void step2b(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
	int col, nOfCoveredColumns;

	/* count covered columns */
	nOfCoveredColumns = 0;
	//轨迹到点迹最优值的个数
	for (col = 0; col < nOfColumns; col++)
		if (coveredColumns[col])		//该行是否有最优解
			nOfCoveredColumns++;
	//轨迹个数小于或等于点迹个数	
	if (nOfCoveredColumns == minDim)
	{
		/* algorithm finished */
		buildassignmentvector(assignment, starMatrix, nOfRows, nOfColumns);
	}
	else
	{
		/* move to step 3 */
		step3(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
	}

}

/********************************************************/
//处理轨迹最优与点迹最优情况，下标标识：
//1、找到点迹中没有最优解的点迹
//2、找到该点迹到轨迹距离的最优解
//3、是否为第一个最优解
//是：step4处理
//不是：
//   a、将该行最优标识设置找到状态；
//   b、将第一个最优解的点迹设置为没有找到；
//	 c、重新遍历，知道将该点迹变为第一最优解；进入step4；
void step3(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
	bool zerosFound;
	int row, col, starCol;

	zerosFound = true;
	while (zerosFound)
	{
		zerosFound = false;
		//遍历点迹信息
		for (col = 0; col < nOfColumns; col++)
		{
			if (!coveredColumns[col]) //找出哪个点没有最优解
			{
				for (row = 0; row < nOfRows; row++) //遍历轨迹信息
				{
					if ((!coveredRows[row]) && (distMatrix[row + nOfRows*col] == 0)) //处理轨迹到点迹的最优解
					{
						/* prime zero */
						primeMatrix[row + nOfRows*col] = true; //row轨迹到点迹的最优解

						/* find starred zero in current row */
						for (starCol = 0; starCol < nOfColumns; starCol++) //遍历点迹信息,找到第一个最优解下标：就是该点迹到其他轨迹中的最优解
						{
							if (starMatrix[row + nOfRows*starCol]) //第一个最优解
								break;
						}


						if (starCol == nOfColumns) /* no starred zero found */
						{
							/* move to step 4 */
							step4(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim, row, col);
							return;
						}
						else
						{
							coveredRows[row] = true;  //轨迹找到最优解
							coveredColumns[starCol] = false; //将该点迹到其他轨迹的最优解设置为false
							zerosFound = true;
							break;
						}
					}
				}
			}
		}

	}

	/* move to step 5 */
	step5(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

/********************************************************/

void step4(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim, int row, int col)
{
	int n, starRow, starCol, primeRow, primeCol;
	int nOfElements = nOfRows*nOfColumns;

	/* generate temporary copy of starMatrix */
	//newStarMatrix : 点迹到轨迹的最优解
	for (n = 0; n < nOfElements; n++)
		newStarMatrix[n] = starMatrix[n];

	/* star current zero */
	//将处理点设置最优
	newStarMatrix[row + nOfRows*col] = true;

	/* find starred zero in current column */
	starCol = col;
	//遍历轨迹，找到该点点迹到轨迹的最优解，历史最优
	for (starRow = 0; starRow < nOfRows; starRow++)
		if (starMatrix[starRow + nOfRows*starCol])
			break;
	//找到历史最优轨迹starRow
	while (starRow < nOfRows)
	{
		/* unstar the starred zero */
		newStarMatrix[starRow + nOfRows*starCol] = false;

		/* find primed zero in current row */
		//轨迹最优的点迹：primeCol
		primeRow = starRow;
		for (primeCol = 0; primeCol < nOfColumns; primeCol++)
			if (primeMatrix[primeRow + nOfRows*primeCol])
				break;

		/* star the primed zero */
		newStarMatrix[primeRow + nOfRows*primeCol] = true;

		/* find starred zero in current column */
		starCol = primeCol;
		for (starRow = 0; starRow < nOfRows; starRow++)
			if (starMatrix[starRow + nOfRows*starCol])
				break;
	}

	/* use temporary copy as new starMatrix */
	/* delete all primes, uncover all rows */
	for (n = 0; n < nOfElements; n++)
	{
		primeMatrix[n] = false;
		starMatrix[n] = newStarMatrix[n];
	}
	for (n = 0; n < nOfRows; n++)
		coveredRows[n] = false;

	/* move to step 2a */
	step2a(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

/********************************************************/
//处理轨迹到点迹最优解信息:
//1、获取未找到最优解的轨迹，在这些轨迹中找到点迹到轨迹最小值h； 轨迹中的最优解
//2、获取找到最优解的轨迹，将轨迹到这些点的距离项 + h； 已有最优解 + h;（除未找到轨迹的点迹，全部+h）
//3、获取未匹配到最优轨迹的点迹，将该点迹到所有轨迹的距离项 - h； 
//获取，轨迹到点迹以及点迹到轨迹的最优解情况，此时待处理的轨迹row为未找到状态，到step3中处理
void step5(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
	double h, value;
	int row, col;

	/* find smallest uncovered element h */
	h = /*mxGetInf();	*/MAX_INF;
	//遍历轨迹到点迹的距离项，找出没有最优解轨迹，并获取轨迹到点迹的最小值
	for (row = 0; row < nOfRows; row++)
	{
		//是否已有路径匹配上该轨迹
		if (!coveredRows[row])
		{//无
			for (col = 0; col < nOfColumns; col++) //遍历点迹信息
			{
				//该点迹是否已被路径匹配上
				if (!coveredColumns[col])
				{//否
					//获取没有匹配路径的点迹距离
					value = distMatrix[row + nOfRows*col];
					//找出最小路劲距离h
					if (value < h)
					{
						h = value;
					}
				}
			}

		}

	}


	/* add h to each covered row */
	//遍历轨迹到点迹的距离项，将已找到轨迹到点迹的最优值的行，加上最小值
	for (row = 0; row < nOfRows; row++)
	{
		//找出已被匹配点迹的轨迹
		if (coveredRows[row])
		{
			//点迹信息遍历
			for (col = 0; col < nOfColumns; col++)
			{
				//将最小值加到以匹配路径的轨迹上
				distMatrix[row + nOfRows*col] += h;
			}
		}

	}


	/* subtract h from each uncovered column */
	//遍历点迹到轨迹的距离项，找出点迹未匹配最优的数据项，将所有轨迹到该点迹均减去最小值
	for (col = 0; col < nOfColumns; col++)
	{
		if (!coveredColumns[col])
		{
			for (row = 0; row < nOfRows; row++)
			{
				distMatrix[row + nOfRows*col] -= h;
			}
		}
	}


	/* move to step 3 */
	step3(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}
void swap(int *p1, int *p2)
{
	int temp;
	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}
