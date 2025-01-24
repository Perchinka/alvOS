# AlvOS
<div align="center">
  <img src="https://github.com/user-attachments/assets/c630a960-0b13-46c2-8926-4b2c99d89d8a" alt="AlvOS Screenshot">
</div>




https://github.com/user-attachments/assets/5d1cad8c-efa1-4fe4-91bc-1e275735e569




Or **Asteroids OS**🎮🚀, my christmas weekend project.
I built a minimal operating system that runs an "Asteroids" game. This project is inspired by materials from [TetrisOS video by jdh](https://www.youtube.com/watch?v=FaILnmUYS_U) and the [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page).

## Why?

Because it's fun! And I’ve wanted to build my own OS since I was in middle school😅


## Installing and Running 

Want to try it out? Here’s how:

### Prerequisites
If you want to simply launch the OS and test it out, ensure you have **qemu** installed on your machine. (I use **qemu-system-i386**)
> It’s designed to run in `qemu`. Running it on real hardware might not work, but if it does, let me know!

If you want to play around and build the OS yourself from sources, ensure you also have the following installed:
- **NASM**
- **i686-elf-gcc** and **i686-elf-ld**
- **Make**

### Steps

1. Clone the repo and navigate to the project folder:
   ```
   git clone https://github.com/Perchinka/alvOS.git
   cd AlvOS
   ```

2. Build `bootable.img` in the `build/` folder:
   ```
   make
   ```

3. Run the OS in `qemu`:
   ```
   make run
   ```

4. To clean up build files use:
   ```
   make clean
   ```

## Controls

When the OS boots, the game starts right away. Use your keyboard to control the spaceship:

- **W** - Throttle
- **A-D** - Steer the spaceship
- **Spacebar** - Fire bullets


## Contributions?

Feel free to explore and modify this project. It's just a weekend experiment, but if you find ways to make it better, go for it! Just leave a pull request😄

---

Enjoy blasting asteroids! 🎉
