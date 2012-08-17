#pragma once
#include "LightMapBaker.h"
#include "CModelDiffuseColor.h"

namespace sqr_tools
{
	class	CEditModel;
}

namespace sqr
{
	class	CMesh;
	class	SGrid;
	class	CLightSystem;
	class	CLightMapFilter;
	struct	PieceClassColor;
	struct	sMaterial;

	SmartPointer(CVertexShadowJudge);
	SmartPointer(CObjectVertexProcessor);
	SmartPointer(CMeshColorSmoother);

	class CObjectVertexBaker : public CLightMapBaker
	{
	public:
		CObjectVertexBaker();
		~CObjectVertexBaker();

		void	Setup();
		void	Discard();
		void	TilePrepare(index_t x,index_t z);
		void	OnProcess(index_t x,index_t z);
		void	RenderReceive();

	private:
		void	ProcessGrid(SGrid& grid);
		
		void	ProcessModel(CEditModel& model, vector<PieceClassColor>& pieceColors, 
					bool bBakingColorMark, const CColor4& matdiffuse, const PieceClassDiffuseVector& pcdv);
		
		void	ProcessMesh(CMesh& mesh, sMaterial& material, const CMatrix& world, bool bBakingColorMark, const PieceDiffuse* pd);

		void	ComputeTriangleShadow(float* outRatios,
					const CVector3f& p1, const CVector3f& p2, const CVector3f& p3);

		float	ComputeShadowArea(const CVector3f& p1, const CVector3f& p2, const CVector3f& p3);

		void	SmoothColorBuffer(CMesh& mesh);

	private:
		CVertexShadowJudgePtr	m_shadowJudge;
		CMeshColorSmootherPtr	m_meshColorSmoother;
		CColor4					m_shadowColor;
		vector<D3DCOLOR>		m_colorBuffer;
	};
}