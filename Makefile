# ibgames fed2d installation makefile

BIN=./obj
PROD=/var/opt/fed2

# fed2d executable dependencies

$(PROD)/fed2d: $(BIN)/fed2d
	mv $(PROD)/fed2d $(PROD)/fed2d.old
	cp $(BIN)/fed2d $(PROD)/fed2d

