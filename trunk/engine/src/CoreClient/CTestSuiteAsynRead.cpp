#include "stdafx.h"
#include <windows.h>
#include "CTestSuiteAsynRead.h"
#include "IGraphicModule.h"
#include "CPkgFile.h"
#include "CTimer.h"
#include "GraphicTest.h"
#include "IGraphicModule.h"
#include "TTestSuiteRenderBase.inl"

IModel*				pModel[11][11];
IPieceGroup*		pPieceGroup = NULL;
IAnimationGroup*	pAniGroup = NULL;

namespace sqr
{
	const char* GetTestSuiteAsynReadName()
	{
		static TestSuiteFactory<CTestSuiteAsynRead> m_factory;
		const char* szSuiteName="CTestSuiteAsynRead";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
}


void CTestSuiteAsynRead::setUp()
{
	Parent_t::setUp();
	m_pCoreGraphic = GetIGraphicModule()->CreateGraphic( m_hWnd );
	m_pCoreGraphic->CreatePieceGroup( "character/model/bnpc001.arp", &pPieceGroup );
	m_pCoreGraphic->CreateAnimationGroup( "character/ani/bnpc001.ara", &pAniGroup );

	for( int32 i = 0; i < 11; i++ )
		for( int32 j = 0; j < 11; j++ )
			m_pCoreGraphic->CreateModel( "", &(pModel[i][j]) );
}

void CTestSuiteAsynRead::tearDown()
{
	for( int32 i = 0; i < 11; i++ )
		for( int32 j = 0; j < 11; j++ )
			SAFE_RELEASE( pModel[i][j] );
	SAFE_RELEASE(m_pCoreGraphic);
	Parent_t::tearDown();
}


void CTestSuiteAsynRead::Run()
{
	Parent_t::Run();
	CPkgFile::Flush( -1 );
}

ICoreGraphic* CTestSuiteAsynRead::GetGraphic()const
{
	return m_pCoreGraphic;
}

void CTestSuiteAsynRead::UpdateFrame()
{
	if( pAniGroup && pPieceGroup && pAniGroup->IsValid() && pPieceGroup->IsValid() )
	{
		for( int32 i = 0; i < 11; i++ )
		{
			for( int32 j = 0; j < 11; j++ )
			{
				for( int32 n = 0; n < pPieceGroup->GetPieceClassNum(); n++ )
					pModel[i][j]->AddMeshPiece( pPieceGroup, pPieceGroup->GetPieceClassName(n), 0 );
				pModel[i][j]->SetAnimateGroup( pAniGroup );
				const char* szName[4] = { "f2_attack03", "attack15_1", "attack17_1", "attack19_2" };
				pModel[i][j]->SetNextAnimation( szName[ ( i + j )%1 ], FramePair( 0, -1 ) );
			}
		}
		SAFE_RELEASE( pAniGroup );
		SAFE_RELEASE( pPieceGroup );
	}

	CMatrix matWorld;
	matWorld.Identity();
	uint32 uTime = (uint32)GetProcessTime();

	if( !pAniGroup )
	{
		for( int32 i = 0; i < 11; i++ )
		{
			for( int32 j = 0; j < 11; j++ )
			{
				matWorld._41 = ( i - 5 )*30.0f;
				matWorld._43 = ( j - 5 )*30.0f;
				pModel[i][j]->Update( matWorld, uTime );
				pModel[i][j]->Render( false );
				pModel[i][j]->Render( true );
			}
		}
	}

	SleepEx( 0, true );
	CPkgFile::Flush( 15 );
}