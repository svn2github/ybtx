#include "stdafx.h"
#include "VertexDiffuseSaver.h"
#include "TerrainMesh.h"
#include "FileStream.h"
#include "BinaryWriter.h"
#include "Engine.h"

namespace sqr
{
	CVertexDiffuseSaver::CVertexDiffuseSaver()
		: m_stream(new FileStream())
		, m_writer(new BinaryWriter())
		, m_pProgress(0)
	{
		m_stream->SetAccessMode(Stream::WriteAccess);
		m_writer->SetStream(m_stream);
	}

	void CVertexDiffuseSaver::SetProcessCallback( IProgressCallback* progress )
	{
		m_pProgress = progress;
	}

	bool CVertexDiffuseSaver::Save( const string& path )
	{
		assert(NULL != m_pProgress);

		m_pProgress->SetProgress(0);
	
		TileModelDiffuseMap& modDifMap = CVertexDiffuseColor::GetInst()->GetAllObjectColor();
		TileModelDiffuseIter it  = modDifMap.begin();
		TileModelDiffuseIter eit = modDifMap.end();
		for ( ; it != eit; ++it )
		{
			stringstream ss;
			TileIndex ti = it->first;
			ss << ti.first << "_" << ti.second << ".diffuse";
			string szSaveName = path + "/" + ss.str();
			m_index.push_back(ti);

			m_stream->SetPath(szSaveName);
			if ( !m_writer->Open() )
				return false;

			saveTileModelDiffuse(it->second);

			m_writer->Close();
		
		}

		saveIndexFile(path + "/index.didx");

		return true;
	}

	void CVertexDiffuseSaver::saveIndexFile( const string& path )
	{
		m_stream->SetPath(path);
		if ( !m_writer->Open() )
			return;

		m_writer->WriteUInt32('DIDX');
		m_writer->WriteUInt32(m_index.size());

		IndexList::iterator it  = m_index.begin();
		IndexList::iterator eit = m_index.end();
		for ( ; it != eit; ++it )
		{
			m_writer->WriteUInt32(it->first);
			m_writer->WriteUInt32(it->second);
		}

		m_writer->Close();
	}

	void CVertexDiffuseSaver::saveTileModelDiffuse( GridModelDiffuseMap& gmdm )
	{
		m_writer->WriteUInt32('DICL');

		const uint32 nGridNum = gmdm.size();
		m_writer->WriteUInt32(nGridNum);

		GridModelDiffuseIter it		= gmdm.begin();
		GridModelDiffuseIter eit	= gmdm.end();
		for ( ; it != eit; ++it )
		{
			uint32 grid = it->first;
			m_writer->WriteUInt32(grid);

			IdxModelGroupDiffuseStruct& i2mgd = it->second;
			saveIndex2ModelGroupDiffuse(i2mgd.i2mgdm);
		}
	}

	void CVertexDiffuseSaver::saveIndex2ModelGroupDiffuse( Idx2ModelGroupDiffuseMap& i2mgd )
	{
		const uint32 nGroupNum = i2mgd.size();
		m_writer->WriteUInt32(nGroupNum);

		Idx2ModelGroupDiffuseIter it  = i2mgd.begin();
		Idx2ModelGroupDiffuseIter eit = i2mgd.end();

		for ( ; it != eit; ++it )
		{
			uint32 index = it->first;
			m_writer->WriteUInt32(index);

			ModelGroupDiffuseMap& mgd = it->second; 
			saveModelGroupDiffuseMap(mgd);
		}
	}

	void CVertexDiffuseSaver::saveModelGroupDiffuseMap( ModelGroupDiffuseMap& mgd )
	{
		const uint32 nModNum = mgd.size();
		m_writer->WriteUInt32(nModNum);

		ModelGroupDiffuseIter it	= mgd.begin();
		ModelGroupDiffuseIter eit	= mgd.end();

		for ( ; it != eit; ++it )
		{
			const ModelName& name = it->first;
			m_writer->WriteString(name);

			PieceClassDiffuseVector& pcdv = it->second;
			savePieceClassDiffuseVector( pcdv );
		}
	}

	void CVertexDiffuseSaver::savePieceClassDiffuseVector( PieceClassDiffuseVector& pcdv )
	{
		const uint32 nClassNum = pcdv.size();
		m_writer->WriteUInt32(nClassNum);

		PieceClassDiffuseIter it	= pcdv.begin();
		PieceClassDiffuseIter eit	= pcdv.end();

		for ( ; it != eit; ++it )
		{
			PieceClassDiffuse& pcd = *it;

			const string& name = pcd.className;
			m_writer->WriteString(name);

			PieceDiffuseVector& pdv = pcd.pieceColorVec;
			savePieceDiffuseVector(pdv);
		}
	}

	void CVertexDiffuseSaver::savePieceDiffuseVector( PieceDiffuseVector& pdv )
	{
		const uint32 nPieceNum = pdv.size();
		m_writer->WriteUInt32(nPieceNum);

		PieceDiffuseIter it	 = pdv.begin();
		PieceDiffuseIter eit = pdv.end();

		for ( ; it != eit; ++it )
		{
			PieceDiffuse& pd = *it;
			const uint32 diffuse_num = pd.size();
			m_writer->WriteUInt32(diffuse_num);

			for ( uint32 i=0; i<diffuse_num; ++i )
			{
				m_writer->WriteUInt32(pd[i]);
			}
		}
	}

}
