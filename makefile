CC=g++
CFLAGS = -g -Wall -std=c++14
TARGET = //filnavn: clustering, seat eller machine

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

run:
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)
