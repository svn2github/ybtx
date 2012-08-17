#pragma once
#include "CBaseEncryptedConnection.inl"

namespace sqr
{
	template<class ImpClass>
	class CBaseEncryptedConnection
	{
	public:
		CBaseEncryptedConnection(ICorePipe* pPipe);

		void SetPipe(ICorePipe* pPipe);
		ICorePipe* GetPipe()const;

		uint8 Encrypt(uint8 uData);
		uint8 Decrypt(uint8 uData);

		void SendEncryptMsg(const void* pData,unsigned int uSize);
		unsigned OnEncryptedMessage(const void* pData,unsigned int uSize);

	private:
		bool m_bShakedHand;
		CByteDecoder m_Decoder;
		CByteEncoder m_Encoder;
		unsigned int m_uDecryptedSize;
		ICorePipe* m_pPipe;

		void SendEncryptKey();
		bool EncryptionEnabled()const;
	};
}