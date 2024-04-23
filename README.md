# Programmer's Calculator

## The Vision

A quick and simple CLI calculator for bit operations, inspired by lots of bit operations in my computer architecture course.
The biggest feature which I want to add which I have not found in other programming calculators is the ability for specifying different word-sizes: ie, 16-bit vs 32-bit vs 64-bit.
Right now, there is the option from 4-bit to 64-bit wordsizes but 1048 bit wordsizes are planned.

## Calculator Features

- Arithmetic ('+', '-')
- Parentheses
- Bitwise Operations ('&', '|', '~')
- Logical Shifts ('>>', '<<')
- Variable Assignment (`a=3+4`)
- Changing Wordsize (`ws [wordsize]`)

## Building

You must have the [meson build system](https://mesonbuild.com/) installed and the dependencies `flex` and `bison`.
After installing run the command:

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

This project uses `bison` and an LR grammar for parsing calulator expressions, and uses `flex` for the tokenization.
They are both dependencies for building the project.
Numbers are represented as `char` arrays of '1's and '0's. 
The program stores a hashtable of recently-accessed numbers keyed by their bitstrings.
Note that while two numbers can have the same value, if they have **different** wordsizes, they are **different** numbers.

## Future Plans

[] better grammar
[] better error messages
[] rotate operations
[] 1024-bit wordsize
[] RIIR

