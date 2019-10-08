CC = gcc
CFLAGS = -Wall  
TARGET = oss
TARGET1 = user
OBJ = main.o 
OBJ1 = user.o 
SRC = main.c
SRC1 = user.c


all: $(TARGET) $(TARGET1) 
	
$(TARGET):$(OBJ)
	$(CC)  -o $(TARGET)  $(OBJ)

$(TARGET1):$(OBJ1)
	$(CC)  -o $(TARGET1)  $(OBJ1)

$(OBJ): $(SRC)
	$(CC)  $(CFLAGS) -c $(SRC)

$(OBJ1): $(SRC1)
	$(CC)  $(CFLAGS) -c $(SRC1)

clean:
	/bin/rm -f *.o $(TARGET)
	/bin/rm -f *.o $(TARGET1)

