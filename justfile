[no-cd]
run: build
  -killall gdb
  -killall clangd
  cd ./build/bin && ./hellfrost

[no-cd]
build:
  rm -rf ./build/hellfrost
  cd ./build && make -j6
  rm -rf ./build/bin/hellfrost
  cp ./build/hellfrost ./build/bin/

[no-cd]
init:
  rm -rf ./build
  mkdir -p ./build/bin
  cd ./build && cmake ..
  cd ./build/bin && ln -s ../../tilesets ../../imgui.ini ../../game.bin ../fonts/* .
