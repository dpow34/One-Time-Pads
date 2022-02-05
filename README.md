# One-Time-Pads
Five small programs that encrypt and decrypt information using a one-time pad-like system

 [Requirements](https://github.com/powdrild/One-Time-Pads/blob/main/requirements.md)

To run One-Time Padst:

1. Navigate to the directory where all the files are stored in the terminal.

2. Then type "chmod +x compileall" and press enter in the terminal.

3. Then type "compileall" in the terminal and press enter.

3. There should now be 5 exectuable files in the directory: dec_client, dec_server, enc_client, enc_server, and keygen.

4. In the termainl type "chmod +x ./p5testscript" and press enter.

5. Then in the terminal type "./p5testscript RANDOM_PORT1 RANDOM_PORT2 > mytestresults 2>&1" 
   (where RANDOM_PORT1 & RANDOM_PORT2 are random numbers between 55000 and 65000) and then press enter.

6. There should now be a file named "mytestresults" in the current directory with the results from the grading test script. 
