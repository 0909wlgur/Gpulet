CC=g++
SHARED_FLAGS=-shared -fPIC
L_FLAGS=-lsystem -lgpu -lgpulet -lcurve_fit -lgsl -lgslcblas

all: main

main: main.cpp libsystem.so
	$(CC) -o $@ main.cpp -L. $(L_FLAGS)

libsystem.so: libgpu.so libconfig.so system.cpp
	$(CC) $(SHARED_FLAGS) -o $@ system.cpp

libgpu.so: libgpulet.so gpu.cpp
	$(CC) $(SHARED_FLAGS) -o $@ gpu.cpp

libgpulet.so: libcurve_fit.so gpulet.cpp
	$(CC) $(SHARED_FLAGS) -o $@ gpulet.cpp

libcurve_fit.so: curve_fit.cpp
	$(CC) $(SHARED_FLAGS) -o $@ curve_fit.cpp

clean:
	rm -f main libsystem.so libgpulet.so