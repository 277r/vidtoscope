# Video to scope

# Warning
This program was written quick and dirty, to complete one task: Play Bad Apple on an oscilloscope.
The runtime of the program is near a minute on my PC, and uses about one gigabyte of memory.
In the bad apple video, there is about a 1% time discrepancy.
## Required
An oscilloscope that supports XY mode / vector 
A computer with a line out connector.
Cables to connect scope with computer.

## Code
requires libpng++
`g++  vidtoSB.cpp -lpng16 ` to compile the program

## Commands ran 
Scale a video down to 480x360, make sure it runs at 30fps if you want the time to be near-equal. 
`ffmpeg -i video %04d.png` to output each frame as a png image
`./a.out 6572` run the program, tell it to take frames 0001.png to 6572.png
Then play video_signal.wav with your desired media player

## demonstration

[Youtube Video without proper audio](https://youtu.be/woJMKU1RYtg)
[Youtube video with proper audio](https://youtu.be/yURjn6atQYc)