EFFECTS_DIR = $(shell ls -d */ | sed 's/\///')

-include MAKEFILE_CONFIG

all:
	@for fx in $(EFFECTS_DIR); do \
	cd $$fx; \
	$(MAKE); \
	cd ..; \
	done

install:
	@for fx in $(EFFECTS_DIR); do \
	cd $$fx; \
	$(MAKE) install INSTALL_PATH=$(INSTALL_PATH); \
	cd ..; \
	done

clean:
	@for fx in $(EFFECTS_DIR); do \
	cd $$fx; \
	$(MAKE) clean; \
	cd ..; \
	done
