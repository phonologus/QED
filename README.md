# QED for Unix, UTF8/Unicode aware.

This is a port of the University of Toronto Unix version of the QED editor,
which was originally written by Rob Pike, David Tilbrook, Hugh
Redelmeier and Tom Duff. I have updated it to work with UTF8 and Unicode.

The behaviour of this `qed` when displaying Unicode is entirely dependent on the
capabilities of the terminal it is running under. In particular, `qed` 
outputs Unicode in Logical Order, leaving the handling of the
complexities of rendering bi-directional text and wide-characters on-screen
to the terminal. On macOS, the `Terminal.app` program is quite successful with
displaying bi-directional text, whereas `xterm` on Linux sticks to
Logical Order.

This port was developed on a modern Linux (Fedora 32) and macOS. It
depends on just a few low-level Unix system calls, so should be reasonably
portable across any Unix-like OS. I have built this `qed` successfully on:

+  Linux (Fedora 32, `gcc`)
+  macOS Intel (Catalina 10.15 with Commandline Developer Tools, `clang`)
+  macOS Apple Silicon (Big Sur 11.2 with Commandline Developer Tools, `clang`)
+  OpenBSD (6.7, both `gcc` and `clang`)

The sources that this port began with were scavenged from Arnold Robbins's
[qed-archive](https://github.com/arnoldrobbins/qed-archive). Specifically
they are from the `unix-1992` sub-directory, which is from a tarball
of Rob Pike's original sources of the University of Toronto version of QED.

The manpage and tutorial in the `doc` subdirectory are essential (and fascinating) reading.
Recently generated `pdf` run-offs of the manpage and the tutorial are
there, too.
It seems sacrilegious to make any changes to the manpage and tutorial,
so I have left them as is. Where the behaviour of this `qed` differs
significantly from the manpage is noted below.

Rob Pike's sources had already evolved beyond the QED
described in the manpage and tutorial. I have noted below where this is the case.

The tutorial is in manpage format, so can be read with `man` and
typeset with `man -t`, as any other manpage. If you `make install`,
you should be able to read the tutorial with `man qed-tutorial`.

The `Makefile` provides a simple way to build a `qed` binary.
`make clean && make` will build a fully stand-alone binary `qed`, which
can be installed anywhere. `make install` will install the binary and
the manpage and tutorial. Edit the `Makefile` to change where this all goes.

The `doc/read.me` file is Rob Pike's, accompanying his original tarball. It
provides an interesting snapshot into the work-in-progress at the time, and the
relationship between the QED and ED editors.

The `q` subdirectory contains Rob's library of useful QED programs. Again
a tutorial in themselves. The manpage builder is particularly cute,
whereas the parenthesis-counter is a _tour-de-force_.

## Differences introduced in this port

+ This `qed` consumes and outputs UTF8 Unicode. Internally, all
  "characters" are stored and manipulated as Unicode code-points
  represented as `int`-s. This means that even
  _emojis_ can appear in regexes. Shudder.

+ In *list mode*, this `qed` displays non-printing codepoints in the
  ASCII range as `\xhh`. Additionally, it displays non-ASCII
  code-points in the Basic Multilingual Plane as `\uhhhh`, and
  code-points beyond the BMP as `\Uhhhhhh`.

+ This `qed` adds the special characters `\x`, `\u`, `\U` which
  can be used to enter (valid) Unicode codepoints directly, using 
  (lowercase) hexadecimal. For example `\x53`, will insert
  ASCII `0x53` (`S`); `\u05d0` will insert Hebrew Aleph (`א`); and
  `\U01f600` will insert a grinning _emoji_ (`😀`). The format
  corresponds to the new *list mode* format described above.
  If the wrong number of digits is supplied, or the resulting
  codepoint falls outside the legitimate range, this `qed` generates
  a `?U` error.

+ The program `qedbufs` referred to in the manpage is not ported,
  and likely won't be.

+ When reading in a file which does not terminate in a newline, this
  `qed` supplies the newline, and notifies the user with the message
  `?N: appended`. This matches the Research Unix Version 10 `ed` and
  the Plan9 `ed` behaviour. Previously
  QED would throw an `?N` error, and discard any input from the last
  newline to the end of file.

## Differences already in the original sources

+ The QED in the tutorial accepts `*` as an alias for the address
  range `1,$`. This `qed` does not, but the single character `,` as
  an address stands for `1,$`, as described in the manpage.

+ When reading in files given on the commandline, this `qed` reports
  the buffer name, `dol` (_i.e._ line count), and filename of each. The
  QED in the tutorial and manpage simply reports a character count in
  this case.

+ The Tutorial does not mention that strings with embedded newlines
  can be assigned to registers by escaping a literal newline with
  a backslash. On page 11 of the Tutorial, the effect of the command
  `za:line1\cNline2` can also be achieved by typing:

  			za:line1\
			line2

  This facility is correctly described in the manpage.

+ The syntax of arithmetic operators on registers is correctly described
  in the `manpage`. The description on page 12 of the Tutorial
  is incorrect. In particular, multiple operators chained together should _not_
  be prefixed with `#`: the command `za#:4#*-5#p` in the
  Tutorial generates an error in this `qed`. The correct syntax is:
  `za#:4*-5p`
