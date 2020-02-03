Multithreading function graph operationing program.

Build a graph using the formula y = A + B * sin (C * t). Where t is the time in seconds.
Coefficients A, B, C are each controlled by its own flow. The range of changes is A [3-7], B [1-3], c [1-10].
Make an opportunity to pause any thread and set the coefficients manually.
When the thread is running, corresponding coefficient varies within the specified limits depending on the time of the sine wave
with the frequency specified by the operator.
At the command of the operator, the display flow on the monitor screen starts
and stream starts to write the chart to a text file.

Control flow operator with the keyboard (under Windows):

To start streams of automatic coefficient change
(within the specified limits depending on the time of the sinusoid): button '3'

To change the frequency (Hz) by the operator: buttons '1' '2' - change - + in increments of 1.

Coefficient A: buttons 'q' 'w' - change - + with step 1. 'e' - pause flow
Coefficient B: buttons 'a' 's' - change - + in increments of 1. 'd' - pause flow
Coefficient C: buttons 'z' 'x' - change - + in increments of 1. 'c' - pause flow

Start the stream of writing the chart to a file on disk: the 'f' button
