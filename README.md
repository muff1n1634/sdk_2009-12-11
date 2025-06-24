sdk 2009 blah "` blah blah blah`" blah blah blah

requires
- make
- python
- ar
- dtk
- objdiff
- wine if not using windows natively

byob
- wii 1.0
- soon: gc 3.0

build
1. configure in config.mk
2. put slamWiiD.a and slamWii.a into orig/
3. make extract
4. make
5. make objdiff

should compile all the wpad and kpad files
