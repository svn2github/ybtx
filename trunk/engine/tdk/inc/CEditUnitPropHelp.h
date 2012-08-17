#pragma once
#include "CEditEffectBase.h"
#include "CEffectUnit.h"
#include "CEditEffectTrack.h"
#include "AutoDataHelper.h"

#ifndef   IEditEffectUnitProp
#define  IEditEffectUnitProp CEditUnitPropHelp
#endif

namespace sqr
{
	typedef void(* EFN_PROC)( void* );
	class CEditUnitPropHelp : public AutoDataHelper
	{
	public:
		void		SetUnitVisible(bool isShow);
		bool		GetUnitVisible(void);

	public:
		CEditUnitPropHelp();
		virtual ~CEditUnitPropHelp();
		void					SetUnitProp(CEffectUnitProp* pProp);
		void					SetBaseUintData();

		CEffectUnitProp*			&GetUnitProp() { return m_pEffectUnitProp;}
		virtual void				SetProp( const EFFECT_PROP& Prop ){};
		virtual int32				GetProp( EFFECT_PROP* pProp, int32 nBufLen );
		virtual void				SetProp( const EFFECT_PROP* pProp, int32 nBufLen );
		virtual uint32				GetAdvancedProp( ADVANCE_PROP* pOffset,  int32& nOffLen,
			ADVANCE_PROP* pTexture, int32& nTextLen,
			ADVANCE_PROP* pColor,   int32& nColorLen,
			ADVANCE_PROP* pScale,   int32& nScaleLen );

		virtual uint32				GetAdvancedProp(	ADVANCE_PROP* pOffset,  int32& nOffLen,
			ADVANCE_PROP* pTexture, int32& nTextLen,
			ADVANCE_PROP* pColor,   int32& nColorLen,
			ADVANCE_PROP* pScale,   int32& nScaleLen,
			ADVANCE_PROP* pXYZcale,  int32& nXYZcale );

		virtual uint32				GetAdvancedProp( ADVANCE_PROP* pOffset,  int32& nOffLen,
			ADVANCE_PROP* pTexture,   int32& nTextLen,
			ADVANCE_PROP* pColor,     int32& nColorLen,
			ADVANCE_PROP* pScale,     int32& nScaleLen, 
			ADVANCE_PROP* pXYZcale,   int32& nXYZcale,
			ADVANCE_PROP* pUVRCenter, int32& nUVRCenter);

		virtual uint32				GetAdvancedProp( ADVANCE_PROP* pOffset,  int32& nOffLen,
			ADVANCE_PROP* pTexture,   int32& nTextLen,
			ADVANCE_PROP* pColor,     int32& nColorLen,
			ADVANCE_PROP* pScale,     int32& nScaleLen, 
			ADVANCE_PROP* pXYZcale,   int32& nXYZcale,
			ADVANCE_PROP* pUVRCenter, int32& nUVRCenter,
			ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
			ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius);

		virtual void				SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
			const ADVANCE_PROP* pTexture, int32 nTextLen,
			const ADVANCE_PROP* pColor,   int32 nColorLen,
			const ADVANCE_PROP* pScale,   int32 nScaleLen );

		virtual void				SetAdvancedProp(	const ADVANCE_PROP* pOffset,  int32 nOffLen,
			const ADVANCE_PROP* pTexture, int32 nTextLen,
			const ADVANCE_PROP* pColor,   int32 nColorLen,
			const ADVANCE_PROP* pScale,   int32 nScaleLen,
			const ADVANCE_PROP* pXYZcale,  int32 nXYZcale);

		virtual void				SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
			const ADVANCE_PROP* pTexture,   int32 nTextLen,
			const ADVANCE_PROP* pColor,     int32 nColorLen,
			const ADVANCE_PROP* pScale,     int32 nScaleLen,
			const ADVANCE_PROP* pXYZcale,   int32 nXYZcale,
			const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter);

		virtual void				SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
			const ADVANCE_PROP* pTexture,   int32 nTextLen,
			const ADVANCE_PROP* pColor,     int32 nColorLen,
			const ADVANCE_PROP* pScale,     int32 nScaleLen,
			const ADVANCE_PROP* pXYZcale,   int32 nXYZcale,
			const ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
			const ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius);

		virtual void				SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
			const ADVANCE_PROP* pTexture,   int32 nTextLen,
			const ADVANCE_PROP* pColor,     int32 nColorLen,
			const ADVANCE_PROP* pScale,     int32 nScaleLen,
			const ADVANCE_PROP* pXYZcale,   int32 nXYZcale,
			const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter,
			const ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
			const ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius);

		virtual void				SetAdvancedProp( const ADVANCE_PROP* pEmiMinRad, int32 nEmissionMinRadius,
			const ADVANCE_PROP* pEmiMaxRad,   int32 nEmissionMaxRadius);

		virtual void				GetBaseProp( EVector< EFFECT_PROP >& Prop );
		virtual void				SetBaseProp( const EVector< EFFECT_PROP >& Prop );

	private:
		bool				m_isUnitVisible;
		CEffectUnitProp*	m_pEffectUnitProp;
	};

	class CEffectUnitPropMiddle
		: public CEffectUnitProp
	{
		friend class CEditUnitPropHelp;
	};
	
}