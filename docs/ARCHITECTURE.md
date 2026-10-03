# Interrun Architecture

Interrun is one native application with replaceable subsystems.

```text
Native Interrun window
   |
   +-- Browser / renderer
   |     +-- Netscape/Mozilla donor path
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

## v0.0.2

The application window is native X11 and launches directly as the `interrun` executable. No external browser is used to display Interrun's own UI.

The previous standalone HTML/CSS/JS harness is no longer the application shell.

Current working layers:

- native application window
- navigation chrome
- local profile
- privacy policy
- address classification/router
- local Interrun join-link generation
- Debian packaging

Not yet claimed complete:

- Netscape/Mozilla web rendering
- HTTP page loading in the renderer
- Tor process integration
- DK Media/libVLC playback integration
- Baresip/libre media transport
- encrypted P2P signaling
