
PROJ = $(shell ls -d */ 2>/dev/null | grep -v util | sed "s:/::")
.PHONY: clean commit $(PROJ) stats

BUILDS = $(shell find . -maxdepth 2 -name build)

dflt:
	@echo make $(PROJ)

$(PROJ):
	@echo $(CFLAGS)
	@make -C $@

clean: $(BUILDS)
	rm -rf $^

commit: clean
	git add ./* && git commit && git push

stats:
	@find . -name "*.c" -print0 | xargs -0 cat | awk "NF" | wc
