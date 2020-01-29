CXX:=gcc
FLAGS:=-Wall -Wextra -Wpedantic -Werror -std=c11 -fbounds-check
INSTALL_PATH?=/usr/local/bin

BUILD_DIR:=build

EXE_NAME:=stodo


###--- Build all ---###
build : stodo

stodo : main.c
	@mkdir -p $(BUILD_DIR)
	@$(CXX) -o $(BUILD_DIR)/$(EXE_NAME) main.c $(FLAGS)

install : stodo
	@cp $(BUILD_DIR)/$(EXE_NAME) $(INSTALL_PATH)/$(EXE_NAME)

###--- Cleaning ---###
clean :
	@echo Cleaning...
	rm -r ./$(BIN_DIR) ./$(BUILD_DIR)
