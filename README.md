# Aerospike PHP Client

**The Aerospike PHP Client works on PHP 5.3**. We are working on getting the
extension to compile against PHP versions 5.4 and 5.5.

The PHP extension was tested to build on

 - Ubuntu 12.04 LTS, 14.04 LTS and related distros using the **apt-get** package manager
 - CentOS 6.x, 7.x, RedHat 6.x, 7.x and related distros using the **yum** package manager
 - Mac OS X 10.9 (Mavericks)

Windows is currently not supported.

## Documentation

Documentation of the Aerospike PHP Client may be found in the [doc directory](doc/README.md).
The API described there is the [specification](doc/aerospike.md) for the PHP Client.
Notes on the internals of the implementation are in [doc/internals.md](doc/internals.md).

Example PHP code can be found in [examples/basic_examples/](examples/basic_examples).

Full documentation of the Aerospike database is available at http://www.aerospike.com/docs/

## Dependencies

In distributions such as Ubuntu 14.04 LTS, where the package manager defaults
to versions higher than 5.3, you will need to [install PHP 5.3 manually](http://www.php.net/downloads.php).

### CentOS and RedHat (yum)

    sudo yum groupinstall "Development Tools"
    sudo yum install openssl-devel
    sudo yum install php-devel.x86_64 php-pear.noarch # unless installing manually

### Ubuntu and Debian (apt-get)

    sudo apt-get install build-essential autoconf sudo apt-get install libssl-dev
    sudo apt-get install php5-dev php-pear # unless installing manually

### Mac OS X

By default Mac OS X will be missing command line tools. On Mavericks (OS X 10.9)
and higher those [can be installed without Xcode](http://osxdaily.com/2014/02/12/install-command-line-tools-mac-os-x/).

    xcode-select --install # install the command line tools, if missing

The required utility automake can be installed through the OS X package manager
[Homebrew](http://brew.sh/).

    ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
    brew update && brew doctor
    brew install automake

If the preinstalled PHP has a version higher than 5.3 you will need to install it
from the [Homewbrew-PHP](https://github.com/Homebrew/homebrew-php) repository:

    brew tap homebrew/dupes
    brew tap homebrew/versions
    brew tap homebrew/homebrew-php
    brew install php/php53
    php -v # verify that it is PHP 5.3.x

## Build Instructions

To build the PHP extension, you must first invoke the setup script, which will
download the Aerospike C client SDK if necessary (into `aerospike-client-c/`):

    $ . scripts/setup

This will set the `CLIENTREPO_3X` environment variable, add the `scripts/` 
directory to your path, and change the working directory to `src/aerospike/`.
To grab the latest release of the C client SDK (rather than a specified
version) modify the `AEROSPIKE_C_CLIENT` variable of the setup script
to *latest*. You may need to remove the `aerospike-client-c` directory before
running the setup script once more.

Next, build the Aerospike PHP extension.

    $ build

For a debug build specify the log level (default being OFF):

    $ build [-l|--loglevel <Desired log level: one among TRACE, DEBUG, INFO, WARN, ERROR, OFF>] [--help]

The PHP extension will be built as `modules/aerospike.so`

## Confirming The Build

To test the Aerospike PHP Client interactively after building, do:

    $ php -dextension=modules/aerospike.so -a

## Installing The PHP Extension

To install the PHP extension do:

    $ sudo make install

Now edit the php.ini file.  If PHP is configured --with-config-file-scan-dir
(usually set to `/etc/php.d/`) you can create an `aerospike.ini` file in the
directory, otherwise edit php.ini directly. Add the following directive:

    extension=aerospike.so

The *aerospike* module should now be available to the PHP CLI:

    $ php -m | grep aerospike
    aerospike

## Cleanup

To clean up artifacts created by the build process you can do:

    $ build-cleanup

At this point if you want to use **make** you will need to rebuild.

## License

The Aerospike PHP Client is made availabled under the terms of
the Apache License, Version 2, as stated in the file `LICENSE`.

Individual files may be made available under their own specific license,
all compatible with Apache License, Version 2. Please see individual files for
details.


