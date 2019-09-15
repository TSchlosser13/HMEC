# Makefile


CC        = gcc
CFLAGS    = -march=native -Ofast -pipe -std=c99 -Wall
includes  = CHIP/Misc/Precalcs.c CHIP/CHIP/Hexint.c CHIP/CHIP/Hexarray.c
includes += HMEC/HMEC.c
LDFLS     = -lm `pkg-config opencv --cflags --libs`


.PHONY: HMEC-Test

HMEC-Test: HMEC-Test.c
	$(CC) $(CFLAGS) HMEC-Test.c -o HMEC-Test $(includes) $(LDFLS)

help:
	@echo "/*****************************************************************************"
	@echo " * help : HMEC-Test - v1.0 - 01.04.2016"
	@echo " *****************************************************************************"
	@echo " * + update : Aktualisierung abhängiger Repos (CHIP)"
	@echo " * + test   : Testlauf"
	@echo " * + clean  : Aufräumen"
	@echo " *****************************************************************************/"

clean:
	find . -maxdepth 1 ! -name "*.c" ! -name "*.man" ! -name "COPYING" \
		! -name "Makefile" -type f -delete

_update:
	git pull
	@if [ -d CHIP ]; then \
		cd CHIP;               \
		echo "CHIP: git pull"; \
		git pull;              \
		cd ..;                 \
	else \
		git clone https://github.com/TSchlosser13/CHIP CHIP; \
	fi

update: _update HMEC-Test

test:
	./HMEC-Test

