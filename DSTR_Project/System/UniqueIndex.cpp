#include "Pch/Pch.h"
#include "UniqueIndex.h"

unsigned int UniqueIndex::getUniqueIndex()
{
	static unsigned int index = 0;
	return index++;
}
