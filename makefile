CC = g++
RM = rm -f
BT = bt/bt
BB = bb/bb
GEN = Generador/generador
OBJS = $(BT).o $(BB).o $(GEN).o 
EXEC = $(BT) $(BB) $(GEN) 

all: bt bb generador 

bt: $(BT).o
	$(CC) $(BT).o -o $(BT)
	
bt.o: $(BT).cpp
	$(CC) -o $(BT).cpp

bb: $(BB).o
	$(CC) $(BB).o -o $(BB)
	
bb.o: $(BB).cpp
	$(CC) -o $(BB).cpp

generador: $(GEN).o
	$(CC) $(GEN).o -o $(GEN)

generador.o: $(GEN).cpp
	$(CC) -o $(GEN).cpp

clean:
	$(RM) $(OBJS) $(EXEC) Generador/data 
	
cleanobj:
	$(RM) $(OBJS)

cleanexec:
	$(RM) $(EXEC)


