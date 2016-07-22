//
// Created by Fardin Abdi on 6/27/16.
//

#include "quanser.h"
#include <math.h>
#include<time.h>
#include <stdio.h>



double P[10] = {-1.000, -2.4000, -0.0943, 0.1200, 0.1200, -2.5000, -0.0200, 0.200, 2.1000, 10.0000};

double MAX_VOLTAGE = 2.5;


struct state eval_state(struct state state_x, struct command U){

    struct state d_state;

    d_state.elevation = state_x.d_elevation;
    d_state.pitch = state_x.d_pitch;
    d_state.travel = state_x.d_travel;
    d_state.d_elevation = P[1]*cos(state_x.elevation)+ P[2]*sin(state_x.elevation) 			+ P[8]*cos(state_x.pitch)*(U.u1+U.u2) ;
    d_state.d_pitch = P[5]*sin(state_x.pitch) + P[4]*cos(state_x.pitch)+ P[6]*state_x.d_pitch	+ P[9]*(U.u1-U.u2);
    d_state.d_travel = P[7]*state_x.d_travel							+ P[10]*sin(state_x.pitch)*(U.u1+U.u2);

    state_x.elevation += SIM_STEP*d_state.elevation;
    state_x.pitch += SIM_STEP*d_state.pitch;
    state_x.travel += SIM_STEP*d_state.travel;
    state_x.d_elevation += SIM_STEP*d_state.d_elevation;
    state_x.d_pitch += SIM_STEP*d_state.d_pitch;
    state_x.d_travel += SIM_STEP*d_state.d_travel;

    return state_x;

}

//the function that you should call
struct state simulate_fixed_control(struct state init_state, struct command U, double t){


    time_t start = time(NULL);

    struct state state_x;

    state_x = init_state;
    printf("comm u1: %lf u2: %lf \n", U.u1, U.u2);
    printf("sim init: elevation: %lf, pitch: %lf, travel: %lf, d_elevation: %lf, d_pitch: %lf, d_travel: %lf\n", init_state.elevation, init_state.pitch, init_state.travel, init_state.d_elevation, init_state.d_pitch, init_state.d_travel);

    int steps = t/SIM_STEP;
    int k = 0;
    for (k = 0; k <steps; k++){
        state_x = eval_state(state_x, U);
    }

    printf("sim fina: elevation: %lf, pitch: %lf, travel: %lf, d_elevation: %lf, d_pitch: %lf, d_travel: %lf\n", state_x.elevation, state_x.pitch, state_x.travel, state_x.d_elevation, state_x.d_pitch, state_x.d_travel);
	

    time_t end = time(NULL);

    //sleep(time - end);
	
    return state_x;
}


struct command decide(struct command SC,struct command CC,struct state s)
{
	const randomBit = rand() % 2;
	if(randomBit ==0)
	{
		printf("SC\n");
		return SC;
	}
	else
	{
		printf("CC\n");
		return CC;
	}
}

