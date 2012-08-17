#include "stdafx.h"
#include "CEditModCoder.h"
#include "CEditPieceGroup.h"

namespace sqr_tools
{
	BEGIN_VERSION_MAP(CEditModCoder)
		ON_VERSION(1, DeCode_1)
	END_VERSION_MAP()

	FourCC CEditModCoder::GetChunkName()
	{
		return 'EMOD';
	}

	uint16 CEditModCoder::GetNewVer()
	{
		return 1;
	}

	bool CEditModCoder::Code( WriteDataInf& wdi ,OBJ_HANDLE handle )
	{
		CEditPieceGroup* piece_group = (CEditPieceGroup*)handle;
		piece_group->m_str_sample = "hello mod!";
		piece_group->m_int_sample = 0xDEADC0DE;

		int	str_sample_len = piece_group->m_str_sample.length();
		wdi.Write(&str_sample_len, sizeof(str_sample_len));
		wdi.Write(piece_group->m_str_sample.c_str(), str_sample_len);

		wdi.Write(&piece_group->m_int_sample, sizeof(piece_group->m_int_sample));

		return true;
	}

	bool CEditModCoder::DeCode_1( const CDataChunk& chunk, OBJ_HANDLE handle )
	{
		CEditPieceGroup* edit_model = (CEditPieceGroup*)handle;
		CBufFile buf(chunk.GetBuffer(), chunk.GetSize());

		int sample_str_len = 0;
		buf.Read(&sample_str_len, sizeof(int));
		buf.Read(&edit_model->m_str_sample[0], sample_str_len);

		buf.Read(&edit_model->m_int_sample, sizeof(edit_model->m_int_sample));

		return true;
	}
}