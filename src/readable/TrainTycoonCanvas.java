import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.media.Player;
import javax.microedition.midlet.MIDlet;

/**
 * Readable structural reconstruction of the original obfuscated class {@code a}.
 *
 * The lifecycle, frame scheduler, input normalization, screen transitions, music
 * control and resource loading shown here come directly from bytecode. The game
 * simulation itself remains in {@code a.class}; it spans 197 declared methods and
 * hundreds of tightly coupled arrays, so invented names would be less trustworthy
 * than the supplied javap dump.
 */
public final class TrainTycoonCanvas extends Canvas implements Runnable {
    private static final int FRAME_TIME_MS = 35;
    private static final int KEY_RELEASED = -999;

    // Exact numeric state IDs used by the original switch statements.
    private static final int SCREEN_SPLASH_0 = 0;
    private static final int SCREEN_SPLASH_1 = 1;
    private static final int SCREEN_SPLASH_2 = 2;
    private static final int SCREEN_SPLASH_3 = 3;
    private static final int SCREEN_DATA_LOADING = 10;
    private static final int SCREEN_MAIN_MENU_OR_PAUSE = 20;
    private static final int SCREEN_TRANSITION_25 = 25;
    private static final int SCREEN_GAME = 30;
    private static final int SCREEN_DIALOG_35 = 35;
    private static final int SCREEN_DIALOG_40 = 40;
    private static final int SCREEN_DIALOG_45 = 45;
    private static final int SCREEN_DIALOG_55 = 55;
    private static final int SCREEN_DIALOG_65 = 65;
    private static final int SCREEN_DIALOG_105 = 105;
    private static final int SCREEN_DIALOG_120 = 120;

    private GameMIDlet midlet;                 // original: ex
    private Thread gameThread;                 // original: gn
    private boolean exitRequested;             // original: gQ
    private boolean applicationHidden;         // original: cv
    private boolean displayInstalled;          // original: gy

    private int screen;                        // original: bF
    private int previousScreen;                // original: hx
    private int pendingKeyCode;                // original: aS
    private int repeatedKeyCode;               // original: z
    private int repeatDelayFrames;             // original: aF
    private int inputCooldown;                  // original: dw
    private int frameNumber;                    // original: ge
    private int simulationStepsPerFrame = 1;   // original: aZ

    private boolean fastForwardLoading;         // original: ev
    private boolean skipNormalSimulation;       // original: bJ
    private int fastForwardCounter;             // original: gg

    private long frameStartedAt;                // original: cX
    private long updateDuration;                // original: dL
    private int sleepBudget;                    // original: cS

    private XorResourceInput packedResource;    // original: aU
    private PackedImageReader packedImages;
    private Image currentFullScreenImage;       // original: P
    private Image gameBackBuffer;               // original: fG
    private Graphics gameBackBufferGraphics;    // original: C
    private Player musicPlayer;                 // original: bi

    public void start(GameMIDlet owner) {
        midlet = owner;
        initializeGame();                       // original: Z()
        changeScreen(SCREEN_SPLASH_0);          // original: t(0)
        Display.getDisplay(owner).setCurrent(this);
        gameThread = new Thread(this);
        gameThread.start();
    }

    public void run() {
        while (!exitRequested) {
            if (applicationHidden) {
                Thread.yield();
                continue;
            }

            frameStartedAt = System.currentTimeMillis();
            processKeyRepeat();
            dispatchPendingInput();

            if (inputCooldown > 0) {
                inputCooldown--;
            }
            frameNumber++;

            runScreenUpdate();
            updateDuration = System.currentTimeMillis() - frameStartedAt;

            if (!displayInstalled) {
                Display.getDisplay(midlet).setCurrent(this);
                displayInstalled = true;
            }

            if (!applicationHidden) {
                repaint();
                serviceRepaints();
                waitForFrameDeadline();
            }
            Thread.yield();
        }

        if (!applicationHidden) {
            stopMusic();
            midlet.notifyDestroyed();
        }
    }

    private void processKeyRepeat() {
        if (repeatDelayFrames == 0) {
            if (repeatedKeyCode != 0) {
                pendingKeyCode = repeatedKeyCode;
            }
        } else {
            repeatDelayFrames--;
        }
    }

    private void dispatchPendingInput() {
        if (pendingKeyCode != 0 && pendingKeyCode != KEY_RELEASED) {
            handleKey(normalizeKey(pendingKeyCode));            // original: j(int)
        }
        if (pendingKeyCode == KEY_RELEASED) {
            handleKeyRelease();                                // original: d(int)
        }
    }

    private void runScreenUpdate() {
        if (fastForwardLoading) {
            fastForwardCounter = 1;
            while (fastForwardCounter > 0) {
                updateCurrentScreen();                          // original: m()
                if (screen == SCREEN_MAIN_MENU_OR_PAUSE
                        || screen == SCREEN_DIALOG_40) {
                    fastForwardLoading = false;
                }
            }
            fastForwardCounter = 0;
        }

        if (screen == SCREEN_DATA_LOADING) {
            for (int i = 0; i < 5; i++) {
                updateCurrentScreen();
            }
            return;
        }

        if (screen == SCREEN_GAME) {
            for (int i = 0; i < simulationStepsPerFrame; i++) {
                if (!skipNormalSimulation) {
                    updateCurrentScreen();
                }
                updateSelectedWorldObjectWhenRequired();        // original: B() conditionally
            }
            return;
        }

        updateCurrentScreen();
    }

    private void waitForFrameDeadline() {
        sleepBudget = FRAME_TIME_MS
                - (int) (System.currentTimeMillis() - frameStartedAt);
        if (sleepBudget < 0) {
            sleepBudget = 0;
        }

        long waitStartedAt = System.currentTimeMillis();
        while (System.currentTimeMillis() - waitStartedAt < sleepBudget) {
            try {
                Thread.sleep(1);
            } catch (Exception ignored) {
            }
            Thread.yield();
        }
    }

    protected void keyPressed(int rawKeyCode) {
        if (fastForwardLoading) {
            fastForwardLoading = false;
            return;
        }
        pendingKeyCode = rawKeyCode;
    }

    protected void keyReleased(int rawKeyCode) {
        pendingKeyCode = KEY_RELEASED;
    }

    private int normalizeKey(int keyCode) {
        // Letter aliases and vendor-specific soft-key codes from the original.
        if (keyCode == 'A' || keyCode == 'a') return '0';
        if (keyCode == 'Y' || keyCode == 'y') return '*';
        if (keyCode == 'Z' || keyCode == 'z') return '*';
        if (keyCode == 'X' || keyCode == 'x') return '#';
        if (keyCode == -6 || keyCode == -202 || keyCode == 21 || keyCode == -21) return '*';
        if (keyCode == -7 || keyCode == -203 || keyCode == 22 || keyCode == -22) return '#';

        int action = getGameAction(keyCode);
        if (action == LEFT)  return '4';
        if (action == RIGHT) return '6';
        if (action == UP)    return '2';
        if (action == DOWN)  return '8';
        if (action == FIRE)  return '5';
        return keyCode;
    }

    private void handleKey(int keyCode) {
        pendingKeyCode = 0;
        repeatDelayFrames = 0;
        repeatedKeyCode = 0;

        switch (screen) {
            case SCREEN_SPLASH_0:
            case SCREEN_SPLASH_1:
            case SCREEN_SPLASH_2:
            case SCREEN_SPLASH_3:
                markSplashAsSkippable();
                break;
            case SCREEN_MAIN_MENU_OR_PAUSE:
                handleMenuKey(keyCode);
                break;
            case SCREEN_GAME:
                handleGameKey(keyCode);
                break;
            case SCREEN_DIALOG_40:
                handleDialogKey(keyCode);
                break;
            default:
                break;
        }
    }

    private void changeScreen(int newScreen) {
        previousScreen = screen;
        clearTransientScreenState();

        switch (newScreen) {
            case SCREEN_SPLASH_0:
                openPackedImages("/1");
                loadPackedMetadata("/4");
                // fall through: state 0 immediately reads the first packed image
            case SCREEN_SPLASH_1:
            case SCREEN_SPLASH_2:
                currentFullScreenImage = packedImages.readImage();
                resetSplashTimer();
                break;

            case 4: // used internally although paint() has no separate case
                loadStringTable("/3");
                break;

            case SCREEN_DATA_LOADING:
                loadSpriteDescriptors("/2");
                break;

            case SCREEN_MAIN_MENU_OR_PAUSE:
                initializeMenu();
                break;

            case SCREEN_TRANSITION_25:
                resetTransition();
                break;

            case SCREEN_GAME:
                gameBackBuffer = Image.createImage(getWidth(), getHeight());
                gameBackBufferGraphics = gameBackBuffer.getGraphics();
                initializeGameScreen();
                break;

            case SCREEN_DIALOG_35:
            case SCREEN_DIALOG_45:
            case SCREEN_DIALOG_55:
            case SCREEN_DIALOG_65:
            case SCREEN_DIALOG_105:
            case SCREEN_DIALOG_120:
                initializeModalScreen();
                break;
            default:
                break;
        }
        screen = newScreen;
    }

    public void onApplicationHidden() {
        applicationHidden = true;
        if (screen > SCREEN_DATA_LOADING && screen != SCREEN_DIALOG_120) {
            stopMusic();
        }
        if (screen == SCREEN_GAME) {
            preparePauseMenu();
            changeScreen(SCREEN_MAIN_MENU_OR_PAUSE);
        }
    }

    protected void showNotify() {
        if (screen > SCREEN_DATA_LOADING && screen != SCREEN_DIALOG_120) {
            startMusic();
        }
        applicationHidden = false;
    }

    protected void hideNotify() {
        onApplicationHidden();
    }

    private void startMusic() {
        try {
            if (musicPlayer != null) {
                musicPlayer.prefetch();
                musicPlayer.setMediaTime(0);
                musicPlayer.setLoopCount(-1);
                musicPlayer.start();
            }
        } catch (Exception ignored) {
        }
    }

    private void stopMusic() {
        try {
            if (musicPlayer != null && musicPlayer.getState() == Player.STARTED) {
                musicPlayer.stop();
            }
        } catch (Exception ignored) {
        }
    }

    private void openPackedImages(String name) {
        packedResource = new XorResourceInput(name);
        packedImages = new PackedImageReader(packedResource);
    }

    public void paint(Graphics graphics) {
        // The original dispatches on the exact state IDs listed above. See
        // analysis/PAINT_DISPATCH.md and raw-bytecode/a.javap.txt for all calls.
        renderCurrentScreen(graphics);
    }

    // ---------------------------------------------------------------------
    // Boundaries of the trustworthy structural reconstruction. The methods
    // below represent large obfuscated subsystems retained in a.class.
    // ---------------------------------------------------------------------
    private void initializeGame() { }
    private void updateCurrentScreen() { }
    private void updateSelectedWorldObjectWhenRequired() { }
    private void handleKeyRelease() { pendingKeyCode = 0; repeatedKeyCode = 0; }
    private void handleMenuKey(int keyCode) { }
    private void handleGameKey(int keyCode) { }
    private void handleDialogKey(int keyCode) { }
    private void markSplashAsSkippable() { }
    private void clearTransientScreenState() { }
    private void resetSplashTimer() { }
    private void loadPackedMetadata(String name) { }
    private void loadStringTable(String name) { }
    private void loadSpriteDescriptors(String name) { }
    private void initializeMenu() { }
    private void resetTransition() { }
    private void initializeGameScreen() { }
    private void initializeModalScreen() { }
    private void preparePauseMenu() { }
    private void renderCurrentScreen(Graphics graphics) { }
}
