# ZSKompile - Online C++ Compiler
This compiler was made for our school project.
Its dependencies:
* Windows (Desktop/Server)
* Any web server capable of running CGI (we've tested it on XAMPP)
* GCC/G++ compatible compiler (we've burnt it in MinGW G++ compiler)

The code to compile and STDIN inputs are encoded to UTF-8 decimal values and, using POST request, are being sent to our virtual backend, which then decodes them and compiles the code using GCC and launches the compiled program with "echoed" STDIN.
