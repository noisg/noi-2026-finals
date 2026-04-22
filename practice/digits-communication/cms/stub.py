import sys
import struct
import signal
import os

signal.signal(signal.SIGPIPE, signal.SIG_IGN)

PYTHON_MARKER = 0x50595448  # 'PYTH'

# Signal to manager that this is a Python submission
sys.stdout.buffer.write(struct.pack('<i', PYTHON_MARKER))
sys.stdout.buffer.flush()

# Keep stdin open so the manager can write to Alice's FIFO without EPIPE.
# Blocks until the manager sends DIE and closes its write end.
try:
    sys.stdin.buffer.read()
except Exception:
    pass

# Use os._exit to skip Python's cleanup (avoids BrokenPipeError on stdout
# flush when the manager has already closed the read end of the FIFO).
os._exit(0)
