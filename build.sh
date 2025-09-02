rgbasm -i src -o build/main.o src/main.asm
rgblink -n build/roguelike.sym -m build/roguelike.map -o build/roguelike.gb build/main.o
rgbfix -v -p 0xFF build/roguelike.gb