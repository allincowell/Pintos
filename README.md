PintOS (Ben Pfaff et al.) [http://pintos-os.org/SIGCSE2009-Pintos.pdf] is a popular minimal operating used at many Universities – in India and overseas – to train students in the internal details of the Unix-like operating systems. PintOS is minimal since many OS capabilities are missing and we had to implement them by modifying the codebase.

This project was built as part of OS Course Project at IIT-KGP.

The final project is the "src" folder & the design details is under "Design doc.pdf". The final project comprised of 3 phases.

The changes made in each of the 3 phases are in folders "Project-1", "Project-2", and "Project-3". The design docs of the 3 phases can also be found in the cooresponding folders.

The details of these individual projects are "Project-Details-($project_number).pdf" files.

Brief overview:

1. Developed system based on x86 architecture can run on any regular PC.

2. Can efficiently schedule user and kernel threads.

3. Can load and run user programs on qemu simulator. (Had been thoroughly tested)

4. C language has been used for implementing various algorithms and data structures to make the OS efficient.

Details of Project phases:

1. Project-1: Reimplemented timer_sleep(), defined in devices/timer.c. Previously the implementation "busy waits" that is it spins in a loop checking the current time and calling thread_yield() until enough time has gone by.

2. Project-2: Implemention wake-up thread and MLFQS (multilevel feedback queue scheduler) to Pintos. The resulting operating system should be able to run either the basic priority scheduler or the MLFQ scheduler. The choice is made when Pintos boots. By default, it will run the basic priority scheduler.

3. Project 3: Setup of basic file system in Pintos & User program's memory stack structure. Enabling minimal running of user programs. Finally, argument passing and most common system calls like exit, exec, create, remove, open, filesize, read, write, and close were implemented.