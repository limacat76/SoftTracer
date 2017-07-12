# SoftTracer
A Software RayTracer written in C++.
I expect to be able to import a RayTracer "soon".

## Features
Currently SoftTracer
- supports multi threaded jobs.
- uses SDL 2.0 with a software rendered image and how to avoid hogging the cpu after the rendering finishes.
- has been tested with a raytracing algorithm but it was ugly and it was set on fire.

## Getting Started
The project has been currently tested only on Windows 10. I am trying to avoid platform dependent code, but I am using Visual Studio 2017 CE for development so some windows-isms might slip.

In order to compile the project, you need a source distribution of SDL in C:\SDL\SDL2-devel-2.0.5-VC\SDL2-2.0.5.1 
If you are opening the project on windows you currently need to have SDL2.dll (the version of your build, 64 bits, set as default, or 32 bits if you need to change) in the visual studio directory. 

## What are you currently working on?
Studying the code from the first part of Jacco Bikker's articles on flipcode.
https://www.flipcode.com/archives/Raytracing_Topics_Techniques-Part_1_Introduction.shtml

## Need help?
You can contact Davide Inglima at limacat@gmail.com
