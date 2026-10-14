CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDLIBS = -lm

TARGET = raytracer
OBJECTS = RayTracer.o sphere.o imagen_avs.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

RayTracer.o: RayTracer.c poligonos.h imagen_avs.h
	$(CC) $(CFLAGS) -c RayTracer.c -o $@

sphere.o: sphere.c poligonos.h
	$(CC) $(CFLAGS) -c sphere.c -o $@

imagen_avs.o: imagen_avs.c imagen_avs.h
	$(CC) $(CFLAGS) -c imagen_avs.c -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)