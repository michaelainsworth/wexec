# Overview

**The primary purpose of `wexec` is to reduce (or completely remove)
compilation times.**

`wexec` reads and ignores all data arriving on standard input, and,
after a period of inactivity on standard input, executes a particular
command.

Example:

    echo test | wexec -s 1 -n 500000000 -c "echo hi"

If lines are supplied on standard input, followed by 1.5 seconds of
nothing arriving on standard input, "hi" will be displayed on the
screen.

# What the hell is it for?

I built this in about 20 minutes for a single purpose: to automatically
build software. Here is how I use it.

I use the bundled script called `wmake`, which uses the `inotifywait`
command to recursively monitor the file system. The `inotifywait`
command will report on changes to the file system in real time, but
quite often many files are changed at the same time (e.g., the editor
`vim` *replaces* files rather than rewriting them, causing multiple
inotify events), and it's not economically to build a project multiple
times in the space of second.

So I run this in one terminal:

    wmake

And I edit code in another terminal.

    vim thing.c

As soon as I type ":w" into `vim`, my entire project is rebuilt.
