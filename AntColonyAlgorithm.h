#ifndef __ANTCOLONYALGORITHM__
#define __ANTCOLONYALGORITHM__
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "CmdParse.h"
#include <vector>
#include <iostream>
#include <random>
#include <time.h>
#include <memory>
#include <algorithm>
#include <string.h>
#include <iomanip>
#include <cstdlib>
#include <string>

//���������
#define ONE_INDEXING
#define MAX_INF 999999999999999999
#define FIGHT_PLANE_NUM 3
//#define PRINT_2D_MAP //�Ƿ���Ҫ���2D��ͼ
const int nindex3[3] = { -1, 0, 1 }; //���������̶���ȷ���±�
struct antPosition
{
	int x = -1;
	int y = -1;
	int z = -1;
	antPosition* next = NULL;
};

struct sAntPath
{
	//bool goodsstatus; //TRUE�����Ѿ�����

	antPosition *xy = NULL;//������� ·����Ϣx,y,z
	int npathLenth;//��������ӿ�ʼ�滮�㵽�ѻ����͵�Ŀ������Ҫ�ƶ����ܲ���
	sAntPath* next = NULL;
};
//�������������Լ��ɻ���״̬
struct sPlaneStatus
{
	int fx = -1, fy = -1, fz = 0;//���
	int goodsx, goodsy;//�����
	int dstx, dsty;//Ŀ�ĵ�
	int nID;//�ɻ���ID��ײ��֮�������
	bool istakeoff = false;//false ����δ��ɣ�true�������
	UAV_STATUS nStatus = UAV_NOMAL; //0��������-1��������
	int nGoodsStatus;//n�����Ѿ��滮·���Ļ��ţ�-1����δ�滮
	int nGoodValue = 0;
	int nisArrival;//0��ʾ���-1��ʾδ����

	//�²���
	int nGoodWeight;
	int remain_electricity = 0;
	int capacity=0;
	int charge=0;
	int nbirthTime;
	bool brandomPath = false;
	bool isFightPlane = false;//˳���һ�µз�ID��
	int nAttackID = -1;
	static int nNumofFightPlane;
	static int nEnemyParkingX;
	static int nEnemyParkingY;
	static int nWePlaneatEnemyParking;

	sAntPath* AntPath = NULL;
	sPlaneStatus* next = NULL;

};
//��Ⱥ�㷨����
const int  nPopNumber = 10; //��Ⱥ����
const int nMoveXMax = 1, nMoveYMax = 1, nMoveZMax = 1;//ÿ������ƶ�����
const float fDecr = 0.9;  //˥���ٶ�
const int nMapSize = 20;
const int nMapHigh = 9;
int Random(int start, int end);
float aver(int *a, int l);//������������a��ǰl��Ԫ�ص�ƽ��ֵ��;
float sum(int *a, int l);//������������a��ǰl��Ԫ�صĺ͡�;
//�ҵ�����Ϊparam2�����е����ֵ���±�
int findmax(float Q, int param2);
//��ʼ���ɻ�״̬����
void freememoryAntPosition(antPosition *xy);
void freememoryAntPath(sAntPath * temp);
void freememory(sPlaneStatus * temp);
void PlaneStatusinit(sPlaneStatus *sPlaneStatus1, FLAY_PLANE *pstFlayPlane, MAP_INFO * pstMap);
void PlaneCheck(sPlaneStatus * sPlaneStatus1, FLAY_PLANE * pstFlayPlane, MATCH_STATUS *pstMatchStatus, MAP_INFO *pstMap);
void GoodsStatusinit(int * pgoods, MAP_INFO * pstMapInfo);
void Antmemset(float * pPheromone, float param2, int nLength);
//����ʽ��ͼת��Ϊ����
void JSONMap22Darray(int * pMap, MAP_INFO * pstMapInfo);
void SearchAntPath(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO * pstMap);
antPosition* FindPath(sAntPath * p, MAP_INFO *pstMap,int startx,int starty,int startz,int x, int y, int *pMap, float *pPheromone);
void SearchAntPathPart(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO * pstMap);
void AntColonyAlgorithm(sPlaneStatus *AntPath, int *pMap, UAV uav, GOODS goods, MAP_INFO * pstMap);
void movenext(FLAY_PLANE * pstFlayPlane, int i, sPlaneStatus * q);
void AlgorithmCalculationFun(sPlaneStatus *sPlaneStatus1, int *pMap, MAP_INFO *pstMap, MATCH_STATUS * pstMatch, FLAY_PLANE *pstFlayPlane);
void DeletePlane(FLAY_PLANE * pstFlayPlane);
//���ӷ�ײ�߼�������Ϊ��·������ײ����ֵС��̧���ü�ֵ������ߣ�Ҳ������ssPlaneStatus1��Ӧ����������
//�����ڵ� ̧�� �� �½�
void CollisionDectection(FLAY_PLANE * pstFlayPlane, FLAY_PLANE * pstFlayPlaneLast, int * pMap, MAP_INFO * pstMapInfo, MATCH_STATUS * pstMatchStatus, sPlaneStatus * sPlaneStatus1);
//�ɻ�����
void PurchasePlane(FLAY_PLANE * pstFlayPlane, MAP_INFO *pstMap, MATCH_STATUS * pstMatchStatus, std::vector<int> *listofweight);
//���÷ɻ�״̬
void ResetPlaneStatus(FLAY_PLANE * pstFlayPlane, int i, sPlaneStatus * q);
void GoodsAllocation(sPlaneStatus * sPlaneStatus1, FLAY_PLANE * pstFlayPlane, MATCH_STATUS * pstMatchStatus, MAP_INFO *pstMap, std::vector<int> *listofweight);
int findindex(std::vector<int> a, int b);
int FindPlanetype(UAV astWeUav, MAP_INFO * pstMap);


void assignmentoptimal(double *assignment, double *cost, double *distMatrix, int nOfRows, int nOfColumns);
void buildassignmentvector(double *assignment, bool *starMatrix, int nOfRows, int nOfColumns);
void computeassignmentcost(double *assignment, double *cost, double *distMatrix, int nOfRows);
void step2a(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
void step2b(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
void step3(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
void step4(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim, int row, int col);
void step5(double *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
void swap(int *p1, int *p2);
#endif