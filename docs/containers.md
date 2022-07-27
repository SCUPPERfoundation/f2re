Background
----------

Lenton developed Federation with the help of one or two other programmers
over the decades. Code sharing techniques have evolved significantly
since the 1980s. A persistent problem with larger codebases, is the
difficulty of *porting* the code as hardware features evolve. The trend
from Binary releases to Container releases of software comes with upsides
and downsides, and plenty of history.

At first glance, I want an easier way to build software on macOS and also
run that software on some kind of handwavy Cloud POSIX platform. One path
for this would be to modify the CMake build configuration. Tweaking a
legacy codebase can be daunting, so a more "obvious" choice in 2022 is
converting the code to run in a Container.

Container Strategy
------------------

There are so many Linux distributions. One of them will surely have an
easy path to portability. Three distros in? So many rabbit holes for such
a relatively simple C++ codebase with only three dependencies!

As of this writing, both `Containerfile.fedora` and `Containerfile.ubuntu`
produce functional builds. Based on the commit history, Fedora was Lenton's
preferred distribution for development. The container strategy gives us the
Build-Anywhere capability, but container images are still built around a CPU
architecture.

An image built from macOS arm64 will execute on a Raspberry Pi 4 running
Fedora. This image will not execute on a cloud-base x86_64 machine.

TODO: Size the rabbit hole of building multi-arch container images.

Fun Links
---------
https://github.com/containers/podman/issues/11541
