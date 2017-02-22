# UnitTestingTools/Mocker

This is a simple utility for Mocking functions (and objects) in C++.

## Building the mocker

You should be able to build MockerTest by just running:
gcc MockerTest.cpp -lstdc++ -o MockerTest

## Sample Execution

MockerTest.cpp provides a sample that can be executed on most *nix-x86/x64 systems.

Since the Mocker is using x86 or x64 instructions directly, there is no way to run this on other architectures.

Here is some sample output:
```
$ ./MockerTest
Calling foo without the mock
In foo.
Calling foo with the mock.
In foo_mock.
Calling foo after deleting the mock.
In foo.
```

## Using the Mocker

To use the mocker you should be able to just include Mocker.h into the project that you want to add mocks to.
