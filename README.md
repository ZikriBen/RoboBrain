# RoboBrain

## My assumptions:
 - If I understand correctly, we are using rotating motors (for simplicity only clockwise). 
 - The motor positions are angular positions, and will be represented as angles 0-360 when zero is the motor starting point. 
 -  The motors' main movement is by calling `netFunctions` in the `mainLoop` that controls the motors position. 
 - The netFunction will be added to a queue and removed in FIFO. netFunction will be exhausted in `mainLoop`, but this behavior could be to run a single `netFunction` per cycle. 
 - Also, the motors are constantly moving in `motorsLoop` in one direction. 
 - Also, the motors also have some `noise` produced in `positionLoop` that is constantly rotating but in a small portion 
 - The difference in Hz frequency will let the motors change positions with "priority where the lower Hz will get precedence like so: `NetFunction(MainLoop) -> MotorsLoop -> PositionLoop`

## VSC++ dependencies and linkage:
As I was struggling to get the Visual Studio solution to build/link/compile correctly, so I wrote down the steps to configure it:
1. Clone the yaml-cpp repository:
`git clone https://github.com/jbeder/yaml-cpp.git`
2. Navigate to the cloned repository:
`cd yaml-cpp`
3. Create a build directory:
`mkdir build`
4. Navigate to the build directory:
`cd build`
5. Run CMake with the appropriate options:
`cmake -DYAML_BUILD_SHARED_LIBS=ON ..` This will create a new Release/Debug folder that we will use later.
5. Install the Windows 10 SDK with all the necessary versions (in my case `10.0.19041.0`).
6. Open Visual Studio and create a new DLL project.
7. Inside the new solution, go to Project -> Properties -> Configuration Properties -> VC++ Directories -> Include Directories, and add the paths to all the `include` of `yaml-cpp`,  `pybind11`, and global Python.
8. Still in the project properties, go to Project -> Properties -> Configuration Properties -> VC++ Directories -> Library Directories, and add the paths to the `yaml-cpp.lib` directory and the Python310 `libs` directory.
9. Go to Project -> Properties -> Configuration Properties -> Linker -> Input, and add `yaml-cpp.lib` to Additional Dependencies (`pybind11` is only header lib, so no need to link).
10. Build your project. If the build is successful, the output folder will contain the generated DLL file.
11. Change the file extension of the generated `.dll` from to `.pyd`. Make sure the file name matches the module name in `pybind11` code in the `.cpp` file.


## Usage:
Follow the steps below, build and run the `main.py` python file inside the output folder (where the `.dll` / `.pyd` is generated).
Prints are a bit messy, but Python menu still works.

## Conclusion:
- Didn't get to the bonus question :(  
- I struggled with the VS environment setup, linking, and building. I think it took me an overall 4 hrs, but next time I can follow the steps and be more efficient.  
- After all, I had a good time working on the task and enjoyed it very much, it took me around 10 hours to finish.
