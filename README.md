ReSecure: Restart-Based Security Protocol for Constantly Actuated Real-Time Cyber-Physical Systems
=====================

######*This work has been submitted to [2nd ACM Workshop on Cyber-Physical Systems Security & Privacy (CPS-SPC)](http://eecs.oregonstate.edu/cps-spc/).*



The server (Simple Controller) is a multithreaded program running the safety controller, the decision module and the client (Complex Controller) as asynchronous events.

**_The applications are prototyped and checked for the Linux kernel versions specified below. Although this instructions should work with other Linux kernel versions and real-time patches as well, we did not verify this._**

The server is a multithreaded program running the safety controller, the decision module and the complex controller as asynchronous events.

Instructions
----

###Flashing the Beagle Bone Black###

*See detailed instructions [here](http://derekmolloy.ie/write-a-new-image-to-the-beaglebone-black/).*

__1.__ Download the image:

	https://rcn-ee.com/rootfs/2016-06-09/flasher/BBB-eMMC-flasher-debian-8.5-console-armhf-2016-06-09-2gb.img.xz

__2.__ Extract the file

	tar -xvf BBB-eMMC-flasher-debian-8.5-console-armhf-2016-06-09-2gb.img.xz

__3.__ Create a bootable SD card from the OS image (instructions for [Windows](https://learn.adafruit.com/beaglebone-black-installing-operating-systems/windows) and [Mac](https://learn.adafruit.com/beaglebone-black-installing-operating-systems/mac-os-x)).


__4.__ Insert the SD card in the Board.


__5.__ Push S2 switch while connecting to a power source.


__6.__ When all four lights lit up together the Board is flashed with the new image.

###Applying Real-Time patch

__1.__ Log in as root user:

    sudo -i

__2.__ Check the version of Linux that is running:

    uname -r

__3.__ Download package lists from the repository:

    sudo apt-get update

__4.__ Search for available Real-­time Linux kernels:

    sudo apt­-cache search linux-image | grep ti-rt

__5.__  Install the Real-time patch that matches with currently installed kernel:

    sudo apt-get install linux-image-4.4.12-ti-rt-r30



###Compiling a periodic timer (on the BeagleBone Black)
Run make

    cd periodic_timer && make

Run the periodic timer

    insmod hrmodule_test.ko

###Simple Controller (on the x86 machine)

Run make

    cd Server && make

Run the server

	./main

###Safety Controller (on the x86 machine)

Run make

    cd Safety_controller && make

Run the safety controller

	./main

###Compile rootkit (on the BeagleBone Black)
Run make

    cd attacks/root_kit && make

[//]: # (We just need to compile the rootkit. In our application any dummy task will be high-jacked and the rootkit will run instead.)

###Real-time applications on Complex Controller (on the BeagleBone Black)

Run make

    cd Rt_task && make

Run the application

	./main
