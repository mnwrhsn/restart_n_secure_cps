main.o: main.c controller.c dummy.c task.c controller_task.c fork.c side_channel.c monitoring_unit.c
	gcc -o main main.c controller.c dummy.c task.c controller_task.c fork.c side_channel.c monitoring_unit.c -lrt -pthread
clean:
	rm *o main

