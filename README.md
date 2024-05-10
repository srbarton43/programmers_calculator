# Programmer's Calculator

## The Vision

A quick and simple CLI calculator for bit operations, inspired by lots of bit operations in my computer architecture course.
The biggest feature which I want to add which I have not found in other programming calculators is the ability for specifying different word-sizes: ie, 16-bit vs 32-bit vs 64-bit.
Right now, there is the option from 4-bit to 64-bit wordsizes but 1048 bit wordsizes are planned.

## Calculator Features

- Arithmetic (`+`, `-`)
- Parentheses
- Bitwise Operations (`&`, `|`, `~`)
- Logical Shifts (`>>`, `<<`)
- Variable Assignment (`a=3+4`)
- Changing Wordsize (`ws [wordsize]`)
- Printing Wordsize (`ws`)

## Building

You must have the [meson build system](https://mesonbuild.com/) installed and the dependencies `flex` and `bison`.

After installing the dependencies run the command:

```sh
meson setup [build_dir] && cd [build_dir] && ninja
```

Then run the program using:

```sh
./pcalc
```

## Testing

Run the unittest suite from the build directory using:

```sh
meson test
```

Currently there are no integration testing but that may change in the future.

## Internals

This project uses `bison` and an LR grammar for parsing calculator expressions, and uses `flex` for the tokenization.
They are both dependencies for building the project.
Each number is represented as a u64 number and its wordsize.
The wordsize is vital for all operations involving two's complement, and for tracking overflows/invalid numbers.
Note that while two numbers can have the same value, if they have _different_ wordsizes, they are _different_ numbers.

## Future Plans

- [ ] better grammar
- [ ] add integration tests
- [x] better error messages (especially regarding overflows and OOB)
- [x] history + use of arrow keys to navigate
- [ ] rotate operations
- [ ] 1024-bit wordsize (by storing array of u64)
- [ ] RIIR?
