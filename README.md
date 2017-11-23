![Tremulous Banner](misc/tremulous-banner.jpg)

[Homepage](http://grangerhub.com) |
[Download](https://github.com/GrangerHub/tremulous/releases)

Tremulous is a first-person multiplayer tower defense game, pitting
technologically advanced humans against ferocious space aliens. The goal of
each team is to eliminate the members of the opposing team as well as the
structures which allow them to re-enter the arena.

# Development on Linux

### Building the game

First, install [Git](https://git-scm.org) and the [CMake](http://cmake.org/)
build system.

Clone the Tremulous repository and run `setup.sh` to initialize the build
system:

~~~{.sh}
$ git clone https://github.com/GrangerHub/tremulous
$ sh setup.sh
~~~

If CMake tells you that some dependencies are missing, install them and re-run
`setup.sh`. After it finishes successfully, you can build and install Tremulous.

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
together with other players, follow the instructions below. For more in-depth
information, see [RunningServers](docs/RunningServers.md).

First, you need to copy `install/server.sh.template` to `install/server.sh` and
edit it as needed. After that, you need to open the ports you specified in your
firewall configuration and forward them in your router configuration.

Put all maps you want the server to be able to load into the `install/base`
directory, and all additional game assets that you want the client to always
download (this includes any new sounds, texture, player or buildable models your
game modification might need) into the `install/gpp` directory.

To run the server, simply do:

~~~{.sh}
$ cd build
$ ninja install
$ sh ../install/server.sh
~~~
