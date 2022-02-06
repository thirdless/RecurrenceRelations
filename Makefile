all : main.c queue.c queue.h semaphore.h semaphore.c message.c message.h bootstrap.S 
	riscv32-unknown-elf-gcc main.c queue.c semaphore.c message.c bootstrap.S -o main -nostartfiles -g -march=rv32ima -mabi=ilp32

sim: all
	tiny32-mc16 main --quiet 

dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D -S main
	
clean:
	rm -f main
