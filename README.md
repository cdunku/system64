# system!_64


system!_64 is a cycle-accurate implementation of the famous Commodore 64 8-bit home computer realesed in 1982. This emulator is fully written in the `c2x` standard of the systems programming language `C`. 


This project is licensed under the `GNU General Public License v3.0`.


### What are the goals?

- `Accuracy`: This Commodore 64 emulator claims cycle-accuracy, and being similar to the hardware as close as possible without sacrificingreadability and modularity. Additionally, the emulator will provide different kind of test suites for every chip/component that can be ran. People are free to compare the states and results of their own tests and mine :D. I would like to also add that testing this emulator to the real hardware and comparing the results is planned for the future.
 
- `Debuggability`: system!_64 aims to be a tool for anyone that wants to debug demos, games and software designed for this very computer. The emulator will try to offer component-dependent debugging tools and prompts that help reading the state of pins and registers of each component easier.

- `Features`: This C64 emulator will try to support as many revisions, chip versions and software as possible. For an example, chip versions such as the (NTSC) 6562 and (PAL) 6569 will be supported in the future.

- `Educational`: This is probably the most important aspect of the emulator. I aim for it to be an educational piece of software that both hardware enthusiasts, newcomers and the free and open-source community as a whole can benefit from. Anyone is able to freely modify, distribute and privately use this emulator as mentioned in the `GNU GPL v3.0` license. 


You can access my blog where I document the `Commodore 64` together with my code and my journey [here](https://cdunku.github.io/cdunku/system!_64/index.html).


### How do I build?


The project can be currently built through `make`, but I was planning to migrate to `Cmake` instead.

Dependencies such as `SDL3` and `SDL3-devel` have to be installed on your system in order to build the project.

```sh

# If you want to compile using debug + sanitizer flags:

make 

#or 

make production


# If you want to compile using optimization flags:

make release


# In order to run the production/release executable:

./system!_64


# If you would like to run tests (with debug flags):

make tests


# In order to run a test for specific component, they are compiled in ./test:

./tests/6510_tests

# or

./tests/906114-01_test

#...
```
