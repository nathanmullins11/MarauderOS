# R6 Warnings
Due to the complex implementation of R6, there are a few features that do not work as intended. They are as follows:
- Arrow keys: the arrow keys are disabled. Pressing them will not do anything. 
- Delete & Escape Key: these keys are also disabled and will nto do anything.
- Running `pcb -l`: If listing out all the processes on the system is ran too many times, it eats up all the memory in the system and causes heap to fill up and crashes.
- `alarm`: The alarm function is only functional if the alarm time is set prior to the machine time (i.e. if the machine time is 23:23:23, the function will work properly if the input to terminal is "alarm -t 12:12:12". Function is non-functional otherwise due to an issue with comhand getting blocked and not becoming unblocked, leaving sys_idle and the alarm process transfering execution back and forth.
- `load_r3`: This command is non-functional due to a bug with exiting `proc5`. It gets stuck exiting, saying "Failure to Exit Proc 5". 
- BONUS: the color coding, entry/exit messages, and clear command can be tested in R6. The history of previously entered commands must be viewed in our R4 tag. It would be preferable if you tested all BONUS in the R4 tag. 