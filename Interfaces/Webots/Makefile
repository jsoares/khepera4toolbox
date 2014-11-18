CC=gcc
INCLUDE=-I"$(WEBOTS_HOME)/include"
COMPILER_OPTS= $(CC) -c $(INCLUDE) -o

libk3_webots.a: khepera4_drive.o khepera4.o khepera4_infrared.o khepera4_ultrasound.o odometry_track.o
	ar rcs $@ $^

khepera4_drive.o: khepera4_drive.c
	$(COMPILER_OPTS) $@ $^

khepera4.o: khepera4.c
	$(COMPILER_OPTS) $@ $^

khepera4_infrared.o: khepera4_infrared.c
	$(COMPILER_OPTS) $@ $^

khepera4_ultrasound.o: khepera4_ultrasound.c
	$(COMPILER_OPTS) $@ $^

odometry_track.o: odometry_track.c
	$(COMPILER_OPTS) $@ $^
clean:
	rm -f *.o *~

	


