CC		= g++
CFLAGS 	=-O2 -O3 -Wall -static
SOURCE	= fulltext
TARGET	= fulltext

linux:
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)_linux.cpp

windows:
	$(CC) $(CFLAGS) -o $(TARGET).exe $(SOURCE)_win.cpp

clean:
	rm -f $(TARGET)

copy:
#	cp $(TARGET) ../../build-CCR-Plus-Desktop_Qt_5_4_1_GCC_64bit-Debug/$(TARGET)
	copy $(TARGET).exe ..\..\build-CCR-Plus-Desktop_Qt_5_4_1_MinGW_32bit-Debug
	copy $(TARGET).exe ..\..\build-CCR-Plus-Desktop_Qt_5_4_1_MinGW_32bit-Debug\debug

