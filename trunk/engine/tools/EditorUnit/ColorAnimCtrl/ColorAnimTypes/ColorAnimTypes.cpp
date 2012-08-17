#include "stdafx.h"
#include "ColorAnimTypes.h"
#include "CColor.inl"
#include "CColor4.inl"
#include "CGraphicMallocObject.h"
#include "CEffectMallocObject.h"

namespace sqr_tools
{
	typedef GMap<float, CColor4> Color4Map;
	typedef GMap<float, CColor>	ColorMap;
	typedef GMap<float, uint32>	Uint32Map;
	typedef GMap<float, float>	FloatMap;

	typedef EMap<float, CColor>	EColorMap;

	class CColor4Type : public CColorAnimTypeBase
	{
	public:
		CColor4Type( TrackNodeMap& nodeMap )
			: CColorAnimTypeBase(nodeMap)
			, m_data_ref(NULL)
		{
		}

		virtual void SetData( void* dataIn )
		{
			if ( dataIn == NULL || dataIn == m_data_ref )
				return;

			m_data_ref = (Color4Map*)dataIn;
			if ( m_data_ref->size() < 2 )
			{
				m_data_ref->clear();
				m_data_ref->insert(make_pair(0.0f, CColor4(0x00000000)));
				m_data_ref->insert(make_pair(1.0f, CColor4(0xffffffff)));
			}

			m_nodeMap.clear();
			ColorAnimNode node;
			Color4Map::iterator it, eit = m_data_ref->end();
			for ( it = m_data_ref->begin(); it != eit; ++it )
			{
				node.a = it->second.a;
				node.r = it->second.r;
				node.g = it->second.g;
				node.b = it->second.b;
				m_nodeMap.insert(make_pair(it->first, node));
			}
		}

		virtual void Update()
		{
			if ( m_data_ref == NULL )
				return;
			
			m_data_ref->clear();
			TrackNodeMapIter it, eit = m_nodeMap.end();
			for ( it = m_nodeMap.begin(); it != m_nodeMap.end(); ++it )
			{
				ColorAnimNode& node = it->second;
				CColor4 color(node.r, node.g, node.b, node.a);
				m_data_ref->insert(make_pair(it->first, color));
			}
		}

	private:
		Color4Map* m_data_ref;
	};


	class CColorType : public CColorAnimTypeBase
	{
	public:
		CColorType( TrackNodeMap& nodeMap )
			: CColorAnimTypeBase(nodeMap)
			, m_data_ref(NULL)
		{
		}

		virtual void SetData( void* dataIn )
		{
			m_data_ref = (ColorMap*)dataIn;
			if ( m_data_ref->size() < 2 )
			{
				m_data_ref->clear();
				m_data_ref->insert(make_pair(0.0f, 0x00000000));
				m_data_ref->insert(make_pair(1.0f, 0xffffffff));
			}

			m_nodeMap.clear();
			ColorAnimNode node;
			ColorMap::iterator it, eit = m_data_ref->end();
			for ( it = m_data_ref->begin(); it != eit; ++it )
			{
				node.a = it->second.A/255.0f;
				node.r = it->second.R/255.0f;
				node.g = it->second.G/255.0f;
				node.b = it->second.B/255.0f;
				m_nodeMap.insert(make_pair(it->first, node));
			}
		}

		virtual void Update()
		{
			if ( m_data_ref == NULL )
				return;

			m_data_ref->clear();
			TrackNodeMapIter it, eit = m_nodeMap.end();
			for ( it = m_nodeMap.begin(); it != eit; ++it )
			{
				ColorAnimNode& node = it->second;
				CColor color(uint8(node.a*255), uint8(node.r*255), uint8(node.g*255), uint8(node.b*255));
				m_data_ref->insert(make_pair(it->first, color));
			}
		}

	private:
		ColorMap* m_data_ref;
	};


	class CEColorType : public CColorAnimTypeBase
	{
	public:
		CEColorType( TrackNodeMap& nodeMap )
			: CColorAnimTypeBase(nodeMap)
			, m_data_ref(NULL)
		{
		}

		virtual void SetData( void* dataIn )
		{
			m_data_ref = (EColorMap*)dataIn;
			if ( m_data_ref->size() < 2 )
			{
				m_data_ref->clear();
				m_data_ref->insert(make_pair(0.0f, 0x00000000));
				m_data_ref->insert(make_pair(1.0f, 0xffffffff));
			}

			m_nodeMap.clear();
			ColorAnimNode node;
			EColorMap::iterator it, eit = m_data_ref->end();
			for ( it = m_data_ref->begin(); it != eit; ++it )
			{
				node.a = it->second.A/255.0f;
				node.r = it->second.R/255.0f;
				node.g = it->second.G/255.0f;
				node.b = it->second.B/255.0f;
				m_nodeMap.insert(make_pair(it->first, node));
			}
		}

		virtual void Update()
		{
			if ( m_data_ref == NULL )
				return;

			m_data_ref->clear();
			TrackNodeMapIter it, eit = m_nodeMap.end();
			for ( it = m_nodeMap.begin(); it != eit; ++it )
			{
				ColorAnimNode& node = it->second;
				CColor color(uint8(node.a*255), uint8(node.r*255), uint8(node.g*255), uint8(node.b*255));
				m_data_ref->insert(make_pair(it->first, color));
			}
		}

	private:
		EColorMap* m_data_ref;
	};


	class CUint32Type : public CColorAnimTypeBase
	{
	public:
		CUint32Type( TrackNodeMap& nodeMap )
			: CColorAnimTypeBase(nodeMap)
			, m_data_ref(NULL)
		{
		}

		virtual void SetData( void* dataIn )
		{
			m_data_ref = (Uint32Map*)dataIn;
			if ( m_data_ref->size() < 2 )
			{
				m_data_ref->clear();
				m_data_ref->insert(make_pair(0.0f, 0x00000000));
				m_data_ref->insert(make_pair(1.0f, 0xffffffff));
			}

			m_nodeMap.clear();
			Uint32Map::iterator it, eit = m_data_ref->end();
			for ( it = m_data_ref->begin(); it != eit; ++it )
			{
				CColor4 tmp(it->second);
				ColorAnimNode node(tmp.r, tmp.g, tmp.b, tmp.a);
				m_nodeMap.insert(make_pair(it->first, node));
			}
		}

		virtual void Update()
		{
			if ( m_data_ref == NULL )
				return;

			m_data_ref->clear();
			TrackNodeMapIter it, eit = m_nodeMap.end();
			for ( it = m_nodeMap.begin(); it != eit; ++it )
			{
				CColor4 tmp(it->second.r, it->second.g, it->second.b, it->second.a);
				m_data_ref->insert(make_pair(it->first, tmp.MakeDWord()));
			}
		}

	private:
		Uint32Map* m_data_ref;
	};

	
	class CFloatType : public CColorAnimTypeBase
	{
	public:
		CFloatType( TrackNodeMap& nodeMap )
			: CColorAnimTypeBase(nodeMap)
			, m_data_ref(NULL)
		{
		}

		virtual void SetData( void* dataIn )
		{
			m_data_ref = (FloatMap*)dataIn;
			if ( m_data_ref->size() < 2 )
			{
				m_data_ref->clear();
				m_data_ref->insert(make_pair(0.0f, 0.0f));
				m_data_ref->insert(make_pair(1.0f, 0.0f));
			}

			m_nodeMap.clear();
			FloatMap::iterator it, eit = m_data_ref->end();
			for ( it = m_data_ref->begin(); it != eit; ++it )
			{
				ColorAnimNode node(it->second, it->second, it->second, it->second);
				m_nodeMap.insert(make_pair(it->first, node));
			}
		}

		virtual void Update()
		{
			if ( m_data_ref == NULL )
				return;

			m_data_ref->clear();
			TrackNodeMapIter it, eit = m_nodeMap.end();
			for ( it = m_nodeMap.begin(); it != eit; ++it )
			{
				it->second.r = it->second.g = it->second.b = it->second.a;
				m_data_ref->insert(make_pair(it->first, it->second.a));
			}
		}

	private:
		FloatMap* m_data_ref;
	};


	CColorAnimTypeBase* CColorAnimTypeBase::CreateImp( ECtrlDataType type, TrackNodeMap& nodeMap )
	{
		switch ( type )
		{
		case E_COLOR4Map:
			return new CColor4Type(nodeMap);
			break;

		case E_COLORMap:
			return new CColorType(nodeMap);
			break;

		case E_UINT32CLMap:
			return new CUint32Type(nodeMap);
			break;

		case E_FLOATCLMap:
			return new CFloatType(nodeMap);
			break;	

		case E_ECOLORMap:
			return new CEColorType(nodeMap);
			break;
		}

		return NULL;
	}


	CColorAnimTypeBase::CColorAnimTypeBase( TrackNodeMap& nodeMap )
		: m_nodeMap(nodeMap)
	{
	}

}
