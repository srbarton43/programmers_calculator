
all: test
	
test:
	make -C src test

clean: 
	make -C src clean
