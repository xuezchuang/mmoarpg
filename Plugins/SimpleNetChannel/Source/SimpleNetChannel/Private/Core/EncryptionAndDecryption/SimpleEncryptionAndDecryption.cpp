#include "SimpleEncryptionAndDecryption.h"
#include "Log/SimpleNetChannelLog.h"

namespace SimpleEncryptionAndDecryption
{
	uint8 Head[2] = { 'D','E' };
	uint16 m_rCode = 130;
}

void SimpleEncryptionAndDecryption::Encryption(TArray<uint8>& InData, uint16 rCode)
{
	InData[0] = Head[0] ^ rCode;
	InData[1] = Head[1] ^ rCode;

	uint16 newcmd = (static_cast<uint16>(InData[6]) | (static_cast<uint16>(InData[7]) << 8));
	newcmd ^= rCode;
	InData[6] = static_cast<uint8>(newcmd & 0xFF);
	InData[7] = static_cast<uint8>(newcmd >> 8);

	int32 len = *reinterpret_cast<int32*>(&InData[2]);
	len = len ^ rCode;
	*reinterpret_cast<int32*>(&InData[2]) = len;
}

void SimpleEncryptionAndDecryption::Decryption(uint8* InData, int32 InLen, uint16 rCode)
{
	InData[0] = InData[0] ^ rCode;
	InData[1] = InData[1] ^ rCode;

	uint16 newcmd = (static_cast<uint16>(InData[6]) | (static_cast<uint16>(InData[7]) << 8));
	newcmd ^= rCode;
	InData[6] = static_cast<uint8>(newcmd & 0xFF);
	InData[7] = static_cast<uint8>(newcmd >> 8);

	uint16 test = static_cast<uint16>(InData[6]);

	int32 len = *reinterpret_cast<int32*>(&InData[2]);
	len = len ^ rCode;
	*reinterpret_cast<int32*>(&InData[2]) = len;

	FSimpleBunchHead* pCheck = reinterpret_cast<FSimpleBunchHead*>(InData);
	if(pCheck->len != InLen)
	{
		UE_LOG(LogSimpleNetChannel, Error, TEXT("Len is Error; [cmd:%d]"), newcmd);
	}

}

void SimpleEncryptionAndDecryption::Decryption(TArray<uint8>& InData, uint16 rCode)
{
	Decryption(InData.GetData(), InData.Num(), rCode);
}
