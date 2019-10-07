CC = gcc
CFLAGS = -Wall
TARGET = oss
OBJ = main.o  
SRC = main.c


all: $(TARGET) $(TARGET1) 
	
$(TARGET):$(OBJ)
	$(CC)  -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(SRC)

clean:
	/bin/rm -f *.o $(TARGET)
