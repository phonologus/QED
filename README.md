# QED for Unix, UTF-8/Unicode aware.

This is a port of the University of Toronto Unix version of the QED editor,
which was originally written by Rob Pike, David Tilbrook, Hugh
Redelmeier and Tom Duff. I have updated it to work with UTF-8 and Unicode.

The behaviour of this `qed` when displaying Unicode is entirely dependent on the
capabilities of the terminal it is running under. In particular, `qed` 
outputs Unicode in Logical Order, leaving the handling of the
complexities of rendering bi-directional text and wide-characters on-screen
to the terminal. On macOS, the `Terminal.app` program is quite successful with
displaying bi-directional text, whereas `xterm` on Linux sticks to
Logical Order.

This port was developed on a modern Linux (Fedora 32), and macOS. It
depends on just a few low-level Unix system calls, so should be reasonably
portable across any Unix-like OS. I have built this `qed` successfully on:

+  Linux (Fedora 32, `gcc`)
+  macOS Intel (Catalina 10.15 with Commandline Developer Tools, `clang`)
+  macOS Apple Silicon (Big Sur 11.2 with Commandline Developer Tools, `clang`)
+  OpenBSD (6.7, both `gcc` and `clang`)

The sources that this port began with were scavenged from Arnold Robbins's
[qed-archive](https://github.com/arnoldrobbins/qed-archive). Specifically
they are from the
[`unix-1992`](https://github.com/arnoldrobbins/qed-archive/unix-1992)
sub-directory, which is from a tarball
of Rob Pike's original sources of the University of Toronto version of QED.

The updated [Qed tutorial](doc/qed-tutorial.pdf) 
and original [manpage](doc/qed.1.pdf)
in the `doc` subdirectory are essential reading.

The tutorial is a heavily edited an updated version of Rob Pike's
original [tutorial](doc/historical/qed-tutorial.pdf) which was
already very out-of-date, even before this port began. Rob's
original tutorial was written in _troff_ using the manpage macros.
The updated tutorial has been translated into `asciidoc`, which
makes for a very pretty html and pdf runoff, as well as a traditional
manpage format.

A brand new tutorial for a modern audience would be
a nice thing to have. Conspicuously missing from the tutorial
here are: introduction to line-oriented editing;
browsing buffers; Unicode input; the `xform` edit mode;
up-to-date examples (`qed` programs to streamline a `git`
work-flow, for instance). I will be writing a new _Qed Book_
from scratch out-of-tree, so the updated tutorial here will
not receive any more revisions.

Unlike the original tutorial, the original manpage is up-to-date,
except for the new Unicode functionality.
This new functionality is described below.

The `Makefile` provides a simple way to build a `qed` binary.
`make clean && make` will build a fully stand-alone binary `qed`, which
can be installed anywhere. `make install` will install the binary,
the manpage, and the tutorial. The tutorial is installed as
`qed-tutorial(7)`, and can be summoned with `man qed-tutorial`, or
`man 7 qed-tutorial`.

Edit the `Makefile` to change where this all goes. 

To regenerate the `html`, `pdf`, and manpage versions of the tutorial,
`cd doc/src && make && make release`.
This depends on an `asciidoctor` and `asciidoctor-pdf`
toolchain being installed.

The `doc/historical/read.me` file is Rob Pike's, accompanying his original tarball. It
provides an interesting snapshot into the work-in-progress at the time, and the
relationship between the QED and ED editors.

The `q` subdirectory contains Rob's library of useful QED programs. Again
a master-class in themselves, and they can be fruitfully studied after working
through the tutorial.

## Differences introduced in this port

+ This `qed` consumes and outputs UTF-8 Unicode. Internally, all
  "characters" are stored and manipulated as Unicode code-points
  represented as `int`-s. The full Unicode range is supported,
  thus even _emojis_ and other exotica beyond the Basic
  Multilingual Plain can appear,
  even in regexes. 

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

* QED's `xform` command (`x`, described in the manpage),
  allows a novel form of character-by-character line
  editing. It is very impressive with single-width, left-to-right,
  text. Any other text is likely to have very unpredictable,
  terminal-dependent, and unintended results.

