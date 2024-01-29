@echo off

del bin\game_gen.exe
cd build\make
make
cd ..\..\bin
:: adding for malloc as test
game_gen
cd ..
