# CSS Unreadabilitier

Make your CSS unreadable, by piping your CSS input into the program
and piping the unreadable stdout to a file. The whole purpose of this
program is to convert large CSS inputs into a one-liner output as fast
as possible.

The inner workings are really straight forward, remove all comments,
replace all characters that are a whitespace token with a space, leave
at most one space and don't modify any string values. That's it.

## Building

I'm assuming you have a Unix-like system with GCC and Make
installed. If so, compile the program by running:

```bash
make -k build
```

## Using the program

For now only very simple piping interactions are possible. Feed in
your CSS blob via `stdin` and catch the `stdout` output.

Example:

```bash
cat input.css | ./main | tee output.css
```

## Trashy and worthless Benchmark

* System specs:

```
CPU: AMD Ryzen 7 7700X (16) @ 5.57 GHz
OS: Fedora Linux 41 (Forty One) x86_64
Weather: Sunny
```

#### Bootstrap 5
```
$ time ./css-unreadabilitier -ibootstrap.css -oout.css
Executed in    3.95 millis    fish           external
   usr time    3.54 millis   47.00 micros    3.50 millis
   sys time    0.02 millis   23.00 micros    0.00 millis

Original size:  195,704 bytes
One-liner size: 177,219 bytes
```

#### Fomantic UI
```
$ time ./css-unreadabilitier -isemantic.css -oout.css
Executed in   30.47 millis    fish           external
   usr time   24.62 millis   25.00 micros   24.59 millis
   sys time    3.97 millis   49.00 micros    3.92 millis

Original size:  2,033,764 bytes
One-liner size: 1,868,374 bytes
```
