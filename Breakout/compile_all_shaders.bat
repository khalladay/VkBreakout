@echo off
del "./shaders/*.spv"
for %%f in (./shaders/*.vert) do glslangValidator -V -o ./shaders/%%f.spv ./shaders/%%f
for %%f in (./shaders/*.frag) do glslangValidator -V -o ./shaders/%%f.spv ./shaders/%%f
