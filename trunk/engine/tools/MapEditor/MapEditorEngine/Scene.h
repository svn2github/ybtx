#pragma once
#include "CEventOwner.h"
#include "CMatrix.h"
#include "TRefPointer.h"
#include "CRefObject.h"

namespace sqr
{
	class CMouse;
	class CWireBox;
	class CSquare;
	class CRiver;
	class CTerrainMesh;
	class CMapEditObject;
	class CTileEditorModel;

	SmartPointer(ISceneNode);
	class ISceneNode : public CRefObject
	{
	protected:
		list< ISceneNodePtr > m_children;
		bool m_bIsVisible;

		ISceneNode * m_pParent;


		CMatrix m_matRelative;
		CMatrix m_matAbsolute;

		CVector3f m_vecRelativeTranslation;
		CVector3f m_vecRelativeRotation;
		CVector3f m_vecRelativeScale;

	public:

		// node management
		ISceneNode(ISceneNode * pParent,bool bIsVisible = true,
				   CVector3f & vecTranslation = CVector3f(0.0f,0.0f,0.0f),
				   CVector3f & vecRotation = CVector3f(0.0f,0.0f,0.0f),
				   CVector3f & vecScale = CVector3f(1.0f,1.0f,1.0f));

		virtual ~ISceneNode(); // destructor will remove all children;

		virtual void SetVisible(bool b);
		bool IsVisible();

		void Clear();
		void AddChild(ISceneNode * p);
		void RemoveChild(ISceneNode * p);
		void Remove();

		// updates the absolute transformation based on the relative and the parent
		void UpdateAbsolute();

		void SetAbsolute(CMatrix mat);
		const CMatrix & GetRelative();
		const CMatrix & GetAbsolute();
		void SetRelativePos(CVector3f & v);
		void SetRelativeRotation(CVector3f & v);

		virtual void Render();
		virtual void Update(const CMsg& msg);
		virtual void UpdateFPSLimited();

	private:
		virtual void OnEvent(const CMsg& msg);
	};

	// ----------------------------------------------------------------------------
	// GUI node

	// each ui scene node has a swapchain for rendering
	SmartPointer(IGUISceneNode);
	class IGUISceneNode : public ISceneNode
	{
	protected:

		//LPDIRECT3DSWAPCHAIN9 m_pSwapChain;
		HWND m_hWnd;

	public:
		IGUISceneNode(ISceneNode * pParent = NULL);
		~IGUISceneNode();
		//void Create(DWORD dwWidth, DWORD dwHeight, HWND hWnd);
		//void Render();
	};

	// ----------------------------------------------------------------------------
	// scene manager

	class CSceneManager 
		: public ISceneNode
		, public Singleton<CSceneManager>
	{
	private:
		friend class Singleton<CSceneManager>;
		CSceneManager(ISceneNode * pParent = NULL);
		~CSceneManager();
	private:
		IGUISceneNodePtr m_pGUIRoot;

		typedef set<ISceneNode*> LoginNodeList;
		LoginNodeList	m_loginObjList;
	public:
		void DrawWorld();
		void DrawWin32GUI();

		void UpdateWorld(const CMsg& msg)
		{
			this->Update(msg);
		}
		void UpdateWin32GUI(const CMsg& msg)
		{
			m_pGUIRoot->Update(msg);
		}
		void UpdateWorldFPSLimited();

		CSquare*			AddNodeSquare( float fSizeInWidth, float fSizeInDepth, DWORD dwColor, string texname, ISceneNode * pParent = NULL );
		CMapEditObject*		AddNodeModel( string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, float fLiftEx, float fShiftXEx, float fShiftZEx, float fRotation = 0.0f, int nRotationCnt = 0,
									 float fPitch = 0.0f, int nPitchCnt = 0, float fYaw = 0.0f, int nYawCnt = 0, float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f, bool bEffect = false, bool bAudio = false, bool bLogin = false, ISceneNode * pParent = NULL );
		CWireBox*			AddNodeWireBox( float fHeight, float fWidth, float fDepth, DWORD dwColor, bool bIsVisible = true, ISceneNode * pParent = NULL );
 		CRiver*				AddNodeRiver( vector<string> vecTextureNames, bool bIsVisible = false, ISceneNode * pParent = NULL );
		CTileEditorModel*	AddNodeTitleModel( string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, int &BlockInfoNum, int &LogicHeightInfoNum );
	};
}