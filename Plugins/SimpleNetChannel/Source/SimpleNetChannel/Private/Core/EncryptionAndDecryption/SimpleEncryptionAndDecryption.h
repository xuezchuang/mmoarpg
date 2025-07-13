#pragma once

#include "CoreMinimal.h"
#include "Global/SimpleNetGlobalInfo.h"

namespace SimpleEncryptionAndDecryption
{
	extern uint8	Head[2];
	extern uint16	m_rCode;

	void Encryption(TArray<uint8>& InData, uint16 rCode);

	void Decryption(uint8* InData, int32 InLen, uint16 rCode);

	void Decryption(TArray<uint8>& InData, uint16 rCode);
}
