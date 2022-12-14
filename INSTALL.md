# Setup

## Autotools
```
cd  $(RMA2SHMEM_SOURCE_DIR) && ./autogen.sh
cd $(RMA2SHMEM_BUILD_DIR) && $(RMA2SHMEM_SOURCE_DIR)/configure CC=mpicc
```
(add `--prefix` if you want to `make install` somewhere)

## CMake
```
cmake $(RMA2SHMEM_SOURCE_DIR) -DCMAKE_C_COMPILER=mpicc
```

# Build

```
make
make check
```
