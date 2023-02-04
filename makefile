OUTFILE = light_sampler
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow

app:
	$(CC_C) $(CFLAGS) -pthread circular_buffer.c light_sampler.c sampler.c -o $(OUTDIR)/$(OUTFILE)
	
clean: 
	rm $(OUTDIR)/$(OUTFILE)
