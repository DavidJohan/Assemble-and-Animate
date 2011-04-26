#include <ase/tools/IDContainer/IDContainer.h>

IDContainer_t IDContainer_ids;

void IDContainer_init() {
	IDContainer_ids.softID = -1;
}

void IDContainer_setSoftID(int ID)  {
	IDContainer_ids.softID = ID;
}

int IDContainer_getSoftID()  {
	return IDContainer_ids.softID;
}
