COMPILER=clang

build:
	@$(COMPILER) src/*.c -Iinclude/ -g -o bin/cavalry
clean:
	@rm -rf ./bin/*
	@rm -f compile_commands.json
rebuild:
	@make clean
	@bear make build