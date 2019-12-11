David Rozmajzl
 -Project 3

After creating the executable using the source code called "main.cpp,"
you can run the code by issuing the following command in the command line:

	./main image.pgm 10 5

	Where 10 is the number of vertical seams to remove and 5 is 
	the number of horizontal seams to remove

This program works by first taking the .pgm file and inputing each of its
integer values into a 2d array. Each of these integer values represents a 
grayscale value for a pixel of the image. I first calculate the energy of 
each individual pixel using the equation given in the project overview. The
energy value for each pixel is stored in a second 2d array. Then, a cumulative
energy value for each pixel is found by adding the least energy pixel above
and touching that pixel. These values are also stored in a third 2d array.
Finally, we start at the bottom row of pixel and look at the least value in
the cumulative energy 2d array. Then we look at the 3 pixels above it and pick
the least value from the culumative energy 2d array. It is important to note that
if the values are the same, the program picks the leftmost pixel. The pixel that
is chosen is marked in the first 2d array with a -1 to indicate that it is to 
be removed. After marking a whole vertical seam with -1's, the program pushes all
these -1's in the array with the original pixel values all the way to the left and
decrements the column count. This is done so that we don't have to create an entirely
new 2d array after removing a column. 

After removing all the vertical seams, the program removes all horizontal seams.
As above when tracing the least path, the program favors the topmost pixel
if the values are the same. It behaves pretty much the same as when removing the
vertical seams just with a different orientation.

After removing all the seams, all the proper values are outputted into a file that 
is the original name with "_processed" tacked on to the end.