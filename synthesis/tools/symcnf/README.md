Convert symbolic CNF files into DIMACS CNF files
================================================

Run `make` to compile.

To convert a symbolic CNF file run:

    ./symconvert FILE > output.cnf
 
This will create a map file `FILE.map` which gives the mapping between
symbolic variable names and DIMACS CNF variable names.

Symbolic CNF file format:

  - Lines starting with `#` are considered comments and are ignored.

  - Every other line defines a clause. 

  - A clause consists of a list of symbolic literals separated by a
    single space.

  - A symbolic variable name is any string that does not start with
    the characters `#` or `-`

  - A symbolic literal may start with the character `-` followed by a
    symbolic variable name.

An example:

    $ cat example.scnf
    # this is an example
    foo -bar 
    -bar baz 
    # another comment appears here
    -baz
    # define some more variables
    str_a str_b str_c 
    # the output file should have 
    # - 4 clauses
    # - 6 variables

    $ ./symconvert example.scnf
    p cnf 6 4
    3 -1 0
    -1 2 0
    -2 0
    4 5 6 0
