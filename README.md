# Foma example codes

(C) 2016-2018 by [Damir Cavar](http://damir.cavar.me/)

Last edited: 2018-08-06, [Damir Cavar](http://damir.cavar.me/)


## Intro

This code example shows how a [Foma](https://fomafst.github.io)-based FST can be used to process multi-word expressions that are given in a dictionary and compiled into a Finite State Transducer.

There is a default window size specified in the code. It can be altered using command line arguments.

The maximum multi-word window size can actually be compiled into the Finite State Transducer and read out using the C-wrapper. This way one can avoid unnecessary lookups. The advantage of this method, assuming that one has a comprehensive list of multi-word expressions to compile into the transducer, is that it is very fast and that it shows internal structural and morphosyntactic properties of multi-word expressions.

The example implementation should be straight forward to understand. It expects an input in form of a file (or a stream) that contains a tokenized sentence per line. See the included *test.txt* file for an example.


## Build from Code

To compile this example, you need to have the entire Foma collection of binaries, includes and libraries set up on your system. You will also need some C++11 compiler and various other libraries for it, for example the [Boost](https://www.boost.org) libraries.

The project is a [CMake](https://cmake.org) project. Make sure that you have also [CMake](https://cmake.org) installed and set up on your system.

To create the running binary for the code in *FomaMWT*, in the folder run:

	cmake CMakeList.txt

This will generate the *Makefile* and other files in the same folder. Run:

	make

and it should compile correctly, if all the paths and folders are OK, and if the libraries were found.


If you want to test the speed of the processor, run the following command:

	time ./mwtagger test.txt > res.txt
