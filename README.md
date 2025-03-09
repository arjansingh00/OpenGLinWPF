## Overview

#### This project is based upon this [CodeProject tutorial](https://www.codeproject.com/Articles/23736/Creating-OpenGL-Windows-in-WPF) and the code provided there.

#### Make sure to debug as x86.

I have rebuilt the program using the same source code (with a few modifications) using a C++ CLR Class Library, because the original project from 2009 did not render on my computer. I use the compiled DLL in the C# WPF project.

The code currently retains the formatting from the original project, which can be slightly confusing, inconsistent, and redundant in places.

This project was part of an exploration into finding out how a C# WPF application can host a C++ rendering application that uses a graphics API such as OpenGL (used in this project) or DirectX. This method can be used for DirectX.

I have found that sending user input from C# WPF to C++ HwndHost is very difficult and limited.
