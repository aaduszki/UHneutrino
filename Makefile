.PHONY: clean

all: propagate_uncertainty

clean:
	rm *.so *.d

%: %.cpp
	reset && g++ -o $@  $< `root-config --cflags --glibs` -O2 -Wall -Wshadow -pedantic && time ./$@
