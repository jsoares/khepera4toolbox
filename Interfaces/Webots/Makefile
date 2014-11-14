CC=gcc
INCLUDE=-I"$(WEBOTS_HOME)/include"
COMPILER_OPTS= $(CC) -c $(INCLUDE) -o

libk3_webots.a: khepera3_drive.o khepera3.o khepera3_infrared.o khepera3_ultrasound.o odometry_track.o
	ar rcs $@ $^

khepera3_drive.o: khepera3_drive.c
	$(COMPILER_OPTS) $@ $^

khepera3.o: khepera3.c
	$(COMPILER_OPTS) $@ $^

khepera3_infrared.o: khepera3_infrared.c
	$(COMPILER_OPTS) $@ $^

khepera3_ultrasound.o: khepera3_ultrasound.c
	$(COMPILER_OPTS) $@ $^

odometry_track.o: odometry_track.c
	$(COMPILER_OPTS) $@ $^
clean:
	rm -f *.o *~

	


