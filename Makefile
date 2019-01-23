build:
	avr-gcc -Os -mmcu=atmega8 main.c -o main.o 
	avr-objcopy -j .text -j .data -O ihex  main.o  main.hex

flash:
	avrdude -p m8 -e -c usbasp -U flash:w:main.hex -F
