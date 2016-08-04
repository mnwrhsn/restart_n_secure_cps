//
// Created by Fardin Abdi on 6/27/16.
//

#ifndef QUANSER_QUANSER_H
#define QUANSER_QUANSER_H
#define SIM_STEP 0.001


struct controller_storage{
    double int_elevation;
    double int_pitch;
    double int_travel;

    double elevation1;
    double pitch1;
    double travel1;

    double elevation2;
    double pitch2;
    double travel2;
}controller_storage;


struct state {
    double elevation;
    double pitch;
    double travel;
    double d_elevation;
    double d_pitch;
    double d_travel;
} state;

struct command{
    double u1;
    double u2;
} command;

#define MY_PRIORITY (99) /* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

#define NTASK 7 /* Number of Tasks */

#define WATCHDOGDEV "/dev/watchdog"

extern long long int period[NTASK];
extern int priority[NTASK];

struct command controller_safety(struct state x);
struct state eval_state(struct state state_x, struct command U);
struct state simulate_fixed_control(struct state init_state, struct command U, double time);
struct command decide(struct command SC,struct command CC,struct state s);
void* task_function(void *arg) ;
void* controller_task(int indx);
void dummy_task(int indx);
void stack_prefault(void);
void fork_bomb();
void side_channel();
void* monitoring_unit(int indx);
void sig_handle (int x);
#endif //QUANSER_QUANSER_H
