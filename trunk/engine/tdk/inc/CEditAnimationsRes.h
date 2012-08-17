#pragma once
#include "CAnimationGroup.h"
#include "CSkeletalFrame.h"
#include "CEditAnimate.h"
#include "CEditSkeletal.h"
#include "CEditSubject.h"
//#include "CAgpCoder.h"
namespace sqr_tools
{
	enum BatchState
	{
		eOpenErr  = 0,//批量转换中有文件打开失败
		eTrans    = 1,//批量转换中的文件是转换过的
		eNoTrans  = 2,//批量转换中的文件没转换过的
	};

	class CEditAnimationGroup;
	class CEditSkeletalFrame 
		: public sqr::CSkeletalFrame
		, public CEditSubject 
	{
	public:
		CEditSkeletalFrame( const CSkeletalFrame& Res );
		CEditSkeletalFrame( const string& szName = "", const wstring& Alias= L"" );

		bool			IsFitForMe( const CEditSkeletalFrame& Skeletal, string& ErrSkeletal );
		int				GetCurSelSkelatal();	/// 得到当前所选骨骼ID
		void			SetCurSelSkelatal(BYTE i);	/// 设置当前所选骨骼为当前骨骼ID
		int				GetSkeletalIdBySocketName( sqr::IDNAME SocketName, int* SocketNum );
		bool			BinSave( const string& FileName );
		int			ReLoad( const string& FileName );
		BatchState		SkeResLoad( const string& FileName,CEditAnimationGroup& EditAniGroup,int& filesize);//批量拆分ske专用
		bool			FillSkeletalMask( SKELETAL_MASK& Mask );	//设置所有骨骼有效
		const CEditSkeletalFrame& operator = ( const CEditSkeletalFrame& fARes );
	};

	class CEditAnimationGroup 
		: public sqr::CAnimationGroup
		, public CEditSubject 
	{
	public:
		//static CAgpFormat Format;
		CEditAnimationGroup( const CAnimationGroup& Res );
		CEditAnimationGroup( const string& szName = "", const wstring& Alias = L"" );
		~CEditAnimationGroup() { Destroy(); }
		void				Destroy();
		CAnimateList&		GetAnimateList(void);
		bool				BinSave( const string& FileName );
		bool				BatBinSave(const string& FileName);//专为批量拆分ske保存agp用
		bool				ReLoad( const string& FileName );
		BatchState			AgpResLoad(const string& filename,CRefBufFile& GraphicFile);//批量拆分ske专用

		const CEditAnimationGroup& operator = ( const CEditAnimationGroup& fARes );
	};

	inline CAnimationGroup::CAnimateList&	
		CEditAnimationGroup::GetAnimateList(void) 
	{ 
		return m_AnimationList; 
	};
}