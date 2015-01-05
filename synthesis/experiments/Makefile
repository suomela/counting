WRKDIR=./work/
OUTPUT_DIR=$(WRKDIR)/counting-experiments/output/
INSTANCE_DIR=$(WRKDIR)/counting-experiments/instances/
MINISAT_UNREAL_OUTPUT=output/minisat-unreal/
LINGELING_UNREAL_OUTPUT=output/lingeling-unreal/
SYMSYNC_UNREAL_OUTPUT=output/symsync-unreal/
MINISAT_OUTPUT=output/minisat/
LINGELING_OUTPUT=output/lingeling/
SYMSYNC_OUTPUT=output/symsync/
SYMSYNC_OVERSHOOT_OUTPUT=output/symsync-overshoot/
CNF_SCRIPT=./simple-cnf
BATCH_DIR=batches

workdirs:
	mkdir -p $(WRKDIR)
	mkdir -p $(OUTPUT_DIR)
	mkdir -p $(INSTANCE_DIR)

setup: workdirs
	@echo "- Checking if symbolic links need to be created"
ifeq ("$(wildcard instances)","")
	@echo "- Linking instance directory to: $(INSTANCE_DIR)"
	@ln -s $(INSTANCE_DIR) instances
endif
ifeq ("$(wildcard output)","")
	@echo "- Linking output directory to: $(OUTPUT_DIR)"
	@ln -s $(OUTPUT_DIR) output
endif
	mkdir -p $(BATCH_DIR)

clean:
	rm $(BATCH_DIR)/batch-*.sh

batches: setup 
	mkdir -p $(MINISAT_OUTPUT) $(LINGELING_OUTPUT) $(SYMSYNC_OUTPUT) $(SYMSYNC_OVERSHOOT_OUTPUT)
	python cnf_gen_batch.py simple < realisable.txt > $(BATCH_DIR)/batch-gen-realisable.sh
	python cnf_batch.py $(CNF_SCRIPT) $(MINISAT_OUTPUT) 100 12345689 minisat < realisable.txt > $(BATCH_DIR)/batch-minisat-realisable.sh
	python cnf_batch.py $(CNF_SCRIPT) $(LINGELING_OUTPUT) 100 12345699 lingeling < realisable.txt > $(BATCH_DIR)/batch-lingeling-realisable.sh
	python symsync_batch.py $(SYMSYNC_OUTPUT) 100 123456798 < realisable.txt > $(BATCH_DIR)/batch-symsync-realisable.sh
	python symsync_batch.py $(SYMSYNC_OVERSHOOT_OUTPUT) 100 123456987 -overshoot=1 < realisable.txt > $(BATCH_DIR)/batch-symsync-overshoot-realisable.sh
	python cnf_gen_batch.py simple < unrealisable.txt > $(BATCH_DIR)/batch-gen-unrealisable.sh
	python cnf_batch.py $(CNF_SCRIPT) $(MINISAT_UNREAL_OUTPUT) 100 912345689 minisat < unrealisable.txt > $(BATCH_DIR)/batch-minisat-unrealisable.sh
	python cnf_batch.py $(CNF_SCRIPT) $(LINGELING_UNREAL_OUTPUT) 100 921345689 lingeling < unrealisable.txt > $(BATCH_DIR)/batch-lingeling-unrealisable.sh
	python symsync_batch.py $(SYMSYNC_UNREAL_OUTPUT) 100 99123456798 < unrealisable.txt > $(BATCH_DIR)/batch-symsync-unrealisable.sh
	chmod u+x batches/batch-*.sh
        

