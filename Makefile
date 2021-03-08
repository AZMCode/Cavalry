COMPILER=clang

build:
	@$(COMPILER) src/*.c -Iinclude/ -o bin/cavalry

clean:
	@rm -rf ./bin/*