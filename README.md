# SoftTracer
A multithreaded software-only RayTracer written in C++.
For now it's just scaffolding, I expect to be able to import a RayTracer algorithm "soon".

## Features
Currently SoftTracer:
- has been tested with a raytracing algorithm but that algorithm was ugly and it was set on fire.
- uses SDL 2.0 as a target for displaying a CPU-rendered image and with tweaks to avoid hogging the cpu after the rendering finishes.
- it also has a headless target for debugging and profiling purposes.
- uses multi threaded jobs.

## Getting Started
The project has currently been tested only on Windows 10. I am trying to avoid platform dependent code, but since I am using
Visual Studio 2017 CE and running on Windows 10 some windows-isms might slip.

In order to compile the project, you need:
- a source distribution of SDL in C:\SDL\SDL2-devel-2.0.5-VC\SDL2-2.0.5.1 
- a source distribution of Eigen in C:\SDL\eigen-3.3.4

If you are opening the project for debug on Windows inside Visual Studio you currently need to have SDL2.dll 
(the version of your build, 64 bits, set as default, or 32 bits if you need to change) in the visual studio directory or to 
have the DLL installed on the system. 

## What are you currently working on?
Studying the code from the first part of Jacco Bikker's articles on flipcode.
https://www.flipcode.com/archives/Raytracing_Topics_Techniques-Part_1_Introduction.shtml

## Need help?
You can contact Davide Inglima at limacat@gmail.com