Sawteeth
======

What?
---------
Sawteeth is a soft-synth (with tracker) for BeOS and Haiku, meant to either create cool synthsounds to use in cubase and such, or cute chip-tunes.
It uses no samples but only generated waveforms.

Originally developed by Jonas and Arvid Norberg and released in 2000, it is now open-sourced under the MIT licence.

Sawteeth generates music files that take very little space and not much CPU time to replay.
It is based around base oscillators, filters, envelopes, and clipping.
The pattern editor allows you to reuse pieces of melody easily (including transposing them).
The instruments and patterns can also be edited in real-time while the music is playing.

Sound Quality
-------------------
The sound synthesis is of high quality with little aliasing noise, 44kHz and float precision.
(some of the filters and definitely the clipping adds aliasing noise, so use them with care)
The anti alias method is "infinite oversampling with boxfilter"
Modulation rate is static for a song but can be anything between 0.673Hz and 44100Hz (lousy performance at high modrate)

Thanks
---------
I've been given all filter code by Stefan Hållén. 

Arvid Norberg is doing big parts of the editor (close to all of the part editor and the instrumenteditor) and a lot of complaining on my bad oo design of the player :) 

A big inspiration is Abyss's AHX tracker. A sawteeth song is pretty similar to a AHX song, in the future a AHX to sawteeth converter could be reality (sawteeth lacks some AHX features so it wouldn't be perfect).

Features
------------
* Retro feel, all the way. 
* All your favourite waveforms : Saw, Square, Triangle, Noise, Sinus
* Filter with many modes,
* Clipping with many modes,
* 'ADSR' for filter and amp 
* Fast synthesis

Working with Sawteeth
-------------------------------
http://www.linusakesson.net/music/elements/index.php
