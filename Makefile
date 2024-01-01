all: linux

linux:
	g++ *.cpp -o server -I. -DDEBUG
