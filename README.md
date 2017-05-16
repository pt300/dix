# Dix
Simple Windows console UI lib that believes **views** will know the best way to draw themselves.

Works with CMAKE, and is as thread-safe as WinAPI, with exception to modules/views. Thread-safety depends on the person who wrote them.

## How to use this thing
For that please refer to wiki pages on GitHub (not finished yet).

## Current plans for the library
- [x] Finalise the idea on how the whole system will work
- [ ] Rewrite basic modules (66% done)
- [ ] Incorporate input methods
- [ ] Write new library which will serve as a collection of useful modules with views
- [ ] Rethink the idea of supporting multiple consoles at once (Involves spawning new processes)