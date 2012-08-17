#pragma once
#include "CRefObject.h"
#include "OffLineBaker/CModelDiffuseColor.h"

namespace sqr
{
	class IProgressCallback;

	SmartPointer(Stream);
	SmartPointer(BinaryReader);
	SmartPointer(CVertexDiffuseLoader);

	class CVertexDiffuseLoader : public CRefObject
	{
	public:
		typedef vector<TileIndex> IndexList;

	public:
		CVertexDiffuseLoader();

		void SetProcessCallback( IProgressCallback* progress );
		bool Load( const string& path );

	private:
		bool loadIndexFile( const string& path );
		void loadTileModelDiffuse( GridModelDiffuseMap& gmdm );
		void loadIndex2ModelGroupDiffuse( Idx2ModelGroupDiffuseMap& i2mgd );
		void loadModelGroupDiffuseMap( ModelGroupDiffuseMap& mgd );
		void loadPieceClassDiffuseVector( PieceClassDiffuseVector& pcdv );
		void loadPieceDiffuseVector( PieceDiffuseVector& pdv );

	private:
		CVertexDiffuseLoader( const CVertexDiffuseLoader& );
		CVertexDiffuseLoader& operator = ( const CVertexDiffuseLoader& );

		IProgressCallback*	m_pProgress;
		StreamPtr			m_stream;
		BinaryReaderPtr		m_reader;
		IndexList			m_index;
	};
}