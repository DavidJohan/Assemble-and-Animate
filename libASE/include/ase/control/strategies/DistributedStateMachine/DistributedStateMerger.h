/**
 * \file DistributedStateMerger.h
 * DistributedStateMerger.h is used by the "dynarole" language
 * to reason about states and perform self-reconfiguration.
 *
 * \see DistributedStateMerger.h
 *
 * \date March 11, 2009
 * \author Ulrik P. Shultz
 * \author Mirko Bordignon
 * \author David Johan Christensen
 */

#ifndef DISTRIBUTED_STATE_MERGER_H_

void DSM_intersect_plus_greater(uint8_t *set0, uint8_t *set1, uint8_t min, uint8_t *dest);
void DSM_intersect(uint8_t *set0, uint8_t *set1, uint8_t *dest);
uint8_t DSM_merge(uint8_t s0, uint8_t *p0, uint8_t s1, uint8_t *p1, uint8_t *p2);

#define DISTRIBUTED_STATE_MERGER_H_

#endif /* DISTRIBUTED_STATE_MERGER_H_ */
