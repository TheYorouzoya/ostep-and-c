# Wish:  A Simple Unix Shell

This project was built as part of the OSTEP course. The specifications could be found in [SPECIFICATIONS.md](./SPECIFICATIONS.md)

To run the shell, simply type

``` shell
    make wish
    ./wish
```

or

``` shell
    ./wish [some_input_file.txt]
```

The project is written in C, so make sure to have the latest version of `gcc` and `make` installed.

Tests for the project are included inside the `tests/` folder. To run the test suite, run

``` shell
    ./test-wish.sh
```

Make sure the executable exists before running the test suite.

## Features

### Modes

The shell can be run in two maodes : *interactive mode* and *bach mode*. Running the shell without an arguments launches it in *interactive mode*. Here, the shell simply wait for input and executes any commands typed into it. *Batch mode* expects a single file from the user and executes each line in the file as input.

### Paths

By default, the shell contains `/bin` as a path. The shell will search this directory for executables whenever an external command is provided. To add a path simply type `path` followed by zero or more directory paths. If a path is prefixed with a `/`, the shell treats this as an absolute path, otherise, the path will be treated as relative and a `./` will be prepended to the supplied path.

### Built-in Commands

The shell provides 3 built-in commands:
* `exit`: Exits the shell. Accepts no arguments. Any supplied arguments are treated as an error.

* `cd`: Changes the shell's current working directory to the provided path. Always takes one argument.

* `path`: Overwrites the current value of the `path` variable with the provided arguments. Takes 0 or more arguments, with each argument separated by whitespace. An example usage would be, `wish> path /bin /usr/bin`, which would add `/bin` and `/usr/bin` to the search path of the shell.

### Redirection

The output for any *external* command can be redirected to another file. In addition, the standard error ouput will also be redirected to the output file. An example usage would be `ls -al > output`. This would redirect the output of `ls` to the provided `output` file.

If the `output` file already exists, it would be truncated and overwritten.

### Parallel Commands

The shell allows commands to be executed in parallel using the `&` as a separator for commands. An example usage could be

``` shell
wish> cmd1 & cmd2 args1 args2 & cmd3 args1 > output.txt
```

This would execute all commands -- `cmd1`, `cmd2`, and `cmd3` -- in parallel.

### Program Errors

Unfortunately, the shell lacks any descriptive feedback in the form of error messages. It always prints the same error message: **An error as occured**. This decision was not made by choice, it was dictated by the project specifications as the tester requires it to be setup this way.
