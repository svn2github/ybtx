#pragma	once
#include "CCameraPath.h"


namespace sqr
{
	class CCameraPath;
	class CCamera;
	class WriteDataInf;
	class CBufFile;

	class CCameraPathEdit : public CCameraPath
	{
	public:
		static const int	INVALID_KEY_POS = CHermiteCurve::INVALID_KEY_POS;

		struct Key
		{
			CVector3f	position;
			CVector3f	target;
			CVector3f	fov;
		};

	public:
		int		AddCamAnimKey( float fNewKeyTime );
		void	DelCamAnimKey( uint32 uAnimKeyIndex );
		void	SetHeadFrame( const Key &camera_key );
		void	SetTailFrame( const Key &camera_key );
		void	SetKeyTime( float fNewTime, uint32 uKeyIndex );
		void	SetKeyValue( uint32 uKeyIndex );
		
		float	GetLength() const;
		float	GetAnimKeyTime( int uKeyIndex ) const;
		uint32	GetKeyCount() const;
		void	GetKeyValue( Key &sFrameValue, uint32 uFrameIndex ) const;
		void	GetHeadFrame( Key &camera_key ) const;
		void	GetTailFrame( Key &camera_key ) const;
		int		GetCurrentKeyIndex() const;

		void	BinLoad(FILE *fp);
		void	BinSave(FILE *fp);

		void	SaveCameraPathInfo(WriteDataInf* wdi);
		void	LoadCameraPathInfo(CBufFile* tData);
	};
}