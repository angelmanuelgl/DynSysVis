# --- Configuración del Compilador ---
CXX = g++

# Carpeta donde exportaste tu libreria (distDynSysVis)
LIB_SDK = DynSysVis

# Ruta del archivo .cpp que pasas por consola
# Uso: gmake run APP=01Epodemiology/SEIR.cpp
APP_PATH = Stochastic-and-Deterministic-Lab/$(APP)

# --- Extrae el nombre base ---
APP_NAME = $(basename $(notdir $(APP_PATH)))

# --- Flags ---
# CXXFLAGS = -I$(LIB_SDK)/include -Wall -std=c++17 -DDSV_DEBUG
# --- Flags ---
CXXFLAGS = -I$(LIB_SDK)/include -I/opt/homebrew/opt/sfml@2/include -Wall -std=c++17 -MMD -MP -DDSV_DEBUG

# --- Linkeo para macOS ---
# --- Linkeo para macOS ---
LDFLAGS = -L$(LIB_SDK)/lib -L/opt/homebrew/opt/sfml@2/lib -lDynSysVis -lsfml-graphics -lsfml-window -lsfml-system -framework OpenGL
# --- Regla de Ejecución ---
run:
	@if [ -z "$(APP)" ]; then \
		echo "ERROR: Indica el archivo. Ej: gmake run APP=01Epodemiology/SEIR.cpp"; \
		exit 1; \
	fi
	@mkdir -p build
	@echo "--- Compilando simulacion: $(APP_PATH) -> build/$(APP_NAME) ---"
	$(CXX) $(APP_PATH) $(CXXFLAGS) -o build/$(APP_NAME) $(LDFLAGS)
	@echo "--- Ejecutando ---"
	./build/$(APP_NAME)