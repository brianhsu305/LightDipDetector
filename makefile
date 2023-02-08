OUTFILE = light_sampler
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow
NOWORKYFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

app:
	$(CC_C) $(CFLAGS) -pthread periodTimer.c circular_buffer.c light_sampler.c sampler.c udp_sockets.c segDisplay.c -o $(OUTDIR)/$(OUTFILE)
	$(CC_C) $(NOWORKYFLAGS) noworky.c -o $(OUTDIR)/noworky
clean: 
	rm $(OUTDIR)/$(OUTFILE)
