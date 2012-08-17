#pragma once
#include <vector>

#define COUT(s)  //cout << #s << endl;		

#define OUT					//参数注释为输出


const double DIR_TO_RADIAN_RATE = 2 * 3.14159265358 / 256.0; //游戏方向角与弧度的比率,  弧度 = 方向 * DIR_TO_RADIAN_RATE
//const int OFFSET_8DIR[8][2] =  { {-1,-1},{ 0,-1},{ 1,-1},    //8向偏移数组
//								 {-1, 0},	     { 1, 0},
//								 {-1, 1},{ 0, 1},{ 1, 1},
//								};
//const int OFFSET_4DIR[4][2] =   {     { 0,-1},				//4向偏移数组
//								{-1, 0},	  { 1, 0},
//									   { 0, 1}
//								};

const int OFFSET_8DIR[8][2] =  { {-1,-1},{ 0,-1},{ 1,-1},{ 1, 0},{ 1, 1},{ 0, 1},{-1, 1},{-1, 0}};   //8向偏移数组

const int OFFSET_4DIR[4][2] =   { { 0,-1},{ 1, 0},{ 0, 1},{-1, 0} };				//4向偏移数组
	 


//#define GRID_PIXEL_W				8
#define MIN_DIS						0.2f   //两个物体间的最小距离
#define COLONY_AREA					12.0f	//群组成员与虚拟npc的距离检测, 防止粘贴的时候相聚太远

#define GAME_VIEW_TOP_DIS			16.5f
#define GAME_VIEW_TOP_LENGTH		39.0f
#define GAME_VIEW_BOTTOM_DIS		9.5f
#define GAME_VIEW_BOTTOM_LENGTH		20.0f
#define GAME_VIEW_ANGLE				0.785398163397   // 45 度


#define GRID_PRECISION			3			//寻路格子的细分次数, 增加精度

#define  PAINT_EVENT_ID			1			//重绘Timer事件ID
#define	 PAINT_ELAPSE			35			//重绘Timer间隔时间
#define  PAUSE_TIMES			3			//暂停绘制次数


#define BLOCK_SIZE				4			//npc,obj,trap 存储块的大小,调节此值达到最优速度	


//#define SELECT_RECT_LINE_SIZE   1			//拖选框线的粗细

#define SELECT_PRECISION		4			//鼠标选择的精度

#define SELECT_MARK_SIZE		18			//被选择对象标记框大小
//#define SELECT_MARK_LINE_SIZE	1			//被选择对象标记框的粗细

#define NPC_IMAGE_SIZE			9			//NPC绘图标记的像素大小
#define OBJ_IMAGE_SIZE			9			//OBJ绘图标记的像素大小
#define TRAP_IMAGE_SIZE			9			//TRAP绘图标记的像素大小
#define PATH_KEY_POS_SIZE		7			//巡逻路径关键点大小
#define PATH_LINE_SIZE			1			//巡逻路线粗细
#define DIR_ARROW_SIZE			25			//方向箭头长度
#define DIR_ARROW_LINE_SIZE		2			//方向箭头线粗细
#define GAME_VIEW_LIEN_SIZE		1
#define ROTATE_ROUND_SIZE		3
#define ROTATE_ROUND_CENTER_RADIUS 9
#define MEASURE_LINE_SIZE		2

#define SELECT_RECT_COLOR			RGB(0,255,0)
#define SELECT_MARK_COLOR			RGB(0,255,0)

#define NPC_IMAGE_COLOR				RGB(255,0,0)		//NPC绘图标记的颜色
#define BOSS_IMAGE_COLOR			RGB(255,255,255)
#define SERVANT_IMAGE_COLOR			RGB(255,128,0)
#define SINGLE_ATTACK_IMAGE_COLOR	RGB(0,255,255)
#define MULTI_ATTACK_IMAGE_COLOR	RGB(255,0,255)
#define NPC_IMAGE_FRAME_COLOR		RGB(0,0,1)
#define OBJ_IMAGE_COLOR				RGB(0,255,0)		//OBJ绘图标记的颜色
#define BARRIER_OBJ_IMAGE_COLOR		RGB(0,0,0)
#define TRAP_IMAGE_COLOR			RGB(0,0,255)		//TRAP绘图标记的颜色
#define PATH_KEY_POS_COLOR			RGB(255,255,0)
#define PATH_START_COLOR			RGB(255,0,255)
#define PATH_LINE_COLOR				RGB(0,128,255)
#define PATH_VIRTUAL_LINE_COLOR		RGB(128,80,0)
#define PATH_LINE_FOCUS_COLOR		RGB(0,255,80)
#define DIR_ARROW_COLOR				RGB(255,0,0)
#define DIR_ARROW_FOCUS_COLOR		RGB(255,255,0)
#define GAME_VIEW_COLOR				RGB(255,255,255)
#define PEN_RECT_COLOR				RGB(255,255,255)
#define BLOCK_COLOR_LV1				RGB(0,0,255)
#define BLOCK_COLOR_LV2				RGB(255,100,100)
#define BLOCK_COLOR_LV3				RGB(255,0,0)
#define GRID_LINE_COLOR				RGB(0,80,0)
#define ROTATE_ROUND_COLOR			RGB(250,55,217)
#define ROTATE_ROUND_FOCUS_COLOR	RGB(255,255,255)
#define MOVE_ROUND_FOCUS_COLOR		RGB(255,255,255)
#define ROTATE_ROUND_CENTER_COLOR	RGB(255,255,255)
#define MEASURE_LINE_COLOR			RGB(255,255,255)
#define MEASURE_BACK_COLOR			RGB(255,255,255)
#define MEASURE_TEXT_COLOR			RGB(0,0,0)

#define MIN_SCALING				0.15		//地图最小缩放比例
#define MAX_SCALING				6.0			//地图最大缩放比例

#define CHAT_BUFFER_SIZE		1024	//字符缓存数组大小,用再Save中, 大小够放就行,爆了再改





#define  eRegionSpan			16  //这样命名只是为了使跟游戏引擎里的命名一致
