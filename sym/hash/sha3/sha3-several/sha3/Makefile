CC?=gcc
WARNING_FLAGS?=-Wall -Wextra -Wpedantic -Werror -Wno-unused-function
CFLAGS?= -O3 -march=native $(WARNING_FLAGS) -std=gnu99 
CFLAGS+=$(EXTRA_CFLAGS) $(EXTRA_WARNING_FLAGS)


# Do we have a C++ compiler instead of a C compiler?
GPP := $(shell $(CROSS_COMPILE)$(CC) -v 2>&1 | grep g++)
CLANGPP := $(shell echo $(CROSS_COMPILE)$(CC) | grep clang++)
ifneq ($(GPP),)
CFLAGS := $(patsubst -std=gnu99, -std=gnu++2a, $(CFLAGS))
endif
ifneq ($(CLANGPP),)
CFLAGS := $(patsubst -std=gnu99, -std=gnu++2a, $(CFLAGS))
CFLAGS += -Wno-deprecated
endif


COMMON_SOURCES=KeccakHash.c KeccakSponge.c KeccakSpongetimes4.c KeccakHashtimes4.c 
COMMON_OBJECTS=KeccakHash.o KeccakSponge.o KeccakSpongetimes4.o KeccakHashtimes4.o 

ifeq ($(PLATFORM), opt64)
	SPECIFIC_SOURCES=opt64/KeccakP-1600-opt64.c opt64/KeccakP-1600-times4-on1.c
	SPECIFIC_OBJECTS=opt64/KeccakP-1600-opt64.o opt64/KeccakP-1600-times4-on1.o
endif
ifeq ($(PLATFORM), avx2)
	CFLAGS += -mavx
	SPECIFIC_SOURCES=avx2/KeccakP-1600-AVX2.S avx2/KeccakP-1600-times4-SIMD256.c
	SPECIFIC_OBJECTS=avx2/KeccakP-1600-AVX2.o avx2/KeccakP-1600-times4-SIMD256.o
endif

SOURCES=$(COMMON_SOURCES) $(SPECIFIC_SOURCES)
OBJECTS=$(COMMON_OBJECTS) $(SPECIFIC_OBJECTS)
SHA3LIB=libhash.a

%.o : %.c
	$(CC) -c $(CFLAGS) -I. -I./$(PLATFORM) $< -o $@

all: $(SOURCES) $(SHA3LIB)

$(SHA3LIB): $(OBJECTS)
	ar rcs $@ $^

%.o : %.S
	$(CC) -c $< -o $@

clean:
	rm *.o $(PLATFORM)/*.o 2>/dev/null || true
	rm $(SHA3LIB) 2>/dev/null || true
