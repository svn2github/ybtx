#pragma once
#include "TSingleton.h"
#include "CVBRef.h"
#include "CModelProcesser.h"

namespace sqr
{
	typedef vector<uint32>					PieceDiffuse;
	typedef vector<PieceDiffuse>			PieceDiffuseVector;
	typedef PieceDiffuseVector::iterator	PieceDiffuseIter;

	typedef vector<VBRefPtr>				PieceVBVector;
	typedef PieceVBVector::iterator			PieceVBIter;
	
	struct PieceClassDiffuse
	{
		string				className;
		PieceDiffuseVector	pieceColorVec;
		PieceVBVector		pieceColorVBVec;
	};
	typedef vector<PieceClassDiffuse>					PieceClassDiffuseVector;
	typedef PieceClassDiffuseVector::iterator			PieceClassDiffuseIter;

	typedef string ModelName;
	typedef map< string, PieceClassDiffuseVector >		ModelGroupDiffuseMap;
	typedef ModelGroupDiffuseMap::iterator				ModelGroupDiffuseIter;
	
	typedef map< uint32, ModelGroupDiffuseMap >			Idx2ModelGroupDiffuseMap;
	typedef Idx2ModelGroupDiffuseMap::iterator			Idx2ModelGroupDiffuseIter;
	struct IdxModelGroupDiffuseStruct
	{
		Idx2ModelGroupDiffuseMap i2mgdm;
	};
	
	typedef map< uint32, IdxModelGroupDiffuseStruct >	GridModelDiffuseMap;
	typedef GridModelDiffuseMap::iterator				GridModelDiffuseIter;
	
	typedef pair<uint32, uint32>						TileIndex;
	typedef map<TileIndex, GridModelDiffuseMap>			TileModelDiffuseMap;
	typedef TileModelDiffuseMap::iterator				TileModelDiffuseIter;

	// -------------------------------------------------------------------------------------------------

	class CMapEditObject;
	class CDiffuseModelProcesser;

	class CVertexDiffuseColor : public TSingleton<CVertexDiffuseColor>
	{
	public:
		CVertexDiffuseColor();
		~CVertexDiffuseColor();

		TileModelDiffuseMap& 
				GetAllObjectColor();

		void	ClearAll();
		void	ClearTile( uint32 x, uint32 z );
		void	CreateModelColorVB( PieceClassDiffuseVector& pcdv );
		void	BakeModelDiffuse( uint32 iModelGroup, CMapEditObject& meObject );
		void	BakeTileDiffuse( uint32 x, uint32 z );
		void	BakeMapDiffuse();
		void	ApplyModelDiffuse( uint32 iModelGroup, CMapEditObject& meObject );
		void	ClearModelDiffuse( uint32 iModelGroup, CMapEditObject& meObject );

		PieceClassDiffuseVector&
				GetModelDiffuse( uint32 nGridIdx, uint32 nModelGroup, const char* name );

	private:
		CVertexDiffuseColor( const CVertexDiffuseColor& );
		CVertexDiffuseColor& operator = ( const CVertexDiffuseColor& );

		TileModelDiffuseMap		m_TileModelDiffuseMap;
		CDiffuseModelProcesser*	m_pModelProcesser;
	};	

	class CDiffuseModelProcesser : public CModelProcesser
	{
		void	ProcessModel( uint32 iModelGroup, CMapEditObject& meObject )
		{
			CVertexDiffuseColor::GetInst()->BakeModelDiffuse(iModelGroup, meObject);
		}
	};
}