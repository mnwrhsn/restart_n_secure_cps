//
// Created by Fardin Abdi on 6/27/16.
//

#ifndef QUANSER_QUANSER_H
#define QUANSER_QUANSER_H
#define SIM_STEP 0.001
#define MAX_SAFE_STACK (100*1024)

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

extern flag;
extern struct controller_storage cs; 
struct command controller_safety(struct state x); //generating the next command using the current state
struct state eval_state(struct state state_x, struct command U); 
struct state simulate_fixed_control(struct state init_state, struct command U, double time); //simluate the next state of the machine
struct command decide(struct command SC,struct command CC,struct state s); //decision module function

#endif //QUANSER_QUANSER_H
