#pragma once

class CSysAlgorithmCrc
{
protected:
	CSysAlgorithmCrc();
	~CSysAlgorithmCrc();

public:
	static CSysAlgorithmCrc* Instance();
	template<class Var, class Ch>
	void Compute( Var& var, Ch ch );
	unsigned __int32 Crc32( const void *pInput, __int32 nInputLength );

private:
	static CSysAlgorithmCrc* s_pInst;

};
