# R6 Warnings
Due to the complex implementation of R6, there are a few features that do not work as intended. They are as follows:
- Arrow keys: the arrow keys are disabled. Pressing them will not do anything. 
- Delete & Escape Key: these keys are also disabled and will nto do anything.
- Running `pcb -l`: If listing out all the processes on the system is ran too many times, it eats up all the memory in the system and causes heap to fill up and crashes.
- `alarm`: The alarm function is non-functional due to an issue with a second `sys_req` read call.
- `load_r3`: This command is non-functional due to a bug with exiting `proc5`.