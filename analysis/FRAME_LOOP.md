# Recovered frame loop

The original scheduler is single-threaded and targets a 35 ms frame. In order:

1. Skip work while the Canvas is hidden.
2. Timestamp frame start.
3. Generate key-repeat events after a frame-count delay.
4. Dispatch press or the `-999` release sentinel.
5. Decrement input cooldown and increment frame count.
6. Run accelerated loading updates when requested.
7. State 10 runs five updates per rendered frame.
8. State 30 runs `aZ` simulation substeps and a conditional secondary update.
9. Other states run one update.
10. Request and synchronously service repaint.
11. Sleep in 1 ms increments until 35 ms have elapsed, yielding each iteration.
12. On exit, stop audio and call `notifyDestroyed()`.

This is faithfully expressed in `src/readable/TrainTycoonCanvas.java`.
