#include "global.h"

static
DWORD
LehmerRandom()
{
	static DWORD dwSeed = 0xDEADBEEF;

	static const DWORD dwMod = 2147483647L;
	static const unsigned __int64 uiFactor = 16807;

	unsigned __int64 uiProduct = dwSeed * uiFactor;

	dwSeed = (DWORD)((uiProduct >> 31) + (uiProduct & dwMod));
	if (dwSeed > dwMod)
	{
		dwSeed -= dwMod;
	}

	return dwSeed;
}

static
DWORD
RandomHeight()
{
	DWORD dwRandom = LehmerRandom();
	DWORD dwBranching = 4;
	DWORD dwHeight = 1;

	while (dwHeight < SKIP_LIST_MAX_HEIGHT &&
		LehmerRandom() % dwBranching == 0)
	{
		dwHeight++;
	}

	return dwHeight;
}

PSSkipListNode
CreateSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue);


PSSkipListNode
CreateSkipListNode(
	DWORD		dwHeight,
	PVOID		pKey,
	PVOID	    pValue
)
{
	DWORD dwSize = sizeof(SSkipListNode) + dwHeight * sizeof(SList);
	PSSkipListNode psSkipListNode = (PSSkipListNode)malloc(dwSize);
	if (psSkipListNode == NULL)
	{
		return NULL;
	}

	psSkipListNode->pKey = pKey;
	psSkipListNode->pValue = pValue;

	return psSkipListNode;
}


PSSkipList
CreateSkipList(
	FSkipListComp* pfComparator,
	FSkipListNodeEraser* pfEraser,
	FSkipListNodeValueChanger* pfValueChanger
)
{
	PSSkipList psSkipList = (PSSkipList)malloc(sizeof(SSkipList));
	if (psSkipList == NULL)
	{
		return NULL;
	}

	psSkipList->dwHeight = 1;
	psSkipList->dwCount = 0;
	psSkipList->pfEraser = pfEraser;
	psSkipList->pfValueChanger = pfValueChanger;
	psSkipList->pfComparator = pfComparator;

	for (size_t i = 0; i < ARRAYSIZE(psSkipList->pHead); i++)
	{
		ListHeadInit(&psSkipList->pHead[i]);
	}

	return psSkipList;
}


PSSkipListNode
SkipListSet(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
)
{
	PSList sListInsertion[SKIP_LIST_MAX_HEIGHT] = { 0 };

	DWORD dwHeight = RandomHeight();
	if (dwHeight > psSkipList->dwHeight)
	{
		psSkipList->dwHeight = dwHeight;
	}

	PSSkipListNode psSkipListNode = CreateSkipListNode(dwHeight, pKey, pValue);
	if (psSkipListNode == NULL)
	{
		return NULL;
	}

	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;
		for (; psListCurrent != psListEnd;
			psListCurrent = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			int nCompareResult = psSkipList->pfComparator(
				psSkipListCurrentNode->pKey, pKey);
			if (nCompareResult > 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}
			else if (nCompareResult == 0)
			{
				free(psSkipListNode);

				psSkipList->pfValueChanger(&psSkipListCurrentNode->pValue, pValue);
				return psSkipListCurrentNode;
			}
		}

		psListCurrent = psListEnd->pBlink;
		if ((DWORD)dwIdx < dwHeight)
		{
			sListInsertion[dwIdx] = psListCurrent;
		}

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	for (DWORD dwIdx = 0; dwIdx < dwHeight; dwIdx++)
	{
		ListAdd(&psSkipListNode->pLink[dwIdx],
			sListInsertion[dwIdx]);
	}

	psSkipList->dwCount++;

	return psSkipListNode;
}


PSSkipListNode
SkipListFind(
	PSSkipList	psSkipList,
	PVOID		pKey
)
{
	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;
		for (; psListCurrent != psListEnd;
			psListCurrent = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			int nCompareResult =
				psSkipList->pfComparator(psSkipListCurrentNode->pKey, pKey);

			if (nCompareResult > 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}
			else if (nCompareResult == 0)
			{
				return psSkipListCurrentNode;
			}
		}

		psListCurrent = psListEnd->pBlink;

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	return NULL;
}


BOOL
SkipListRemove(
	PSSkipList	psSkipList,
	PVOID		pKey
)
{
	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;
		PSList psListSafeIterator = psListCurrent->pFlink;

		for (; psListCurrent != psListEnd;
			psListCurrent = psListSafeIterator,
			psListSafeIterator = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			int nComparationResult = psSkipList->pfComparator(
				psSkipListCurrentNode->pKey, pKey);

			if (nComparationResult > 0)
			{
				psListEnd = &psSkipListCurrentNode->pLink[dwIdx];
				break;
			}
			else if (nComparationResult == 0)
			{
				for (INT dwCurrentHeight = 0; dwCurrentHeight <= dwIdx; dwCurrentHeight++)
				{
					ListNodeDelete(&psSkipListCurrentNode->pLink[dwCurrentHeight]);
					if (ListIsEmpty(&psSkipList->pHead[dwCurrentHeight]))
					{
						psSkipList->dwHeight--;
					}
				}

				psSkipList->pfEraser(psSkipListCurrentNode);
				free(psSkipListCurrentNode);
				psSkipList->dwCount--;

				return TRUE;
			}
		}

		psListCurrent = psListEnd->pBlink;

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}

	return FALSE;
}


VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter* pFSkipListPrinter
)
{
	INT dwIdx = (INT)psSkipList->dwHeight - 1;
	PSList psListCurrent = &psSkipList->pHead[dwIdx];
	PSList psListEnd = &psSkipList->pHead[dwIdx];

	while (dwIdx >= 0)
	{
		psListCurrent = psListCurrent->pFlink;

		if (psListCurrent != psListEnd)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			pFSkipListPrinter(
				dwIdx, TRUE,
				psSkipListCurrentNode->pKey,
				psSkipListCurrentNode->pValue);

			psListCurrent = psListCurrent->pFlink;
		}

		for (; psListCurrent != psListEnd;
			psListCurrent = psListCurrent->pFlink)
		{
			PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
				psListCurrent,
				SSkipListNode,
				pLink[dwIdx]
			);

			pFSkipListPrinter(
				dwIdx, FALSE,
				psSkipListCurrentNode->pKey,
				psSkipListCurrentNode->pValue);
		}

		psListCurrent--;
		psListEnd--;

		dwIdx--;
	}
}


PSList
SkipListGetAll(
	PSSkipList			psSkipList
)
{
	return &psSkipList->pHead[0];
}


VOID
SkipListClear(
	PSSkipList			psSkipList
)
{
	PSList psListCurrent = &psSkipList->pHead[0];
	PSList psListEnd = &psSkipList->pHead[0];

	PSList psListSafeIterator = NULL;

	for (psListCurrent = psListCurrent->pFlink,
		psListSafeIterator = psListCurrent->pFlink;
		psListCurrent != psListEnd;
		psListCurrent = psListSafeIterator,
		psListSafeIterator = psListCurrent->pFlink)
	{
		PSSkipListNode psSkipListCurrentNode = CONTAINING_RECORD(
			psListCurrent,
			SSkipListNode,
			pLink
		);

		psSkipList->pfEraser(psSkipListCurrentNode);

		free(psSkipListCurrentNode);
	}

	for (size_t i = 0; i < psSkipList->dwHeight; i++)
	{
		ListHeadInit(&psSkipList->pHead[i]);
	}

	psSkipList->dwHeight = 1;
	psSkipList->dwCount = 0;
}


VOID
SkipListClose(
	PSSkipList			psSkipList
)
{
	SkipListClear(psSkipList);
	free(psSkipList);
}

VOID
SkipListClone(
	PSSkipList			psDest,
	PSSkipList			psSrc
)
{
	psDest->dwCount = psSrc->dwCount;

	PSList psCurrentEntry = psSrc->pHead[0].pFlink;
	for(;psCurrentEntry != &psSrc->pHead[0];
		psCurrentEntry = psCurrentEntry->pFlink)
	{
		PSSkipListNode psCurretNode = CONTAINING_RECORD(
			psCurrentEntry, SSkipListNode, pLink[0]);
		SkipListSet(psDest, psCurretNode->pKey, psCurretNode->pValue);
	}
}