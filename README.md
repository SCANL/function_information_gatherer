# function_information_gatherer

Before compiling, you need to install:
- Cmake (tested on 3.5)
- libxml2-dev

To compile the C++ script, do the following in the root (i.e., project) directory:
- mkdir build
- cd build
- cmake ..
- make (consider using -j3 to make it go faster)

Once it is compiled, you should have an executable in the build/bin folder. 

Run with: ```./gather_function_data (file name)```

Where ```file name``` is a srcML archive (code that has been run through srcML).
