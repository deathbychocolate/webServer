/*
 * Author: Aj
 * Course:
 * Date created:
 * File name :
 * Purpose:
 * acknowledgement:
 */


/* Multi Level Feedback Priority */
enum MLFP {HIGH_PRIORITY, MED_PRIORITY, LOW_PRIORITY};

/* Scheduler entry points */
extern void admit_scheduler(struct RCB *req);
extern enum scheduler_type scheduler_init(char *scheduler);
extern struct RCB *get_next_scheduler();
extern void resubmit_scheduler(struct RCB *req);

/* Round Robin entry points */
extern void admit_RR(struct RCB *req);
extern struct RCB *get_RR ();

/* Multilevel Feedback queue entry points */
extern void admit_MLFQ(struct RCB *req, enum MLFP priority);
extern struct RCB *get_MLFQ(enum MLFP priority);
extern enum MLFP get_priority();

/* Shortest Job First queue entry points */
extern void admit_SJF(struct RCB *req);
extern struct RCB *get_SJF();
