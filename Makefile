# slightly not basic makefile
include config.mk

# clear default suffixes and implicit rules
.SUFFIXES:

# keep .d files
.SECONDARY: build/deps/**/*.d

# targets
.PHONY: all clean

.DEFAULT: all

all:	$(foreach $\
			f,$\
			$(subst $\
				source/,$\
				,$\
				$(basename $\
					$(filter $\
						%.c %.cpp,$\
						$(wildcard source/**/*)$\
					)$\
				)$\
			),$\
			build/debug/$f.o build/release/$f.o$\
		)

include tools/extract.mk

clean:; @-rm -rf build lib

distclean: clean; @-rm -rf obj asm

# system include search directories
export MWCIncludes = include:include/stdlib

# flags
flags_main			:=	-proc gekko \
						-fp hardware \
						-lang c99 \
						-enum int \
						-cpp_exceptions off \
						-cwd include \
						-enc UTF-8 \
						-flag no-cats

flags_opt_debug		:=	-opt off \
						-inline off \
						-g

flags_opt_release	:=	-O4,p \
						-ipa file \
						-DNDEBUG

# object files

build/debug/%.o: source/%.c | envcheck
	@mkdir -p $(foreach d,debug deps/debug,build/$d/$(*D))
	${WINE} ${MWERKS} ${flags_main} ${flags_opt_debug} ${extra_flags} ${FLAGS} -MMD -o $@ -c $<
	@tools/transform_dep.py $(*F).d build/deps/debug/$(*D)/$(*F).d
	@-rm -f $(*F).d

build/release/%.o: source/%.c | envcheck
	@mkdir -p $(foreach d,release deps/release,build/$d/$(*D))
	${WINE} ${MWERKS} ${flags_main} ${flags_opt_release} ${extra_flags} ${FLAGS} -MMD -o $@ -c $<
	@tools/transform_dep.py $(*F).d build/deps/release/$(*D)/$(*F).d
	@-rm -f $(*F).d

# library archives

.NOTPARALLEL: %.a

lib/%D.a: | envcheck
	@mkdir -p lib
	${WINE} ${MWERKS} ${flags_main} -o $@ -library $^

lib/%.a: | envcheck
	@mkdir -p lib
	${WINE} ${MWERKS} ${flags_main} -o $@ -library $^

.PHONY: envcheck

envcheck:
ifeq ($(strip ${MWERKS}),)
	$(error MWERKS not set)
endif

include tools/files.mk
include flags.mk
