ROOT_DIR:=../../

include $(ROOT_DIR)/Makefile.include

MODULES:=GameGas

all:
	@for i in $(MODULES); do \
		set -e ; cd $$i ; $(MAKE) ; cd ..; \
	done;

clean:
	@for i in $(MODULES); do \
		set -e ; cd $$i ; $(MAKE) clean; cd ..; \
	done;
