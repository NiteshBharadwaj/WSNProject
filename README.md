WSNProject
==========

A mesh network using Digi 865LP and arduino to collect voltage and current measurements from induction motors.

The codebase is divided into 2 parts:

1) Python Interface:
This is to be run from a PC/Laptop connected to a Digi 865LP (co-ordinator). This is used to aggregate the data from various sensors and post-process it.

2) Arduino Interface:
This is to be run from an arduino connected to sensors and a Digi 865LP. These form the core of the network.
