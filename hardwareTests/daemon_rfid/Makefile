#==============================================================================
BLUE=`tput setaf 4`
GREEN=`tput setaf 2`
CYAN=`tput setaf 6`

PRINT_GENERATING="${CYAN}Generating $(shell basename $@) ... "
PRINT_BUILDING	="${BLUE}Building $(shell basename $@) ... "
PRINT_COMPILING	="${GREEN}Compiling $(shell basename $@) ... "
#====================================================================
CC= arm-buildroot-linux-uclibcgnueabihf-g++
#CC=g++
LIBS = -lpthread -lrt -lbcm2835 
CFLAGS = -I $(INC_DIR) $(LIBS) #-Wall 
#====================================================================
IP = 10.42.0.160
#IP = 10.42.0.174
FOLDER = /etc
#====================================================================
BIN_DIR = bin
BLD_DIR = build
INC_DIR = includes
SRC_DIR = src
DOC_DIR = docs

# documentation
DOXYGEN=/usr/local/bin/doxygen
DOXYFILE=$(DOC_DIR)/Doxyfile
HTML=html
LATEX=latex
#====================================================================
#qq.c -> qq.elf
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BLD_DIR)/%.o,$(SRC))
#====================================================================
PROGRAM = rfid_daemon
#====================================================================
.DEFAULT_GOAL = build

build: setup patient new_patient $(PROGRAM)

$(PROGRAM): $(OBJS) ## Compile objects to create executables
	@echo $(PRINT_GENERATING)
	@$(CC) -o $(BIN_DIR)/$@.elf $^ $(CFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.cpp ## Compile all objects
	@echo $(PRINT_BUILDING)
	@$(CC) -c -o $@ $^ $(CFLAGS)

patient: patient.cpp ## Create main test
	@echo $(PRINT_BUILDING)
	@$(CC) -o $(BIN_DIR)/$@.elf $^ $(CFLAGS)

new_patient: new_patient.cpp ## Create main test
	@echo $(PRINT_BUILDING)
	@$(CC) -o $(BIN_DIR)/$@.elf $^ $(CFLAGS)

.PHONY: doc 
doc: ## Generate the documentation
# 	@doxygen -g $(DOC_DIR)/Doxyfile
	@echo "Generating documentation"
	@$(DOXYGEN) $(DOXYFILE)

.PHONY: setup
setup: ## Create directories
	@mkdir -p $(BLD_DIR)
	@mkdir -p $(BIN_DIR)

transfer: 
	scp $(BIN_DIR)/*.elf root@$(IP):$(FOLDER)

.PHONY: clean
clean: ## Clean
	@rm -rf $(BLD_DIR) $(BIN_DIR)
	@rm -rf $(DOC_DIR)/$(HTML)
	@rm -rf $(DOC_DIR)/$(LATEX)

.PHONY: help # Generate list of targets with descriptions
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-13s\033[0m %s\n", $$1, $$2}'
