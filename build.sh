rgbasm -i src/includes -o build/roguelike.o src/main.asm
rgblink -n build/roguelike.sym -m build/roguelike.map -o build/roguelike.gb build/roguelike.o
rgbfix -v -p 0xFF build/roguelike.gb