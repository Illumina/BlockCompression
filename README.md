# BlockCompression
Block compression library used by Nirvana

## Cloning

This repo uses git submodules so we can be easily update it when newer compression algorithms are released. As such, just remember to use the `--recursive` flag when cloning:

```
git clone --recursive https://github.com/Illumina/BlockCompression.git
```

## Building on Linux

Just make sure that a basic C/C++ toolchain is installed.

```
make -j
```

The library and benchmark tool will be available in the `bin` output directory.

## Building on Windows (native)

Grab your favorite MinGW-x64 distribution. I've tried this using the `nuwen.net` and `WinLibs` distributions:
* [nuwen.net](https://nuwen.net/mingw.html)
* [WinLibs](http://winlibs.com/)

```
make -f Makefile.MinGW -j
```

The library and benchmark tool will be available in the `bin` output directory.

## Building on macOS (x86)

Make sure the basic Xcode toolchain is installed.

```
make -f Makefile.macOS_x86

```

The library and benchmark tool will be available in the 'bin' output directory.

## Target Architecture

By default, we're setting `-march` to `ivybridge` since many of our users had older machines that didn't support AVX. If you're not restricted to that, go ahead and update that setting to `native` or perhaps something more nuanced.

## Profile-Guided Optimization

### Linux & Windows

In addition, we usually provide profile-optimized guided (PGO) builds of this library in Nirvana. Profile-guided Optimization (PGO) improves application performance by reorganizing code layout to reduce instruction-cache problems, shrinking code size, and reducing branch mispredictions. PGO provides information to the compiler about areas of an application that are most frequently executed. By knowing these areas, the compiler is able to be more selective and specific in optimizing the application.

To use PGO, update the `CFLAGS` variable to include the following:

```
CFLAGS=-O3 -fPIC -fprofile-dir=$(PGO_DIR) -fprofile-generate=$(PGO_DIR)
```

Compile the program and use the RunBenchmark tool on a sample data set. For this part, you'll need a zstd dictionary so that we can test out the dictionary-based compression algorithms. More information on creating this dictionary can be found here: [https://github.com/facebook/zstd#dictionary-compression-how-to](https://github.com/facebook/zstd#dictionary-compression-how-to)

Once the benchmark has finished, all the interesting data will be stored in the PGO subdirectory. Now we need to change the `Makefile` to use the new PGO data:

```
CFLAGS=-O3 -fPIC -fprofile-dir=$(PGO_DIR) -fprofile-use=$(PGO_DIR) -fprofile-correction
```

We need to get rid of the previous binaries and objects to re-compile the project. Do the following to delete the `bin` and `obj` subdirectories (but not the `pgo` subdirectory) and rebuild:

```
make clean
make -j
```

### macOS

To use PGO, update the `CFLAGS` variable to include the following:

```
CFLAGS=-O3 -fPIC -fprofile-instr-generate=$(PGO_DIR)/BlockCompression.profraw
```

Compile the program and use the RunBenchmark tool on a sample data set. For this part, you'll need a zstd dictionary so that we can test out the dictionary-based compression algorithms. More information on cre>

Once the benchmark has finished, all the interesting data will be stored in the PGO subdirectory. Next we'll need to convert the profiling data before it can be used:

```
/Library/Developer/CommandLineTools/usr/bin/llvm-profdata merge -output=BlockCompression.profdata BlockCompression.profraw

```

Now we need to change the `Makefile` to use the new PGO data:

```
CFLAGS=-O3 -fPIC -fprofile-instr-use=$(PGO_DIR)/BlockCompression.profdata
```

We need to get rid of the previous binaries and objects to re-compile the project. Do the following to delete the `bin` and `obj` subdirectories (but not the `pgo` subdirectory) and rebuild:

```
make clean
make -j
```

## Results

Here are the results when using MinGW (gcc 10.2.0) in Windows 10 on a Ryzen 9 5950X CPU (single thread benchmark):

| Algorithm | CL | TotalTime | CompRatio |  CompSpeed | DecompSpeed | Speed% | Ratio% | SpRank | CrRank |
|:----------|---:|----------:|----------:|-----------:|------------:|-------:|-------:|-------:|-------:|
| Zlib      |  1 |    1.69 s |      2.94 | 263.5 MB/s |  894.9 MB/s |  1.000 |  1.000 |      3 |      7 |
| Zlib      |  5 |    3.11 s |      4.69 | 122.0 MB/s |  1.158 GB/s |  0.544 |  1.592 |      4 |      6 |
| Zlib      |  9 |   11.04 s |      5.02 |  32.1 MB/s |  1.173 GB/s |  0.153 |  1.703 |      5 |      3 |
| Zstd      |  1 |    0.97 s |      4.87 | 458.5 MB/s |  1.518 GB/s |  1.739 |  1.654 |      1 |      4 |
| Zstd      | 17 |  1.16 min |      7.04 |   5.0 MB/s |  1.854 GB/s |  0.024 |  2.390 |      6 |      2 |
| ZstdDict  |  1 |    0.98 s |      4.87 | 451.9 MB/s |  1.527 GB/s |  1.722 |  1.654 |      2 |      5 |
| ZstdDict  | 17 |  1.27 min |      7.04 |   4.5 MB/s |  1.857 GB/s |  0.022 |  2.391 |      7 |      1 |

## Using in Nirvana

As long as the API for the functions specified in `BlockCompression.map` stay the same, the resulting BlockCompression library (*.dll, *.so, or *.dylib) can be dropped into a Nirvana release.
