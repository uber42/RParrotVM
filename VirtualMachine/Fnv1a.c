#include "global.h"


u32
Fnv1aCodeOneByte(
	i8	ucByte,
	u32	dwHashSeed
);


u32
Fnv1aCompute(
	void*		pData,
	u32			dwDataLength,
	u32			dwDataSize,
	u32			dwHashSeed
)
{
	u32 dwBytesCount = dwDataLength * dwDataSize;

	if (pData == NULL)
	{
		return 0;
	}

	u8* pszCastedData = (u8*)pData;
	u32 dwHash = dwHashSeed;

	while (dwBytesCount--)
	{
		dwHash = Fnv1aCodeOneByte(*pszCastedData++, dwHash);
	}

	return dwHash;
}


u32
Fnv1aCodeOneByte(
	i8	ucByte,
	u32	dwHashSeed
)
{
	return (ucByte ^ dwHashSeed) * FNV_1A_PRIME;
}