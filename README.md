# Audio pipeline

The audio pipeline is responsible for producing spatialized audio for XR experience.

# Components

## libspatialaudio

Submodule libspatialaudio provides the backend library for Ambisonics encoding, decoding, rotation, zoom, and binauralizer(HRTF included).

## audio pipeline code

### sound.cpp 

Describes a sound source in the ambisonics sound-field, including the input file for the sound source and its position in the sound-field.

### audio.cpp

Encapsulate preset processing steps of sound source reading, encoding, rotating, zooming, and decoding.

# Installation

Install libspatialaudio

    cmake CMakeLists.txt
    cmake -DCMAKE_INSTALL_PREFIX=/path-to-installation-directory
    make && make install

Add libspatialaudio in library path

    export CPATH=$INCLUDE_PATH:/path-to-installation-directory/include
    export LIBRARY_PATH=$LIBRARY_PATH:/path-to-installation-directory/lib
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path-to-installation-directory/lib

Make audio pipeline files
    
    make
