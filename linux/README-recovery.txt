Hex-Shockolate-Enhanced v0.3.2 RECOVERY TEST

This is a test build, not a confirmed resolution of the reported instability.
It contains a complete clean rebuild and includes the OpenGL shaders missing
from earlier packages. Incompatible saves are checked before the active game
is closed or its working file is overwritten. Physics failures now log the
player's tile coordinates. Berserk lifecycle logging remains enabled.

Extract this entire archive into a NEW folder. Do not replace just the binary.
Import your purchased game data with import-data.py and launch with launch.sh.
Keep your existing installation and saves as a backup. Start a new game for
this test; Nightdive/upstream/seven-weapon saves are incompatible. Expanded
v0.2/v0.3 saves retain the same player layout, but are not needed for this test.

General inventory has 28 slots. Click the selected General category again,
or use Page Up / Page Down, to switch between pages of 14 items. Weapons
have nine slots, on pages of seven and two. These are inventory list pages,
not the MFD automap/logbook pages.

The requested 1.5x medi-patch regeneration, 60-game-second berserk, doubled
railgun/plasma damage and 30-120-second ecology delay are retained.

If problems remain, return run.log from this new folder and describe what
happened (movement, inventory, graphics, patches, or another symptom). The
earlier log contained no berserk dose, so it could not explain its expiry.

Source and licenses are included in source/. No purchased game data is included.
