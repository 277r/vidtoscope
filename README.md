# Video to scope

# Warning
This program was written quick and dirty, to complete one task: Play Bad Apple on an oscilloscope.<br>
The runtime of the program is near a minute on my PC, and uses about one gigabyte of memory.<br>
In the bad apple video, there is about a 1% time discrepancy.<br>
## Required
An oscilloscope that supports XY mode / vector <br>
A computer with a line out connector.<br>
Cables to connect scope with computer.<br>

## Code
requires libpng++<br>
`g++  vidtoSB.cpp -lpng16 ` to compile the program<br>

## Commands ran 
Scale a video down to 480x360, make sure it runs at 30fps if you want the time to be near-equal.<br> 
`ffmpeg -i video %04d.png` to output each frame as a png image<br>
`./a.out 6572` run the program, tell it to take frames 0001.png to 6572.png<br>
Then play video_signal.wav with your desired media player

## demonstration

[Youtube Video without proper audio](https://youtu.be/woJMKU1RYtg)<br>
[Youtube video with proper audio](https://youtu.be/yURjn6atQYc)