#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT}/build"
DIST_DIR="${ROOT}/dist"
STAGE="${ROOT}/.package-stage"
VERSION="0.0.2-1"
ARCH="$(dpkg --print-architecture)"

rm -rf "${BUILD_DIR}" "${STAGE}"
mkdir -p "${DIST_DIR}"

cmake -S "${ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

mkdir -p \
  "${STAGE}/DEBIAN" \
  "${STAGE}/usr/bin" \
  "${STAGE}/usr/share/applications" \
  "${STAGE}/usr/share/doc/interrun"

chmod 0755 "${STAGE}" "${STAGE}/DEBIAN" "${STAGE}/usr" "${STAGE}/usr/bin" "${STAGE}/usr/share" "${STAGE}/usr/share/applications" "${STAGE}/usr/share/doc" "${STAGE}/usr/share/doc/interrun"
chmod g-s "${STAGE}" "${STAGE}/DEBIAN" "${STAGE}/usr" "${STAGE}/usr/bin" "${STAGE}/usr/share" "${STAGE}/usr/share/applications" "${STAGE}/usr/share/doc" "${STAGE}/usr/share/doc/interrun"

install -m 0755 "${BUILD_DIR}/interrun" "${STAGE}/usr/bin/interrun"
install -m 0644 "${ROOT}/packaging/interrun.desktop" \
  "${STAGE}/usr/share/applications/interrun.desktop"
install -m 0644 "${ROOT}/README.md" \
  "${STAGE}/usr/share/doc/interrun/README.md"
install -m 0644 "${ROOT}/LICENSE" \
  "${STAGE}/usr/share/doc/interrun/LICENSE"
install -m 0644 "${ROOT}/THIRD_PARTY_NOTICES.md" \
  "${STAGE}/usr/share/doc/interrun/THIRD_PARTY_NOTICES.md"

cat > "${STAGE}/DEBIAN/control" <<EOF
Package: interrun
Version: ${VERSION}
Section: web
Priority: optional
Architecture: ${ARCH}
Depends: libx11-6, libstdc++6
Maintainer: Elderred Softworks LLC
Description: Interrun local-first browser native preview
 Interrun is an open-source browser project with local profiles,
 privacy-first defaults, P2P-first communications goals, and
 integrated media donor paths.
EOF

OUT="${DIST_DIR}/interrun_${VERSION}_${ARCH}.deb"
dpkg-deb --root-owner-group --build "${STAGE}" "${OUT}"
rm -rf "${STAGE}"

printf '%s\n' "${OUT}"
