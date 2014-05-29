# Aerospike PHP Client API Zend Engine Extension

This directory contains the source code for the Aerospike PHP Client API
Zend engine extension.

## Prerequisites

To build the Aerospike PHP Client API extension, the PHP and PHP
development packages must be installed.

The Aerospike C Client SDK must also be installed.

Set the `CLIENTREPO_3X` environment variable to the filesystem location
of the Aerospike C Client Git repository clone:

	$ export CLIENTREPO_3X=<LocalRepoClone>

The Lua 5.1 language packages (`lua`, `lua-devel`, and `lua-static`)
must also be installed.

## Build Instructions

Process the `config.m4` file to generate the configuration scripts:

	$ phpize

Configure the extension and create the `Makefile`:

	$ ./configure "CFLAGS=-g -O3" --enable-aerospike

Build the `modules/aerospike.so` Zend extension shared library:

	$ make-it

or, equivalently:

	$ make clean all EXTRA_INCLUDES+=-I$CLIENTREPO_3X EXTRA_LDFLAGS="-L$CLIENTREPO_3X/target/Linux-x86_64/lib -laerospike -llua -lrt"

## Installation Instructions

Add the following lines to `/etc/php.d/aerospike.ini` (or else to your
own `php.ini` file to be used via the `--php_ini` command-line option to
`php`):

	; Enable aerospike extension module
	enable_dl=On
	extension=modules/aerospike.so

To install the Aerospike PHP Client API extension shared library, do the
following:

	$ sudo cp -p modules/aerospike.so `php-config --extension-dir`

[_Note:_  On CentOS, the PHP extension directory is usually
`/usr/lib64/php/modules/`, while on Debian / Ubuntu, it is usually
`/usr/lib/php5/extensions/`.]
