#!/bin/bash


holo_toolkit_path="../../../../Holography_Toolkit/bin"


# create a hologram from diffraction pattern
echo "Creating a hologram from diffraction patern ..."
"${holo_toolkit_path}/DFtoHologram" "-c./DFtoHologram.xml"
echo


# verify the created hologram
echo "Propagating the diffraction pattern to verify results ..."
"${holo_toolkit_path}/HoloPropagLarge" "-c./HoloPropagLarge.xml"
echo


# display the resulting images
xdg-open out.png
xdg-open out.Hologram.png


echo "DONE"
