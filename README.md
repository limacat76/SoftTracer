# SoftTracer
A multithreaded software-only RayTracer written in C++. 
This is a for-study project.

## Features
Currently SoftTracer:
- uses SDL 2.0 as a target for displaying a CPU-rendered image and with tweaks to avoid hogging the cpu after the rendering finishes.
- it also has a headless target for debugging and profiling purposes.
- uses multi threaded jobs. Each jobs works on a pixel area before moving on, so slow parts won't hog just one core
- imports Jakko Bikker's code from his old articles on flipcode (currently the third part, more to follow before an in-depth refactoring).

## Getting Started
This project is being developed directly on Windows 10. I am trying to avoid platform dependent code, but since I am using
Visual Studio 2017 CE and running on Windows 10, and not yet testing it in other environments, some windows-isms might slip.

In order to compile the project, you need:
- a source distribution of SDL in C:\SDL\SDL2-devel-2.0.5-VC\SDL2-2.0.5.1 
- a binary distribution of Boost (for Boost::Parameters) in C:\SDL\local\boost_1_64_0

If you are opening the project for debug on Windows inside Visual Studio you currently need to have SDL2.dll 
(the version of your build, 64 bits, set as default, or 32 bits if you need to change) in the visual studio directory or to 
have the DLL installed on the system. 

## What are you currently working on?
I imported Jakko Bikker's code from the first up to the third part of his tutorial on flipcode[1].
The plan is to follow up with the whole tutorial and then work it up by either creating a new algorithm, or to use it as a scaffolding for
new code.

## What is your future work?
- Currently the communication via threads is slow. I identified some fixes that will be implemented soon.
- Import part 7 of the Flipcode Tutorial code (and make it multi-threading).
- Write from scratch my Ray Tracing algorithm.
- Some day: make the solution less dependent on my system configuration

## Need help?
You can contact me at limacat@gmail.com

[1] https://www.flipcode.com/archives/Raytracing_Topics_Techniques-Part_1_Introduction.shtml

30.07.2017