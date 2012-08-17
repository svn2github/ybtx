#pragma once
#include "CMesh.h"
#include "CSkeletalMesh.h"
#include "CStaticMesh.h"
#include "CVertexAniMesh.h"

namespace sqr_tools
{
	class CMeshSaveBase;
	class CEditMeshWriter
	{
	protected:
		CVector3f	m_EditOrgScale;						//模型在自身坐标空间的大小
		CVector3f	m_EditOrgOff;						//模型中心对自身坐标空间原点的偏移量
		float		m_EditfTextScale;
		
		bool		ComSave(ofstream& File,CMeshSaveBase* tSaveBase);
		virtual int	 GetVertexSize(void) { return 0; };
		virtual	bool _SaveCompress(ofstream& File) = 0;
		virtual	bool _SaveFull(ofstream& File) = 0;
	public:
		virtual void SetScale(const CVector3f& OrgScal) = 0;
		virtual void SetOff(const CVector3f& OrgOff)	= 0;
	};

	class CMeshSaveBase 
		: public CMeshBase
	{
		friend class CEditMeshWriter;
	};

	class CEditSkeletalMesh 
		: public CSkeletalMesh
		, public CEditMeshWriter
	{
	public:
		bool BinSave(ofstream& File);
		GVector<VertexSkeletalFormat>	m_OrgVertex;

		virtual void SetScale(const CVector3f& OrgScal);
		virtual void SetOff(const CVector3f& OrgOff);
	protected:
		void _RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale);
		void _RecordVertex(GVector<VertexSkeletalFormat>& VertexOrgBuf) { m_OrgVertex = VertexOrgBuf; };
		bool _SaveCompress(ofstream& File);
		bool _SaveFull(ofstream& File);
		int	 GetVertexSize(void);
		GVector<VertexSkeletalFormat>*	 _GetVertex(void);
	};

	class CEditStaticMesh 
		: public CStaticMesh
		, public CEditMeshWriter
	{
	public:
		bool BinSave(ofstream& File);

		virtual void SetScale(const CVector3f& OrgScal);
		virtual void SetOff(const CVector3f& OrgOff);
	protected:
		void _RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale);
		bool _SaveCompress(ofstream& File);
		bool _SaveFull(ofstream& File);
		int	 GetVertexSize(void);
	};

	class CEditVertexAniMesh 
		: public CVertexAniMesh
		, public CEditMeshWriter
	{
	public:
		bool BinSave(ofstream& File);

		virtual void SetScale(const CVector3f& OrgScal);
		virtual void SetOff(const CVector3f& OrgOff);
	protected:
		void _RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale);
		bool _SaveCompress(ofstream& File);
		bool _SaveFull(ofstream& File);
	};
}
