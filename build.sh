rgbgfx -d1 -o assets/prototype/house-1bpp-no-grid-bw.1bpp assets/prototype/house-1bpp-no-grid-bw.png 

rgbasm -i src/includes -o build/roguelike.o src/main.asm
rgblink -n build/roguelike.sym -m build/roguelike.map -o build/roguelike.gb build/roguelike.o
rgbfix -v -p 0xFF build/roguelike.gb