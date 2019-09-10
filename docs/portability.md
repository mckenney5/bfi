Portability
===========

### Brainfuck Commands
From wikipedia[1] and implemented as follows:

| \> | 	increment the data pointer (to point to the next cell to the right).

| \< |	decrement the data pointer (to point to the next cell to the left).

| \+ |	increment (increase by one) the byte at the data pointer.

| \- |	decrement (decrease by one) the byte at the data pointer.

| \. |	output the byte at the data pointer.

| \, |	accept one byte of input, storing its value in the byte at the data pointer.

| \[ |	if the byte at the data pointer is zero, then instead of moving the instruction pointer forward to the next command, jump it forward to the command after the matching ] command.

| \] |	if the byte at the data pointer is nonzero, then instead of moving the instruction pointer forward to the next command, jump it back to the command after the matching [ command.

Unless the '-d' or '--debug' flag is turned on, all other characters are ignored by the interpreter.

### Debugging Commands
Only enabled when the '-d' or '--debug' flag is set except when in interactive mode where 'q' quits the program and 'c' clears the screen.

| q |	Exit the interpretor with the exit code zero.

| d |	Dumps all memory that has a value that isn't zero, treats data as a char.

| D |	Dumps all memory that has a value that isnt zero, treats data as an int.

| C |	Clears the memory and returns the memory pointer to zero.

### Cells
Each cell is the size of an int, being OS specific.
If the cell's value is greater (or absolute less than) its max size, the cell's number wraps around which is consistant with a typical C interger.

### Array
By default, the memory is a dynamically allocated, zeroed-out, array of ints with 10,000 members.
Memory can be expanded and shortened via the '-m <nmemb>' or '--memory <nmemb>' arg.
When the program goes behond the bounds set to the right, 10 more members are dynamically allocated and set to zero. If the memory cannot be increased, an error is displayed.
When the program tries to go behond the bounds to the left, the command is ignored and the memory pointer stays at zero. An error will also be displayed.

### End-of-file Behavior
When a brainfuck program uses the ',' command, the intperpretor uses getchar() which returns EOF (or -1) when it reaches the end of file or has an error.

### Square Brackets
If the brainfuck program is missing any bracket, the interpretor will warn the user and halt, unless the missing brace is a '[' and the memory pointer is pointing to zero, in that case the ']' is instead ignored.

### Conforming to
- "nice" (Portable Brainfuck)[2]

### Citations
[1] https://en.wikipedia.org/wiki/Brainfuck

[2] http://www.muppetlabs.com/~breadbox/bf/standards.html

