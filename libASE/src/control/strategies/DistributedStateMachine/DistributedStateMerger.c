#include <stdio.h>
#include <stdint.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/config/ConfigASE.h>

void DSM_intersect_plus_greater(uint8_t *set0, uint8_t *set1, uint8_t min, uint8_t *dest) {
  uint8_t index0, index1, desti;
  desti = 0;
  for(index0=0; index0<DSM_MAX_PENDING_STATES; index0++) {
    if(set0[index0]==0) continue;
    if(set0[index0]>=min) {
      dest[desti++] = set0[index0];
      continue;
    }
    for(index1=0; index1<DSM_MAX_PENDING_STATES; index1++)
      if(set0[index0]==set1[index1]) {
		dest[desti++] = set0[index0];
		break;
      }
  }
  for(index0=desti; index0<DSM_MAX_PENDING_STATES; index0++)
    dest[index0] = 0;
}

void DSM_intersect(uint8_t *set0, uint8_t *set1, uint8_t *dest) {
  DSM_intersect_plus_greater(set0,set1,255,dest);
}

/* local state (s0,p0), incoming state (s1,p1), resulting state (return,p2) */
/* correctness: see merge.c (tests equivalent implementation) */
uint8_t DSM_merge(uint8_t s0, uint8_t *p0, uint8_t s1, uint8_t *p1, uint8_t *p2) {
  if(s0>s1) {
    DSM_intersect_plus_greater(p0,p1,s1,p2);
    return s0;
  }    
  else if(s0==s1) {
    DSM_intersect(p0,p1,p2);
    return s0;
  }
  else { /* s0<s1 */
    DSM_intersect_plus_greater(p1,p0,s0,p2);
    return s1;
  }
}

