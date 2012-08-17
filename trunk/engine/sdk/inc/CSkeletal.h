#pragma once
#include "GraphicBase.h"
#include "CBufFile.h"
#include "CMatrixUnit.h"
#include "CCoder.h"
namespace sqr
{
	//骨骼的单个SOCKET
	struct CSkeletalSocket : public CGraphicMallocObject
	{
		CSkeletalSocket()
		{
			m_SocketName = 0;
			m_Socket.Identity();
		}

		IDNAME				m_SocketName;			//SOCKET名字
		CMatrix				m_Socket;				//SOCKET矩阵
	};

	//每根骨骼的SOCKET集合

	class CSkeletalUnit  : public CGraphicMallocObject
	{
	public:
		CSkeletal*				m_pSkeletal;
		CMatrixUnit				m_InverseMatrix;//每个骨骼上面的皮肤从其全局空间到骨骼局部的矩阵
		GVector<CSkeletalSocket>	m_SocketList;			//骨骼上SOCKET的列表

		int BinLoad( CBufFile& GraphicFile );
		int BinSave( WriteDataInf& wdi );
	};

	//骨骼动画
	class CSkeletal  : public CGraphicMallocObject
	{
		friend class CDataSources;
	protected:
		BYTE					m_ParentSkeletalID;		//父骨骼ID
		BYTE					m_SkeletalID;           //骨头ID
		GString					m_SkeletalName;			//骨头名字
		GVector<CSkeletal*>		m_Children;				//子关节
		CSkeletal();
	public:
		
		~CSkeletal();

		void	Destroy();
		bool	IsFitForMe( const CSkeletal* Skeletal );				//匹配骨骼
		int		GetSkeletalIDByName( const string& Name ) const;		//根据骨骼名得到骨骼ID（遍历自身和子骨骼）
		const char*	GetSkeletalNameByID( BYTE SkeletalID) const;

		BYTE	GetSkeletalID() const
		{
			return m_SkeletalID;    //得到骨骼自身ID
		}
		BYTE	GetParentSkeletalID() const
		{
			return m_ParentSkeletalID;    //得到父骨骼ID
		}
		const char* GetName() const
		{
			return m_SkeletalName.c_str();
		}
		void SetName(const char* bonename)
		{
			m_SkeletalName = bonename;
		}
		uint32    GetChildNum() const
		{
			return (uint32)m_Children.size();    //得到子骨骼数量
		}
		const   CSkeletal* GetChild( uint32 Num )const
		{
			return Num < GetChildNum() ? m_Children[Num] : NULL;    //得到指定的子骨骼
		}

		//计算骨骼矩阵
		void	CalculateMatrix( const CMatrixUnit& matUnit, CRenderMatrix& RenderMatrix,const CAnimationGroup& Animates,
			const CSkeletalFrame& SkeletalFrame, CAnimateSeparation* pParentSep = NULL) const;
		int		BinLoad( CBufFile& GraphicFile, BYTE ParentSkeletalID, CSkeletal** pSkeletal );
		int		BinSave( WriteDataInf& wdi);
	};
};
