-------------------------------------------------------------------------------
                            Bee C client libraries
-------------------------------------------------------------------------------

.. image:: https://travis-ci.org/bee/bee-c.svg?branch=master
    :target: https://travis-ci.org/bee/bee-c

===========================================================
                        About
===========================================================

**Bee-c** is a client library written in C for Bee.
The current version is 1.0

Bee-c depends on `msgpuck <https://github.com/wprayudo/msgpuck>`_.

For documentation, please, visit `github pages <http://bee.github.com/wprayudo-c>`_.

It consinsts of:

* beer - bee IProto/networking library

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
       Compilation/Installation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This project using CMake for generating Makefiles:

.. code-block:: console

    $ cmake .
    $ make
    #### For testing against installed bee
    $ make test
    #### For building documentation using Doxygen
    $ make doc
    #### For installing into system (headers+libraries)
    $ make install

Or you can install it using yum/apt into your favorite linux distribution
from `bee`'s repository

===========================================================
                        Examples
===========================================================

Start bee at port 3301 using this command:

.. code-block:: lua

    box.cfg{ listen = '3301' }

After you've installed bee-c build and execute this code:

.. code-block:: c

    #include <stdlib.h>
    #include <stdio.h>

    #include <bee/bee.h>
    #include <bee/beer_net.h>
    #include <bee/beer_opt.h>

    int main() {
        const char * uri = "localhost:3301";
        struct beer_stream * beer = beer_net(NULL); // Allocating stream
        beer_set(beer, BEER_OPT_URI, uri); // Setting URI
        beer_set(beer, BEER_OPT_SEND_BUF, 0); // Disable buffering for send
        beer_set(beer, BEER_OPT_RECV_BUF, 0); // Disable buffering for recv
        beer_connect(beer); // Initialize stream and connect to Bee
        beer_ping(beer); // Send ping request
        struct beer_reply * reply = beer_reply_init(NULL); // Initialize reply
        beer->read_reply(beer, reply); // Read reply from server
        beer_reply_free(reply); // Free reply
        beer_close(beer); beer_stream_free(beer); // Close connection and free stream object
    }

For more examples, please, visit ``test/bee-tcp.c`` or ``test/bee-unix.c`` files.

For RPM/DEB packages - use instructions from http://bee.org/download.html to add repositories.
