//------------------------------------------------------------------------------
//  VertexColorSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "VertexColorSaver.h"
#include "TerrainMesh.h"
#include "FileStream.h"
#include "BinaryWriter.h"
#include "Engine.h"
#include "OffLineBaker/CVertexColor.h"
#include "DepathGroupModelInfo.h"

namespace sqr
{

//------------------------------------------------------------------------------
VertexColorSaver::VertexColorSaver()
		: terrain(NULL)
		, progress(NULL)
		, stream(new FileStream())
		, writer(new BinaryWriter())
{
	this->stream->SetAccessMode(Stream::WriteAccess);
	this->writer->SetStream(stream);
}

//------------------------------------------------------------------------------
bool
VertexColorSaver::Save(const string& path)
{
	Ast(NULL != this->terrain);
	Ast(NULL != this->progress);

	this->progress->SetProgress(0);

	this->stream->SetPath(path);
	if (!this->writer->Open())
		return false;

	// header
	const uint version = 1;
	this->writer->WriteUInt32('BKCL');
	this->writer->WriteUInt32(version);
	uint headSize = sizeof(index_t) * this->terrain->GetGridCount();
	this->writer->WriteUInt32(headSize);
	// gird chunk indices
	index_t girdIndexOffset = this->stream->GetPosition();
	vector<index_t> gridIndices(this->terrain->GetGridCount(), 0);
	this->writer->WriteData(&gridIndices.front(), sizeof(index_t) * gridIndices.size());
	for (index_t gridIndex = 0; gridIndex < size_t(this->terrain->GetGridCount()); ++gridIndex)
	{
		SGrid& grid = this->terrain->GetGrid(gridIndex);
		DepathModelInfoMap::iterator itDepathInfo = this->terrain->m_DepathModelMap.find(gridIndex);

		if (grid.m_ObjectColorMap.empty() && this->terrain->m_DepathModelMap.end() == itDepathInfo)
			continue;	// no model
		gridIndices[gridIndex] = stream->GetPosition();

		// write model color
		this->WriteGridColor(gridIndex);

		// udpate progress
		this->progress->SetProgress(float(gridIndex + 1) / this->terrain->GetGridCount());
	}
	// write gird chunk indices
	this->stream->Seek(girdIndexOffset, Stream::Begin);
	this->writer->WriteData(&gridIndices.front(), sizeof(index_t) * gridIndices.size());

	// end writting
	this->writer->Close();
	return true;
}

//------------------------------------------------------------------------------
void
VertexColorSaver::WriteGridColor(index_t gridIndex)
{
	SGrid& grid = this->terrain->GetGrid(gridIndex);

	// model chunk
	const uint version = 1;
	this->writer->WriteUInt32('GRID');
	this->writer->WriteUInt32(version);
	index_t gridChunkSizeOffset = this->stream->GetPosition();
	this->writer->WriteUInt32(0);

	// model num
	uint numModels = grid.m_ObjectColorMap.size();
	DepathModelInfoMap::iterator it = this->terrain->m_DepathModelMap.find(gridIndex);
	bool hasDepathModel = false;
	if (this->terrain->m_DepathModelMap.end() != it)
	{
		hasDepathModel = true;
		numModels += it->second.size();
	}
	this->writer->WriteUInt32(numModels);
	// depath models
	if (hasDepathModel)
	{
		DepathModelInfoArray& depathModelInfos = it->second;
		for (index_t i = 0; i < depathModelInfos.size(); ++i)
		{
			const DepathModelInfoPtr& depathModelInfo = depathModelInfos[i];
			const string& modelName = depathModelInfo->modelName;
			// find model color
			ModelGroupIndex modelGroupIndex = 0;
			index_t originGridIndex = depathModelInfo->nOriginGridIndex;
			SGrid& originGrid = this->terrain->GetGrid(originGridIndex);
			const vector<PieceClassColor>* classColors = NULL;
			for (map<ModelGroupIndex, ModelGroupColorMap>::const_iterator itg = originGrid.m_DepathObjColorMap.begin();
			        itg != originGrid.m_DepathObjColorMap.end(); ++itg)
			{
				modelGroupIndex = itg->first;
				const ModelGroupColorMap& groupColorMap = itg->second;
				ModelGroupColorMap::const_iterator itm = groupColorMap.find(modelName.c_str());
				if (groupColorMap.end() == itm)
					continue;
				classColors = &itm->second;
				break;
			}
			if( NULL == classColors )
			{
				string log = modelName + " 烘培信息是空，保存失败，如果希望保存烘培文件，请重新进行烘培!";
				MessageBox(NULL, log.c_str(),"提示",MB_OK);
				return;
			}
			this->WriteModelColor(originGridIndex, modelGroupIndex,
			                      modelName, *classColors);
		}
	}
	// lonely models
	for (map<ModelGroupIndex, ModelGroupColorMap>::iterator itg = grid.m_ObjectColorMap.begin();
	        itg != grid.m_ObjectColorMap.end(); ++itg)
	{
		ModelGroupColorMap& groupColorMap = itg->second;
// 		if(groupColorMap.size() == 1)
// 		{
// 			DebugOut("save grid is %d\n", grid.dwGridIndex);
// 		}
		if ( !groupColorMap.empty() )
		{
			this->WriteModelColor(0, itg->first,
				groupColorMap.begin()->first.c_str(),
				groupColorMap.begin()->second);
		}
	}

	// write back grid size
	uint gridChunkSize = this->stream->GetPosition() - gridChunkSizeOffset - sizeof(index_t);
	this->stream->Seek(gridChunkSizeOffset, Stream::Begin);
	this->writer->WriteUInt32(gridChunkSize);
	this->stream->Seek(gridChunkSize, Stream::Current);
}

//------------------------------------------------------------------------------
void
VertexColorSaver::WriteModelColor( index_t originGridIndex,
                                   index_t modelGroupIndex,
                                   const string& modelName,
                                   const vector<PieceClassColor>& classColors)
{
	Ast(!classColors.empty());

	const uint version = 1;
	this->writer->WriteUInt32('MODL');
	this->writer->WriteUInt32(version);
	index_t modelChunkSizeOffset = this->stream->GetPosition();
	this->writer->WriteUInt32(0);
	// model info
	this->writer->WriteUInt32(originGridIndex);
	this->writer->WriteUInt32(modelGroupIndex);
	this->writer->WriteString(modelName);
	// model piece classes
	this->writer->WriteUInt32(classColors.size());
	for (index_t i = 0; i < classColors.size(); ++i)
	{
		const PieceClassColor& classColor = classColors[i];
		this->WriteClassColor(&classColor);
	}

	// write back model chunk size
	uint modelChunkSize = this->stream->GetPosition() - modelChunkSizeOffset - sizeof(index_t);
	this->stream->Seek(modelChunkSizeOffset, Stream::Begin);
	this->writer->WriteUInt32(modelChunkSize);
	this->stream->Seek(modelChunkSize, Stream::Current);
}

//------------------------------------------------------------------------------
void
VertexColorSaver::WriteClassColor(const PieceClassColor* classColor)
{
	Ast(NULL != classColor);
	Ast(!classColor->className.empty());
	Ast(!classColor->pieceColorMap.empty());

	const uint version = 1;
	this->writer->WriteUInt32('CLAS');
	this->writer->WriteUInt32(version);
	index_t classChunkSizeOffset = this->stream->GetPosition();
	this->writer->WriteUInt32(0);
	// class name
	this->writer->WriteString(classColor->className.c_str());
	// pieces
	const PieceColorMap& pieceColorMap = classColor->pieceColorMap;
	this->writer->WriteUInt32(pieceColorMap.size());
	index_t pieceIndex = 0;
	for (PieceColorMap::const_iterator it = pieceColorMap.begin();
	        it != pieceColorMap.end(); ++it, ++pieceIndex)
	{
		CVertexHardwareBuffer* colors = it->second->GetVB();
		Ast(colors->getNumVertices()!=0);
		this->writer->WriteUInt32(colors->getNumVertices());
		void* pScrBuffer = colors->lock(HBL_READ_ONLY);
		this->writer->WriteData(pScrBuffer, colors->getSize());
		colors->unlock();
	}

	// write back class chunk size
	uint classChunkSize = this->stream->GetPosition() - classChunkSizeOffset - sizeof(index_t);
	this->stream->Seek(classChunkSizeOffset, Stream::Begin);
	this->writer->WriteUInt32(classChunkSize);
	this->stream->Seek(classChunkSize, Stream::Current);
}

}// namespace sqr
