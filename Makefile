# Use the following environment variables
#  - LIBMPCITH_SCHEME_PATH
#  - LIBMPCITH_ARITH_LIBS
#  - LIBMPCITH_CONFIG_FLAGS
#  - LIBMPCITH_ADD_FLAGS
#  - LIBMPCITH_DRBG_PATH
#  - LIBMPCITH_PARAM_*

# Basic compiler configuration
CC?=gcc
WARNING_FLAGS=-Wall -Wextra -Wpedantic -Wshadow
CFLAGS= -O3 -flto -fPIC -march=native -std=c11 $(WARNING_FLAGS)
OTHERFLAGS=
LINKFLAGS=

# Add user EXTRA flags if asked
CFLAGS+=$(EXTRA_CFLAGS)
OTHERFLAGS+=$(EXTRA_OTHERFLAGS)
LINKFLAGS+=$(EXTRA_LINKFLAGS)

# Do we want to activate the debug mode?
ifeq (,$(filter 1,$(DEBUG) $(LIBMPCITH_DEBUG)))
	OTHERFLAGS += -DNDEBUG
else
	CFLAGS += -g
endif

# Do we want to activate the detailed benchmarks?
ifneq (,$(filter 1,$(BENCH) $(LIBMPCITH_BENCHMARK)))
	CFLAGS += -DBENCHMARK
endif

# Do we want to activate the benchmark of the number of cycles?
ifneq (,$(filter 1,$(BENCH_CYCLES) $(LIBMPCITH_BENCHMARK_CYCLES)))
	CFLAGS += -DBENCHMARK_CYCLES
endif

# Do we want to activate the sanitizers?
ifneq (,$(filter 1,$(USE_SANITIZERS) $(LIBMPCITH_USE_SANITIZERS)))
	CFLAGS += -fsanitize=undefined -fsanitize=address -fsanitize=leak
	CFLAGS += -fno-sanitize=signed-integer-overflow,shift-base
	LINKFLAGS += -fsanitize=undefined -fsanitize=address -fsanitize=leak
endif

# Do we want agressive compiler?
ifneq (,$(filter 1,$(AGGRESSIVE) $(LIBMPCITH_AGGRESSIVE_COMPILER)))
	CFLAGS += -Wcast-align
	CLANG :=  $(shell $(CROSS_COMPILE)$(CC) -v 2>&1 | grep clang)
	ifneq ($(CLANG),)
		CFLAGS += -Weverything -Werror \
				-Wno-reserved-id-macro -Wno-padded \
				-Wno-packed -Wno-covered-switch-default \
				-Wno-used-but-marked-unused -Wno-switch-enum
		CFLAGS += -Wno-gnu-zero-variadic-macro-arguments
		CFLAGS += -Wno-unused-macros
		CFLAGS += -Wno-poison-system-directories
		CFLAGS += -Wno-cast-qual
	endif
endif


### Utilitary
comma:= ,

# The function 'get_all_subpaths'
#   takes two folders 'a' and 'b'
#   and retuns the list of all the
#   intermediary folders between 'a'
#   and 'b'.
# For example,
#   $(call get_all_subpaths,a/b,c d e)
# returns
#   "a/b/c a/b/c/d a/b/c/d/e"
define get_all_subpaths
$(shell 
	current=$(1); \
	echo $(current); \
	for folder in $(2) ; do \
		current=$$current/$$folder ;\
		echo $$current; \
	done ;
)
endef

# The function 'get_all_subpaths_from_uri'
#   takes a folder 'a' and a uri 'b'
#   and returns the list of all the folders
#   designed by the URI. A URI is a list of
#   leaf folders and the designed folders are
#   all the intermediary folders between 'a'
#   and the leaf folders.
# For example,
#   $(call get_all_subpaths_from_uri,a/b,c/d,e/f/g)
# returns
#   "a/b a/b/c a/b/c/d a/b/e a/b/e/f a/b/e/f/g"
define get_all_subpaths_from_uri
$(sort $(foreach dir,$(subst $(comma), ,$(2)),$(1) $(call get_all_subpaths,$(1),$(subst /, ,$(dir)))))
endef

# The function 'get_header_files'
#   takes a list of folders and returns
#   the list of all the files with
#   extension .h in the given folders.
define get_header_files
$(foreach dir,$(1),$(wildcard $(dir)/*.h))
endef

# The function 'get_source_files'
#   takes a list of folders and returns
#   the list of all the files with
#   extensions .c, .s, .S in the
#   given folders.
define get_source_files
$(foreach dir,$(1),$(wildcard $(dir)/*.c $(dir)/*.s $(dir)/*.S))
endef

# The function 'get_objects'
#   takes a list of file paths
#   and returns the same list
#   in which the files with extensions
#   .c and .s have been replaced by
#   a file with the same name but with
#   extension .o.
define get_objects
$(patsubst %.s,%.o,$(patsubst %.c,%.o,$(1)))
endef

# The function 'release_files'
#   takes a list of files.
#   It copies all of them in a folder
#   'build' (by removing all the subfolders).
# For example,
#   $(call release_files,core/trees/tree.c mpc/protocols.h)
# copies
#   "core/trees/tree.c" to "build/tree.c"
#   "mpc/protocols.h" to "build/protocols.h"
define release_files
	for file in $(1) ; do \
		short=$$(basename $$file) ; \
		cp $$file build/$$short ; \
	done
endef

define update_makefile
	lst=; \
	for path in $(2) ; do \
		filename=$$(basename $$path) ; \
		lst="$$lst $$filename"; \
	done; \
	sed -i".old" "s#{{$(1)}}#$$lst#g" build/Makefile
endef

define update_readme
	lst=; \
	for path in $(2) ; do \
		filename=$$(basename $$path) ; \
		lst="$$lst $$filename"; \
	done; \
	sed -i".old" "s#{{$(1)}}#$$lst#g" build/README.md
endef


############################################################
###                     USER FLAGS                       ###
############################################################

CONFIG_FLAGS=$(patsubst %,-DPARAM_%,$(subst $(comma), ,$(LIBMPCITH_CONFIG_FLAGS)))
PARAM_FLAGS=$(patsubst LIBMPCITH_%,-D%,$(filter LIBMPCITH_PARAM_%, $(shell printenv)))
ADD_FLAGS=$(patsubst %,-D%,$(subst $(comma), ,$(LIBMPCITH_ADD_FLAGS)))
USER_FLAGS=$(strip $(PARAM_FLAGS) $(CONFIG_FLAGS) $(ADD_FLAGS))
SCHEME_FLAGS=$(subst $(comma), ,$(LIBMPCITH_COMPILATION_FLAGS))

############################################################
###            CRYPTOGRAPHIC BUILDING BLOCKS             ###
############################################################

SYM_PATH=sym
DRBG_PATHS=$(call get_all_subpaths_from_uri,$(SYM_PATH)/drbg,$(LIBMPCITH_DRBG_PATH))
HASH_PATHS=$(call get_all_subpaths_from_uri,$(SYM_PATH)/hash,$(LIBMPCITH_HASH_PATH))
SYM_PATHS=$(SYM_PATH) $(DRBG_PATHS) $(HASH_PATHS)
SYM_INCLUDE=$(patsubst %,-I%,$(SYM_PATHS))
SYM_HEADERS=$(call get_header_files,$(SYM_PATHS))
SYM_SRC=$(strip $(call get_source_files,$(SYM_PATHS)))
SYM_OBJ=$(call get_objects,$(SYM_SRC))

# Hash Folder
ifeq ($(LIBMPCITH_HASH_SOURCE_FOLDER),)
	HASH_SOURCE_FOLDER=$(SYM_PATH)/hash/sha3/sha3-several/sha3
else
	HASH_SOURCE_FOLDER=$(SYM_PATH)/hash/$(LIBMPCITH_HASH_SOURCE_FOLDER)
endif
ifeq ($(LIBMPCITH_HASH_MAKE_OPTIONS),)
	HASH_MAKE_OPTIONS=PLATFORM=avx2
else
	HASH_MAKE_OPTIONS=$(LIBMPCITH_HASH_MAKE_OPTIONS)
endif
ifeq ($(LIBMPCITH_HASH_SOURCE_INCLUDE),)
	HASH_SOURCE_INCLUDE_SUBPATH=avx2
else
	HASH_SOURCE_INCLUDE_SUBPATH=$(LIBMPCITH_HASH_SOURCE_INCLUDE)
endif
HASH_SOURCE_PATHS=$(call get_all_subpaths_from_uri,$(HASH_SOURCE_FOLDER),$(HASH_SOURCE_INCLUDE_SUBPATH))
HASH_SOURCE_INCLUDE=$(patsubst %,-I%,$(HASH_SOURCE_PATHS))
HASH_SOURCE_PATHS_SHORT=$(call get_all_subpaths_from_uri,.,$(HASH_SOURCE_INCLUDE_SUBPATH))
HASH_SOURCE_INCLUDE_SHORT=$(patsubst %,-I%,$(HASH_SOURCE_PATHS_SHORT))


############################################################
###                 ARITHMETIC LIBARIES                  ###
############################################################

# The name of the folder which contains all the arithmetics
ARITH_PATH=arithmetic
# The list of all the folders which contains desired source codes at root
ARITH_PATHS=$(call get_all_subpaths_from_uri,$(ARITH_PATH),$(LIBMPCITH_ARITH_LIBS))
# List of all the source files about arithmetics
ARITH_SRC=$(strip $(foreach dir,$(ARITH_PATHS),$(wildcard $(dir)/*.c $(dir)/*.s)))
# List of all the object files about arithmetics
ARITH_OBJ=$(patsubst %.c,%.o,$(ARITH_SRC))
# List of all the header files about arithmetics
ARITH_HEADERS=$(strip $(foreach dir,$(ARITH_PATHS),$(wildcard $(dir)/*.h)))
# Include Arguments to flatten the views on the desired folders
ARITH_INCLUDE=$(patsubst %,-I%,$(ARITH_PATHS))


############################################################
###                    MPC PROTOCOLS                     ###
############################################################

MPC_PATH=mpc
SPE_PATHS=$(call get_all_subpaths_from_uri,$(MPC_PATH),$(LIBMPCITH_SCHEME_PATH))
MPC_SRC=$(strip $(call get_source_files,$(SPE_PATHS)))
MPC_OBJ=$(call get_objects,$(MPC_SRC))
MPC_INCLUDE=$(patsubst %,-I%,$(SPE_PATHS))
MPC_HEADERS=$(call get_header_files,$(SPE_PATHS))


############################################################
###                    LIBRARY CORE                      ###
############################################################

include $(shell find core -type f -name '*.mk')
MODE_LIST := $(sort $(MODE_LIST))

SELECTED_MODES=$(patsubst -DPARAM_%,%,$(foreach mode,$(MODE_LIST),$(filter -DPARAM_$(mode),$(USER_FLAGS))))
MODE_PATH=$(foreach mode,$(SELECTED_MODES),$(MODE_$(mode)_PATH))

CORE_PATH=core
MODE_PATHS=$(call get_all_subpaths_from_uri,$(CORE_PATH),$(MODE_PATH))
CORE_SRC=$(strip $(call get_source_files,$(MODE_PATHS)))
CORE_OBJ=$(call get_objects,$(CORE_SRC))
CORE_INCLUDE=$(patsubst %,-I%,$(MODE_PATHS))
CORE_HEADERS=$(call get_header_files,$(MODE_PATHS))


############################################################
###                    APPLICATIONS                      ###
############################################################

APP_PATH=app
APP_INCLUDE=-I$(APP_PATH)/bench

APP_BENCH_API_SRC=$(APP_PATH)/bench/bench.c $(APP_PATH)/bench/timing.c
APP_BENCH_API_OBJ=$(patsubst %.c,%.o,$(APP_BENCH_API_SRC))
APP_WRITE_BENCH_API_SRC=$(APP_PATH)/bench/bench-write.c
APP_WRITE_BENCH_API_OBJ=$(patsubst %.c,%.o,$(APP_WRITE_BENCH_API_SRC))
APP_READ_BENCH_API_SRC=$(APP_PATH)/bench/bench-read.c
APP_READ_BENCH_API_OBJ=$(patsubst %.c,%.o,$(APP_READ_BENCH_API_SRC))

ifneq (,$(filter -DPARAM_RING_SIGN,$(USER_FLAGS)))
# Ring signature
APP_BENCH_LIB_CORE_SRC=$(APP_PATH)/bench-lib/bench-ring.c $(APP_PATH)/bench/timing.c
APP_BENCH_LIB_SRC=$(APP_BENCH_LIB_CORE_SRC) $(APP_PATH)/bench-lib/bench-ring-main.c
APP_BENCH_LIB_OBJ=$(patsubst %.c,%.o,$(APP_BENCH_LIB_SRC))
APP_RAW_BENCH_LIB_SRC=$(APP_BENCH_LIB_CORE_SRC) $(APP_PATH)/bench-lib/bench-ring-raw.c
APP_RAW_BENCH_LIB_OBJ=$(patsubst %.c,%.o,$(APP_RAW_BENCH_LIB_SRC))
else
# Standard signature
APP_BENCH_LIB_CORE_SRC=$(APP_PATH)/bench-lib/bench.c $(APP_PATH)/bench/timing.c
APP_BENCH_LIB_SRC=$(APP_BENCH_LIB_CORE_SRC) $(APP_PATH)/bench-lib/bench-main.c
APP_BENCH_LIB_OBJ=$(patsubst %.c,%.o,$(APP_BENCH_LIB_SRC))
APP_RAW_BENCH_LIB_SRC=$(APP_BENCH_LIB_CORE_SRC) $(APP_PATH)/bench-lib/bench-raw.c
APP_RAW_BENCH_LIB_OBJ=$(patsubst %.c,%.o,$(APP_RAW_BENCH_LIB_SRC))
endif
APP_WRITE_BENCH_LIB_SRC=$(APP_PATH)/bench-lib/bench-write.c
APP_WRITE_BENCH_LIB_OBJ=$(patsubst %.c,%.o,$(APP_WRITE_BENCH_LIB_SRC))
APP_READ_BENCH_LIB_SRC=$(APP_PATH)/bench-lib/bench-read.c
APP_READ_BENCH_LIB_OBJ=$(patsubst %.c,%.o,$(APP_READ_BENCH_LIB_SRC))

APP_KAT_MAIN_SRC=$(APP_PATH)/generator/PQCgenKAT_sign.c $(APP_PATH)/generator/rng.c
APP_KAT_MAIN_OBJ=$(patsubst %.c,%.o,$(APP_KAT_MAIN_SRC))
APP_KAT_CHECK_SRC=$(APP_PATH)/generator/PQCgenKAT_check.c $(APP_PATH)/generator/rng.c
APP_KAT_CHECK_OBJ=$(patsubst %.c,%.o,$(APP_KAT_CHECK_SRC))

### Units
MPC_UNITS_URI=$(foreach mode,$(SELECTED_MODES),$(MODE_$(mode)_MPC_UNITS_PATH))
ifneq ($(MPC_UNITS_URI),)
	MPC_UNITS_PATHS=$(call get_all_subpaths_from_uri,$(CORE_PATH),$(MPC_UNITS_URI))
	MPC_UNITS_SRC=$(strip $(call get_source_files,$(MPC_UNITS_PATHS)))
	MPC_UNITS_INCLUDE=$(patsubst %,-I%,$(MPC_UNITS_PATHS))
else
# No MPC units tests have been defined for the current MPCitH transformation
	MPC_UNITS_SRC=
	MPC_UNITS_INCLUDE=
endif
APP_INCLUDE:=$(APP_INCLUDE) -I$(APP_PATH)/units $(MPC_UNITS_INCLUDE)
APP_MPC_UNITS_SRC=$(APP_PATH)/units/main.c $(MPC_UNITS_SRC)
APP_MPC_UNITS_OBJ=$(call get_objects,$(APP_MPC_UNITS_SRC))

### All

#All include, except those for app
ALL_INCLUDE=-I. $(SYM_INCLUDE) $(ARITH_INCLUDE) $(MPC_INCLUDE) $(CORE_INCLUDE)
ALL_FLAGS=$(CFLAGS) $(USER_FLAGS) $(OTHERFLAGS) $(SCHEME_FLAGS)

# Do we have a C++ compiler instead of a C compiler?
GPP := $(shell $(CROSS_COMPILE)$(CC) -v 2>&1 | grep g++)
CLANGPP := $(shell echo $(CROSS_COMPILE)$(CC) | grep clang++)
ifneq ($(GPP),)
ALL_FLAGS := $(patsubst -std=c11, -std=c++2a, $(ALL_FLAGS))
ALL_FLAGS := $(patsubst -std=gnu11, -std=gnu++2a, $(ALL_FLAGS))
endif
ifneq ($(CLANGPP),)
ALL_FLAGS := $(patsubst -std=c11, -std=c++2a, $(ALL_FLAGS))
ALL_FLAGS := $(patsubst -std=gnu11, -std=gnu++2a, $(ALL_FLAGS))
endif



.PHONY: print detect release

%.o : %.c
	$(CC) -c $(ALL_FLAGS) $(ALL_INCLUDE) $(HASH_SOURCE_INCLUDE) $(APP_INCLUDE) $< -o $@

all: check_config bench

# Sanity checks and error message if not met
check_config:
ifeq ($(LIBMPCITH_SCHEME_PATH),)
	$(error You did not set the scheme necessary environment variables! Please 'source' one of the configuration files (e.g. 'source mpc/mq/mqom/config/submissions/hypercube-ref-gf251-l1' for the MQOM reference implementation on GF251 with hypercube optimization.))
endif

# This target display the current configuration
#   of the library (current mode, user flags, sources, ...)
print:
	@echo "General configuration:"
	@echo " - Available modes: $(MODE_LIST)"
	@echo " - Current mode: $(SELECTED_MODES)"
	@echo " - Scheme URI: $(LIBMPCITH_SCHEME_PATH)"
	@echo " - User flags: $(USER_FLAGS)"
	@if [ -z "$(filter -DBENCHMARK, $(ALL_FLAGS))" ]; then \
		echo " - Benchmark: off" ; \
	else \
		echo " - Benchmark: on" ; \
	fi
	@if [ -z "$(filter -DNDEBUG, $(ALL_FLAGS))" ]; then \
		echo " - Debug: on" ; \
	else \
		echo " - Debug: off" ; \
	fi
	@echo
	@echo "Symmetric:"
	@echo " - Paths: $(SYM_PATHS)"
	@echo " - Sources: $(SYM_SRC)"
	@echo
	@echo "Arithmetic:"
	@echo " - Paths: $(ARITH_PATHS)"
	@echo " - Sources: $(ARITH_SRC)"
	@echo
	@echo "MPC Protocol:"
	@echo " - Paths: $(SPE_PATHS)"
	@echo " - Sources: $(MPC_SRC)"
	@echo
	@echo "Core:"
	@echo " - Paths: $(MODE_PATHS)"
	@echo " - Sources: $(CORE_SRC)"
	@echo " - MPC Units: $(MPC_UNITS_URI)"

# This target display all the configuration files
#   of the schemes implemented in the library
detect:
	@config_folders=$$(find mpc -type d -name config); \
	for folder in $$config_folders; do \
		config_files=$$(find $$folder -type f); \
		for file in $$config_files; do \
			undetectable_attr=`cat $$file | grep undetectable`; \
			if [ -z "$$undetectable_attr" ]; then \
				echo $$file; \
			fi \
		done; \
	done

libhash:
	$(HASH_MAKE_OPTIONS) make -C $(HASH_SOURCE_FOLDER)

## Known Anwser Test
kat_gen: $(APP_KAT_MAIN_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_KAT_MAIN_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lcrypto -o $@

kat_check: $(APP_KAT_CHECK_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_KAT_CHECK_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lcrypto -o $@

## Benchmark
bench: $(APP_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lm -o $@

bench-raw: $(APP_RAW_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_RAW_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lm -o $@

bench-api: $(APP_BENCH_API_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_BENCH_API_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lm -o $@

bench-write: $(APP_WRITE_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_WRITE_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lm -o $@

bench-read: $(APP_READ_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_READ_BENCH_LIB_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -lm -o $@

## Unitary Tests
#unit-keygen: app/units/keygen.o app/bench/timing.o $(INST_OBJ) $(KEYGEN_OBJ) $(SYM_OBJ)
#	$(CC) app/units/keygen.o app/bench/timing.o $(INST_OBJ) $(KEYGEN_OBJ) $(SYM_OBJ) $(CFLAGS) $(OTHERFLAGS) -L$(HASH_SOURCE_FOLDER) -lhash -L. -o unit-keygen
#
#unit-sign: app/units/sign.o app/bench/timing.o $(INST_OBJ) $(KEYGEN_OBJ) $(MPC_OBJ) $(SIGN_OBJ) $(SYM_OBJ)
#	$(CC) app/units/sign.o app/bench/timing.o $(INST_OBJ) $(KEYGEN_OBJ) $(MPC_OBJ) $(SIGN_OBJ) $(SYM_OBJ) $(CFLAGS) $(OTHERFLAGS) -L$(HASH_SOURCE_FOLDER) -lhash -L. -o unit-sign

units-mpc: $(APP_MPC_UNITS_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) libhash
ifeq ($(MPC_UNITS_URI),)
	$(error No MPC unit tests have been defined for this MPCitH transformation.)
endif
	$(CC) $(APP_MPC_UNITS_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(ALL_FLAGS) $(LINKFLAGS) -L$(HASH_SOURCE_FOLDER) -L. -lhash -o $@

## Cleaning
clean:
	rm -f $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ)
	rm -f $(APP_BENCH_LIB_OBJ) $(APP_RAW_BENCH_LIB_OBJ) $(APP_WRITE_BENCH_LIB_OBJ) $(APP_READ_BENCH_LIB_OBJ)
	rm -f $(APP_BENCH_API_OBJ) $(APP_WRITE_BENCH_API_OBJ) $(APP_READ_BENCH_API_OBJ)
	rm -f $(APP_MPC_UNITS_OBJ) $(APP_KAT_MAIN_OBJ) $(APP_KAT_CHECK_OBJ)
	rm -rf unit-*
	rm -f bench bench-*
	rm -f kat_gen kat_check
	$(HASH_MAKE_OPTIONS) make -C $(HASH_SOURCE_FOLDER) clean

release:
	@echo "Initialize build folder"
	@rm -rf build
	@mkdir build
	@echo "Copy source files"
	@$(call release_files,$(ARITH_SRC))
	@$(call release_files,$(ARITH_HEADERS))
	@$(call release_files,$(SYM_SRC))
	@$(call release_files,$(SYM_HEADERS))
	@$(call release_files,$(MPC_SRC))
	@$(call release_files,$(MPC_HEADERS))
	@$(call release_files,$(CORE_SRC))
	@$(call release_files,$(CORE_HEADERS))
	@cp -r $(HASH_SOURCE_FOLDER) build/
	@echo "Copy app"
	@cp -r $(APP_PATH)/bench build/bench
	@mv build/bench build/benchmark
	@cp -r $(APP_PATH)/generator build/generator
	@make -C build/$$(basename $(HASH_SOURCE_FOLDER)) clean > /dev/null
	@echo "Prepare Makefile"
	@cp release/Makefile build/
	@sed -i".old" "s#{{CC}}#$(CC)#g" build/Makefile
	@sed -i".old" "s#{{ALL_FLAGS}}#$(ALL_FLAGS)#g" build/Makefile
	@$(call update_makefile,SYM_OBJ,$(SYM_OBJ))
	@$(call update_makefile,ARITH_OBJ,$(ARITH_OBJ))
	@$(call update_makefile,MPC_OBJ,$(MPC_OBJ))
	@$(call update_makefile,CORE_OBJ,$(CORE_OBJ))
	@sed -i".old" "s#{{HASH_PATH}}#$$(basename $(HASH_SOURCE_FOLDER))#g" build/Makefile
	@sed -i".old" "s#{{HASH_MAKE_OPTIONS}}#$(HASH_MAKE_OPTIONS)#g" build/Makefile
	@sed -i".old" "s#{{APP_BENCH_OBJ}}#$(patsubst $(APP_PATH)/bench/%,benchmark/%,$(APP_BENCH_API_OBJ))#g" build/Makefile
	@sed -i".old" "s#{{APP_WRITE_BENCH_OBJ}}#$(patsubst $(APP_PATH)/bench/%,benchmark/%,$(APP_WRITE_BENCH_API_OBJ))#g" build/Makefile
	@sed -i".old" "s#{{APP_READ_BENCH_OBJ}}#$(patsubst $(APP_PATH)/bench/%,benchmark/%,$(APP_READ_BENCH_API_OBJ))#g" build/Makefile
	@sed -i".old" "s#{{APP_KAT_MAIN_OBJ}}#$(patsubst $(APP_PATH)/%,%,$(APP_KAT_MAIN_OBJ))#g" build/Makefile
	@sed -i".old" "s#{{APP_KAT_CHECK_OBJ}}#$(patsubst $(APP_PATH)/%,%,$(APP_KAT_CHECK_OBJ))#g" build/Makefile
	@sed -i".old" "s#{{HASH_INCLUDE}}#$(patsubst -I./%,-I$$(basename $(HASH_SOURCE_FOLDER))/%,$(HASH_SOURCE_INCLUDE_SHORT))#g" build/Makefile
	@sed -i".old" "s#{{APP_INCLUDE}}#-Ibenchmark#g" build/Makefile
	@rm build/Makefile.old
	@echo "Copy License"
	@cp LICENSE build/
	@echo "Copy README"
	@cp release/README.md build/
	@sed -i".old" "s#{{SELECTED_MODE}}#$(SELECTED_MODE)#g" build/README.md
	@sed -i".old" "s#{{USER_FLAGS}}#$(USER_FLAGS)#g" build/README.md
	@sed -i".old" "s#{{SYM_SRC}}#$(SYM_SRC)#g" build/README.md
	@sed -i".old" "s#{{ARITH_PATHS}}#$(ARITH_PATHS)#g" build/README.md
	@sed -i".old" "s#{{ARITH_SRC}}#$(ARITH_SRC)#g" build/README.md
	@sed -i".old" "s#{{SPE_PATHS}}#$(SPE_PATHS)#g" build/README.md
	@sed -i".old" "s#{{MPC_SRC}}#$(MPC_SRC)#g" build/README.md
	@sed -i".old" "s#{{MODE_PATHS}}#$(MODE_PATHS)#g" build/README.md
	@sed -i".old" "s#{{CORE_SRC}}#$(CORE_SRC)#g" build/README.md
	@rm build/README.md.old
	@cp release/.gitignore build/
