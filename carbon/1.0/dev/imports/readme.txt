steps to build different projects:


LogModule: 

1. Go to "\Log Module\apache-log4cxx-0.10.0\projects" and launch log4cxx.sln project file in vc8 or vc9

2. Build the project in Debug and Release mode.



Crypto++:

1. Go to "Crypto++\cryptopp561" and launch cryptest.sln in vc8 or vc9

2. Build the project cryptopp in Debug and Release mode to generate the dll file.

3. Build the project cryptlib in Debug (and not DLL-import/export Debug) and Release mode to generate the static lib file. 



Sqlite: 

1. This is the com[plete source code of sqlite. Include all .h and .c files except shell.c to get the sqlite functionalities. 


