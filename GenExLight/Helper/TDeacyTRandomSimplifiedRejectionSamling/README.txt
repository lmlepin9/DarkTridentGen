Description:
  
	Toy MC generator that using modified TGenPhaseSpace

Warning:
 	
	It makes spherical decay and is unsiutable for strong cuts, e.g., 
	on p_{t} of some particles.

	It is only proof of concept - not suitable for production. Use FOAM version instead.

Authors:
	
	Radoslaw Kycia kycia.radoslaw@gmail.com
	Cracow University of Technology Krakow, Poland
 
	Jacek Trunau

License:

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>


Commands:

make run - compile and run generator
make clean - clean executables
make cleanest - clean executables and results of simulation


Dependence:

Program depends highly on ROOT framework [1]. 

For compiling (GNU) make system and c++ compiler is required - if ROOT was installed properly from the source then all necessary things to run generator should be installed.

There is a simple (GNU) Makefile available. Before run please check if locations to ROOT folder, ROOT includes and ROOT libraries in your linking system were set properly.

Warnings: 1. Underlaying random number generator TRandom3 from ROOT package has the period = 2^19937-1. It is big but finite number. It should be large enough for any today's applications, however anyone who seriously consider generation of much larger set of events should change the generator, change the seed/use the presistency or use hardware random number generators. 2. It is assumed that first two particles are leading ones.

Bibliography:


[1] ROOT framework, (access date: 04/01/2013): http://root.cern.ch/drupal/

[2] Gnu Scientific Library, (access date: 04/01/2013): http://www.gnu.org/software/gsl/

[3] Doxygen documentation, (access date: 04/01/2013): http://www.stack.nl/~dimitri/doxygen/
