#ifndef HASH_TABLE_H
#define	HASH_TABLE_H

/** ������������ ������ */
#define SKIP_LIST_MAX_HEIGHT 11

typedef int FSkipListComp(
	PVOID pFirst,
	PVOID pSecond
);

typedef int FSkipListPrinter(
	DWORD dwHeight,
	BOOL  fNewLevel,
	PVOID pKey,
	PVOID pValue
);

typedef VOID FSkipListNodeEraser(
	PVOID	pNode
);

typedef VOID FSkipListNodeValueChanger(
	PVOID* pValueDest,
	PVOID	pValueSrc
);

typedef struct _SkipList
{
	/** ���������� ������� */
	DWORD  dwCount;

	/** ������ */
	DWORD  dwHeight;

	/** ���������� */
	FSkipListComp* pfComparator;

	/** ������� �������� ���� */
	FSkipListNodeEraser* pfEraser;

	/** ������� ��������� �������� ���� */
	FSkipListNodeValueChanger* pfValueChanger;

	/** ������ ������� */
	SList  pHead[SKIP_LIST_MAX_HEIGHT];
} SSkipList, * PSSkipList;

/**
 * ��������� ����� ������ � ����������
 */
typedef struct _SSkipListNode
{
	/** ���� */
	PVOID pKey;

	/** �������� */
	PVOID pValue;

	/** ������ ����� */
	SList pLink[];
} SSkipListNode, * PSSkipListNode;


PSSkipList
CreateSkipList(
	FSkipListComp* pfComparator,
	FSkipListNodeEraser* pfEraser,
	FSkipListNodeValueChanger* pfValueChanger
);


PSSkipListNode
SkipListSet(
	PSSkipList	psSkipList,
	PVOID		pKey,
	PVOID		pValue
);


PSSkipListNode
SkipListFind(
	PSSkipList	psSkipList,
	PVOID		pKey
);


BOOL
SkipListRemove(
	PSSkipList	psSkipList,
	PVOID		pKey
);


VOID
SkipListPrint(
	PSSkipList			psSkipList,
	FSkipListPrinter* pFSkipListPrinter
);


PSList
SkipListGetAll(
	PSSkipList			psSkipList
);


VOID
SkipListClear(
	PSSkipList			psSkipList
);


VOID
SkipListClose(
	PSSkipList			psSkipList
);



#endif