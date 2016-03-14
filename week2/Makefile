SUBDIRS = 3a 4c

all: $(SUBDIRS)

.PHONY: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: clean-all

clean-all:
	@for dir in $(SUBDIRS); \
	do \
		make clean -C $${dir}; \
	done
