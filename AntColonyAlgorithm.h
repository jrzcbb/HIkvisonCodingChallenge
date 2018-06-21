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

//构建随机数
#define ONE_INDEXING
#define MAX_INF 999999999999999999
#define FIGHT_PLANE_NUM 3
//#define PRINT_2D_MAP //是否需要检查2D地图
const int nindex3[3] = { -1, 0, 1 }; //用来在轮盘赌中确定下标
struct antPosition
{
	int x = -1;
	int y = -1;
	int z = -1;
	antPosition* next = NULL;
};

struct sAntPath
{
	//bool goodsstatus; //TRUE代表已经拿上

	antPosition *xy = NULL;//链表，存放 路径信息x,y,z
	int npathLenth;//整数代表从开始规划点到把货物送到目的所需要移动的总步数
	sAntPath* next = NULL;
};
//链表，用来索引自己飞机的状态
struct sPlaneStatus
{
	int fx = -1, fy = -1, fz = 0;//起点
	int goodsx, goodsy;//货物点
	int dstx, dsty;//目的地
	int nID;//飞机的ID，撞毁之后会跳过
	bool istakeoff = false;//false 代表未起飞，true代表起飞
	UAV_STATUS nStatus = UAV_NOMAL; //0代表健康，-1代表销毁
	int nGoodsStatus;//n代表已经规划路径的货号，-1代表未规划
	int nGoodValue = 0;
	int nisArrival;//0表示到达，-1表示未到达

	//新参数
	int nGoodWeight;
	int remain_electricity = 0;
	int capacity=0;
	int charge=0;
	int nbirthTime;
	bool brandomPath = false;
	bool isFightPlane = false;//顺便存一下敌方ID号
	int nAttackID = -1;
	static int nNumofFightPlane;
	static int nEnemyParkingX;
	static int nEnemyParkingY;
	static int nWePlaneatEnemyParking;

	sAntPath* AntPath = NULL;
	sPlaneStatus* next = NULL;

};
//蚁群算法参数
const int  nPopNumber = 10; //种群个数
const int nMoveXMax = 1, nMoveYMax = 1, nMoveZMax = 1;//每次最大移动距离
const float fDecr = 0.9;  //衰退速度
const int nMapSize = 20;
const int nMapHigh = 9;
int Random(int start, int end);
float aver(int *a, int l);//计算整型数组a，前l个元素的平均值。;
float sum(int *a, int l);//计算整型数组a，前l个元素的和。;
//找到长度为param2的数列的最大值的下标
int findmax(float Q, int param2);
//初始化飞机状态链表
void freememoryAntPosition(antPosition *xy);
void freememoryAntPath(sAntPath * temp);
void freememory(sPlaneStatus * temp);
void PlaneStatusinit(sPlaneStatus *sPlaneStatus1, FLAY_PLANE *pstFlayPlane, MAP_INFO * pstMap);
void PlaneCheck(sPlaneStatus * sPlaneStatus1, FLAY_PLANE * pstFlayPlane, MATCH_STATUS *pstMatchStatus, MAP_INFO *pstMap);
void GoodsStatusinit(int * pgoods, MAP_INFO * pstMapInfo);
void Antmemset(float * pPheromone, float param2, int nLength);
//将格式地图转化为数组
void JSONMap22Darray(int * pMap, MAP_INFO * pstMapInfo);
void SearchAntPath(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO * pstMap);
antPosition* FindPath(sAntPath * p, MAP_INFO *pstMap,int startx,int starty,int startz,int x, int y, int *pMap, float *pPheromone);
void SearchAntPathPart(float * pPheromone, int * pMap, sAntPath *AntPath, UAV uav, GOODS goods, MAP_INFO * pstMap);
void AntColonyAlgorithm(sPlaneStatus *AntPath, int *pMap, UAV uav, GOODS goods, MAP_INFO * pstMap);
void movenext(FLAY_PLANE * pstFlayPlane, int i, sPlaneStatus * q);
void AlgorithmCalculationFun(sPlaneStatus *sPlaneStatus1, int *pMap, MAP_INFO *pstMap, MATCH_STATUS * pstMatch, FLAY_PLANE *pstFlayPlane);
void DeletePlane(FLAY_PLANE * pstFlayPlane);
//增加防撞逻辑，策略为在路线上相撞，价值小的抬起，让价值大的先走，也就是在ssPlaneStatus1相应链表中增加
//两个节点 抬起 和 下降
void CollisionDectection(FLAY_PLANE * pstFlayPlane, FLAY_PLANE * pstFlayPlaneLast, int * pMap, MAP_INFO * pstMapInfo, MATCH_STATUS * pstMatchStatus, sPlaneStatus * sPlaneStatus1);
//飞机购买
void PurchasePlane(FLAY_PLANE * pstFlayPlane, MAP_INFO *pstMap, MATCH_STATUS * pstMatchStatus, std::vector<int> *listofweight);
//重置飞机状态
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