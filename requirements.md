Your program will encrypt and decrypt plaintext into ciphertext, using a key, in exactly the same fashion as above, except it will be using modulo 27 operations: your 27 characters are the 26 capital letters, and the space character. All 27 characters will be encrypted and decrypted as above.

To do this, you will be creating five small programs in C. Two of these will function as servers, and will be accessed using network sockets. Two will be clients, each one of these will use one of the servers to perform work, and the last program is a standalone utility.

Your programs must use the API for network IPC that we have discussed in the class (socket, connect, bind, listen, & accept to establish connections; send, recv to send and receive sequences of bytes) for the purposes of encryption and decryption by the appropriate servers. The whole point is to use the network, even though for testing purposes we’re using the same machine to run all the programs: if you just open the datafiles from the server without using the network calls, you’ll receive 0 points on the assignment.

Here are the specifications of the five programs:
enc_server

This program is the encryption server and will run in the background as a daemon.

    Its function is to perform the actual encoding, as described above in the Wikipedia quote.
    This program will listen on a particular port/socket, assigned when it is first ran (see syntax below).
    Upon execution, enc_server must output an error if it cannot be run due to a network error, such as the ports being unavailable.
    When a connection is made, enc_server must call accept to generate the socket used for actual communication, and then use a separate process to handle the rest of the servicing for this client connection (see below), which will occur on the newly accepted socket.
    This child process of enc_server must first check to make sure it is communicating with enc_client (see enc_client, below).
    After verifying that the connection to enc_server is coming from enc_client, then this child receives plaintext and a key from enc_client via the connected socket.
    The enc_server child will then write back the ciphertext to the enc_client process that it is connected to via the same connected socket.
    Note that the key passed in must be at least as big as the plaintext.

Your version of enc_server must support up to five concurrent socket connections running at the same time; this is different than the number of client connection requests that could queue up on your listening socket (which is specified in the second parameter of the listen call). Again, only in the child server process will the actual encryption take place, and the ciphertext be written back: the original server daemon process continues listening for new connections, not encrypting data.

In terms of creating that child process as described above, you may either create a new process with fork when a connection is made, or set up a pool of five processes at the beginning of the program before the server allows connections. Regardless of the method you choose, your system must be able to do five separate encryptions at once.

Use this syntax for enc_server:

enc_server listening_port

listening_port is the port that enc_server should listen on. You will always start enc_server in the background, as follows (the port 57171 is just an example; yours should be able to use any port):

$ enc_server 57171 &

In all error situations, this program must output errors to stderr as appropriate (see grading script below for details), but should not crash or otherwise exit, unless the errors happen when the program is starting up (i.e. are part of the networking start up protocols like bind). Once running, enc_server should recognize any bad input it receives, report an error to stderr, and continue to run. Generally speaking, though, this server shouldn’t receive bad input, since that should be discovered and handled in the client first. All error text must be output to stderr.

This program, and the other 3 network programs, should use localhost as the target IP address/host. This makes them use the actual computer they all share as the target for the networking connections.
enc_client

This program connects to enc_server, and asks it to perform a one-time pad style encryption as detailed above. By itself, enc_client doesn’t do the encryption - enc_server does. The syntax of enc_client is as follows:

enc_client plaintext key port

In this syntax, plaintext is the name of a file in the current directory that contains the plaintext you wish to encrypt. Similarly, key contains the encryption key you wish to use to encrypt the text. Finally, port is the port that enc_client should attempt to connect to enc_server on. When enc_client receives the ciphertext back from enc_server, it should output it to stdout. Thus, enc_client can be launched in any of the following methods, and should send its output appropriately:

$ enc_client myplaintext mykey 57171
$ enc_client myplaintext mykey 57171 > myciphertext
$ enc_client myplaintext mykey 57171 > myciphertext &

If enc_client receives key or plaintext files with ANY bad characters in them, or the key file is shorter than the plaintext, then it should terminate, send appropriate error text to stderr, and set the exit value to 1.

enc_client should NOT be able to connect to dec_server, even if it tries to connect on the correct port - you’ll need to have the programs reject each other. If this happens, enc_client should report the rejection to stderr and then terminate itself. In more detail: if enc_client cannot connect to the enc_server server, for any reason (including that it has accidentally tried to connect to the dec_server server), it should report this error to stderr with the attempted port, and set the exit value to 2. Otherwise, upon successfully running and terminating, enc_client should set the exit value to 0.

Again, any and all error text must be output to stderr (not into the plaintext or ciphertext files).
dec_server

This program performs exactly like enc_server, in syntax and usage. In this case, however, dec_server will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to dec_client.
dec_client

Similarly, this program will connect to dec_server and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like enc_client, and must be runnable in the same three ways. dec_client should NOT be able to connect to enc_server, even if it tries to connect on the correct port - you’ll need to have the programs reject each other, as described in enc_client.
keygen

This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, generated using the standard Unix randomization methods. Do not create spaces every five characters, as has been historically done. Note that you specifically do not have to do any fancy random number generation: we’re not looking for cryptographically secure random number generation. rand() (Links to an external site.) is just fine. The last character keygen outputs should be a newline. Any error text must be output to stderr.

The syntax for keygen is as follows:

keygen keylength

where keylength is the length of the key file in characters. keygen outputs to stdout.

Here is an example run, which creates a key of 256 characters and redirects stdout a file called mykey (note that mykey is 257 characters long because of the newline):

$ keygen 256 > mykey


Example Usage

Here is an example of usage, if you were testing your code from the command line:

$ cat plaintext1
THE RED GOOSE FLIES AT MIDNIGHT STOP
$ enc_server 57171 &
$ dec_server 57172 &
$ keygen 10
EONHQCKQ I
$ keygen 10 > mykey
$ cat mykey
VAONWOYVXP
$ keygen 10 > myshortkey
$ enc_client plaintext1 myshortkey 57171 > ciphertext1
Error: key ‘myshortkey’ is too short
$ echo $?
1
$ keygen 1024 > mykey
$ enc_client plaintext1 mykey 57171 > ciphertext1
$ cat ciphertext1
WANAWTRLFTH RAAQGZSOHCTYS JDBEGYZQDQ
$ keygen 1024 > mykey2
$ dec_client ciphertext1 mykey 57172 > plaintext1_a
$ dec_client ciphertext1 mykey2 57172 > plaintext1_b
$ cat plaintext1_a
THE RED GOOSE FLIES AT MIDNIGHT STOP
$ cat plaintext1_b
WSXFHCJAEISWQRNO L ZAGDIAUAL IGGTKBW
$ cmp plaintext1 plaintext1_a
$ echo $?
0
$ cmp plaintext1 plaintext1_b
plaintext1 plaintext1_b differ: byte 1, line 1
$ echo $?
1
$ enc_client plaintext5 mykey 57171
enc_client error: input contains bad characters
$ echo $?
1
$ enc_client plaintext3 mykey 57172
Error: could not contact enc_server on port 57172
$ echo $?
2
$
