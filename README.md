# CSS Unreadabilitier

Make your CSS unreadable, by piping your CSS input into the program
and piping the unreadable stdout to a file. The whole purpose of this
program is to convert large CSS inputs into a one-liner output as fast
as possible.

## Building

I'm assuming you have a Unix-like system with GCC and Make
installed. If so, compile the program by running:

```bash
$ make -k main
```

## Using the program

For now only very simple piping interactions are possible. Feed in
your CSS blob via `stdin` and catch the `stdout` output.

Example:

```bash
cat input.css | ./main | tee output.css
```
