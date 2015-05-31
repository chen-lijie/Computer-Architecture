all: read.yo write.yo

read.yo: read.ys
	../misc/yas read.ys
	
write.yo: write.ys
	../misc/yas write.ys
	
start:
	./../misc/run_bus

read:
	./../pipe/psim read.yo
	
write:
	./../pipe/psim write.yo
	
clean:
	rm *.yo
