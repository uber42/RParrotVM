#include "global.h"




int main()
{
	auto s = Fnv1aCompute("new", 3, 1, FNV_1A_SEED);
	printf("%d", s);

	//system("PAUSE");
}