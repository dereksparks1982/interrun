# Donor Map

This file records what Interrun should reuse instead of rebuilding.

## Nougat Media Plus

Confirmed useful donor areas:

- `src/search/secure_search.cpp/.hpp`
  - local-first secure-search control
  - fail-closed behavior when private remote search is unavailable

- `src/privacy/privacy_policy.cpp/.hpp`
  - privacy invariants
  - no direct fallback
  - no plaintext DNS
  - no query logging
  - no persistent search identifier

- `src/privacy/privacy_receipt.cpp/.hpp`
  - human-readable privacy receipts

- `src/privacy/privacy_broker_client.cpp/.hpp`
  - local UNIX-socket privacy-broker pattern

- `src/p2p_engine.cpp/.hpp`
  - libtorrent-rasterbar engine
  - magnet/torrent startup
  - DHT/LSD
  - resume state
  - speed/seed controls
  - playback-window prioritization

- `src/p2p_stream_server.cpp/.hpp`
  - localhost HTTP range server
  - stream-while-downloading
  - loopback-only client acceptance

- `src/ytdlp_stream_server.cpp/.hpp`
  - localhost yt-dlp media bridge
  - range/seek support
  - YouTube compatibility handling

- `src/chat/nougat_chat.cpp/.hpp`
  - local chat/message model and connection state
  - useful model layer, but not a finished encrypted P2P transport

- web modules
  - `modules-search-discover.js`
  - `modules-p2p-network.js`
  - `modules-stream.js`

## DK Media Player

Use as the base for the Interrun media subsystem:

- VLC/libVLC playback
- local audio/video
- remembered playback position and volume
- subtitles and audio tracks
- frame stepping
- playback speed
- yt-dlp
- aria2/P2P foundations
- Linux packaging knowledge

Nougat's later player work should be compared against DK Media and selectively brought forward rather than discarded.

## Baresip + libre

Planned donor for:

- audio calls
- video calls
- SIP/RTP plumbing
- device access
- NAT traversal
- messaging primitives

It should become an internal Interrun subsystem rather than a separate user-facing application.

## Netscape / early Mozilla

Use as browser ancestry and historical source material. Do not blindly assume 1998 code can compile unchanged on a modern Linux toolchain. Preserve original licensing and source provenance while the usable browser/rendering path is mapped.

## Integration order

1. Keep the native X11 shell and core compiling cleanly.
2. Map and begin the Netscape/Mozilla browser/rendering path so Interrun can display actual pages itself.
3. Bring DK Media/Nougat media code behind an Interrun media interface.
4. Bring Nougat P2P/search/privacy pieces behind Interrun-owned interfaces.
5. Bring Baresip/libre behind the communications interface.
6. Wire Tor as an isolated transport for onion destinations.
