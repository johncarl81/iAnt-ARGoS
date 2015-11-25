cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DARGOS_BUILD_NATIVE=ON
make clean
make
cd ..
time argos3 -c experiments/iAnt.xml