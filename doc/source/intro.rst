Introduction
============

Joedb is the Journal-Only Embedded Database. Its purpose is to allow crash-safe manipulation of data stored in permanent storage. Data is stored as a journal of all modifications. This way, the whole data history is remembered, and it is possible to re-create any past state of the database. It is also a way to make the system extremely simple, and fast.

Joedb comes with a compiler that takes a database schema as input, and produces C++ code. The generated C++ data-manipulation code is convenient to use, efficient, and type-safe.


Pros and Cons
-------------

Joedb offers many nice features that may make it more attractive than typical alternatives such as protobuf, SQLite, XML, or JSON:

- Since a joedb file is append-only, its crash-safe operation does not require flushing data to disk as frequently as typical relational databases, which can make it an order of magnitude faster (see :doc:`checkpoints` for details and benchmarks).
- The whole data history is remembered. So, no old data can ever be lost. It is also possible to add time stamps and comments to the journal, and use it as a log of the application (if the history has to be forgotten for privacy or disk-space reasons, it is also possible to pack it).
- If the database schema of an application changes over time, joedb can upgrade old files to the new version automatically. The upgrade includes changes to the schema as well as custom data manipulation (see :doc:`schema_upgrade`).
- The database schema is compiled into C++ code that allows convenient type-safe data manipulation. Many errors that would be detected at run time with SQL, XML, or JSON will be detected at compile time instead.
- Joedb is very simple, light, and fast.

Joedb currently has some limitations that may be removed with future improvements:

- The database is stored in memory. So it must be small enough to fit in RAM, and the full journal has to be replayed from scratch when opening a file. This may change with support of on-disk data storage.
- Only one process can open the database at the same time. This may change with a database server that would handle connections of many clients.
- C++ is the only supported programming language. A rudimentary C wrapper is available. SQL support is planned.

Compared to history-less databases, joedb has one fundamental drawback: frequently-updated values may make the joedb journal file grow very large.

So joedb might not be the best choice for every situation, but it is great if data fits in RAM, has to be stored safely on disk, and is manipulated by C++ code.

An Example
----------

The database schema is described in a file like this one:

.. literalinclude:: ./tutorial/tutorial.joedbi
   :language: none

Compiler instructions are in a separate file:

.. literalinclude:: ./tutorial/tutorial.joedbc
   :language: none

Running the compiler with these files will generate tutorial.h and tutorial.cpp, two files that can be used to manipulate data conveniently in C++:

.. literalinclude:: ./tutorial/tutorial_main.cpp
   :language: c++

Running this tutorial will produce this output:

.. literalinclude:: ./tutorial/tutorial.out
   :language: none

All the data was stored in the tutorial.joedb file. The database file is a binary file, so it is not convenient to inspect it directly. The joedb_logdump tool will produce a readable log:

.. literalinclude:: ./tutorial/logdump.out
   :language: none
