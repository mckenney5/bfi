Brain Fuck Interpreter
======================
bfi — _A brain fuck interpreter_

![screen-shot](https://github.com/mckenney5/bfi/raw/master/sc.png)

## Description
An interactive interpreter and crosspiler for brain fuck.
See docs/ for more info.

## Project Meta
This software is in a **working alpha** state.
The software may not work and may have drastic changes in the future.

## License
This code (see exception) is licensed under the MIT license. See 'LICENSE' for details. 

Note, the source code for [linenoise-mob](https://github.com/rain-1/linenoise-mob) is also 
included in this repo which is under the BSD 2-Clause "Simplified" License. 
See their license for more details.

The example folder contains brain fuck source collected from different websites and repos and
is therefor not owned by me.

## Compiling and Testing
This program allows a couple of compiling options.

The simplest way is to compile by `make`

##### Make Options:

* `make debug`		: adds debugging symbols to the program, along with verbose output
* `make`		: makes the UI use the [linenoise-mob library](https://github.com/rain-1/linenoise-mob)
* `make install`	: [TODO] moves bfi to the /usr/bin file and installs the manpage
* `make uninstall`	: [TODO] deletes /usr/bin/bfi and the man file from the system

