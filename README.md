#What is this project ?#

This project is my bachelor's thesis and it deals with rendering synthetic holograms.
It takes an input in form of an image or an XML description of scene composed of
simple geometric shapes. The program uses a simple diffraction integral evaluation
algorithm. That is for each point on optical field it computes a sum of complex
amplitudes from all point sources.
The program is capable of generating holograms of only very simple scenes. That
is images will work and simple 3D wireframes might also work, but do not expect
any complicated stuff. These restrictions are given mainly due to the algorithm
itself.

#What are the dependancies ?#

The program exploits GPU to boost its rendering process, therefore you need
a computer equipped with graphics card supporting OpenCL (basically any newer card
will do).
The program only generates the optical field (a matrix of complex numbers, that
represents the state of light at each point on hologram plate), which however
needs yet to be postprocessed to become an image, which can then be printed and
reconstructed with laser.
This postprocessing is performed via HoloToolkit, which can be found at
[http://www.kiv.zcu.cz/en/research/downloads/product-detail-en.html?produkt%20id=31]
(http://www.kiv.zcu.cz/en/research/downloads/product-detail-en.html?produkt%20id=31).

#Known issues#

The program is quite crude and for an uninitiated individual it will be probably
quite frustrating to get it working. I plan to improve this and also to provide
a GUI shell to all of the command line tools (the current one is ... eh, how to
say it - quite cumbersome).
Also I would like to improve the current algorithm and implement new and more
efficient ones too.
So there is a plenty of work to be done and this SW can still be improved greatly,
but hopefully it can be usefull to someone as there are still not many programs
that deal with holograms.

#Links#
In the following three sections I will put some interesting links, that I have
gathered while working on this project.

#Links to code samples (projects)#

A Holography toolkit developed by University of West Bohemia. I think this is the
best source to start experimenting with CGH. It contains utilities for numerical
reconstruction of holograms, utilities for optical field postprocessing and even
programs to generate synthetic holograms (like my program does). This is where
I have drawn most of my inspiration from. The disadvantage might be that the codebase
is not so tiny and some utilities are quite difficult to get working:

 - [http://www.kiv.zcu.cz/en/research/downloads/product-detail-en.html?produkt%20id=31]
   (http://www.kiv.zcu.cz/en/research/downloads/product-detail-en.html?produkt%20id=31)

This program uses the same algorithm as I do and it is written in Java. The speed
is terrible, but the code is pretty easy to read (even for a non-Java programmer
like me):

 - [http://www.corticalcafe.com/prog_CGHmaker.htm](http://www.corticalcafe.com/prog_CGHmaker.htm)
 - [http://sourceforge.net/projects/cghconstruction/](http://sourceforge.net/projects/cghconstruction/)

This project claims to be doing numerical reconstruction of computed holograms.
I have not tried it, because I have used tools from HoloToolkit, but maybe it
will be of some use to you: 

 - http://sourceforge.net/projects/cudaholo/?source=recommended

#Links to learning material#

This doctoral thesis deals directly with CGH and covers various methods of generating
holograms with computer. It also discusses some theoretical basics of holography,
but if you are completely new to this field and to optics, then you are better
of finding some other materials: 

 - [http://holo.zcu.cz/txt/TR-2007-02.pdf](http://holo.zcu.cz/txt/TR-2007-02.pdf)

If you can read Slovak, then you can have a look at my bachelor's thesis:

 - [http://www.fit.vutbr.cz/study/DP/BP.php?id=10367&y=2012](http://www.fit.vutbr.cz/study/DP/BP.php?id=10367&y=2012)
                                                                                  
Again, if you read Slovak, I found this to be a very easy to understand source:

 - [http://www.docstoc.com/docs/29063563/DO-NOT-BE-AFRAID-OF-WAVE-OPTICS_-HOLOGRAPHY-IS-WORTH-THAT]
   (http://www.docstoc.com/docs/29063563/DO-NOT-BE-AFRAID-OF-WAVE-OPTICS_-HOLOGRAPHY-IS-WORTH-THAT)

and this (in czech):

 - [http://fyzika.jreichl.com/main.article/view/431-optika]
   (http://fyzika.jreichl.com/main.article/view/431-optika)

Slides from See Real presentation, they won't really enlighten you without further
explanation, but it might be interesting to have a look at:

 - [http://www.pcgameshardware.de/Grafikkarten-Grafikkarte-97980/News/GTC-2012-Hologramm-Berechnung-auf-der-GPU-884417/]
   (http://www.pcgameshardware.de/Grafikkarten-Grafikkarte-97980/News/GTC-2012-Hologramm-Berechnung-auf-der-GPU-884417/)

#Links to some fun stuff about holography#

This is how amazing computer generated holography can be, if you know what you
are doing:

 - [http://www.youtube.com/watch?v=DdOveIue3sc](http://www.youtube.com/watch?v=DdOveIue3sc)

Even better are the optical holograms:

 - [http://www.youtube.com/watch?v=Xp7BP00LuA4](http://www.youtube.com/watch?v=Xp7BP00LuA4)
 - [http://www.youtube.com/watch?v=_9QR3qaK_Cs](http://www.youtube.com/watch?v=_9QR3qaK_Cs)
 - [http://www.zebraimaging.com/](http://www.zebraimaging.com/)
 - [https://www.youtube.com/watch?v=_Ss0ttMRGg8](https://www.youtube.com/watch?v=_Ss0ttMRGg8)
 - [http://www.holoworld.com/](http://www.holoworld.com/)

And some even try to run holography in real time (of course by employing certain tricks):

 - [http://www.youtube.com/watch?feature=player_embedded&v=ttHJ3mmvZEw]
   (http://www.youtube.com/watch?feature=player_embedded&v=ttHJ3mmvZEw)
 - [http://www.seereal.com/]
   (http://www.seereal.com/)
   
The video from the optical reconstruction of my own hologram:

 - [https://www.youtube.com/watch?v=Re91mA0Q4sU](https://www.youtube.com/watch?v=Re91mA0Q4sU)