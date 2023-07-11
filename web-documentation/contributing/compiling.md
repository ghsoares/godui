# Compiling

To be able to see and test changes made to Godui from yourself, you must compile it first

## Basics

To compile Godui in your development machine, firstly follow the official [Godot's GDExtension tutorial](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html) which has instructions of tools to install and how the build system works.

## Build process

- Clone the project to the desired folder and navigate to it (in the case of a fork, clone it instead):
```bash
$ git clone https://github.com/ghsoares/godui.git
$ cd godui
```

- Inside `gdextension/godot-cpp/` folder, you must run two commands, one to grab latest changes from godot-cpp repository and other to compile the bindings:
```bash
$ cd gdextension/godot-cpp
$ git submodule update --init
$ scons platform={platform} -j4
```
Replace `{platform}` with the platform depending on your target system:
  - `linux` to target Linux distributions;
  - `macos` to target MacOS systems;
  - `windows` to target Windows systems;
  - `android` to target Android systems;
  - `ios` to target iOS systems;
  - `javascript` to target Web;

- Now, inside `gdextension` folder, run scons with the target system:  
```bash
$ cd ..
$ scons platform={platform} -j4 
```
Replace `{platform}` with the same platform you've used at the previous step.
  
Feel free to change `-j4` argument, it tells how many threads are used to compile the extension, you can replace with `-j{threads}` with `{threads}` being the number threads used.

- The compiled binaries will be inside `addons/godui/bin/` folder, you can then copy the contents to the `addons/godui/bin/` folder inside your project.




