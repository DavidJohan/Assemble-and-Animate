#include <ase/infrastructure/Entity/Entity.h>
#include <stdint.h>
#include <stdbool.h>


void Entity_init(Entity_t* entity, short label) {
	entity->label = label;
	entity->initialized = true;
}

bool Entity_isInitialized(Entity_t* entity) {
	return entity->initialized;
}

short Entity_getLabel(Entity_t* entity) {
	return entity->label;
}

void Entity_setLabel(Entity_t* entity, short label) {
	entity->label = label;
}
