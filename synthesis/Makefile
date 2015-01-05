.PHONY: all

all: tools symsync

tools: check-mroot
	$(MAKE) -C tools/symcnf

symsync:
	$(MAKE) -C symsync/

clean:
	$(MAKE) -C tools/symcnf clean
	$(MAKE) -C symsync/ clean

check-mroot:
ifndef MROOT
    $(error The environment variable MROOT should be point to MiniSAT root directory)
endif
