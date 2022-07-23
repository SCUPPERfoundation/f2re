Federation 2: Research Edition
==============================

Container Quick Start:
```
$ git clone git@github.com:SCUPPERfoundation/f2re.git
$ podman build -t f2re:latest -f Containerfile.fedora
$ podman run -p 30303:30303 -it -d f2re:latest
$ nc localhost 30303
```

Earlier Notes
-------------

Setup Guide for Researchers:
```
$ git clone git@github.com:SCUPPERfoundation/f2te.git
$ cd f2te
$ cmake .
$ make
```

Take the proverbial ☕ break.
It should compile if you have figured out the dependencies you need, like
we did several weeks ago when we first started testing this...

If you are on macOS, you can use `homebrew` to install BerkeleyDB with:
```
$ brew install berkeley-db@4
```

When complete, homebrew should mention something like:
```
For compilers to find berkeley-db@4 you may need to set:
  export LDFLAGS="-L/opt/homebrew/opt/berkeley-db@4/lib"
  export CPPFLAGS="-I/opt/homebrew/opt/berkeley-db@4/include"
```
You will need to update line 445 and line 449 of `CMakeLists.txt` to
use these paths. Just setting the environment variables does not seem
to work.

Now try:
```
$ ./fed2d
Cannot get password structure for 'fed2' account.
Success
Home directory is '/$HOME/f2te/log/fed2.log'

 --- Starting Thu Jan  1 00:00:00 1970 (UTC/GMT)---

 --- Starting Thu Jan  1 00:00:00 1970 (UTC/GMT)---
Unable to find /$HOME/f2te/maps

```
Then proceed to commit the readme here, while you figure out where to
download the maps.

Update: The maps are available as a [zip file](http://www.ibgames.net/fed2/workbench/sol/Sol%20Files.zip)
from the [IB Games website](http://www.ibgames.net/fed2/workbench/sol/index.html)
