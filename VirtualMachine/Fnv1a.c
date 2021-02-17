#include "global.h"


DWORD
Fnv1aCodeOneByte(
	BYTE	ucByte,
	DWORD	dwHashSeed
);


DWORD
Fnv1aCompute(
	PVOID		pData,
	DWORD		dwDataLength,
	DWORD		dwDataSize,
	DWORD		dwHashSeed
)
{
	DWORD dwBytesCount = dwDataLength * dwDataSize;

	if (pData == NULL)
	{
		return 0;
	}

	BYTE* pszCastedData = (BYTE*)pData;
	DWORD dwHash = dwHashSeed;

	while (dwBytesCount--)
	{
		dwHash = Fnv1aCodeOneByte(*pszCastedData++, dwHash);
	}

	return dwHash;
}


DWORD
Fnv1aCodeOneByte(
	BYTE	ucByte,
	DWORD	dwHashSeed
)
{
	return (ucByte ^ dwHashSeed) * FNV_1A_PRIME;
}