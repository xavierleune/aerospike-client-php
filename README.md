# Aerospike PHP Client [![Build Status](https://travis-ci.org/aerospike/aerospike-client-php.svg?branch=master)](https://travis-ci.org/aerospike/aerospike-client-php)

The Aerospike PHP Client works with PHP 5.3, 5.4, 5.5, and 5.6.

The PHP extension was tested to build on 64-bit

 - Ubuntu 12.04 LTS, 14.04 LTS, Debian 6, 7 and related distros using the **apt** package manager
 - CentOS 6.x, 7.x, RedHat 6.x, 7.x and related distros using the **yum** package manager
 - Mac OS X 10.9 (Mavericks)

Windows is currently not supported.

## Documentation

Documentation of the Aerospike PHP Client may be found in the [doc directory](doc/README.md).
The API described there is the [specification](doc/aerospike.md) for the PHP Client.
Notes on the internals of the implementation are in [doc/internals.md](doc/internals.md).

[Example PHP code](examples/) can be found in the `examples/` directory.

Full documentation of the Aerospike database is available at http://www.aerospike.com/docs/

## Dependencies

### CentOS and RedHat (yum)

    sudo yum groupinstall "Development Tools"
    sudo yum install openssl-devel
    sudo yum install lua-devel # on Fedora 20+ use compat-lua-devel-5.1.5
    sudo yum install php-devel php-pear # unless PHP was manually installed

### Ubuntu and Debian (apt)

    sudo apt-get install build-essential autoconf libssl-dev liblua5.1-dev
    sudo apt-get install php5-dev php-pear # unless PHP was manually installed

### Mac OS X

By default Mac OS X will be missing command line tools. On Mavericks (OS X 10.9)
and higher those [can be installed without Xcode](http://osxdaily.com/2014/02/12/install-command-line-tools-mac-os-x/).

    xcode-select --install # install the command line tools, if missing

The dependencies can be installed through the OS X package manager [Homebrew](http://brew.sh/).

    brew update && brew doctor
    brew install automake
    brew install openssl
    brew install lua

To switch PHP versions [see this gist](https://gist.github.com/rbotzer/198a04f2315e88c75322).

## Build Instructions

To build the PHP extension run the `build.sh` script in the `src/aerospike/`
directory.

    cd src/aerospike
    ./build.sh

This will download the Aerospike C client SDK if necessary into
`aerospike-client-c/`, and set the `CLIENTREPO_3X` environment variable
for `make`.

To grab the latest release of the C client SDK (rather than the specified
version) modify the `AEROSPIKE_C_CLIENT` variable of `scripts/setup` to
*latest*. You may need to remove the `aerospike-client-c`
directory before running `build.sh` once more.

For a debug build specify the log level (default being OFF):

    ./build.sh [-l|--loglevel <Desired log level: one among TRACE, DEBUG, INFO, WARN, ERROR, OFF>] [--help]

The PHP extension will be built as `modules/aerospike.so`

## Confirming the Build

To test the Aerospike PHP Client interactively after building, do:

    php -dextension=modules/aerospike.so -a

## Installing the PHP Extension

To install the PHP extension do:

    sudo make install
    php -i | grep ".ini "

Now edit the php.ini file.  If PHP is configured --with-config-file-scan-dir
(usually set to `/etc/php.d/`) you can create an `aerospike.ini` file in the
directory, otherwise edit php.ini directly. Add the following directive:

    extension=aerospike.so

The *aerospike* module should now be available to the PHP CLI:

    php -m | grep aerospike
    aerospike

## License

The Aerospike PHP Client is made availabled under the terms of
the Apache License, Version 2, as stated in the file `LICENSE`.

Individual files may be made available under their own specific license,
all compatible with Apache License, Version 2. Please see individual files for
details.


