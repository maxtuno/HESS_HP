# HESS black-box for Hamiltonian Path Problem

HESS is an Algorithm developed by was developed by Oscar Riveros [oscar.riveros@peqnp.science] from 2012 to 2018.

Info:
https://www.youtube.com/watch?v=mA4gv-LuZXw

Example for TSP:
https://colab.research.google.com/drive/1crfbZyVXahi54zzQp0ENN-bNzMzWCYAa

# Build

	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Release ..
	make

# Usage
	
	./hp instance

	./hc_check instance path