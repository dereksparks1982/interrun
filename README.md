# Interrun

**Interrun** is an open-source, local-first browser project built around one idea: the browser should work for the user, not for a cloud vendor.

Interrun is being assembled from code we already own or from open-source donor projects instead of rebuilding solved problems:

- **Netscape / early Mozilla** for browser ancestry and historical browser architecture.
- **Nougat Media Plus** for privacy/search, P2P, streaming, and media infrastructure developed by Elderred Softworks LLC.
- **DK Media Player** for the working VLC/libVLC media-player foundation.
- **Baresip + libre** for native audio/video calls and communications plumbing.
- **Tor** as a planned first-class transport so normal addresses and onion addresses can share one address bar.

## v0.0.2 native preview

This is the first build that opens as its **own Linux application window**. It does not require another browser to display Interrun's UI.

Current native pieces:

- X11 application window and browser chrome
- address bar and routing
- Home, Search, Messages, Calls, Media, and Privacy views
- locally generated `interrun://join/...` links
- local profile creation with no online account
- privacy defaults with telemetry off and third-party cookies blocked
- normal web / `.onion` / local file / media / search / Interrun-link classification
- C++17 build with warnings treated as errors
- test suite and Debian packaging

This preview **does not yet render web pages**. The Netscape/Mozilla rendering path is the next browser-engine milestone. libVLC, Tor, Baresip/libre, and encrypted P2P signaling are also donor paths that are mapped but not yet linked into this executable.

The old HTML/CSS/JS UI harness was removed from the application root so Interrun is not presented as a web app.

## Build from source

Ubuntu/Debian build requirements:

```bash
sudo apt install build-essential cmake libx11-dev
```

Build and run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/interrun
```

You can also start it with an address or file:

```bash
./build/interrun https://example.com
./build/interrun examplehiddenservice.onion
./build/interrun movie.mkv
./build/interrun interrun://join/ABC123
```

Headless route check:

```bash
./build/interrun --route https://example.com
```

## Build a Debian package

```bash
./packaging/build-deb.sh
```

The package is written to:

```text
dist/interrun_0.0.2-1_<architecture>.deb
```

Install it with:

```bash
sudo apt install ./dist/interrun_0.0.2-1_amd64.deb
```

Then launch **Interrun** from the desktop application menu or run:

```bash
interrun
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

**Version:** 0.0.2 native preview  
**Primary platform:** Linux / Ubuntu  
**Branch model:** main only  
**License:** GPL-3.0-or-later

---

**Elderred Softworks LLC · Interrun**
