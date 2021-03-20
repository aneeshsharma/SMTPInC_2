# SMPTP Server
SMTP server and client application implemented in C.

The project consists of 2 applications - a server and a client. The communication between the client and the server happens using the SMTP protocol. The server can be started and will listen to any PORT provided as a command line argument. Similarly client can be run providing the PORT as a command line argument.

## Prerequisites
To run the application, you will need to compile the source code. The packages that are required are
- `gcc` - The GNU compiler collection
- `make` - The make utility
- A Linux (Unix) system

## Usage

To run the applications, you first need to clone this repository and change the current working directory to the cloned repo.

```
$ git clone https://github.com/aneeshsharma/SMTPInC
$ cd SMTPInC/
```

Then, compile the program by running `make` or by compiling using `gcc`

```
$ make
```
OR
```
$ gcc server.c -o server.o -lpthread
$ gcc client.c -o client.o
```
> NOTE: The `-lpthread` flag is passed as the server uses the POSIX thread library

Now, first run the server specifying the PORT as command line argument (not specifying the PORT will use 5000 as the default port).

```
$ ./server.o 3000
```

Next, run the client and connect to the same PORT
```
$ ./client.o 3000
```

A login screen would appear on the client application. On entering authenticating with a correct credentials, the application shows a menu with the options available.
> The credentials are stored in the [`logincred.txt`](#login-credentials-file) file

---
```
$ ./server.o --help
Usage: ./server.o PORT
PORT is the port number on which the SMTP server should start
```

```
$ ./client.o --help
Usage: ./client.o PORT
PORT is the port number to use to connect to the SMTP server
```

## Login Credentials File

The credentials of the user accounts are stored inside the `logincred.txt` file in the current working directory of the server application.

They are stored in the following format.

```
user1,password1
user2,password2
.
.
.
userN,passwordN
```
> Currently there is no way to add users other than editing this file directly