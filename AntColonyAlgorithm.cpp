/* �Ȳ��ܶ�̬�ڴ��ͷ����⣬����һ��headָ�룬ѭ���ͷż���*/
//extern int nNumofFightPlane;
#include "AntColonyAlgorithm.h"
int sPlaneStatus::nNumofFightPlane = 0;
int sPlaneStatus::nEnemyParkingX = -1;
int sPlaneStatus::nEnemyParkingY = -1;
int sPlaneStatus::nWePlaneatEnemyParking = 0;
//������ָ������start~end֮����������
int Random(int start, int end) {
	int dis = end - start;
	return rand() % dis + start;
}
//ɾ��antPosition����
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
//ɾ��sAntPath����
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
//ɾ��sPlaneStatus����
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
float aver(int *a, int l)//������������a��ǰl��Ԫ�ص�ƽ��ֵ��
{
	float r = 0;
	int i;
	for (i = 0; i < l; i++)
		r += a[i];//�ۼӡ�
	r /= l;//ƽ��ֵ
	return r;//���ؽ����
}
float sum(float *a, int l)//������������a��ǰl��Ԫ�ص�ƽ��ֵ��
{
	float r = 0;
	int i;
	for (i = 0; i < l; i++)
		r += a[i];//�ۼӡ�
	return r;//���ؽ����
}
void uniform(float *a, int l)//������������a��ǰl��Ԫ�صĹ�һ����
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
// 2018��5��24��22:53:34�����¹��ܣ�
// �ҵ�ÿ���ɻ�����װ�ص�����
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
//ɾ���ٻ��ɻ��ڵ�,��������ڵ�
//Ӧ�ø���match��Ϣ���½ṹ��
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
			if (p->isFightPlane)//ɾ����׹�ٵ����˻�
			{
				p->isFightPlane = false;
				p->nNumofFightPlane--;
				p->nAttackID = -1;
			}
		}
		//��ӽڵ�
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
//��ʼ������״̬���󣬷�����价��
void GoodsStatusinit(int * pgoods, MAP_INFO * pstMapInfo)
{

}
//��ʼ���ڴ�,�ڴ���ÿ��λ��Ϊparam2
void Antmemset(float * pPheromone, float param2, int nLength)
{
	if (nLength > 2147483647)
	{
		std::cout << "���ݹ���" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < nLength;i++)
	{
		*(pPheromone + i) = param2;
	}
}
//����ʽ��ͼת��Ϊ����
//��ͼ��������ʽΪ��X,Y�� = *(pMap + X*pstMapInfo->nMapY + Y)
void JSONMap22Darray(int * pMap, MAP_INFO * pstMapInfo)
{
	
	for (int i = 0; i < pstMapInfo->nBuildingNum; i++)
	{
		//Y����ѭ��
		for (int k = pstMapInfo->astBuilding[i].nX; k <= pstMapInfo->astBuilding[i].nX + pstMapInfo->astBuilding[i].nL - 1; k++)
		{
			//X����ѭ��
			for (int j = pstMapInfo->astBuilding[i].nY; j <= pstMapInfo->astBuilding[i].nY + pstMapInfo->astBuilding[i].nW - 1; j++)
			{
				*(pMap + k*pstMapInfo->nMapY + j) = pstMapInfo->astBuilding[i].nH;// -pstMap->nHLow;Ӧ��������ʱ���Ƹ߶�
			}
		}
	}

	//��ӡ��ͼ����֤
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
//��������Ϊ�滮�ӽӻ����ͻ�������·��
void SearchAntPath(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO *pstMap)
{
	int astUavX = uav.nX, astUavY = uav.nY, astUavZ = uav.nZ;
	int astGoodsX = goods.nStartX, astGoodsY = goods.nStartY;
	// Ϊÿ����Ⱥ��һ��·��
	sAntPath *p = AntPath; 
	if (p->xy != NULL)
	{
		freememoryAntPosition(p->xy);
	}
	p->xy = new antPosition;
	p->npathLenth = 0;
	antPosition *q = FindPath(p, pstMap,astUavX, astUavY, astUavZ, astGoodsX, astGoodsY, pMap, pPheromone);//q->ӦΪ��ָ�� 
	//�������ص㣬����ȡ��
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
	//ȡ���������ͻ�
	while (q->z < pstMap->nHLow)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z + 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//�滮�ͻ�·��
	q->next = new antPosition; //2018��5��23��21:32:33�¼��룬������bug�Ĺؼ�
	astGoodsX = goods.nEndX; astGoodsY = goods.nEndY;
	q = FindPath(p, pstMap, q->x, q->y, q->z, astGoodsX, astGoodsY, pMap, pPheromone);
	//�������Ŀ�ĵأ������ͻ�
	while (q->z > 0)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z - 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//���ͻ����������
	while (q->z <= pstMap->nHLow)
	{
		q->next = new antPosition;
		ntempx = q->x, ntempy = q->y, ntempz = q->z + 1;
		q = q->next;
		q->x = ntempx; q->y = ntempy;
		q->z = ntempz;
		p->npathLenth++;
	}
	//�����ʼ������һ��
	//antPosition * ptemp = AntPath->xy;
	//antPosition * ptemp1 = new antPosition;
	//ptemp1->x = ptemp->x;
	//ptemp1->y = ptemp->y;
	//ptemp1->z = ptemp->z;
	//ptemp1->next = ptemp;
	//AntPath->xy = ptemp1;
}
//���������켣
void SearchAntPathPart(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO * pstMap)
{
	int astUavX = uav.nX, astUavY = uav.nY, astUavZ = uav.nZ;
	int astGoodsX = goods.nStartX, astGoodsY = goods.nStartY;
	// Ϊÿ����Ⱥ��һ��·��
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
	////����ͼת��Ϊ2D���飬�洢x-y-h��Ϊ��ͼʹ�ã�������ʱ��Ϊ�ǿյ�,���ֻ��һ�Σ���󽫴˹��̷���mian������ʼ�����ڵ�����
	if (goods.nNO == -1)//û�л���ʱһ��Ҫ��-1
	{
		AntPath->fx = uav.nX;
		AntPath->fy = uav.nY;
		AntPath->fz = uav.nZ;
		AntPath->goodsx = goods.nStartX;
		AntPath->goodsy = goods.nStartY;
		AntPath->dstx = goods.nEndX;
		AntPath->dsty = goods.nEndY;
		//��Ϣ��Ũ�ȣ���СΪX*Y*H
		//������ʽ��X,Y,Z�� = X*W + Y + Z *W*L
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
		//��Ϣ��Ũ�ȣ���СΪX*Y*H
		//������ʽ��X,Y,Z�� = X*W + Y + Z *W*L
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
	q1->next = NULL;//һ��Ҫ������ֹ������������·���ͷš�����
	freememoryAntPosition(q1);//�ͷ��ڴ�
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
*  @brief	ѧ�����㷨���㣬 ����ʲô�Ķ��Լ�д��
*	@return void
*/
void  AlgorithmCalculationFun(sPlaneStatus *sPlaneStatus1, int *pMap, MAP_INFO *pstMap, MATCH_STATUS * pstMatch, FLAY_PLANE *pstFlayPlane)
{
	//2018��5��22��09:29:53���ڵ�bugӦ�����������֮�󣬻��ﲻ���ڣ�����ֱ�ӹ滮��һ��������´���
	//2018��5��22��10:17:13���Ը���QAQ��ȡ���ﷸ��(1���ж�ʧ�����������߳����������û����λ���ܻ�ȡ����)������ε�����������(����֤��ײ��)��ȡ��������
	//������pstflayplane�����㣬�������ִ���
	//���ﱻȡ��Ҳ������ڻ����б��У����Ի�Ҫ���з��ɻ��Ƿ�ȡ���˻����ȡ��Ҫ���·��䡣���ҷ���֮��
	//���������״̬
	//2018��5��23��20:55:55��������˷�ײ�߼������кܶ����⣬���⽫·���滮д��FindPath�ļ��󣬳����˷ɻ�ײǽBUG�������
	sPlaneStatus *q = sPlaneStatus1;
	for (int i = 0; i < pstMatch->nUavWeNum; i++)//���i��q��Ӧ��iÿ��1��q����һ���Ϳ�����
	{
		//�жϷɻ��Ƿ���
		if (pstMatch->astWeUav[i].nStatus == UAV_CRASH)//������ڣ���������
		{
			q = q->next;
			continue;
		}
		else
		{
			//�����ж��Ƿ����
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
					else //��û����ȥһ������ĵط�
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
						q->AntPath->npathLenth--;//Ӧ�����ڷ������
						//�����������nHLowʱ���Żᴥ���ͷ�·��
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
					//�ж��Ƿ񵽴����ص㣬��ȡ��
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

					if (q->AntPath->npathLenth == pstMap->nHLow)//���û������
					{
						//pstFlayPlane->astUav[i].nGoodsNo = -1;
					}
					q->AntPath->npathLenth--;//Ӧ�����ڷ������
											 //�����������nHLowʱ���Żᴥ���ͷ�·��
					if (q->AntPath->npathLenth == 0)
					{
						q->nGoodsStatus = -1;
						q->nGoodValue = 0;
						freememoryAntPath(q->AntPath);
						q->AntPath = NULL;
					}
				}
				else if (q->isFightPlane && q->AntPath == NULL)//��ս����������û·��,֮���������жϣ���Ŀ����������
				{
					int itenemy, xx = q->nEnemyParkingX, yy = q->nEnemyParkingY, zz = 0;
					for (int itenemyNum = 0; itenemyNum < pstMatch->nUavEnemyNum;itenemyNum++)
					{
						if (pstMatch->astEnemyUav[itenemyNum].nNO == q->nAttackID)
						{
							if (pstMatch->astEnemyUav[itenemyNum].nGoodsNo != -1)//�Է��Ƿ��õ�����
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
									// �����������Ѿ�����������ֱ��ȥ�������ڵ�λ��
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
					q->AntPath->npathLenth--;//Ӧ�����ڷ������
					//�����������nHLowʱ���Żᴥ���ͷ�·��
					if (q->AntPath->npathLenth == 0)
					{
						q->nGoodsStatus = -1;
						q->nGoodValue = 0;
						freememoryAntPath(q->AntPath);
						q->AntPath = NULL;
						q->brandomPath = true;
					}
				}
				else if (q->isFightPlane && q->AntPath != NULL)//��ս����������·��
				{
					movenext(pstFlayPlane, i, q);
					q->AntPath->npathLenth--;//Ӧ�����ڷ������
					//�����������nHLowʱ���Żᴥ���ͷ�·��
					if (q->AntPath->npathLenth == 0)
					{
						q->nGoodsStatus = -1;
						q->nGoodValue = 0;
						freememoryAntPath(q->AntPath);
						q->AntPath = NULL;
					}
				}
			}
			else//��û��ɣ������
			{
				bool flag = true;
				for (int m = 0; m < pstFlayPlane->nUavNum; m++)
				{
					if (pstFlayPlane->astUav[i].nZ + 1 == pstFlayPlane->astUav[m].nZ && pstFlayPlane->astUav[i].nX == pstFlayPlane->astUav[m].nX && pstFlayPlane->astUav[i].nY == pstFlayPlane->astUav[m].nY)
					{
						flag = false;
					}
				}
				if (pstFlayPlane->astUav[i].nZ >= pstMap->nHLow)//����͸߶ȼ�1�������ڷɻ���ɺͻ����켣
				{
					q->istakeoff = true;
				}
				//if (flag && pstFlayPlane->astUav[i].nZ <pstMap->nHLow - 1 && pstFlayPlane->astUav[i].remain_electricity == q->capacity)
				if (flag && pstFlayPlane->astUav[i].nZ < pstMap->nHLow)
				{
					// ��绹�����
					if (pstFlayPlane->astUav[i].nX == pstMap->nParkingX && pstFlayPlane->astUav[i].nY == pstMap->nParkingY && pstFlayPlane->astUav[i].remain_electricity != q->capacity)
					{
						pstFlayPlane->astUav[i].remain_electricity += q->charge;
						if (pstFlayPlane->astUav[i].remain_electricity >= q->capacity)
						{
							pstFlayPlane->astUav[i].remain_electricity = q->capacity;
						}
						q->remain_electricity = pstFlayPlane->astUav[i].remain_electricity;
					}
					else//��֮��Ϳ��������
					{
						pstFlayPlane->astUav[i].nZ++;
						q->fz++;
					}
				}
				else//����һ�ε磬���������е��ŵģ����Ի�Ҫ�������
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
				if (pstFlayPlane->astUav[i].nZ >= pstMap->nHLow)//����͸߶ȼ�1�������ڷɻ���ɺͻ����켣
				{
					q->istakeoff = true;
				}
			}
		}
		q = q->next;
	}
}
//ɾ��FLAY_PLANE�ṹ����ײ�ٵķɻ�����������������ȷ�ķɻ�������λ��
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
//��ʱֻ�ܷ�ֹ����ײ,������Ҫ��֮ǰ����Ϣ������������ǰһ֡λ�ú͵�ͼ��Ϣ
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
			//������λ����ͬ�ģ����ж�Ϊ��ײ
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
				//Bug:�п��ܻᳬ�����߶�
				if (p2->AntPath != NULL)
				{
					if (p2->AntPath->xy->z + 1 <= pstMapInfo->nHHigh )
					{
						antPosition *temp = p2->AntPath->xy;
						//������,ͦ����
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
						//���½�
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
						//���½�
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
						//������
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
	//����ɻ�
	//�������Ϊ���δ����Ļ���ڻ���ѡ�����м�����������ɻ����������ɻ������˻������
	//��δ���䣬�Ͳ�����
	std::vector<int> listofplane;
	for (int i = 0; i < pstMap->nUavPriceNum;i++)
	{
		listofplane.push_back(pstMap->astUavPrice[i].nLoadWeight);
	}
	sort(listofplane.begin(), listofplane.end());
	//for (int i = listofweight->size() - 1; i >= 0;i--)
	for (int i = 0; i < listofweight->size(); i++)
	{
		//�ҵ�������Ӧ�ķɻ�����(����)���ڸ�Ϊ������˵ģ�����ʲôЧ��
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
	//1 2λ��д��ɻ���ʼλ��
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
	//Ϊ��k����Ⱥ�ҳ�һ��·��
	while (q->y != astGoodsY || q->x != astGoodsX)//����ǰ��xyû�е���ʱ���ҵ���һ��·�������ԣ�����յ������㣬�������ɹ켣
	{
		//��û�е��������ҵ���һ��
		q->next = new antPosition;
		//����ɵ�����ӵ���Ӧ��
		float Q[11] = { 0.0 };
		int nq = 0;
		for (int i = -nMoveXMax; i <= nMoveXMax; i++)//Y
		{
			for (int j = -nMoveYMax; j <= nMoveYMax; j++)//X
			{
				if (q->x + i >= 0 && q->x + i <= pstMap->nMapX - 1 && q->y + j >= 0 && q->y + j <= pstMap->nMapY - 1)//Ӧ��-1
				{
					//�ж�Ŀ����Ƿ���ڿ��ӷ�Χ��,���ڣ�ֱ���˳�ѭ��
					if (q->x + i == astGoodsX &&  q->y + j == astGoodsY)
					{
						int ntempx = q->x + i, ntempy = q->y + j, ntempz = q->z;
						q = q->next;
						q->x = ntempx; q->y = ntempy;
						q->z = ntempz;
						p->npathLenth++;
						break;
					}
					//��������ֵ�������ģ�ͣ�Q = S*D��SΪ��һ���Ƿ�ɵ��Ҳ�����Ƿ���bulidings��DΪ��һ�㵽Ŀ�������ƽ��
					if (q->z - *(pMap + (q->x + i)*pstMap->nMapY + q->y + j) > 0)//���ܳ���9
					{
						Q[nq++] = 50 * *(pPheromone + (q->x + i)*pstMap->nMapY + q->y + j + q->z*pstMap->nMapX*pstMap->nMapY) / sqrtf(float((((q->x + i - astGoodsX) * (q->x + i - astGoodsX))) + ((q->y + j - astGoodsY) * (q->y + j - astGoodsY)) + (q->z * q->z)));//50ΪȨֵ,��СӰ��Ӧ�ò��󣬴���
					}
					else
					{
						Q[nq++] = 0;
					}

					//����Ϣ�ؾ����Ϊ���飬�Ա��ڳ�ʼ��Ϊ1;
					//�������ּ�����1���߹�����Ϣ��˥����ʹ����Ⱥ��·����Ԫ��
					//              2�����·������Ϣ�ؼӴ�ʹ�����·�����ָ��ʱ��

				}
				else { Q[nq++] = 0; }
			}
			if (q->x == astGoodsX &&  q->y == astGoodsY) break;//�ж�Ŀ����Ƿ���ڿ��ӷ�Χ��,���ڣ�ֱ���˳�ѭ��
		}
		if (q->x == astGoodsX &&  q->y == astGoodsY) break;//�ж�Ŀ����Ƿ���ڿ��ӷ�Χ��,���ڣ�ֱ����
														   //���ӹ���������֮�󣬽������������������ӣ�һ��11��(!!���Բ�����������ĸ���)
		if (q->z + 1 <= pstMap->nHHigh)
		{
			Q[nq++] = 50 * (*(pPheromone + q->x*pstMap->nMapY + q->y + q->z*pstMap->nMapX*pstMap->nMapY)) / sqrtf(float(((q->x - astGoodsX) * (q->x - astGoodsX)) + (q->y - astGoodsY) * (q->y - astGoodsY )+ (q->z + 1)*(q->z + 1)));//50ΪȨֵ,��СӰ��Ӧ�ò��󣬴���
		}
		//��ѭ��
		//uniform(Q, ++nq);
		//�����������׼�����̶�
		//!!�����̶���������
		//float froll = float(d(e));
		int   nindex = 0;//��ʾ���������±�
		nindex = findmax(Q, 11);
		int ntempx, ntempy, ntempz;
		if (nindex < 9)//С��9��������
		{
			ntempx = q->x + nindex3[(nindex) / 3]; ntempy = q->y + nindex3[(nindex) % 3]; ntempz = q->z;
			q = q->next;
			q->x = ntempx; q->y = ntempy;
			q->z = ntempz;
			p->npathLenth++;
		} //����9����������Ǹ�����
		else
		{
			ntempx = q->x; ntempy = q->y; ntempz = q->z + 1;
			q = q->next;
			q->x = ntempx; q->y = ntempy;
			q->z = ntempz;
			p->npathLenth++;
		}
		//������Ϣ��Ũ��
		*(pPheromone + q->x*pstMap->nMapY + q->y + q->z*pstMap->nMapX*pstMap->nMapY) *= 0.5;
	}
	return q;
}
void GoodsAllocation(sPlaneStatus * sPlaneStatus1, FLAY_PLANE * pstFlayPlane, MATCH_STATUS * pstMatchStatus, MAP_INFO *pstMap, std::vector<int> *listofweight)
{
//#define TEST_ASSIGNMENT
	// �ҳ�δ������ķɻ�N�ͻ���M,�����Ӧ��ID��
	std::vector<int> M, N, Allocated;
	std::vector<int> vPlaneNo, vGoodsNo;//��˳�������������ɻ��ͻ����ID��
	sPlaneStatus *q = sPlaneStatus1;
	while (q != NULL && pstMatchStatus->nTime >=pstMap->nHLow +0)//15
	{
		// ��δ������ķɻ�����������ﱻ�з����ߵķɻ�
		//N.push_back(q->nID);
		if (q->nGoodsStatus == -1 && q->nStatus != UAV_CRASH && !q->isFightPlane )					// û�з���ģ�ֱ�ӽ���δ�����б�
		{
			N.push_back(q->nID); 
		}
		// ���ѱ�����Ļ���
		if (q->nGoodsStatus != -1 && q->nStatus != UAV_CRASH && !q->isFightPlane )					// �ѷ���ģ�û��5time���¹���һ��
		{
			int flag = false;//�����Ƿ���ʧ
			for (int i = 0; i < pstMatchStatus->nGoodsNum; i ++)//����ʧʱ�����if���ᴥ��
			{
				if (pstMatchStatus->astGoods[i].nNO == q->nGoodsStatus)
				{
					flag = true;//���ﻹ�ڳ�
					if (pstMatchStatus->astGoods[i].nState == 0)//�����ڳ�����û������
					{
						//
						//if (pstMatchStatus->nTime %20 == 0)//ÿ��5time�����·���һ��
						//{
						//	if (q->nGoodValue != 1)
						//	{
						//		q->nGoodsStatus = -1;
						//		q->istakeoff = false;
						//		freememoryAntPath(q->AntPath);
						//		q->AntPath = NULL;
						//		N.push_back(q->nID);//�ѷ��䣬��ʱ�䵽��Ҫ���·���
						//	}
						//}
						//else
							//
						{
							Allocated.push_back(q->nGoodsStatus);
						}
					}
					else//������
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
									N.push_back(q->nID);//�ѷ��䣬���Ǳ��з������ˣ������ѷ���
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
								N.push_back(q->nID);//�ѷ��䣬���Ǳ��з������ˣ������ѷ���
							}
						}
					}	
				}	
			}
			if (!flag)//�ѷ���û�����ߣ�������ʧ��  ��ʱ
			{
				N.push_back(q->nID);
				//����q
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
		if (find(Allocated.begin(), Allocated.end(), pstMatchStatus->astGoods[i].nNO) == Allocated.end())//������ѷ�����û���Ҽ�
		{
			M.push_back(pstMatchStatus->astGoods[i].nNO);
		}
		vGoodsNo.push_back(pstMatchStatus->astGoods[i].nNO);
	}
	for (int i = 0; i < pstMatchStatus->nUavWeNum; i++)
	{
		vPlaneNo.push_back(pstMatchStatus->astWeUav[i].nNO);
	}
	// ���ȼ��������󣬾����СΪN*M������MΪδ�������ķɻ�����NΪδ������Ļ�����

	double *pCostMat = (double *)malloc(sizeof(double) * M.size() * N.size());
	bool   *pisOverElectricity = (bool *)malloc(sizeof(bool) * M.size() * N.size());
	memset(pisOverElectricity, 0, sizeof(bool) * M.size() * N.size());
	//double pCostMat[] = { 0,1,1,1,1,1, 1,0,1,1,1,1, 1,1,0,1,1,1, 1,1,1,0,1,1, 1,1,1,1,1,0, 1,1,1,1,0,1 };
	// ��¼��Ӧλ�õ��±�
	int xa, xb, ya, yb, xc, yc;
	int lx = 0, ly = 0;
	int lax = 0, lay = 0, lbx = 0, lby = 0;
	int it = 0;
	int nWeightplane, nWeightGoods, nValue;
	int remain_electricity;
	double dist,distend;
	for (int i = 0; i < N.size();i++)//i��j��
	{//N[i]
		//�ɻ���Ϣ
		it = findindex(vPlaneNo, N[i]);
		xa = pstMatchStatus->astWeUav[it].nX;
		ya = pstMatchStatus->astWeUav[it].nY;
		nWeightplane = pstMatchStatus->astWeUav[it].nLoadWeight;
		remain_electricity = pstMatchStatus->astWeUav[it].remain_electricity;
		//it = FindPlanetype(pstMatchStatus->astWeUav[i], pstMap);
		//nWeightplane = pstMatchStatus->astWeUav[it].nLoadWeight;
		for (int j = 0; j < M.size(); j++)
		{//M[j]
			//������Ϣ
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
			if (lax < lay) swap(&lax, &lay);// ��xΪ�ϴ���
			lx = abs(xc - xb); ly = abs(yc - yb);
			if (lx < ly) swap(&lx, &ly);
			dist = lax + 15;
			distend = lx + pstMap->nHLow*2 +10;
			//����
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
	assignmentoptimal(assignment, &Cost, pCostMat1, 6, 6);//��ʱ��������̫���������ȫ������
	for (int i = 0; i < 6;i++)
	{
		std::cout << assignment[i] << " ";
	}
#endif
	assignmentoptimal(assignment, &Cost, pCostMat, N.size(), M.size());//��ʱ��������̫���������ȫ������
	//����һ�������б�
	// step1:�ҳ�δ����ķɻ�
	std::vector<int> listofasmgoods;
	// ֮�󽫷����������ṹͼ
	q = sPlaneStatus1;
	int it1, it2;
	while (q != NULL)
	{
		for (int i = 0; i < N.size();i++)
		{
			int j = int(*(assignment + i))-1;// ������Ӧ��-1����Ϊ�Ǵ�1��ʼ��
			it1 = findindex(vPlaneNo, N[i]);
			if (j != -1)
			{
				it2 = findindex(vGoodsNo, M[j]);
				if (q->nID == pstMatchStatus->astWeUav[it1].nNO)
				{
					//����ͬ����Ҫ�жϵ���
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
	// �����������С���ܻ�������������Ҫ��ɻ�
	// �ҵ�δ������������
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
	// ����δ����ɻ����������˵���Ϊ������
	// ����Ӧ��ֱ�ӷ���õз��ɻ�ID
	q = sPlaneStatus1;
	
	while (q->next != NULL && q->nNumofFightPlane <FIGHT_PLANE_NUM)
	{
		if (q->isFightPlane)//����ǹ����������һ��ԭ���з�Ŀ���Ƿ��ڴ���
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
			// �ҵ���ֵ���з�δ����ķɻ�
			// �����Ƿ���δ�����F5 - F1 - F2 - F4 - F3
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
					//�ҵ���i��ķɻ�
					if (strcmp(pstMatchStatus->astEnemyUav[i].szType, aaa) == 0 && pstMatchStatus->astEnemyUav[i].nStatus != UAV_FOG)
					{
						flag = false;
						sPlaneStatus *p = sPlaneStatus1;
						while (p->next != NULL)//��ܷɻ���û�б�����
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
					if (!flag)//û���䣬����
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
	std::cout << "�޴˷ɻ����ͣ�" << std::endl;
	return -1;
}

// �������㷨
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
	//�켣���㼣�ľ������ С�� �㼣���켣�ľ������ 
	if (nOfRows <= nOfColumns)
	{
		minDim = nOfRows;
		//�����켣��,����㼣���켣�е���Сֵ��������С���������㼣���Ϊtrue
		for (row = 0; row < nOfRows; row++)
		{
			/* find the smallest element in the row */
			//Ѱ��ÿ����Сֵ
			distMatrixTemp = distMatrix + row;
			minValue = *distMatrixTemp;
			//ÿ��ƫ����������Ϊ��nOfRows
			distMatrixTemp += nOfRows;
			//���ÿ����Сֵ������ <= �У�
			while (distMatrixTemp < distMatrixEnd)
			{
				value = *distMatrixTemp;
				if (value < minValue)
					minValue = value;
				distMatrixTemp += nOfRows;
			}

			/* subtract the smallest element from each element of the row */
			//��ȥÿ����Сֵ����ø�������λ��
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
		//���ÿ�м�ȥ��Сֵ���±���Ϣ
		for (row = 0; row < nOfRows; row++)
		{
			for (col = 0; col < nOfColumns; col++)
			{
				//printf("%0.3f      ", distMatrix[row + nOfRows*col]);
				if (distMatrix[row + nOfRows*col] == 0)
					if (!coveredColumns[col])
					{
						starMatrix[row + nOfRows*col] = true; //��ʶ��row�е�col����������
						coveredColumns[col] = true; //��ʶcol�������Ž⣬ע��û�м����е����Ž�
						break;
					}
			}
			//	printf("\n");
		}


	}
	else /* if(nOfRows > nOfColumns) */ //�켣���㼣�ľ������ ���� �㼣���켣�ľ������ 
	{
		minDim = nOfColumns;
		//�����㼣��������㼣���켣�е���Сֵ��������С��
		for (col = 0; col < nOfColumns; col++)
		{
			/* find the smallest element in the column */
			//Ѱ��ÿ����Сֵ
			//��ȡÿ�е��׸�����
			distMatrixTemp = distMatrix + nOfRows*col;
			//�е����ݳ���
			columnEnd = distMatrixTemp + nOfRows;

			minValue = *distMatrixTemp++;
			//��ȡ�������Ž��ֵ
			while (distMatrixTemp < columnEnd)
			{
				value = *distMatrixTemp++;
				if (value < minValue)
					minValue = value;
			}

			/* subtract the smallest element from each element of the column */
			//ÿ�м�ȥ���Ž��ֵ����ʶ�˸������Ž��λ��
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
		//���ÿ�м�ȥ��Сֵ�����Ž�λ��
		for (col = 0; col < nOfColumns; col++)
			for (row = 0; row < nOfRows; row++)
				if (distMatrix[row + nOfRows*col] == 0)
					if (!coveredRows[row])
					{
						starMatrix[row + nOfRows*col] = true; //row��col������
						coveredColumns[col] = true; //��ʶcol�������Ž�
						coveredRows[row] = true; //��ʶrow�������Ž�
						break;
					}
		for (row = 0; row < nOfRows; row++)
			coveredRows[row] = false;  //�������о������Ž�

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
//����켣����С�ڻ���ڵ㼣���������
void buildassignmentvector(double *assignment, bool *starMatrix, int nOfRows, int nOfColumns)
{
	int row, col;
	//�ҳ��켣���㼣�������±�
	for (row = 0; row < nOfRows; row++) //�켣
		for (col = 0; col < nOfColumns; col++) //�㼣
			if (starMatrix[row + nOfRows*col]) //�켣���㼣������ֵ
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
	//�켣���㼣����ֵ�ĸ���
	for (col = 0; col < nOfColumns; col++)
		if (coveredColumns[col])		//�����Ƿ������Ž�
			nOfCoveredColumns++;
	//�켣����С�ڻ���ڵ㼣����	
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
//����켣������㼣����������±��ʶ��
//1���ҵ��㼣��û�����Ž�ĵ㼣
//2���ҵ��õ㼣���켣��������Ž�
//3���Ƿ�Ϊ��һ�����Ž�
//�ǣ�step4����
//���ǣ�
//   a�����������ű�ʶ�����ҵ�״̬��
//   b������һ�����Ž�ĵ㼣����Ϊû���ҵ���
//	 c�����±�����֪�����õ㼣��Ϊ��һ���Ž⣻����step4��
void step3(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
	bool zerosFound;
	int row, col, starCol;

	zerosFound = true;
	while (zerosFound)
	{
		zerosFound = false;
		//�����㼣��Ϣ
		for (col = 0; col < nOfColumns; col++)
		{
			if (!coveredColumns[col]) //�ҳ��ĸ���û�����Ž�
			{
				for (row = 0; row < nOfRows; row++) //�����켣��Ϣ
				{
					if ((!coveredRows[row]) && (distMatrix[row + nOfRows*col] == 0)) //����켣���㼣�����Ž�
					{
						/* prime zero */
						primeMatrix[row + nOfRows*col] = true; //row�켣���㼣�����Ž�

						/* find starred zero in current row */
						for (starCol = 0; starCol < nOfColumns; starCol++) //�����㼣��Ϣ,�ҵ���һ�����Ž��±꣺���Ǹõ㼣�������켣�е����Ž�
						{
							if (starMatrix[row + nOfRows*starCol]) //��һ�����Ž�
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
							coveredRows[row] = true;  //�켣�ҵ����Ž�
							coveredColumns[starCol] = false; //���õ㼣�������켣�����Ž�����Ϊfalse
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
	//newStarMatrix : �㼣���켣�����Ž�
	for (n = 0; n < nOfElements; n++)
		newStarMatrix[n] = starMatrix[n];

	/* star current zero */
	//���������������
	newStarMatrix[row + nOfRows*col] = true;

	/* find starred zero in current column */
	starCol = col;
	//�����켣���ҵ��õ�㼣���켣�����Ž⣬��ʷ����
	for (starRow = 0; starRow < nOfRows; starRow++)
		if (starMatrix[starRow + nOfRows*starCol])
			break;
	//�ҵ���ʷ���Ź켣starRow
	while (starRow < nOfRows)
	{
		/* unstar the starred zero */
		newStarMatrix[starRow + nOfRows*starCol] = false;

		/* find primed zero in current row */
		//�켣���ŵĵ㼣��primeCol
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
//����켣���㼣���Ž���Ϣ:
//1����ȡδ�ҵ����Ž�Ĺ켣������Щ�켣���ҵ��㼣���켣��Сֵh�� �켣�е����Ž�
//2����ȡ�ҵ����Ž�Ĺ켣�����켣����Щ��ľ����� + h�� �������Ž� + h;����δ�ҵ��켣�ĵ㼣��ȫ��+h��
//3����ȡδƥ�䵽���Ź켣�ĵ㼣�����õ㼣�����й켣�ľ����� - h�� 
//��ȡ���켣���㼣�Լ��㼣���켣�����Ž��������ʱ������Ĺ켣rowΪδ�ҵ�״̬����step3�д���
void step5(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
	double h, value;
	int row, col;

	/* find smallest uncovered element h */
	h = /*mxGetInf();	*/MAX_INF;
	//�����켣���㼣�ľ�����ҳ�û�����Ž�켣������ȡ�켣���㼣����Сֵ
	for (row = 0; row < nOfRows; row++)
	{
		//�Ƿ�����·��ƥ���ϸù켣
		if (!coveredRows[row])
		{//��
			for (col = 0; col < nOfColumns; col++) //�����㼣��Ϣ
			{
				//�õ㼣�Ƿ��ѱ�·��ƥ����
				if (!coveredColumns[col])
				{//��
					//��ȡû��ƥ��·���ĵ㼣����
					value = distMatrix[row + nOfRows*col];
					//�ҳ���С·������h
					if (value < h)
					{
						h = value;
					}
				}
			}

		}

	}


	/* add h to each covered row */
	//�����켣���㼣�ľ���������ҵ��켣���㼣������ֵ���У�������Сֵ
	for (row = 0; row < nOfRows; row++)
	{
		//�ҳ��ѱ�ƥ��㼣�Ĺ켣
		if (coveredRows[row])
		{
			//�㼣��Ϣ����
			for (col = 0; col < nOfColumns; col++)
			{
				//����Сֵ�ӵ���ƥ��·���Ĺ켣��
				distMatrix[row + nOfRows*col] += h;
			}
		}

	}


	/* subtract h from each uncovered column */
	//�����㼣���켣�ľ�����ҳ��㼣δƥ�����ŵ�����������й켣���õ㼣����ȥ��Сֵ
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
