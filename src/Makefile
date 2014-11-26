SOURCES=$(wildcard *.cpp */*.cpp)
ALL_DIRS := $(shell find . -maxdepth 3 -type d)
EXCLUDE_DIRS := $(shell find . -maxdepth 3 -type d | grep .svn)
DIRS := $(filter-out $(EXCLUDE_DIRS),$(ALL_DIRS))
OBJS=$(patsubst %.cpp,%.o,$(SOURCES))
INCS=-I./ 
LIBS=-L/usr/lib64/mysql -lluajit-5.1 -lmysqlclient
OBJ_DIR=../output
TARGET=../bin/serverd
CXXFLAGS=-O0 -g -std=c++11 
XX=/usr/local/bin/g++

OUTPUT_OBJS=$(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(SOURCES)))

$(OBJ_DIR)/%.o : %.cpp
	$(XX) $(CXXFLAGS) $(INCS) -c $< -o $(OBJ_DIR)/$@

.PHONY : all
all : dirs main

main : $(OUTPUT_OBJS)
	$(XX) -o $(TARGET) $(CXXFLAGS) $^ $(LIBS)

test :
	@echo $(DIRS)
	@echo $(EXCLUDE_DIRS)

dirs :
	-mkdir $(OBJ_DIR) 2>/dev/null
	-for t in $(DIRS); do mkdir $(OBJ_DIR)/$$t 2>/dev/null; done

clean :
	-rm -f -r $(OBJ_DIR) $(TARGET)
