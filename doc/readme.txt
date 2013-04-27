This readme contains brief information on how to setup and
generate doxygen documentation. It servers as a refresher as
this kind of information gets quickly forgotten.

A)
The steps to generate Doxygen documentation together with diagrams (command line):

1) Run doxygen like this to create Doxyfile:

    doxygen -g

2) Now edit these options in generated doxy file:

    PROJECT_NAME      ... set to the actula project name (e.g holoren)
    PROJECT_NUMBER    ... this is the version number (e.g 1.0)
    PROJECT_BRIEF     ... this is a short project description
    OUTPUT_DIRECTORY  ... this is the directory where the generated documentation will be saved
                          When this path is not absolute, it is take relative to the current
                          doxygen's working directory. So when you run doxygen for example from
                          `/home/me/Documents`, then doxygen will generate directories
                          with documentation in `/home/me/Documents`.

    INPUT             ... a directory with source files (e.g. ../src)

3) To have graphs generated install dot. In Ubuntu:

    sudo apt-get install graphviz

  And alter these options like this:
    EXTRACT_ALL            = YES
    HAVE_DOT               = YES
    UML_LOOK               = YES

4) To view the generated html documentation, go to `html` directory and open the `index.html` file.
   To view the latex documentation, go to `latex` directory, run `make` and open the `refman.pdf` file.

B)
The step to generate Doxygen documentation using doxygen gui:

1) Install doxygen-gui. In Ubuntu:

    sudo apt-get install doxygen-gui

2) Run doxygen gui:

    doxywizard
