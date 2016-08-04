//
// Created by Fardin Abdi on 6/27/16.
//

#include "quanser.h"
struct controller_storage cs;
int flag =0;

/**
 * x is the current state of the system
 * cs is a varibale where the states of the system is being held.
 */




struct command controller_safety(struct state x){
    
    if(flag==0)
    {
  	cs.int_elevation=0;
  	cs.int_pitch=0;
  	cs.int_travel=0;

  	cs.elevation1=0;
  	cs.pitch1=0;
  	cs.travel1=0;

  	cs.elevation2=0;
  	cs.pitch2=0;
  	cs.travel2=0;
	
	flag=1;
     }

    struct command U;

    //printf("\n:info int_elevation: %lf elevation_conv: %lf int_pitch:%lf, pitch_conv:%lf\n", int_elevation, elevation_conv, int_pitch, pitch_conv);

    cs.int_travel +=  x.travel;
    cs.int_pitch +=  x.pitch;
    cs.int_elevation +=  x.elevation;

    U.u1 = -4.5 * x.elevation  - .701 * x.pitch  -25.7161 * x.d_elevation -3.051 * x.d_pitch; //-0.0333*cs->int_elevation -0.001*cs->int_pitch;
    U.u2  = -4.5 * x.elevation  + .5701 * x.pitch -25.7529 * x.d_elevation +5.970 * x.d_pitch; //-0.03*cs->int_elevation +0.001*cs->int_pitch;

    cs.elevation2 = cs.elevation1;
    cs.elevation1 = x.elevation;

    cs.pitch2 = cs.pitch1;
    cs.pitch1 = x.pitch;

    cs.travel2 = cs.travel1;
    cs.travel1 = x.travel;

    return U;
}
