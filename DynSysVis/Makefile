# para la carpetas
NAME = DynSysVis
# para el .a libreria estatica      
LIBNAME = lib$(NAME)
# para exportar toda la libreria haciendo una copia
DIST_DIR = out$(NAME)

# --- Configuración del Compilador ---
CXX = g++
# Si necesitas cabeceras de Homebrew (ej. SFML) para compilar la librería, descomenta la siguiente línea:
CXXFLAGS = -Iinclude -I/opt/homebrew/include -Wall -std=c++17 -MMD -MP
# CXXFLAGS = -Iinclude -Wall -std=c++17 -MMD -MP

ifeq ($(LOG), 1)
    CXXFLAGS += -DDSV_DEBUG
endif
CXXFLAGS += -DDSV_DEBUG

# --- Rutas de Carpetas ---
SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

# --- --- --- Deteccion de Archivos --- --- ---

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
HEADERS = $(wildcard include/*.hpp) $(wildcard include/dsv/*/*.hpp)

LIBRARY := $(LIB_DIR)/$(LIBNAME).a

# --- --- ---  Reglas de Compilacion --- --- --- 

all: directories $(LIBRARY)

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)

$(LIBRARY): $(OBJECTS)
	@echo Generando libreria estatica: $@
	@rm -f $@
	ar rcs $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compilando $<...
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJECTS:.o=.d)

# --- --- ---  LIMPIAR --- --- --- 
clean:
	@rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(LIB_DIR)/*.a
	@echo Limpieza terminada

# --- --- ---  LIMPIAR DISTRIBUCIÓN --- --- --- 
cleandist:
	@if [ -d "$(DIST_DIR)" ]; then \
		echo Borrando carpeta de distribucion $(DIST_DIR)...; \
		rm -rf "$(DIST_DIR)"; \
	fi

# --- --- ---  EXPORTAR --- --- --- 
dist: cleandist all
	@echo --- Preparando distribucion en /$(DIST_DIR) ---
	@mkdir -p $(DIST_DIR)/lib
	@echo Exportando libreria estatica...
	cp "lib/$(LIBNAME).a" "$(DIST_DIR)/lib/"
	@echo Exportando headers...
	cp -R "include" "$(DIST_DIR)/include"
	@echo --- Libreria $(DIST_DIR) lista para usar ---