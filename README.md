![Tremulous Banner](misc/tremulous-banner.jpg)

[Homepage](http://grangerhub.com) |
[Download](https://github.com/GrangerHub/tremulous/releases)

Tremulous is a first-person multiplayer tower defense game, pitting
technologically advanced humans against ferocious space aliens. The goal of
each team is to eliminate the members of the opposing team as well as the
structures which allow them to re-enter the arena.

# Development on Linux

### Building the game

First, install [Git](https://git-scm.org) and the
[Meson](http://mesonbuild.com/) build system.

Clone the Tremulous repository and run `git submodule update` to download the
assets:

~~~{.sh}
$ git clone https://github.com/GrangerHub/tremulous
$ cd tremulous
$ git submodule update
~~~

Then use Meson to set up the build directory. The following command will tell
the build system to create a portable Tremulous installation in the `install`
subdirectory:

~~~{.sh}
$ meson -D prefix=$PWD/install -D portable=true build
~~~

If Meson tells you that some dependencies are missing, install them and
run Meson again. When Meson completes successfully, switch to the build
directory and install Tremulous:

~~~{.sh}
$ cd build
$ ninja install
~~~

When this is done, you can run the game:

~~~{.sh}
$ ../install/tremulous
~~~

You can pass console commands via the command line as well:

~~~{.sh}
$ ../install/tremulous +connect 162.248.95.116:7341
~~~

### Running a server

If you simply want to start a test server to test the changes you've made
together with other players, simply copy `install/server.sh.template` to
`install/server.sh`, edit it as you want, open and forward the ports you chose,
and do:

~~~{.sh}
$ cd install
$ sh server.sh
~~~

For more in-depth information, see [RunningServers](docs/RunningServers.md).
