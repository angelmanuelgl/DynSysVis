CXX = g++
# variable APP que definiremos al llamar al comando
APP_PATH = $(APP)
CXXFLAGS = -Ilib_grafica/include -Wall -std=c++17
LDFLAGS = -Llib_grafica/lib -linsightrt -lsfml-graphics -lsfml-window -lsfml-system

# regla generica
run:
	@if not exist build mkdir build
	$(CXX) $(APP_PATH) $(CXXFLAGS) $(LDFLAGS) -o build/app.exe
	./build/app.exe