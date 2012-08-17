//------------------------------------------------------------------------------
//  VertexColorLoader.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "VertexColorLoader.h"
#include "TerrainMesh.h"
#include "Engine.h"
#include "FileStream.h"
#include "BinaryReader.h"
#include "OffLineBaker/CVertexColor.h"

namespace sqr
{

//------------------------------------------------------------------------------
VertexColorLoader::VertexColorLoader()
: terrain(NULL)
, progress(NULL)
, stream(new FileStream())
, reader(new BinaryReader())
{
	this->stream->SetAccessMode(Stream::ReadAccess);
	this->reader->SetStream(this->stream);
}

//------------------------------------------------------------------------------
bool
VertexColorLoader::Load( const string& path )
{
	Ast(NULL != this->terrain);
	Ast(NULL != this->progress);

	this->progress->SetProgress(0);

	this->stream->SetPath(path);
	if (!this->reader->Open())
		return false;

	// header
	uint fourcc = this->reader->ReadUInt32();
	Ast('BKCL' == fourcc);
	uint version = this->reader->ReadUInt32();
	Ast(1 == version);
	size_t numGrids = this->reader->ReadUInt32() / sizeof(index_t);
	Ast(this->terrain->GetGridCount() == numGrids);
	vector<index_t> gridIndices(numGrids, 0);
	this->reader->ReadData(&gridIndices.front(), sizeof(index_t) * gridIndices.size());
	// grids
	for (index_t gridIndex = 0; gridIndex < gridIndices.size(); ++gridIndex)
	{
		index_t gridOffset = gridIndices[gridIndex];
		if (0 == gridOffset)
			continue;// no model

		if ( this->stream->GetPosition() != gridOffset )
		{
			this->reader->Close();
			return false;
		}

		if ( !this->ReadGridColor(gridIndex) )
		{
			this->reader->Close();
			return false;
		}
		
		// update progress
		this->progress->SetProgress(float(gridIndex + 1) / numGrids);
	}

	// end reading
	this->reader->Close();
	return true;
}

//------------------------------------------------------------------------------
bool
VertexColorLoader::ReadGridColor( index_t gridIndex )
{
	SGrid& grid = this->terrain->GetGrid(gridIndex);

	// model chunk
	uint fourcc = this->reader->ReadUInt32();
	if( 'GRID' != fourcc )
		return false; 
	
	uint version = this->reader->ReadUInt32();
	if( 1 != version )
		return false;
	
	size_t chunkSize = this->reader->ReadUInt32();
	if( 0 == chunkSize )
		return false;

	size_t numModels = this->reader->ReadUInt32();
	for (index_t i = 0; i < numModels; ++i)
	{
		index_t originGridIndex = InvalidIndex;
		index_t modelGroupIndex = InvalidIndex;
		string modelName;
		vector<PieceClassColor> classColors;
		this->ReadModelColor(originGridIndex, modelGroupIndex,
							 modelName, classColors);
		if (0 != originGridIndex)
		{
			// depath model
			SGrid& originGrid = this->terrain->GetGrid(originGridIndex);
			ModelGroupColorMap& groupColorMap = originGrid.m_DepathObjColorMap[modelGroupIndex];
			groupColorMap[modelName.c_str()] = classColors;
		}
		else
		{
			// lonely model
			ModelGroupColorMap& groupColorMap = grid.m_ObjectColorMap[modelGroupIndex];
			groupColorMap[modelName.c_str()] = classColors;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
/**
	参数都是返回值
 */
void
VertexColorLoader::ReadModelColor( index_t& originGridIndex,
								   index_t& modelGroupIndex,
								   string& modelName,
								   vector<PieceClassColor>& classColors )
{
	uint fourcc = this->reader->ReadUInt32();
	Ast('MODL' == fourcc);
	uint version = this->reader->ReadUInt32();
	Ast(1 == version);
	size_t chunkSize = this->reader->ReadUInt32();
	Ast(0 != chunkSize);
	// model info
	originGridIndex = this->reader->ReadUInt32();
	modelGroupIndex = this->reader->ReadUInt32();
	modelName = this->reader->ReadString();
	// piece classes
	size_t numClasses = this->reader->ReadUInt32();
	classColors.resize(numClasses);
	for (index_t i = 0; i < numClasses; ++i)
	{
		PieceClassColor& classColor = classColors[i];
		this->ReadClassColor(&classColor);
	}
}

//------------------------------------------------------------------------------
/**
	参数是返回值
 */
void
VertexColorLoader::ReadClassColor( PieceClassColor* classColor )
{
	Ast(NULL != classColor);
	Ast(classColor->pieceColorMap.empty());

	uint fourcc = this->reader->ReadUInt32();
	Ast('CLAS' == fourcc);
	uint version = this->reader->ReadUInt32();
	Ast(1 == version);
	size_t chunkSize = this->reader->ReadUInt32();
	Ast(0 != chunkSize);

	// class name
	classColor->className = this->reader->ReadString().c_str();
	// pieces
	size_t numPieces = this->reader->ReadUInt32();
	for (index_t i = 0; i < numPieces; ++i)
	{
		size_t numColors = this->reader->ReadUInt32();
		CVertexHardwareBuffer* pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(
			sizeof(DWORD),
			numColors,
			HBU_WRITE_ONLY);
		void* pDstBuf = pBuffer->lock(HBL_NORMAL);
		this->reader->ReadData(pDstBuf, pBuffer->getSize());
		pBuffer->unlock();
		VBRef* newVB = new VBRef();
		newVB->SetVB(pBuffer);
		classColor->pieceColorMap[i] = newVB;
	}
}

}// namespace sqr
