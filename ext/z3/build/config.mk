PREFIX=/usr
CC=gcc
CXX=g++
CXXFLAGS= -D_MP_INTERNAL -D_AMD64_ -D_USE_THREAD_LOCAL  -c -fopenmp -mfpmath=sse -O3 -D _EXTERNAL_RELEASE -fomit-frame-pointer -fno-strict-aliasing -D_LINUX_ -fPIC -msse -msse2
CXX_OUT_FLAG=-o 
OBJ_EXT=.o
LIB_EXT=.a
AR=ar
AR_FLAGS=rcs
AR_OUTFLAG=
EXE_EXT=
LINK=g++
LINK_FLAGS=
LINK_OUT_FLAG=-o 
LINK_EXTRA_FLAGS=-lpthread  -fopenmp -lrt
SO_EXT=.so
SLINK=g++
SLINK_FLAGS=-shared
SLINK_EXTRA_FLAGS= -fopenmp -lrt
SLINK_OUT_FLAG=-o 
