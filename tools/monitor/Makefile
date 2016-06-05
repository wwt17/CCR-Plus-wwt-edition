CC		= gcc
CFLAGS 	=-O2 -O3 -Wall -static -m32
SOURCE	= monitor.c
TARGET	= monitor

linux:
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

windows:
	$(CC) $(CFLAGS) -o $(TARGET).exe $(SOURCE) -lpthread -lpsapi

clean:
	rm -f $(TARGET)

copy:
#	cp $(TARGET) ../../build-CCR-Plus-Desktop_Qt_5_4_1_GCC_64bit-Debug/$(TARGET)
	copy $(TARGET).exe ..\..\build-CCR-Plus-Desktop_Qt_5_4_1_MinGW_32bit-Debug
	copy $(TARGET).exe ..\..\build-CCR-Plus-Desktop_Qt_5_4_1_MinGW_32bit-Debug\debug

