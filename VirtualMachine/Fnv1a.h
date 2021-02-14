#ifndef FNV1A_H
#define FNV1A_H

#define FNV_1A_SEED		0x811C9DC5
#define FNV_1A_PRIME	0x01000193

u32
Fnv1aCompute(
	void*		pData,
	u32			dwDataLength,
	u32			dwDataSize,
	u32			dwHashSeed
);

#endif