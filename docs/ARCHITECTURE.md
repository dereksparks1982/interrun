# Interrun Architecture

Interrun is one application with replaceable subsystems.

```text
Interrun UI
   |
   +-- Browser / renderer
   |     +-- normal web transport
   |     +-- Tor transport for .onion
   |
   +-- Communications
   |     +-- Baresip/libre donor path
   |     +-- direct P2P preferred
   |     +-- encrypted messaging/calls
   |     +-- interrun://join/... links
   |
   +-- Media
   |     +-- DK Media/libVLC donor path
   |     +-- Nougat player improvements
   |     +-- yt-dlp stream bridge
   |     +-- P2P stream-while-downloading
   |
   +-- Search / privacy
   |     +-- Nougat Secure Search donor design
   |     +-- privacy policy / receipts
   |
   +-- Local profile
         +-- bookmarks
         +-- history
         +-- contacts
         +-- messages
         +-- site storage
         +-- keys
```

v0.0.1 implements the local profile, privacy policy, privacy receipt, input router, join-link route, test suite, and plain UI harness.

The renderer, Tor process, libVLC engine, Baresip engine, and encrypted P2P signaling are intentionally not claimed as complete yet.
