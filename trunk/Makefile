ROOT_DIR:=.

include $(ROOT_DIR)/Makefile.include

MODULES:=engine game

all: so
	@for i in $(MODULES); do \
		set -e ; cd $$i ; $(MAKE) ; cd ..; \
	done;

clean:
	@for i in $(MODULES); do \
		set -e ; cd $$i ; $(MAKE) clean; cd ..; \
	done;

so: 
	@set -e ; cd $(ROOT_DIR)/bin/${BUILD_FLAG} && ${MAKE}
