# GEANT4_Radiation_to_Diamond
Simulating the interaction of diverse particle species across energy regimes with diamond

After each run, a Mydata.root file is generated, containing information such as PKA (Primary Knock-on Atom) and SKA (Secondary Knock-on Atom) energies, positions, penetration depths, etc. These data are stored in a tree named Mydata, with each piece of information as a separate branch.

Subsequently, the extract.C script can be used (root -l -q extract.C) to extract the PKA position and energy spectrum data from the tree and record them in a position_energy.txt file.
