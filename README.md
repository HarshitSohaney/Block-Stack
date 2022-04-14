# Block-Stack
Block Stacking game using low level language programming on the ARM processor with the DE1 - SoC Board

<p align="center">
<img width="400" alt="Screen Shot" src="https://i.postimg.cc/DwT09xDR/Capture.png">
</p>

## Project Description:

Block Stack is a game where your goal is to stack the maximum number of blocks on top of each other. If the box stacked is misaligned with the previous box, the edge off the box is cut off and affects the size of the subsequent boxes. As you move onto the second level, the number of boxes generated on the screen increases, making the game more difficult. 

The game is implemented using lowlevel C and can be played on a VGA display.  

## How to Play: 
-	Use the left key to move left, and the up key to move right
-	Use the down key to fix the position of the box

## Project Demo 
![ezgif com-gif-maker](https://user-images.githubusercontent.com/52717128/163480458-08ffc5a5-b4c6-4f61-bc8f-ee330c3237ff.gif)

## How to Run Game: 
### Option 1: On computer
1. Open https://cpulator.01xz.net/ 
2. Select the following options and press go

<p align="center">
<img width="962" alt="Screen Shot 2022-04-14 at 5 35 59 PM" src="https://user-images.githubusercontent.com/52717128/163480761-b429fd9b-bb0b-494b-8760-234f0b4c1e9a.png">
</p>

3. Load in the c file named "cpulater_project" stored in the folder src
4. Press Compile, Load and Continue to run the game 

### Option 2: On the DE1-SoC FPGA Board
1. Load the files on the Quartus Monitor Program
2. Create a new project and select the ARM Cortex-A9 
3. After creating the files and setting up the program, hit compile & load
4. Run the program and start the VGA
