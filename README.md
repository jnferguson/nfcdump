# proxmark3-modifications
Rewrite of some of the firemware and entirely new userspace iso1443a toolchain.

Currently not totally functional and moderately to not at all useful. Modifications to the proxmark firmware add a couple of new modes of operation, primarily included is functionality to:

- Provide ISO-1443a PCD functionality with full driving and dump capability over USB; modulated and unmodulated signal dumps available
- By "over USB", I am referencing a still being hashed out file format I called ncap
- Provide ISO-1443a sniffer that dumps overheard transactions over USB (id est to your phone)
- Memory corruption fix in anti-collision sequences
- Basic intention is to better support security research for NXP et al chips that are more recent; providing full network stack fuzzing functionality, et cetera
- Earnestly, the entirety of the proxmark3 firmware needs to be dumped and rewritten; pending my learning more about FPGAs

User-space currently:
- ONLY REQUIRES LIBUSB not wonky linux modem drivers (this layer needs to be rewritten)
- Provides fully drivable, active and passive ISO-1443a support
- In process of providing 7816 layer support
- In process of providing NXP propietary extensions support
- Idea was to write code in a portable manner that would support more than just the proxmark and provide a generic fuzzing framework for NFC devices (Your phone is the LEAST important thing that uses this technology); due to offline issues, code quality suffered and needs to be refactored to better meet this goal

Project is currently stalled due to real life SNAFUs; currently only usable to sniff real transactions and dump the full packets and not the translation the stock firmware provides; by full I mean you can elect to receive the data still manchester/miller encoding which allows for unique issues "other security researchers" have ignored as unimportant or too simple to have bugs, of particular note is that ISO-1443A is a seriously fucked up protocol and has implicit assumptions about the anti-collision sequence, by allowing an application to drive the encoding and decoding, you can introduce collisions and encoding errors that were not previously possible with the stock firmware or any other tools to the best of my knowledge.

