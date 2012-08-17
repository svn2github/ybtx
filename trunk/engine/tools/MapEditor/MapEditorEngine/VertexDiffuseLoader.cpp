#include "stdafx.h"
#include "VertexDiffuseLoader.h"
#include "FileStream.h"
#include "BinaryReader.h"
#include "Engine.h"

namespace sqr
{
	CVertexDiffuseLoader::CVertexDiffuseLoader()
		: m_stream(new FileStream())
		, m_reader(new BinaryReader())
	{
		m_stream->SetAccessMode(Stream::ReadAccess);
		m_reader->SetStream(m_stream);
	}

	void CVertexDiffuseLoader::SetProcessCallback( IProgressCallback* progress )
	{
		m_pProgress = progress;
	}

	bool CVertexDiffuseLoader::Load( const string& path )
	{
		assert(NULL != m_pProgress);

		m_pProgress->SetProgress(0);

		loadIndexFile( path + "/index.didx" );
		
		TileModelDiffuseMap& modDifMap = CVertexDiffuseColor::GetInst()->GetAllObjectColor();
		IndexList::iterator it  = m_index.begin();
		IndexList::iterator eit = m_index.end();
		for ( ; it != eit; ++it )
		{
			stringstream ss;
			TileIndex ti = *it;
			ss << ti.first << "_" << ti.second << ".diffuse";
			string szLoadName = ss.str();

			m_stream->SetPath(szLoadName);
			if ( !m_reader->Open() )
				return false;

			GridModelDiffuseMap& gmdm = modDifMap[ti];
			loadTileModelDiffuse(gmdm);

			m_reader->Close();
		}
		
		return true;
	}

	bool CVertexDiffuseLoader::loadIndexFile( const string& path )
	{
		m_stream->SetPath(path);
		if ( !m_reader->Open() )
			return false;

		uint32 fourcc = m_reader->ReadUInt32();
		assert(fourcc == 'DIDX');

		uint32 nIdxNum = m_reader->ReadUInt32();
		for ( uint32 i=0; i < nIdxNum; ++i )
		{
			TileIndex ti;
			ti.first  = m_reader->ReadUInt32();
			ti.second = m_reader->ReadUInt32();
			m_index.push_back(ti);
		}

		m_reader->Close();
		return true;
	}

	void CVertexDiffuseLoader::loadTileModelDiffuse( GridModelDiffuseMap& gmdm )
	{
		uint32 fourcc = m_reader->ReadUInt32();
		assert(fourcc == 'DICL');

		uint32 nGridNum = m_reader->ReadUInt32();
		for ( uint32 i=0; i < nGridNum; ++i )
		{
			uint32 grid = m_reader->ReadUInt32();
			gmdm.insert( make_pair(grid, IdxModelGroupDiffuseStruct()) );

			loadIndex2ModelGroupDiffuse(gmdm[nGridNum].i2mgdm);
		}
	}


	void CVertexDiffuseLoader::loadIndex2ModelGroupDiffuse( Idx2ModelGroupDiffuseMap& i2mgd )
	{
		uint32 nGroupNum = m_reader->ReadUInt32();

		for ( uint32 i=0; i < nGroupNum; ++i )
		{
			uint32 index = m_reader->ReadUInt32();
			i2mgd.insert( make_pair(index, ModelGroupDiffuseMap()) );

			loadModelGroupDiffuseMap(i2mgd[index]);
		}
	}

	void CVertexDiffuseLoader::loadModelGroupDiffuseMap( ModelGroupDiffuseMap& mgd )
	{
		uint32 nModNum = m_reader->ReadUInt32();

		for ( uint32 i=0; i < nModNum; ++i )
		{
			ModelName name = m_reader->ReadString();
			mgd.insert( make_pair(name, PieceClassDiffuseVector()) );

			loadPieceClassDiffuseVector(mgd[name]);
		}
	}

	void CVertexDiffuseLoader::loadPieceClassDiffuseVector( PieceClassDiffuseVector& pcdv )
	{
		uint32 nClassNum = m_reader->ReadUInt32();

		for ( uint32 i=0; i < nClassNum; ++i )
		{
			pcdv.push_back(PieceClassDiffuse());
			pcdv[i].className = m_reader->ReadString();
			
			loadPieceDiffuseVector(pcdv[i].pieceColorVec);
		}
	}

	void CVertexDiffuseLoader::loadPieceDiffuseVector( PieceDiffuseVector& pdv )
	{
		uint32 nPieceNum = m_reader->ReadUInt32();

		for ( uint32 i=0; i < nPieceNum; ++i )
		{
			pdv.push_back( PieceDiffuse() );

			PieceDiffuse& pd = pdv[i];
			uint32 diffuse_num = m_reader->ReadUInt32();

			for ( uint32 j=0; j < diffuse_num; ++j )
			{
				pd.push_back(m_reader->ReadUInt32());
			}
		}
	}
}