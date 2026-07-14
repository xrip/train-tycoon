# Screen/state machine

The numeric IDs below are exact. Descriptions marked “likely” are semantic inferences.

| ID | Verified behavior | Interpretation |
|---:|---|---|
| 0 | Opens `/1`, parses `/4`, then reads a packed image | Initial loading/splash |
| 1 | Reads next packed image and resets timer | Splash page |
| 2 | Reads next packed image and resets timer | Splash page |
| 3 | Shares splash-key behavior in input dispatcher | Splash transition |
| 4 | Parses `/3` into indexed byte arrays | Internal data-loading state |
| 10 | Parses `/2`; runs five updates per frame | Bulk loading/initialization |
| 20 | Calls `o()` and initializes menu flags | Main menu or pause menu, likely |
| 25 | Resets a transition counter | Transition |
| 30 | Allocates full-screen back buffer; executes simulation substeps | Main game |
| 35 | Initializes modal selector fields | Modal/dialog |
| 40 | Has dedicated input and fast-forward termination | Modal/dialog |
| 45 | Initializes modal mode | Modal/dialog |
| 55 | Initializes modal mode | Modal/dialog |
| 65 | Initializes modal mode | Modal/dialog |
| 105 | Initializes modal mode | Modal/dialog |
| 120 | Initializes modal mode and suppresses music resume/stop | Special terminal/network screen, uncertain |

`paint(Graphics)` has explicit cases for: `0, 1, 2, 3, 10, 20, 25, 30, 35, 40, 45, 55, 65, 105, 120`.
