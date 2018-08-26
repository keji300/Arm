CROSS_COMPILE = arm-none-eabi-
NAME =test
#CFLAGS += -g   -O0  -mabi=apcs-gnu -mfpu=neon -mfloat-abi=softfp  -fno-builtin \
#			-nostdinc  -I ./include -I ./lib 
CFLAGS=-mfloat-abi=softfp -mfpu=vfpv3 -mabi=apcs-gnu -fno-builtin  -fno-builtin-function -g -O0 -c  -I ./include -I ./lib 			     		                                       
LD	= $(CROSS_COMPILE)ld     
CC	= $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
OBJS=./cpu/start.o ./driver/uart.o  \
        ./driver/_udivsi3.o ./driver/_divsi3.o ./driver/_umodsi3.o main.o ./lib/printf.o 
#=============================================================================#
all:  $(OBJS)
	$(LD)  $(OBJS) -T map.lds -o $(NAME).elf
	$(OBJCOPY)  -O binary  $(NAME).elf $(NAME).bin 
	$(OBJDUMP) -D $(NAME).elf > $(NAME).dis 
%.o: %.S 
	$(CC) $(CFLAGS) -c -o  $@ $<
%.o: %.s 
	$(CC) $(CFLAGS) -c -o  $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o  $@ $<
clean:
	rm -rf $(OBJS) *.elf *.bin *.dis *.o
#============================================================================#