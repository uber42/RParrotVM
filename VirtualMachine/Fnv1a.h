#ifndef FNV1A_H
#define FNV1A_H

#define FNV_1A_SEED		0x811C9DC5
#define FNV_1A_PRIME	0x01000193

DWORD
Fnv1aCompute(
	PVOID		pData,
	DWORD		dwDataLength,
	DWORD		dwDataSize,
	DWORD		dwHashSeed
);

#endif