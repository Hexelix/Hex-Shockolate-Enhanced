HEX-SHOCKOLATE-ENHANCED v0.3.1 — BERSERK DIAGNOSTIC

This is a diagnostic build, not a confirmed fix. It keeps v0.3 gameplay and
save compatibility. It adds logging to identify a reported 10-15 second
berserk cutoff that has not been reproduced in isolated tests.

INSTALL / TEST
1. Exit the game. In your existing v0.3 folder, rename systemshock to
   systemshock.v0.3-backup and run.log to run.previous.log, if it exists.
2. Copy this package's systemshock into that existing folder. Keep your res
   data and saves in place. Ensure the copied executable is executable:
     chmod +x systemshock
3. Run that folder's launch.sh explicitly. The new run.log must contain:
     Hex-Shockolate-Enhanced v0.3.1 BERSERK DIAGNOSTIC build
   This confirms that the diagnostic executable actually started.
4. Load your game, apply a NEW berserk patch, and note approximately how long
   it takes for the bonus/status and colors to stop. Stay alive and take no
   Detox during the test. Exit the game and attach run.log to the chat.

The log records berserk application, elapsed game ticks and seconds at each
patch update, timer expiry, and early removal through Detox or player
regeneration. Its wall-clock timestamps let us compare game time with real
elapsed time. Logging does not extend a patch or block normal cancellation.

Full patch-use/update tests, including the real callback table, pass at
normal and high fatigue. Refreshed doses also last one minute. The original
helper-only tests did not cover this full path; that gap is now covered.
No in-game reproduction or confirmed cause for the reported cutoff yet.

RESTORE
Exit, remove the diagnostic systemshock, and rename systemshock.v0.3-backup
to systemshock. Saves remain compatible with v0.2/v0.3.

The source for this diagnostic is on the diagnostics/berserk-duration branch
of https://github.com/Hexelix/Hex-Shockolate-Enhanced.
