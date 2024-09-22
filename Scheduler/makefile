all: scheduler

scheduler : Job_Scheduler.c
	gcc -Wall -o scheduler Job_Scheduler.c -ladpcm_dec -ladpcm_enc -lanagram -laudiobeam -lcjpeg_transupp -lcjpeg_wrbmp -ldijkstra -lepic -lfmref -lg723_enc -lgsm_dec -lgsm_enc -lh264_dec -lhuff_dec -lhuff_enc -lmpeg2 -lndes -lpetrinet -lrijndael_dec -lrijndael_enc -lstatemate -lsusan -lammunition

clean: scheduler
	rm -rf scheduler
