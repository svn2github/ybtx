#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"
#include "ExsertStructDB.h"


namespace sqr
{
	class CCheckBox;
	class ITexture;
	struct RenderState;
	class CRiver;
	class CCmdSetSerialTexture;

	class CToolSetTileTexture: public Singleton<CToolSetTileTexture>
	{
	public:
		struct SProcessParams
		{
			SProcessParams()
			{
				dwBrushSizeInX = dwBrushSizeInZ = 1;
				nLayerHot = INVALID;
				eAlphaType = sqr::EAT_LINEAR;
				eWayType = sqr::EWT_INC;
				eDirType = sqr::EDT_BOTH;
				bPowerMode = true;
				pRiver = NULL;
			}

			bool bPowerMode;
			DWORD dwBrushSizeInX;
			DWORD dwBrushSizeInZ;
			vector<string> vecTextureNames;
			int nLayerHot;
			sqr::EALPHA_TYPE eAlphaType;
			sqr::EDIR_TYPE eDirType;
			sqr::EWAY_TYPE eWayType;
			CRiver * pRiver;
		};

		enum ECopyTerrainType	{ ECT_ALL = 0, ECT_HEIGHT, ECT_VCOLOR, ECT_TEXTURE };

	private:
		HWND			m_hGroundView;
		SProcessParams  m_processParams;
		bool			m_bInSubGrid;
		bool			m_bCliffLayerOne;
		float			m_fAlphaStrength;
		float			m_fAlphaSpecular;
		float		    m_fCliffStrength;

		// UI
		CCheckBox * m_pCheckBoxTextureLayerOne;
		CCheckBox * m_pCheckBoxTextureLayerTwo;

		void ResetCheckBoxTextureLayerOne(vector<string> & vecTextureNames);
		void ResetCheckBoxTextureLayerTwo(vector<string> & vecTextureNames);
		void ResetCheckBoxTextureLayerThree(vector<string> & vecTextureNames);
		void ResetCheckBoxTextureLayerForth(vector<string> & vecTextureNames);

		//画刷纹理对象
		ITexture *m_pBrushTexture;
		RenderState *m_pRS;

		vector<int> m_ReplaceGrids;
		vector<int> m_ReplaceTwoGrids;//layer 2

		ECopyTerrainType m_eCopyTerrainType;
		vector<int> m_copyGridIndexs;

	public:
		vector<string> m_ReplaceTexNames;

		CToolSetTileTexture();
		~CToolSetTileTexture();
		void OnCtrKeyEvent(const MSG_KEY_INF key);
		void OnOnlyKeyEvent(const MSG_KEY_INF key);
		void OnEvent(const CMsg& msg);
		void Render();

		void ShowLayers(bool b);
		void ResetLayers(int winX,int winY,int winWidth,int winHeight);

		int GetLayerHot();
		void SetLayerHot(int n);

		void SetAlphaType(sqr::EALPHA_TYPE eAlphaType);
		void SetTextureNameHot(string name);
		void SetTextureNameHot(vector<string> names);

		///根据选中某层具体纹理，替换成选中的四方连续纹理图素集
		void ReplaceTerrainTexture(const int& mark, vector<string>& deletetexnames);

		///找到与某个四方连续图图素包有关的格子索引列表
		bool SelectRelateContinueTex(const int& mark, vector<string>& deletetexnames);

		///删除选中的某单个具体纹理
		void DeleteSignleTexeture( const string& deletetexname );

		//得到当前纹理名
		string GetTextureNameHot();
		vector<string> & GetTextureNamesHot();

		DWORD GetBrushSizeInX();
		void SetBrushSizeInX(DWORD dw);
		void IncBrushSizeInX();
		void DecBrushSizeInX();

		DWORD GetBrushSizeInZ();
		void SetBrushSizeInZ(DWORD dw);
		void IncBrushSizeInZ();
		void DecBrushSizeInZ();

		void SetDirType(sqr::EDIR_TYPE e);

		void SetWayType(sqr::EWAY_TYPE e);

		vector<int> & GetBrushGridIndices();

		void InitProcessParams(HWND hGroundView);
		void CreateCheckBox();

		CRiver * GetRiver();
		void SetRiver(CRiver * p);

		void DoOne();
		void DoTwo();

		//第一层全铺
		void BrushLayerOneTexture();
		void DeleteTexture();

		inline void SetCopyTerrainType(const int nCopyType)
		{
			m_eCopyTerrainType = (CToolSetTileTexture::ECopyTerrainType)nCopyType;
		}

		// 悬崖
		inline void SetCliffLayerOne(bool b)
		{
			m_bCliffLayerOne = b;
		}

		inline bool GetCliffLayerOne() const
		{
			return m_bCliffLayerOne;
		}

		inline void SetCliffStrength(const float f)
		{
			m_fCliffStrength = f;
		}

		inline float GetCliffStrength() const
		{
			return m_fCliffStrength;
		}

		void SetAlphaStrength(float f);

		float GetAlphaStrength();

		void PasteCopyTerrain();

		void SetGridMaterialIndex( const int8 nMatIndex, bool bForce );
		void HideAllSpecialMaterialGrid() const;

		void ShowCurLayerTexture();

		void GetHotGridTextureInfo(string& tex1, string& tex2, string& tex3, string& tex4);
		void ReplaceTerrainTexture( const int& mark );

		///还原所有的悬崖贴图坐标
		void RevertCliffTexCoordinate();

		///find all minmap not dxt1 format
		void FindErrorMinMapFormat();

		bool IsSequenceGridTextures(const vector<string>& texnames);
		bool NeiborGridIsSequenceTexture(const string& texname);
		void CmdNeiborGridSequenceTexture(CCmdSetSerialTexture *pCmd, const int nGridIndex1, const int nGridIndex2, const int nNeiGridIndex1, const int nNeiGridIndex2);
		void ReBrushSequenceGridTextures(const vector<int>& gridIndexs, const int nLayer);
		void PatchSceneSequenceGridTex();
		void CmdSequenceGridTex(CCmdSetSerialTexture *pCmd);

		void BrushSingleTexture();
		void BrushSerialTextures();

		void SwitchEnableTexInstead();
	};
}