#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#define MSG_RECV_EVENT 	 "MSG_RECV_EVENT"
#define INIT_DONE_EVENT  "INIT_DONE_EVENT"
#define ACT_EVENT 		 "ACT_EVENT"

void ase_init(void (*ctrl_init)());
void ase_act();
void ase_handleMessage(char* message, char messageSize, char channel);

#endif /* SCHEDULER_H_ */
