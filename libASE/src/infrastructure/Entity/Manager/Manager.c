#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/infrastructure/Entity/Entity.h>

void Manager_init(Manager_t* manager, short label) {
	Entity_init((Entity_t*) manager,label);
}

bool Manager_isInitialized(Manager_t* manager) {
	return Entity_isInitialized((Entity_t*) manager);
}
