#pragma once
#include "CBufFile.h"
#include "atlimage.h"
#include "PlaceElement.h"


enum EUpdateElementState 
{
	eUES_NONE = 0,
	eUES_NPC = 1,
	eUES_OBJ = 1 << 1,
	eUES_TRAP = 1 << 2,
	//在此添加新枚举,并格式同上, 就可以不修改任何代码,保证原来逻辑
	eUES_PATH_KEY_POS = 1 << 3,


	eUES_SINGLE_END,
	eUES_ALL = ((eUES_SINGLE_END - 1) << 1) - 1
};




enum EShowState  //显示状态比较多,以后可能超过32,所以就不用 EUpdateElementState 的管理方式了
{
	eSS_NPC,
	eSS_OBJ,
	eSS_TRAP,
	eSS_PATH,
	eSS_ARROW,
	eSS_GRID_LINE,
	eSS_SELECT_RECT,//鼠标框选时候的矩形
	eSS_SELECT_MARK,
	eSS_NPC_FULL,//NPC补全, 当某个NPC为焦点,或被选状态, 是否补全其他显示信息,(eSS_PATH,eSS_ARROW)
	eSS_GAME_VIEW,//游戏中的视窗
	eSS_GAME_VIEW_FOLLOW,
	eSS_AREA,
	eSS_BLOCK,
	eSS_IGNORE_BLOCK,

	eSS_COUNT
};

class CMapView : public CStatic
{
	DECLARE_DYNAMIC(CMapView)

public:
	typedef vector<PPlaceNpc>	Npc_Vec;
	typedef vector<PPlaceObj>	Obj_Vec;
	typedef vector<PPlaceTrap>	Trap_Vec;
	typedef vector<CElement*>   Element_Vec;
	typedef vector<FPos>			FPos_Vec;

	typedef Npc_Vec::iterator	Npc_Iter;
	typedef Obj_Vec::iterator	Obj_Iter;
	typedef Trap_Vec::iterator	Trap_Iter;
	typedef Element_Vec::iterator Element_Iter;
	CMapView();
	virtual ~CMapView();
	
	void ClearAllImage();
	bool LoadMap(const CString& strPath, const CString& mapFilePath, OUT CString& errorMessage);
	bool LoadBarrierData(const CString& strPathName);
	bool LoadSceneInfo(const CString& strPathName);
	bool LoadRegionInfo();
	void LoadOneRegion(CBufFile& File, uint32 nChunk,uint32 RegionX, uint32 y);
	void LoadOneChunk(CBufFile& File, uint32 x, uint32 y, uint32 uMagicID);

	void Init();
	void RePaint();
	void DrawArea(CDC* pDC);
	void DrawBlock(CDC* pDC);
	void DrawGridLine(CDC* pDC);
	void DrawMap(CDC* pDC);
	void DrawNpc(CDC* pDC);
	void DrawAllDirection(CDC* pDC);
	void DrawNpcDirection(CDC* pDC);
	void DrawObjDirection(CDC* pDC);
	void DrawTrapDirection(CDC* pDC);
	void DrawPasteElementDirection(CDC* pDC);
	void DrawObj(CDC* pDC);
	void DrawTrap(CDC* pDC);
	void DrawAllPath(CDC* pDC);
	void DrawSingleNpcPath(CDC* pDC, PPlaceNpc pNpc);
	void DrawDirection(CDC* pDC,PPlaceNpc pNpc);
	void DrawSelectRect(CDC* pDC);
	void DrawSelectMark();
	void DrawFocus(CDC* pDC);
	void DrawSelectElement(CDC* pDC);
	void DrawGameView(CDC* pDC);
	void DrawPenRect(CDC* pDC);
	void DrawPasteElement(CDC* pDC);
	void DrawRotateRound(CDC* pDC);
	void DrawMeasure(CDC* pDC);

	
	inline void GetGridRectOfView(OUT FRect& gridFRect) const;//获取当前视图的矩形区域对应的场景矩形区域,
	bool GetCurCursorViewPos(OUT Pos& pos) const; //获取当前鼠标的位置,相对视窗的坐标. 返回是否在视窗内
	int CurCursorX()const;//获取当前鼠标的位置,相对视窗的X坐标.
	int CurCursorY()const;//获取当前鼠标的位置,相对视窗的Y坐标.
	bool GetCurCursorGridPos(OUT FPos& pos) const;//获得鼠标的地图上的格子位置  (地图坐下角为 (0,0)坐标 ) 返回是否在视窗内的地图内,
	inline void GridPosToViewPos(OUT Pos& viewPos, const FPos& gridFPos) const;//场景的格子坐标转换为视图的坐标
	inline void ViewPosToGridPos(OUT FPos& gridFPos, const Pos& viewPos) const;//视图的坐标转换为场景的格子坐标
	inline void GridRectToViewRect(OUT Rect& viewRect, const FRect& gridFRect) const;
	inline void ViewRectToGridRect(OUT FRect& gridFRect, const Rect& viewRect) const;
	inline float GetViewDistance(float gridDis) const;//格子距离转换成视窗距离
	inline float GetGridDistance(float viewDis) const;//视图距离转换成格子距离 
	//inline double GetRadianDir(int dir) const;//游戏中的方向转成弧度
	
	bool IsPosNearLine(OUT float& dis ,const FPos& fPosLineStart, const FPos& fPosLineEnd, const FPos& fPosTest, float fRange) const;//判断点是否在线附件
	bool IsCursorInView();
	
	int GetMapGridWidth(){return m_nMapGridX;};
	int GetMapGridHeight(){return m_nMapGridY;};

	void MoveMap(int xOffset, int yOffset);//相对之前偏移
	void SetMapPos(int posX, int posY);//直接设置偏移, 会进行边界处理
	void MoveMapToCenter(float fGridX, float fGridy);
	void MoveMapToCenter(const FPos& fPos);
	void MoveMapToSelectCenter();
	bool IsMapOk();
	//bool IsMouseInView();
	void ReSetView();
	void ReSetShowState();
	bool SetScaling(float fScaling);
	void UpdateAll();
	void UpdateObj();
	void UpdateNpc();
	void UpdateTrap();
	void SetUpdateMark(EUpdateElementState eState);//设置更新状态
	void RemoveUpdateMark(EUpdateElementState eState);//移除更新状态
	bool IsUpdate(EUpdateElementState eState);//更新状态检测
	
	void SetMyFocus(CElement* pElement);
	bool IsSelectElement(CElement* pElement);
	bool AddSelectElement(CElement* pElement);
	void SelectColony(int iColonyId);
	int RemoveSelectElement(int eType);
	bool RemoveSelectElement(CElement* pElement);
	void ClearSelectElement();
	bool GetVecElementCenter(OUT FPos& fPosCenter, const Element_Vec& srcVec, int eType);
	bool GetVecElementRect(OUT FRect& fRect, const Element_Vec& srcVec, int eType);

	bool UserCopyElement(Element_Vec* pVecSrc = NULL);
	bool CheckUserCopy(Element_Vec& vecSrc, OUT CString& msg);
	void CutElement();
	void ClearCopy();
	void PasteElement();
	void ClearRotateBasic();

	void CopeyVecElement(OUT Element_Vec& destVec, const Element_Vec& srcVec, int type);
	
	CElement* PutElement(CElement* pElement, const FPos& gridPos);
	CElement* PutNpc(PPlaceNpc pNpc, const FPos& gridPos);
	CElement* PutObj(PPlaceObj pObj, const FPos& gridPos);
	CElement* PutTrap(PPlaceTrap pTrap, const FPos& gridPos);
	bool PutNpcCheck(PPlaceNpc pNpc, const FPos& gridPos, OUT CString* msg = NULL);
	bool PutObjCheck(PPlaceObj pObj, const FPos& gridPos, OUT CString* msg = NULL);
	bool PutTrapCheck(PPlaceTrap pTrap, const FPos& gridPos, OUT CString* msg = NULL);
	void PutPasteElement();


	CElement* FindNearestElement(const FPos& fPos, float fRange);
	int GetShowElementMark(int rangeMark = eET_ALL) const;
	int FindElement(OUT Element_Vec& vec, const FRect& fRect, int mark)const;//Element 包括Npc,obj,trap,pathKeyPos
	CArrow* FindArrow(const FPos& fPos) const;

	CPathLine* FindPathLine(const FPos& fPos) const;

	void UpdateFocus();
	void OnMoveElement();
	void MoveElement(CElement* pElement, const FPos& fPos);
	void RotateElement();
	void BeginRotateElement();
	bool IsCursorOnRotateRound();
	void UpdateRotateElement();

	bool CheckInsertPathKeyPos(PPathKeyPos pPathKeyPos, const FPos& fPos);
	bool CheckInsertPathKeyPos(PPlaceNpc pNpc, const FPos& fPos);
	bool CheckInsertPathKeyPos(CPathLine* pPathLine, const FPos& fPos);
	bool CheckMovePathKeyPos(PPathKeyPos pPathKeyPos, const FPos& fPos);
	bool CheckMoveNpc(PPlaceNpc pNpc, const FPos& fPos);

	void MoveAllSelectElement(const FPos& offset);
	void MoveAllSelectElementToGridMiddle();
	void DeleteElement(CElement* pElement);
	bool DeleteElemetVec(Element_Vec& vecElement);	//所有的删除都走这里
	bool DeleteAllSelectElement();

	void EditElement(CElement* pElement);


	CElement* GetMyFocus(){return m_pFocusElement;}
	
	bool CheckPos(const FPos& fPos, int mark, CElement* pExclude = NULL);//检测该点是否重叠
	
	void SetSelectStart();
	void SetSelecting();
	
	bool IsShowSate(EShowState eShowState){return m_bShowStates[eShowState];}
	void SetShowState(EShowState eShowState, bool isShow = true);
	
	void SetAreaImageAlpha(int alpha);
	void SetAreaImageColor(float centerX, float centerY, int size,int color);//color 为-1 将是清除
	void FillArea(int x, int y, int color);
	void ClearArea();
	void ReplaceColor(int oldColor, int newColor);
	void DelColor(int color);
	void SetContourColor(int gridX, int gridY, int color);
	void DrawAreaLine(int gridX, int gridY1, int gridY2, int color);
	int GetAreaColor(int gridX, int gridY);
	bool IsAreaEdge(int ix, int iy);//检测 ix,iy 点是否区域边缘

	void ComputeContour();//计算轮廓
	void FillContour();//填充轮廓
	bool IsInsidePos(int x, int y);
	bool IsContourPos(int gridX, int gridY);
	void ClearContour(){ memset(m_pAreaContour, false, sizeof(bool) * m_nMapGridX * m_nMapGridY);}
	
	void AddBarrierObj(int x, int y);
	void AddBarrierObj(const Pos& gridFPos);
	bool IsBarrierObj(int gridX, int gridY);

	void SetBlockImageColor(int gridX, int gridY, int color);
	bool IsBlock(int gridX, int gridY);
	bool IsBlock(const FPos& gridFPos);
	bool Line(const FPos& srcPos, const FPos& destPos);
	
	bool IsPosDraw(int gridX, int gridY);
	void EmptyFillRecord();

	void PauseRender();
	
	void Measure();
	void AddMeasurePos(const FPos& fPos);
	void ClearMeasurePos();
	bool IsOverArea;
protected:
	DECLARE_MESSAGE_MAP()
private:
	CImage		m_ImageMap;//存放地图图片
	CImage		m_ImageBuffer;//缓存图片.先把所以东西画这里,然后一次性画到控件上
	CImage		m_ImageArea;//半透明区域绘制到这里, 
	CImage		m_ImageBlock;//障碍半透明图片

	bool*		m_pAreaContour;//轮廓数组, 通过ComputeContour 计算的区域轮廓存放在这里

	byte		m_AlphaValue;
	COLORREF	m_curAreaColor;
	
	float		m_fScaling;//地图缩放比例
	
	int			m_iCurCursorPosX;//当前鼠标在视图中的X坐标, 但是获取当前鼠标位置函数不根据他们得到, 这两个变量仅仅计算鼠标2帧间移动多少距离时候有作用
	int			m_iCurCursorPosY;//当前鼠标在视图中的Y坐标  同上

	int			m_nViewWidth;//地图视窗的宽
	int			m_nViewHeight;//地图视窗的高
	int			m_nMapWidth;//地图图片的宽
	int			m_nMapHeight;//地图图片的高
	int			m_nMapGridX; //地图宽的格子数 = m_nMapWidth/m_nGridPixel
	int			m_nMapGridY; //地图高的格子数 = m_nMapHeight/m_nGridPixel


	int			m_nCurMapWidth;//缩放之后的地图的宽
	int			m_nCurMapHeight;//缩放之后的地图的高
	
	int			m_iOffsetX;//地图的X偏移(地图相对视窗左偏移为正)
	int			m_iOffsetY;//地图的Y偏移(地图相对视窗上偏移为正)
	int			m_iViewOffsetX;//视窗相对 对话框的X偏移
	int			m_iViewOffsetY;//视窗相对 对话框的Y偏移
	int			m_nGridPixel;//格子的像素数

	
	int			m_iUpdateViewElementMark;//是否需要更新视窗内的元素(当地图移动,Npc,Obj,Trap修改时候需要)
	
	Npc_Vec		m_vecCurViewNpc;//保存当前视窗中的npc
	Obj_Vec		m_vecCurViewObj;//保存当前视窗中的Obj
	Trap_Vec	m_vecCurViewTrap;//保存当前视窗中的Trap
	
	CElement*	m_pFocusElement;
	Element_Vec	m_vecSelectElement;//只包含NPC,OBJ,TRAP, PathKeyPos
	Element_Vec m_vecSelectSpare;
	mutable CPathLine	m_PathLine;//记录焦点巡逻线

	Element_Vec m_vecCopyElement;//复制的记录

	Rect		m_rectSelect;//用来保存选择矩形
	long		m_lFrameCount;//帧记录
	FPos		m_fPosGameViewCenter;
	bool		m_bShowStates[eSS_COUNT];
	
	FPos_Vec		m_vecMeasurePos;

	FRect		m_fRectRotate;
	FPos		m_fPosRotateStart;
	Element_Vec m_vecRotateBasic;	//为了旋转精准, 拷贝旋转前的对象

	bool*	 m_pFillRecord; //填充记录

	int	 m_iPauseTimes;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
