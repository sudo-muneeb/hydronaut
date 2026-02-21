#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")" && pwd)"
SRC="$ROOT/src"
INC="$ROOT/include"
OUT="$ROOT/hydronaut"

echo "═══════════════════════════════════════"
echo "  Building Hydronaut"
echo "═══════════════════════════════════════"

g++ \
  "$SRC/main.cpp"             \
  "$SRC/AssetManager.cpp"     \
  "$SRC/Player.cpp"           \
  "$SRC/ConvexObstacle.cpp"   \
  "$SRC/SineObstacle.cpp"     \
  "$SRC/ParabolicObstacle.cpp"\
  "$SRC/SecObstacle.cpp"      \
  "$SRC/ExpSineObstacle.cpp"  \
  "$SRC/Treasure.cpp"         \
  "$SRC/Level.cpp"            \
  "$SRC/Level1.cpp"           \
  "$SRC/Level2.cpp"           \
  "$SRC/Level3.cpp"           \
  "$SRC/Menu.cpp"             \
  -I"$INC"                    \
  -o "$OUT"                   \
  -lsfml-graphics             \
  -lsfml-window               \
  -lsfml-system               \
  -lsfml-audio                \
  -std=c++17                  \
  -Wall -Wextra               \
  -O2

echo "Build successful → $OUT"
echo ""
echo "Running Hydronaut..."
cd "$ROOT"
"$OUT"