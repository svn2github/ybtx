#pragma once
#include "CRefObject.h"
#include "OffLineBaker/CModelDiffuseColor.h"

namespace sqr
{
	class CTerrainMesh;
	class IProgressCallback;
	struct PieceDiffuseColor;

	SmartPointer(Stream);
	SmartPointer(BinaryWriter);
	SmartPointer(CVertexDiffuseSaver);

	class CVertexDiffuseSaver : public CRefObject
	{
	public:
		typedef vector<TileIndex> IndexList;

	public:
		CVertexDiffuseSaver();
		
		void SetProcessCallback( IProgressCallback* progress );
		bool Save( const string& path );

	private:
		void saveIndexFile( const string& path );
		void saveTileModelDiffuse( GridModelDiffuseMap& gmdm );
		void saveIndex2ModelGroupDiffuse( Idx2ModelGroupDiffuseMap& i2mgd );
		void saveModelGroupDiffuseMap( ModelGroupDiffuseMap& mgd );
		void savePieceClassDiffuseVector( PieceClassDiffuseVector& pcdv );
		void savePieceDiffuseVector( PieceDiffuseVector& pdv );

	private:
		IProgressCallback*  m_pProgress;
		StreamPtr			m_stream;
		BinaryWriterPtr		m_writer;
		IndexList			m_index;
	};
}