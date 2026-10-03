# Interrun

**Interrun** is an open-source, local-first browser project built around one idea: the browser should work for the user, not for a cloud vendor.

Interrun is being assembled from code we already own or from open-source donor projects instead of rebuilding solved problems:

- **Netscape / early Mozilla** for browser ancestry and historical browser architecture.
- **Nougat Media Plus** for privacy/search, P2P, streaming, and media infrastructure developed by Elderred Softworks LLC.
- **DK Media Player** for the working VLC/libVLC media-player foundation.
- **Baresip + libre** for native audio/video calls and communications plumbing.
- **Tor** as a planned first-class transport so normal addresses and onion addresses can share one address bar.

## v0.0.1 bootstrap

The first build establishes the spine:

- C++17 native core with warnings treated as errors.
- Local profile creation with no online account.
- Address routing for normal web, .onion, local files, media, searches, internal pages, and Interrun join links.
- Privacy defaults with telemetry off, third-party cookies blocked, tracking storage blocked, and local data as the source of truth.
- P2P-preferred and end-to-end-encrypted communications policy.
- A simple browser-shell UI prototype for layout testing.
- A donor map for the parts we are bringing forward from Nougat, DK Media, Netscape/Mozilla, and Baresip.

The web shell is deliberately plain. It is a UI harness, not a claim that the Netscape renderer, Tor, Baresip, or libVLC are already fully wired into the executable.

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/interrun
```

Route examples:

```bash
./build/interrun https://example.com
./build/interrun examplehiddenservice.onion
./build/interrun movie.mkv
./build/interrun interrun://join/ABC123
./build/interrun "search words"
```

## Hard rules

1. One repository branch only: **main**.
2. No required cloud account.
3. Local profiles are the source of truth.
4. No advertising network or sale of user data.
5. No telemetry by default.
6. Third-party cookies are blocked by default.
7. Browsing, messaging, calls, and media belong to one application.
8. P2P is preferred for person-to-person communication.
9. Communications are designed for end-to-end encryption.
10. Third-party dependencies are tools, not owners of the architecture.
11. Approved working parts are not changed outside the approved scope.

See `docs/PROJECT_RULES.md`, `docs/ARCHITECTURE.md`, and `docs/DONOR_MAP.md`.

## Status

**Version:** 0.0.1 bootstrap candidate  
**Primary platform:** Linux / Ubuntu  
**Branch model:** main only  
**License:** GPL-3.0-or-later

---

**Elderred Softworks LLC · Interrun**
