# Running Servers

## Simple Way

If you simply want to start a test server to test the changes you've made
together with other players, follow the instructions below.

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

## In-Depth Information

Every server requires two primary directories, which are both configurable:

- The base directory is where the maps are stored. It is configurable via
  `fs_basepath` and for portable installations is the same directory where the
  Tremulous binaries are located.

- The home directory is where the server writes its logs and stores the player
  registration data. It is configurable via `fs_homepath` and by default is the
  same directory where the client stores its data (`~/.tremulous` on Linux).

The base directory directory is further divided into two sub-directories:

- The base directory (`base`) where the maps are stored. Tremulous does not
  currently support dependencies between packages, so maps have to bundle all
  data they depend on with themselves.

- The game data directory (`gpp` by default) is where the game assets is stored.
  Note that the client will always download *every* file in the game data
  directory, so do not place maps there.

The command line syntax of the server is the same as that of the client. See
`docs/server.sh.template` for the most common options.
