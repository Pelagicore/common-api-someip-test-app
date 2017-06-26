
# DEPRECATED
This repo is kept for reference only, there is no further development planned.

Introduction
------------

This package contains 2 CommonAPI based test applications:
  * An application implementing a service
  * An client application using a proxy to access the service. That client application cyclically sends some requests to the service and listens to notifications from that service.

Thanks to the distributed nature of Some/IP, those 2 applications don't have to run on the same machine.

Build
-----

After getting into the folder containing the clone of the repository, you can build the package by executing the following commands:

  $ mkdir build
  $ cd build
  $ cmake ..
  $ make install


Starting the examples
---------------------

You first need to start the SomeIP dispacher using the following command (the "-c" parameter ensures that our service is not going to be automatically activated by the dispatcher since we want to activate it manually) :
  $ someip_dispatcher -c /tmp &

Then you can start the service implementation with the following command:
  $ common-api-test-service &

The service should now be running, which means any local or remote application could access it.

There are now 2 ways to deploy the client application:
  * On the same machine as the service implementation. In that case, a pure Inter-Process-Communication (IPC) is used.
  * On another machine. In that case, the TCP/IP protocol is used.

To start the test client application the same machine, simply execute the following command:
  $ common-api-test-client &

If you want to start the client application on another machine, you need to have a SomeIP dispatcher running on that machine, before you can actually start the client application:
  $ someip_dispatcher -c /tmp &
  $ common-api-test-client &
