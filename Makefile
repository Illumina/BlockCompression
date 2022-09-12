# define our object and binary directories
TOP_DIR=$(PWD)
OBJ_DIR=obj
BIN_DIR=bin
PGO_DIR=$(TOP_DIR)/pgo

# define our directories
BENCHMARK_DIR=RunBenchmark
BGZF_DIR=bgzf
UTILITIES_DIR=utilities
LIBDEFLATE_DIR=libdeflate
ZSTD_DIR=zstd

# define our object directories
LIBDEFLATE_OBJ_DIR=$(OBJ_DIR)/libdeflate
ZSTD_OBJ_DIR=$(OBJ_DIR)/zstd

# define our include directories
BENCHMARK_INCLUDE=-I $(TOP_DIR)/$(BENCHMARK_DIR)
BGZF_INCLUDE=-I $(TOP_DIR)/$(LIBDEFLATE_DIR)
LIBDEFLATE_INCLUDE=-I $(TOP_DIR)/$(LIBDEFLATE_DIR)
BENCHMARK_INCLUDE=-I $(TOP_DIR)/$(BENCHMARK_DIR) -I $(TOP_DIR)/$(BGZF_DIR) -I $(TOP_DIR)/$(ZSTD_DIR)/lib

# defines
ZSTD_DEFINES=-DDEBUGLEVEL=0 -DHAVE_THREAD=0 -DZSTD_LEGACY_SUPPORT=0 -DHAVE_ZLIB=0 -DHAVE_LZMA=0 -DHAVE_LZ4=0 -DZSTD_NOBENCH -DBACKTRACE_ENABLE=0
LIBDEFLATE_DEFINES=-DDISABLE_GZIP -DDISABLE_ZLIB -D_ANSI_SOURCE

# define our output library
SHAREDLIB=libBlockCompression.so
SHAREDLIB_PATH=$(BIN_DIR)/$(SHAREDLIB)

# define our output program
RUNBENCHMARK_PATH=$(BIN_DIR)/RunBenchmark
LIBS=-L $(TOP_DIR)/$(BIN_DIR) -Wl,-rpath,$(TOP_DIR)/$(BIN_DIR) -lBlockCompression

# define some default flags
CC=gcc
CPP=g++
CFLAGS=-O3 -fPIC -flto=full -flto-partition=one
# For profile-guided optimization:
# -fprofile-dir=$(PGO_DIR) -fprofile-generate=$(PGO_DIR)
# -fprofile-dir=$(PGO_DIR) -fprofile-use=$(PGO_DIR) -fprofile-correction
CPPFLAGS=-std=c++11
# Ideally this should be -march=native, but too many of our users didn't have modern AVX support - so I left it at ivybridge. 
# For true non-AVX support, you could use westmere
MARCH=-march=ivybridge -mtune=haswell
LDFLAGS=-Wl,-Bstatic -static-libgcc -Wl,-Bdynamic
LDSHARED=@$(CC) -shared -Wl,-soname,$(SHAREDLIB),--version-script,BlockCompression.map

# define our source files
BGZF_SOURCES=$(BGZF_DIR)/bgzf.c
BGZF_OBJS=$(addprefix $(OBJ_DIR)/,$(notdir $(BGZF_SOURCES:.c=.o)))

UTILITIES_SOURCES=$(UTILITIES_DIR)/get_library_id.c
UTILITIES_OBJS=$(addprefix $(OBJ_DIR)/,$(notdir $(UTILITIES_SOURCES:.c=.o)))

LIBDEFLATE_SOURCES=$(LIBDEFLATE_DIR)/lib/crc32.c $(LIBDEFLATE_DIR)/lib/deflate_compress.c $(LIBDEFLATE_DIR)/lib/deflate_decompress.c $(LIBDEFLATE_DIR)/lib/utils.c $(LIBDEFLATE_DIR)/lib/x86/cpu_features.c
LIBDEFLATE_OBJS=$(addprefix $(LIBDEFLATE_OBJ_DIR)/,$(notdir $(LIBDEFLATE_SOURCES:.c=.o)))

ZSTD_SOURCES=$(ZSTD_DIR)/lib/common/entropy_common.c $(ZSTD_DIR)/lib/common/error_private.c $(ZSTD_DIR)/lib/common/fse_decompress.c $(ZSTD_DIR)/lib/common/zstd_common.c $(ZSTD_DIR)/lib/common/xxhash.c $(ZSTD_DIR)/lib/common/pool.c $(ZSTD_DIR)/lib/compress/fse_compress.c $(ZSTD_DIR)/lib/compress/hist.c $(ZSTD_DIR)/lib/compress/huf_compress.c $(ZSTD_DIR)/lib/compress/zstd_compress.c $(ZSTD_DIR)/lib/compress/zstd_compress_literals.c $(ZSTD_DIR)/lib/compress/zstd_compress_sequences.c $(ZSTD_DIR)/lib/compress/zstd_compress_superblock.c $(ZSTD_DIR)/lib/compress/zstd_double_fast.c $(ZSTD_DIR)/lib/compress/zstd_fast.c $(ZSTD_DIR)/lib/compress/zstd_lazy.c $(ZSTD_DIR)/lib/compress/zstd_ldm.c $(ZSTD_DIR)/lib/compress/zstd_opt.c $(ZSTD_DIR)/lib/decompress/huf_decompress.c $(ZSTD_DIR)/lib/decompress/zstd_ddict.c $(ZSTD_DIR)/lib/decompress/zstd_decompress.c $(ZSTD_DIR)/lib/decompress/zstd_decompress_block.c
ZSTD_OBJS=$(addprefix $(ZSTD_OBJ_DIR)/,$(notdir $(ZSTD_SOURCES:.c=.o)))

BENCHMARK_SOURCES=$(BENCHMARK_DIR)/Benchmark.cpp $(BENCHMARK_DIR)/BenchmarkStatistics.cpp $(BENCHMARK_DIR)/CompressionData.cpp  $(BENCHMARK_DIR)/MemoryUtilities.cpp $(BENCHMARK_DIR)/RunBenchmark.cpp $(BENCHMARK_DIR)/Runner.cpp $(BENCHMARK_DIR)/Zlib.cpp $(BENCHMARK_DIR)/Zstandard.cpp $(BENCHMARK_DIR)/ZstandardDict.cpp
BENCHMARK_OBJS=$(addprefix $(OBJ_DIR)/,$(notdir $(BENCHMARK_SOURCES:.cpp=.o)))

all: $(ZCONF-NG_H) $(OBJ_DIR) $(BIN_DIR) $(PGO_DIR) $(SHAREDLIB_PATH) $(RUNBENCHMARK_PATH)

clean:
	@echo "- cleaning the output directory"
	@-rm -rf $(OBJ_DIR) $(BIN_DIR)

realclean: clean
	@-rm -rf $(PGO_DIR)

$(OBJ_DIR):
	@mkdir -p $(LIBDEFLATE_OBJ_DIR) $(ZSTD_OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(PGO_DIR):
	@mkdir -p $(PGO_DIR)

$(SHAREDLIB_PATH): $(BGZF_OBJS) $(UTILITIES_OBJS) $(LIBDEFLATE_OBJS) $(ZSTD_OBJS)
	@echo "- creating the shared library: $(SHAREDLIB_PATH)"
	@$(LDSHARED) $(CFLAGS) $(DEFINES) -o $(SHAREDLIB_PATH) $(BGZF_OBJS) $(UTILITIES_OBJS) $(LIBDEFLATE_OBJS) $(ZSTD_OBJS) $(LDFLAGS)

$(RUNBENCHMARK_PATH): $(SHAREDLIB_PATH) $(BENCHMARK_OBJS)
	@echo "- creating RunBenchmark: $(RUNBENCHMARK_PATH)"
	@$(CPP) $(CFLAGS) $(CPPFLAGS) $(MARCH) $(BENCHMARK_INCLUDE) -o $(RUNBENCHMARK_PATH) $(BENCHMARK_OBJS) $(LIBS)

$(OBJ_DIR)/%.o : $(BGZF_DIR)/%.c
	@echo "- compiling $(*F).c (bgzf)"
	@$(CC) $(CFLAGS) $(MARCH) $(BGZF_INCLUDE) -c $< -o $@

$(OBJ_DIR)/%.o : $(UTILITIES_DIR)/%.c
	@echo "- compiling $(*F).c (utilities)"
	@$(CC) $(CFLAGS) $(MARCH) $(UTILITIES_INCLUDE) -c $< -o $@

$(LIBDEFLATE_OBJ_DIR)/%.o : $(LIBDEFLATE_DIR)/lib/%.c
	@echo "- compiling $(*F).c (libdeflate)"
	@$(CC) $(CFLAGS) $(MARCH) $(LIBDEFLATE_INCLUDE) $(LIBDEFLATE_DEFINES) -c $< -o $@

$(LIBDEFLATE_OBJ_DIR)/%.o : $(LIBDEFLATE_DIR)/lib/x86/%.c
	@echo "- compiling $(*F).c (libdeflate)"
	@$(CC) $(CFLAGS) $(MARCH) $(LIBDEFLATE_INCLUDE) $(LIBDEFLATE_DEFINES) -c $< -o $@

$(ZSTD_OBJ_DIR)/%.o : $(ZSTD_DIR)/lib/common/%.c
	@echo "- compiling $(*F).c (zstd)"
	@$(CC) $(CFLAGS) $(MARCH) $(ZSTD_INCLUDE) $(ZSTD_DEFINES) -c $< -o $@

$(ZSTD_OBJ_DIR)/%.o : $(ZSTD_DIR)/lib/compress/%.c
	@echo "- compiling $(*F).c (zstd)"
	@$(CC) $(CFLAGS) $(MARCH) $(ZSTD_INCLUDE) $(ZSTD_DEFINES) -c $< -o $@

$(ZSTD_OBJ_DIR)/%.o : $(ZSTD_DIR)/lib/decompress/%.c
	@echo "- compiling $(*F).c (zstd)"
	@$(CC) $(CFLAGS) $(MARCH) $(ZSTD_INCLUDE) $(ZSTD_DEFINES) -c $< -o $@

$(OBJ_DIR)/%.o : $(BENCHMARK_DIR)/%.cpp
	@echo "- compiling $(*F).cpp (RunBenchmark)"
	@$(CPP) $(CFLAGS) $(CPPFLAGS) $(MARCH) $(BENCHMARK_INCLUDE) -c $< -o $@
