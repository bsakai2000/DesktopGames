# Desktop Games

A collection of games attempting to emulate a WinXP-style offering.

## Games
The following games are intended to be included in this repository
 - Minesweeper (functionally complete, requires additional aesthetic work)
 - Solitaire (planned)

## Justification
The goal of this project is to create easy-to-play games that can be built and
run with as few dependencies as possible. I find that other games for Linux in
this genre are often either console-only or require dependencies like Java,
Electron, or even Wine, which on a Gentoo system adds up to a lot of unecessary
compile time and disk usage. Because I expect most everyone using Linux to have
a Firefox installation, GTK, glib, automake, and gcc seemed like safe
dependencies. Therefore these games are written in C using the GTK+-3.0
framework.

## Compilation
This project is compiled using automake. Simply run `autoreconf -i` to generate
a `configure` script, `./configure` to generate a Makefile, and finally `make`
to generate executables.

## Images
All images uploaded to this repository are licensed under the same license as
the rest of the repository. They were hand-drawn (as should be easily noticed)
by me using the [Pixilart](https://www.pixilart.com/) tool. I have reproduced
below a section of the [Pixilart terms of use](https://www.pixilart.com/terms)

>Created Artwork Ownership
>
>By submitting content from the pixel drawing application (located at
>"pixilart.com/draw") to Pixilart for inclusion on the website, you grant
>Pixilart worldwide, royalty-free, and non-exclusive license to
>reproduce, modify, adapt, and publish and advertise the content without
>prior notice. Drawings you create on Pixilart may be used for
>public/commercial use. Content may be removed at any time for
>registered/non-registered users. Any content you create using the
>drawing application has been rightfully owned and created by you and
>does not infringe in any copyright infringement laws.

## Manpages
I believe that all software should be distributed with a manpage, and I'll
strive to do the same for this repository. Because I lack the patience to learn
how to write a manpage by hand, I'll be using Asciidoc to build them from
txt files. In the spirit of minimizing dependencies, I'll try to upload
those "compiled" manpages as well. The manpage format used by AsciiDoc
is documented in a couple places, but the truth is that copying an existing
file and editing it is probably easier than reproducing or linking to that
documentation. I eventually plan to automate manpage construction according to
https://zgyarmati.de/blog/generating-man-pages-with-asciidoc-from-automake/

## How does GTK work?
This project is not just so I can play solitaire instead of doing work. I am
also using this as a way to get experience with the GTK framework, and it seems
only fair that I return some of the knowledge to the Internet. Below are some
of the tricks I picked up while creating this project. Note that this is not
meant to be a getting started guide, nor do I believe it to be entirely
accurate. Rather, it is a list of things I wish I had known before starting
this project, as it would have greatly simplified my work. See some of the
early commits to understand the kinds of hacks I was using when I started.

### Using a UI builder file
A UI Builder file is an XML file that describes the layout of your application.
The format can be seen in the [Minesweeper UI
file](Minesweeper/minesweeper.ui). This format is fairly well documented by GTK
(see [here](https://developer.gnome.org/gtk3/stable/ch01s03.html) for one
example), so I won't do much here. Of not is that nested objects require a
`child` tag, and the `property` and `style` tags correspond to the `Properties`
and `Style Properties` of a [GTK
manpage](https://developer.gnome.org/gtk3/stable/GtkWidget.html), respectively.
Builder files are activated by calling one of the `gtk_builder_new` functions,
which will import the layout.

### Using a CSS file
GTK CSS mostly overlaps with standard CSS, although it contains a few subtle
differences. I believe that `@define-color` as used in minesweeper.css is
non-standard, and that standard CSS variables using custom properties are not
available. CSS style classes can be created as in standard CSS, and are applied
either using a `class` tag in a `style` tag in a UI file, or by using the
`gtk_style_context_add_class` function. Applying CSS to a project is slightly
more complicated than a UI file, so I will direct the reader to my solution in
`load_css` from `minesweeper.c`

### Embedding files in an executable
Initially, it seemed like distributing a UI file and CSS file with the
executable might be acceptable, but as the number of external resources grew it
became obvious that that wouldn't work. I first attempted to link the resources
directly into the executable using a linker trick explained
[here](http://www.burtonini.com/blog/2007/07/13/embedding-binary-blobs-with-gcc/).
This worked, but was rather cumbersome. Eventually I realized that GResource
did exactly the thing I wanted to do, so I'll explain what I did and how it works.

GResource uses an XML file to identify dependencies, as can be seen in my
[Minesweeper project](Minesweeper/minesweeper.gresource.xml). Once this file is
created, the `glib-compile-resources` tool is used to generate a C file from
the specified resources. Finally, this C file can be compiled and linked into
the executable. From there, the GTK library has functions like
`gtk_builder_new_from_resource` that can access those resources. This is a much
easier way to include files in the executable!

## How does Automake work?
See the above GTK disclaimer. This is purely represents things I wish I had
known at the beginning of this project

### CONFIGURE.AC
A `configure.ac` file identifies to the automake system what files or
dependencies need to be present for a build to succeed. To generate a usable
`configure.ac` file, run the `autoscan` command to generate `configure.scan`,
and copy that file to `configure.ac`. `AC_CONFIG_FILES` should list all
Makefiles in your build (Don't include the .am suffix) I also had to manually
identify `gtk3+` as a dependency using the `PKG_CHECK_MODULES` directive

### MAKEFILE.AM
The top-level directory should have a `Makefile.am` that contains very few
lines. In this project, we have just a single line that lists subdirectories to
build. Each subdirectory that needs to be built should have its own
`Makefile.am` that describes what should be built in that directory. We'll talk
about the Minesweeper directory, first in the common case and then some edge
cases.

The common case is to build an executable from a set of C files. In this case,
specify `bin_PROGRAMS` as the list of programs to generate, and for each
program set `<program name>_SOURCES` as the list of source files to build
`<program name>` from. If additional `CFLAGS` or `LDLIBS` are required, use
`AM_CFLAGS` and `<program name>_LDADD` to do so. Note that for Minesweeper, we
use variables generated by `PKG_CHECK_MODULES` to specify these flags instead
of doing it ourselves.

The edge case I encountered was with `glib-compile-resources`. Apparently, the
`$(shell <command>)` instruction that I use to specify GResource dependencies
is a non-standard GNU extension to Make. I decided I don't care about
supporting non-GNU make, and so ignored the warnings. The other edge case is
the generation of the manpages, which requires a special command using Asciidoc
that Automake doesn't know about, so I have to specify the command manually as
one would with a normal Makefile.

### Building with automake
First, run ``autoreconf -i` to generate automake files including the
`./configure` script. Then run `./configure` to generate a `Makefile` from the
`Makefile.am` templates, then finally run `make` to generate the final files
from the `Makefile`

### Packaging with automake
The maintainer (me) should run `make dist` to generate a tar file containing
all required source materials as well as the `./configure` script, so end users
will not need to run `autoconf` on their own systems. This removes a build
dependency for the final user. Files that are required for distribution that
are not required by default can be specified using the `EXTRA_DIST` variable in
the `Makefile.am` that declares how to build it. This is how I package built
manpages.

## How do I write my own ebuild?
I use Gentoo, so it makes sense that I would need to write a file for
installation on my own system. Because I handle everything in the Makefile, I
actually have no need for anything other than the default functions, so the
ebuild is super short. Of particular note are the `KEYWORD`, which identifies
which platform it supports, the `DEPEND` for runtime dependencies, and
`BDEPEND` for build dependencies.
