/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  javax.microedition.io.Connector
 *  javax.microedition.io.HttpConnection
 *  javax.microedition.lcdui.Canvas
 *  javax.microedition.lcdui.Display
 *  javax.microedition.lcdui.Displayable
 *  javax.microedition.lcdui.Graphics
 *  javax.microedition.lcdui.Image
 *  javax.microedition.media.Control
 *  javax.microedition.media.Manager
 *  javax.microedition.media.Player
 *  javax.microedition.media.control.VolumeControl
 *  javax.microedition.midlet.MIDlet
 *  javax.microedition.rms.RecordEnumeration
 *  javax.microedition.rms.RecordStore
 */
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.util.Random;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.media.Control;
import javax.microedition.media.Manager;
import javax.microedition.media.Player;
import javax.microedition.media.control.VolumeControl;
import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;

public class GameCanvas
extends Canvas
implements Runnable {
    GameMIDlet midlet;
    public boolean stopRequested = false;
    final int at;
    final int cB;
    final int ar;
    final int D;
    int mapWidth;
    int mapHeight;
    int hj = (this.mapWidth - 1) * 8;
    int mapScreenWidth = (this.mapWidth + this.mapHeight) * 16;
    int mapScreenHeight = (this.mapWidth + this.mapHeight) * 8;
    int gU = this.mapScreenWidth / 2;
    int fO = this.mapScreenHeight / 2;
    int dQ;
    int hf;
    int archiveOrRecallMethod;
    int gG;
    int co;
    int cK;
    int dK;
    int bX;
    int weightAccel;
    int weightAccel2;
    int weightAccel3;
    int weightAccel4;
    int moneyBaseA;
    int moneyBaseB;
    int baseTrainSpeed;
    int accel1;
    int brake1;
    int baseTrainSpeed2;
    int accel2;
    int costPower;
    int costStruct;
    int costOverhead;
    int costMaintenance;
    int costCrew;
    int gWagonCost;
    int gWagonCost2;
    int gWagonCost3;
    int gWagonCost4;
    int gWagonCost5;
    int gWagonCost6;
    int costGroupC1;
    int costGroupC2;
    int costGroupC3;
    int costGroupC4;
    Thread gameThread;
    int simSpeed = 1;
    long frameStartTime;
    long sleepStart;
    int sleepMillis;
    int frameCount = 0;
    int gg = 0;
    boolean inputPending = false;
    boolean pauseFlag = false;
    boolean paused = false;
    long frameTime;
    boolean ce = false;
    long bT = System.currentTimeMillis();
    int keyRepeatDelay = 0;
    int heldKey = 0;
    boolean displayInitialized = false;
    int dw = 0;
    int currentKey = 0;
    boolean ee = false;
    int dp = 255;
    static int e = 1;
    static int hs = 1;
    static int eN = hs - 30;
    static int fX = hs - 15;
    XorResourceInput aU = null;
    Image[] ca = new Image[6];
    short[] spriteOffsetTable;
    byte[] spriteData;
    int cursorScale = 0;
    int cursorDragY = -1;
    int cursorSelectY = -1;
    int cursorSelectX = -1;
    boolean cursorOverUI;
    boolean cursorAllowZoomOut;
    int cursorDragX = -1;
    boolean mapRedrawPending = false;
    boolean bC = true;
    boolean pauseMenuOpen = false;
    int dialogContentHeight;
    boolean needsRepaint = false;
    int clipX;
    int clipY;
    int clipWidth;
    int clipHeight;
    int currentSpriteId;
    int lastRenderedSpriteId = -1;
    int spriteJustChanged = 0;
    byte[][] tileElevation;
    byte[][] heightMap;
    int terrainSmoothCost = 0;
    boolean initialIndustryPlaced = true;
    byte[][] terrainEditBuffer;
    int selectionStartX;
    int selectionStartY;
    boolean fx = false;
    int mapGenStage = 0;
    int tilesGeneratedInStage = 0;
    int totalTilesToGenerate = 0;
    int mapGenTilesDone = 0;
    int viewportBaseX = this.gU;
    int viewportBaseY = this.fO;
    int mapScaleX = this.gU * 2;
    int mapScaleY = this.fO * 2;
    int keySubState = 0;
    int lastKeyCode = 0;
    int hh = 3;
    int trackStartX;
    int trackStartY;
    int trackStepX;
    int trackStepY;
    int buildMenuIndex = 0;
    int[] selectionTileX = new int[4];
    int[] selectionTileY = new int[4];
    int cursorScreenX;
    int cursorScreenY;
    int swapMapX;
    int swapMapY;
    int tileCursorX;
    int tileCursorY;
    boolean vehicleMenuSelectIndex;
    boolean trainMenuSelectIndex;
    boolean blockBuildMenu = false;
    int bg;
    boolean ck;
    boolean menuDirty = true;
    int buildMenuMode = 0;
    int gj = 0;
    byte[][] buildMenuItems;
    byte[][] buildMenuResultCodes;
    byte[][] menuItemCode;
    byte[][] menuTextIndex;
    byte[][] menuTransitionCode;
    boolean[][] menuRowEnabled;
    int eY;
    int fN;
    int cf;
    byte scenarioBits;
    int confirmToggle = 0;
    int[] K = new int[3];
    byte[] fJ = new byte[3];
    int confirmCounter = 0;
    int confirmTargetState = -1;
    boolean flagsA = true;
    byte junctionDirCursor = 0;
    byte numSelectedTrains = 0;
    byte[] constructionTimers = new byte[64];
    short[] zonePopulation = new short[64];
    short[] di = new short[64];
    short[] trainY2 = new short[64];
    short[] trainX2 = new short[64];
    byte stationDirCursor = 0;
    byte aE = 0;
    byte[] da = new byte[48];
    byte[] cityPopulation = new byte[48];
    byte[] be = new byte[48];
    int pendingTransitionCode = -1;
    boolean discountApplied = false;
    int baseTrainCostSnapshot = 0;
    byte ae = 0;
    byte difficultyA = 0;
    byte difficultySlot = 0;
    byte difficultyB = 0;
    byte difficultyC = 0;
    byte cZ = 0;
    byte cW = 0;
    int J;
    int fY;
    int dR;
    int gJ;
    boolean ad = false;
    byte[] bE = new byte[10];
    byte[] cD = new byte[10];
    byte cu = 0;
    byte fi = 0;
    boolean S = false;
    boolean cH = false;
    boolean dP = false;
    int screenParam;
    short[][] aQ;
    int placementX;
    int placementY;
    int demolishX;
    int demolishY;
    int fl;
    int dX;
    int gO;
    int ed = 0;
    byte bW = 0;
    byte df = 1;
    int selectionCost = 0;
    boolean aC = true;
    int et = 0;
    int[] dk = new int[]{-1, 1, 0, 0, -1, 1, 0};
    int[] j = new int[]{0, 0, -1, 1, 0, 0, -1};
    int aiWagonsLastSeen = 0;
    int wagonType = 0;
    int wagonDiscountType = 0;
    int[] wagonOptionSlots = new int[5];
    int[] wagonCountMatch = new int[5];
    int unitCost = 0;
    int currentTrainCostSnapshot = 0;
    int difficultyPct = 100;
    byte[] vehicleFlags = new byte[250];
    byte[] wagonAJ = new byte[250];
    byte[] wagonEQ = new byte[250];
    short[][] trainOptionSlots = new short[250][5];
    byte[] trainFlagBits = new byte[250];
    byte[] wagonServiceCounter = new byte[250];
    byte[] trainX = new byte[500];
    byte[] trainY = new byte[500];
    byte[] trainDir = new byte[500];
    byte[] trainPhase = new byte[500];
    byte[] trainFlags = new byte[500];
    byte[] trainEraOrStage = new byte[500];
    byte[] trainAgeOrTimer = new byte[500];
    byte[] wagonEnergyRemaining = new byte[500];
    byte[] trainEnergyOrCount = new byte[500];
    byte[] wagonIorHost = new byte[500];
    byte[] trainServiceCounter = new byte[500];
    short[] nextCar = new short[500];
    int[] eL = new int[500];
    byte[] bz = new byte[500];
    int trainAtCursor;
    int era = 1;
    byte[][] eR = new byte[][]{{0, 1, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}, {2, 3, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}, {4, 5, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}, {6, 7, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}, {8, 9, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}, {10, 11, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}};
    byte[][] aH = new byte[][]{{1, 0, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}, {3, 2, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}, {5, 4, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}, {7, 6, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}, {9, 8, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}, {11, 10, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}};
    int dI = 5;
    short[][] cs = new short[][]{{130, 131, 132, 133, 134, 135, 136, 137}, {140, 141, 142, 143, 144, 145, 146, 147}, {160, 161, 162, 163, 160, 161, 162, 163}, {165, 166, 167, 168, 165, 166, 167, 168}, {170, 171, 172, 173, 170, 171, 172, 173}, {175, 176, 177, 178, 175, 176, 177, 178}, {180, 181, 182, 183, 180, 181, 182, 183}, {185, 186, 187, 188, 185, 186, 187, 188}};
    byte[] saveBuffer = null;
    int saveReadOffset = 0;
    int saveSlot = 0;
    int savePendingMode = 0;
    int snapshotSize = 0;
    boolean snapshotLoaded = false;
    int bm = 0;
    int fw = 0;
    int[] trainRevenue = new int[500];
    int[] prevTrainRevenue = new int[500];
    short[] bb = new short[48];
    short[] prevStationRevenue = new short[48];
    boolean y = false;
    int W = 0;
    boolean bR = false;
    boolean dS = false;
    int totalSpent = 0;
    int aiMoneySpent = 0;
    int fuelCostAccrued = 0;
    int ho = 0;
    int totalRevenue = 0;
    int eg = 0;
    int eJ = 0;
    int eX = 0;
    int br = 0;
    int gV = 0;
    int cj = 0;
    int aA = 0;
    int dg = 0;
    int gs = 0;
    int dq = 0;
    int bM = 0;
    int gx = 0;
    int eM = 0;
    int prevFuelCost = 0;
    int prevSubsidyCost = 0;
    int prevTotalRevenue = 0;
    int prevTerrainTotal = 0;
    int prevEnterpriseTotal = 0;
    int prevXIndicator = 0;
    int bG = 0;
    int he = 0;
    int bP = 0;
    int ht = 0;
    int cr = 0;
    int cg = 0;
    int cm = 0;
    int gq = 0;
    int decadeCostTotal = 0;
    int decadeSpentTotal = 0;
    int decadeFuelCost = 0;
    int decadeSubsidyTotal = 0;
    int decadeRevenueTotal = 0;
    int eb = 0;
    int decadeEnterpriseTotal = 0;
    int decadeXTotal = 0;
    int fe = 0;
    int decadeOtherXTotal = 0;
    int[] yrN = new int[12];
    int[] yrCx = new int[12];
    int[] yrZ = new int[12];
    int[] dG = new int[12];
    int[] ef = new int[12];
    int[] L = new int[12];
    byte[] o = new byte[12];
    byte[] dm = new byte[12];
    byte[] fA = new byte[12];
    int cA;
    boolean aY;
    int gS;
    int bp;
    int dD;
    int k;
    byte T = 1;
    int hu = 66;
    byte cd = (byte)15;
    byte fP = (byte)12;
    byte[] eA = new byte[6];
    byte[][] aK = new byte[][]{{1, 3, 13}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, {1, 2, 3, 13, 14, 15}, {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, {1, 2, 3, 13, 14, 15}, {4, 9}, {5, 10}, {6, 11}, {7, 12}, {8, 12}, {1, 2, 3, 13}, {1, 2, 3, 14}};
    byte[][] a = new byte[][]{{0, 0}, {0, 0}, {5, 20}, {10, -56}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {8, 100}};
    byte[] menuRowType = new byte[6];
    byte[] bh = new byte[6];
    byte[] bY = new byte[6];
    byte[] dn = new byte[6];
    byte[] dE = new byte[6];
    byte[] fW = new byte[6];
    byte[] fF = new byte[6];
    int[] menuRowCost = new int[6];
    int[] menuRowValue = new int[6];
    byte[] menuRowAltType = new byte[6];
    byte[] menuRowEra = new byte[6];
    int[] menuRowRevenue = new int[6];
    int er = 0;
    short[] dU = new short[12];
    boolean flagsB;
    int bq;
    int bv = 0;
    int aO = 0;
    byte[] gP = new byte[5];
    byte scenarioMilestoneIndex;
    int currentMilestoneScore;
    int milestoneCoThreshold2;
    int bD = 0;
    int milestoneThreshold1 = 0;
    int milestoneThreshold2 = 0;
    int milestoneThreshold3 = 0;
    int milestoneThreshold4 = 0;
    int milestoneThreshold5 = 0;
    int aiWagonsBought = 0;
    boolean milestonesEnabled;
    boolean musicPlaying = false;
    boolean ay = false;
    boolean musicAutoStart = false;
    boolean ac = false;
    int gh = 2;
    Player musicPlayer = null;
    VolumeControl volumeControl = null;
    int by = 0;
    int H = 0;
    int[] gk = new int[13];
    int[] bd = new int[14];
    static byte[] dO = new byte[]{-15, 12, -41, -41, 37, 109, -35, 61, -10, 28, -25, 49, -31, 51, 15, 104, -32, 54, 51, 77, 42, 76};
    static byte[] el = new byte[]{-11, 0, 17, 17, 53, 125, 28, 99, -49, 37, -4, 0, 40, 124, 68, 68, -49, -41, -26, 59, 41, 76};
    static byte[] fz = new byte[]{-46, 45, -20, 60, 60, 116, 60, 71, -24, 14, -17, 8, 44, 76, 40, 122, -8, 30, 29, 125, 57, 91};
    static byte[] dM = new byte[]{49, 76, -59, -59, -34, -42, 8, 17, -33, 61, 41, 77, -22, 54, -59, -34, -9, 29, -41, 33, 0, 18};
    static byte[] l = new byte[]{-21, 54, -29, 35, -30, 42, -60, -40, 40, 76, 28, 100, -23, 62, -42, 56, -59, -39};
    static byte[] ag = new byte[]{23, 98, -57, -57, -11, 61, -29, 55, 51, 72, -2, 18};
    static byte[] i = new byte[]{40, 123, 39, 28, 9, 15, 65, 127, -35, -58, 3, 6, -59, -29, 18, 6, -21, -45, 41, 17, 31, 24};
    static byte[] viewportZoomStep = new byte[]{-53, -63, -55, 57, 13, 124, -53, -50, -65, 45, 26, 29};
    static byte[] cQ = new byte[]{-30, 61, -6, 10, -3, 53, 20, 122, 57, 93, 44, 127, -2, 12, -31, 56, -58, 40};
    static byte[] ec = new byte[]{18, 1, -28, -57, -10, 41, -37, -62, -40, 53, 12, 116, 53, -83, 36, 19, 38, 20, -33, -33, 36, 19, 68, 120, -25, 43, 6, 62, 29, 2, -25, 41, -68, 37, 66, 125, 0, 62, 62, 127, -54, -52, 17, 49, 17, 3, -59, -54, -30, 28, 5, 51, 24, 1, -65, -30, -8, 58, -6, 47, 11, 1, 17, 5, 47, 18, 53, 99, 42, 27, 57, 123, -9, 96, -54, -50, 17, 9, 34, 28, -41, 15, 3, 39, 46, 100, 64, 97, -44, -58, -19, 22, -38, -63, 18, 1, 8, 63};
    byte[] tileOffsetX = new byte[1];
    byte[] tileOffsetY = new byte[1];
    int numActiveTileAnimations = 0;
    static boolean ch = false;
    static boolean selectionHidden = false;
    static boolean cc = true;
    static boolean q = true;
    int gameState;
    byte[][] tileMap;
    byte[][] trackMap;
    int viewportCenterX = 0;
    int viewportCenterY = 0;
    int money = 200000;
    int cy = 0;
    int loanBalance = 0;
    int loanCeiling = 600000;
    byte aD = (byte)4;
    int dayOfMonth = 1;
    int monthOfYear = 1;
    int year = 1950;
    int fr = 0;
    int fd = 0;
    int dN = 0;
    int bI = 0;
    int eC = 2;
    int aM = 1;
    int scenarioStep = 0;
    int ao = 0;
    Random rng = new Random(System.currentTimeMillis());
    Image backbuffer = Image.createImage((int)e, (int)hs);
    Graphics backbufferG = this.backbuffer.getGraphics();
    Image[] tileSprites;
    byte[] tileSpriteIndex = new byte[255];
    Image P;
    long loadingStartTime;
    boolean loadingTimeout;
    int eW;
    int al = 0;
    int transitionCode = 0;
    boolean ready = true;
    int loadStep = 0;
    byte[] ds;
    byte[][] n;
    int timerA1 = 0;
    int timerA2 = 0;
    int timerB1 = 0;
    int timerB2 = 0;
    int timerC1 = 0;
    int timerC2 = 0;
    int woodAgeTimer = 0;
    int phaseTimer = 0;
    byte[][] tileQueue = new byte[50][3];
    boolean[] go = new boolean[50];
    int[] dc = new int[]{16756834, 16765610, 0xC08000, 0xFF9696, 0xFFC8C8, 0xC80000, 0xA0FFA0, 0xC8FFC8, 51200};
    int bx = 0;
    int fR = 0;
    byte fZ;
    byte ff;
    int eD = -1;
    int aJ = -1;
    int hx = 0;
    int loadingTimer = 0;
    int dH = 50;
    long tickStart;
    long tickEnd;
    long gw = 0L;
    byte[] tileSpriteYOffsetByCode = new byte[]{0, 0, -4, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 0, -4, 4, 4, 0, 0, 4, 0, -4, 0};
    int bH = -1;
    int viewportCenterYCached = -1;
    short[] visibleTrains = new short[500];
    int visibleTrainCount = 0;
    int viewportX1;
    int viewportY1;
    int viewportX0;
    int viewportY0;

    public GameCanvas() {
        this.at = 3;
        this.cB = 33;
        this.ar = 20;
        this.D = 24;
    }

    public int c(byte n) {
        return n < 0 ? 256 + n : n;
    }

    public void start(GameMIDlet gameMIDlet) {
        this.midlet = gameMIDlet;
        this.Z();
        this.setState(0);
        Display.getDisplay((MIDlet)this.midlet).setCurrent((Displayable)this);
        this.gameThread = new Thread(this);
        this.gameThread.start();
    }

    public void run() {
        while (!this.stopRequested) {
            int n;
            if (this.paused) continue;
            this.frameStartTime = System.currentTimeMillis();
            if (this.keyRepeatDelay == 0) {
                if (this.heldKey != 0) {
                    this.currentKey = this.heldKey;
                }
            } else {
                --this.keyRepeatDelay;
            }
            if (this.currentKey != 0 && this.currentKey != -999) {
                this.handleKey(this.currentKey);
            }
            if (this.currentKey == -999) {
                this.d(this.currentKey);
            }
            if (this.dw > 0) {
                --this.dw;
            }
            ++this.frameCount;
            if (this.inputPending) {
                this.gg = 1;
                while (this.gg > 0) {
                    this.update();
                    if (this.gameState != 20 && this.gameState != 40) continue;
                    this.inputPending = false;
                    break;
                }
                this.gg = 0;
            }
            if (this.gameState == 10) {
                for (n = 0; n < 5; ++n) {
                    this.update();
                }
            }
            if (this.gameState == 30) {
                for (n = 0; n < this.simSpeed; ++n) {
                    if (!this.pauseFlag) {
                        this.update();
                    }
                    if (!this.menuRowEnabled[this.buildMenuMode][this.buildMenuIndex]) continue;
                    this.viewportZoomKey();
                }
            } else {
                this.update();
            }
            this.frameTime = System.currentTimeMillis() - this.frameStartTime;
            if (!this.displayInitialized) {
                Display.getDisplay((MIDlet)this.midlet).setCurrent((Displayable)this);
                this.displayInitialized = true;
            }
            if (!this.paused) {
                this.repaint();
                this.serviceRepaints();
                this.sleepMillis = (int)(35L - (System.currentTimeMillis() - this.frameStartTime));
                if (this.sleepMillis < 0) {
                    this.sleepMillis = 0;
                }
                this.sleepStart = System.currentTimeMillis();
                while (System.currentTimeMillis() - this.sleepStart < (long)this.sleepMillis) {
                    try {
                        Thread.sleep(1L);
                    }
                    catch (Exception exception) {
                        // empty catch block
                    }
                    GameCanvas gameCanvas = this;
                    gameCanvas.gameThread.yield();
                }
            }
            GameCanvas gameCanvas = this;
            gameCanvas.gameThread.yield();
        }
        if (!this.paused) {
            this.stopMusic();
            this.midlet.notifyDestroyed();
        }
    }

    public void pointerPressed(int n, int n2) {
        if (this.inputPending) {
            return;
        }
    }

    public void pointerReleased(int n, int n2) {
        if (this.inputPending) {
            this.inputPending = false;
            return;
        }
    }

    public void pointerDragged(int n, int n2) {
        if (this.inputPending) {
            return;
        }
    }

    public void keyPressed(int n) {
        if (this.inputPending) {
            this.inputPending = false;
            return;
        }
        this.currentKey = n;
    }

    public void keyReleased(int n) {
        this.currentKey = -999;
    }

    void a(Graphics graphics, int n, int n2, int n3, int n4) {
        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[n])], n2, n3, n4);
    }

    void handleKey(int n) {
        int n2;
        if (n == 65 || n == 97) {
            n = 48;
        }
        if (n == 89 || n == 121) {
            n = 42;
        }
        if (n == 90 || n == 122) {
            n = 42;
        }
        if (n == 88 || n == 120) {
            n = 35;
        }
        if (n == -6) {
            n = 42;
        }
        if (n == -7) {
            n = 35;
        }
        if (n == -202) {
            n = 42;
        }
        if (n == -203) {
            n = 35;
        }
        if (n == 21) {
            n = 42;
        }
        if (n == 22) {
            n = 35;
        }
        if (n == -21) {
            n = 42;
        }
        if (n == -22) {
            n = 35;
        }
        if ((n2 = this.getGameAction(n)) == 2) {
            n = 52;
        }
        if (n2 == 5) {
            n = 54;
        }
        if (n2 == 1) {
            n = 50;
        }
        if (n2 == 6) {
            n = 56;
        }
        if (n2 == 8) {
            n = 53;
        }
        this.currentKey = 0;
        this.keyRepeatDelay = 0;
        this.heldKey = 0;
        switch (this.gameState) {
            case 0: 
            case 1: 
            case 2: 
            case 3: {
                this.loadingTimeout = true;
                break;
            }
            case 20: {
                if (n == 56 || n == 54) {
                    this.handleMenuAction((byte)1);
                    this.keyRepeatDelay = 4;
                    this.heldKey = n;
                }
                if (n == 50 || n == 52) {
                    this.handleMenuAction((byte)2);
                    this.keyRepeatDelay = 4;
                    this.heldKey = n;
                }
                if (n == 53) {
                    this.handleMenuAction((byte)3);
                }
                if (!this.pauseMenuOpen || n != 42) break;
                this.cursorDragY = -3;
                this.cursorDragX = -1;
                this.handleMenuAction((byte)3);
                break;
            }
            case 30: {
                this.lastKeyCode = n;
                this.keySubState = 0;
                if (this.dw == 0) {
                    if (n == 42) {
                        this.cycleBuildMenuIndex(false);
                        this.dw = 7;
                        this.currentKey = n;
                    }
                    if (n == 35) {
                        this.cycleBuildMenuIndex(true);
                        this.dw = 7;
                        this.currentKey = n;
                    }
                    if (n == 53) {
                        this.openBuildMenu();
                    }
                    if (n != 48 || this.gameState != 30) break;
                    this.setState(35);
                    break;
                }
                if (this.dw == 1) {
                    if (n == 42 && this.buildMenuMode != 4 && this.buildMenuMode != 2) {
                        this.buildMenuMode = 0;
                        this.buildMenuIndex = 0;
                        this.menuDirty = true;
                    }
                    if (n != 35) break;
                    if (this.buildMenuMode == 1) {
                        this.buildMenuMode = 0;
                        this.buildMenuIndex = 1;
                    }
                    if (this.buildMenuMode == 3) {
                        this.buildMenuMode = 1;
                        this.buildMenuIndex = 3;
                    }
                    if (this.buildMenuMode == 5) {
                        this.buildMenuMode = 1;
                        this.buildMenuIndex = 1;
                    }
                    this.menuDirty = true;
                    break;
                }
                if (n != 42 && n != 35) break;
                this.currentKey = n;
                break;
            }
            case 40: {
                this.lastKeyCode = n;
                this.keySubState = 40;
                if (n == 53) {
                    if (this.confirmToggle == 0) {
                        this.setState(30);
                        this.buildMenuIndex = 0;
                        this.menuDirty = true;
                    } else {
                        this.ready = false;
                        this.transitionCode = 36;
                        this.setState(20);
                    }
                }
                if (n != 42 && n != 35) break;
                if (this.confirmToggle == 0) {
                    this.confirmToggle = 1;
                    this.confirmTargetState = 30;
                } else {
                    this.confirmToggle = 0;
                    this.confirmTargetState = 17;
                }
                this.menuDirty = true;
                this.confirmCounter = 20;
            }
        }
    }

    void d(int n) {
        this.currentKey = 0;
        this.lastKeyCode = 0;
        this.keySubState = 0;
        this.keyRepeatDelay = 0;
        this.heldKey = 0;
        this.dw = 0;
    }

    public void a(String string) {
        this.aU = new XorResourceInput(string);
    }

    public Image aa() {
        byte[] byArray = null;
        int n = 0;
        try {
            int n2;
            int n3 = this.aU.readByte();
            if (n3 == 2) {
                n = 256 * this.aU.readByte() + this.aU.readByte();
                byArray = new byte[n];
                byArray[0] = -119;
                byArray[1] = 80;
                byArray[2] = 78;
                byArray[3] = 71;
                for (n2 = 4; n2 < n; ++n2) {
                    byArray[n2] = (byte)this.aU.readByte();
                }
            }
            if (n3 == 1) {
                int n4;
                n2 = 256 * this.aU.readByte() + this.aU.readByte();
                int n5 = this.aU.readByte() * 3;
                int n6 = this.aU.readByte();
                n = n2 + n5 + 69 + n6 * 13;
                byArray = new byte[n];
                byArray[0] = -119;
                byArray[1] = 80;
                byArray[2] = 78;
                byArray[3] = 71;
                byArray[4] = 13;
                byArray[5] = 10;
                byArray[6] = 26;
                byArray[7] = 10;
                byArray[11] = 13;
                byArray[12] = 73;
                byArray[13] = 72;
                byArray[14] = 68;
                byArray[15] = 82;
                byArray[18] = (byte)this.aU.readByte();
                byArray[19] = (byte)this.aU.readByte();
                byArray[22] = (byte)this.aU.readByte();
                byArray[23] = (byte)this.aU.readByte();
                byArray[24] = (byte)this.aU.readByte();
                byArray[25] = 3;
                byArray[29] = (byte)this.aU.readByte();
                byArray[30] = (byte)this.aU.readByte();
                byArray[31] = (byte)this.aU.readByte();
                byArray[32] = (byte)this.aU.readByte();
                byArray[35] = (byte)(n5 >> 8 & 0xFF);
                byArray[36] = (byte)(n5 & 0xFF);
                byArray[37] = 80;
                byArray[38] = 76;
                byArray[39] = 84;
                byArray[40] = 69;
                for (n4 = 0; n4 < n5 + 4; ++n4) {
                    byArray[41 + n4] = (byte)this.aU.readByte();
                }
                n4 = 41 + n5 + 4;
                if (n6 == 1) {
                    n4 += 3;
                    byArray[n4++] = 1;
                    byArray[n4++] = 116;
                    byArray[n4++] = 82;
                    byArray[n4++] = 78;
                    byArray[n4++] = 83;
                    int n7 = ++n4;
                    byArray[n7] = 64;
                    int n8 = ++n4;
                    byArray[n8] = -26;
                    int n9 = ++n4;
                    byArray[n9] = -40;
                    int n10 = ++n4;
                    ++n4;
                    byArray[n10] = 102;
                }
                n4 += 2;
                byArray[n4++] = (byte)(n2 >> 8 & 0xFF);
                byArray[n4++] = (byte)(n2 & 0xFF);
                byArray[n4++] = 73;
                byArray[n4++] = 68;
                byArray[n4++] = 65;
                byArray[n4++] = 84;
                for (int i = 0; i < n2 + 4; ++i) {
                    byArray[n4++] = (byte)this.aU.readByte();
                }
                n4 += 4;
                byArray[n4++] = 73;
                byArray[n4++] = 69;
                byArray[n4++] = 78;
                byArray[n4++] = 68;
                byArray[n4++] = -82;
                byArray[n4++] = 66;
                byArray[n4++] = 96;
                byArray[n4++] = -126;
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
        return Image.createImage(byArray, (int)0, (int)n);
    }

    public void o() {
        this.cursorScale = 0;
        this.cursorDragY = -1;
        this.cursorSelectY = -1;
        this.cursorSelectX = -1;
        this.cursorOverUI = false;
        this.cursorAllowZoomOut = false;
        this.dialogContentHeight = 0;
        this.needsRepaint = false;
    }

    public void b(String string) {
        try {
            XorResourceInput xorResourceInput = new XorResourceInput(string);
            int n = xorResourceInput.readByte();
            int n2 = xorResourceInput.readByte();
            int n3 = 256 * xorResourceInput.readByte() + xorResourceInput.readByte();
            int n4 = 0;
            this.spriteOffsetTable = new short[n2 + 1];
            this.spriteData = new byte[n3];
            for (int i = 0; i < n; ++i) {
                int n5 = xorResourceInput.readByte();
                int n6 = 256 * xorResourceInput.readByte() + xorResourceInput.readByte();
                this.spriteOffsetTable[n5] = (short)n4;
                for (int j = 0; j < n6; ++j) {
                    this.spriteData[n4] = (byte)xorResourceInput.readByte();
                    ++n4;
                }
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    byte[] m(int n) {
        byte by = this.spriteData[n];
        byte[] byArray = new byte[by];
        System.arraycopy(this.spriteData, n + 1, byArray, 0, by);
        return byArray;
    }

    public void a(Graphics graphics, int n) {
        int n2;
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        int n8;
        int n9;
        int n10;
        int n11;
        byte by;
        int n12;
        int n13;
        if (this.lastRenderedSpriteId != n) {
            this.spriteJustChanged = 1;
            this.lastRenderedSpriteId = n;
        }
        if (this.spriteJustChanged != 0) {
            this.needsRepaint = false;
        }
        if (this.mapRedrawPending) {
            this.menuDirty = true;
            this.renderMap(graphics);
            this.mapRedrawPending = false;
        }
        short s = this.spriteOffsetTable[n];
        int n14 = 65536 * this.c(this.spriteData[s + 1]) + 256 * this.c(this.spriteData[s + 2]) + this.c(this.spriteData[s + 3]);
        int n15 = 65536 * this.c(this.spriteData[s + 4]) + 256 * this.c(this.spriteData[s + 5]) + this.c(this.spriteData[s + 6]);
        int n16 = 65536 * this.c(this.spriteData[s + 7]) + 256 * this.c(this.spriteData[s + 8]) + this.c(this.spriteData[s + 9]);
        int n17 = 65536 * this.c(this.spriteData[s + 10]) + 256 * this.c(this.spriteData[s + 11]) + this.c(this.spriteData[s + 12]);
        int n18 = this.c(this.spriteData[s + 13]);
        int n19 = this.c(this.spriteData[s + 14]);
        if (this.dialogContentHeight < n19) {
            this.dialogContentHeight = n19;
        } else {
            n19 = this.dialogContentHeight;
        }
        if (this.cursorOverUI || this.cursorAllowZoomOut) {
            n18 += 6;
        }
        this.currentSpriteId = this.spriteData[s + 15];
        int n20 = (e - n18) / 2;
        int n21 = (hs - n19) / 2;
        int n22 = 6;
        int n23 = 6;
        int n24 = 6;
        if (this.needsRepaint) {
            graphics.setColor(n14);
            graphics.fillRect(n20, n21, n18, n19);
            graphics.setColor(n16);
            graphics.drawRect(n20, n21, n18, n19);
            graphics.setColor(n15);
            graphics.drawRect(n20, n21, 0, n19);
            graphics.drawRect(n20, n21, n18, 0);
        }
        this.pauseMenuOpen = false;
        if ((this.spriteData[s] & 1) == 1) {
            if (this.needsRepaint && this.cursorDragY == -1) {
                graphics.setColor(n17);
                graphics.fillRect(n20 + n18 - 10, n21 + 3, 7, 7);
            }
            if (this.needsRepaint) {
                graphics.setColor(n16);
                graphics.drawRect(n20 + n18 - 10, n21 + 3, 7, 7);
                graphics.drawLine(n20 + n18 - 8, n21 + 5, n20 + n18 - 5, n21 + 8);
                graphics.drawLine(n20 + n18 - 8, n21 + 8, n20 + n18 - 5, n21 + 5);
            }
            this.pauseMenuOpen = true;
            n24 = 19;
        } else if (this.cursorDragY == -1) {
            this.cursorDragY = -3;
        }
        int n25 = s + 16;
        if ((this.spriteData[s] & 2) == 2) {
            if (this.needsRepaint) {
                graphics.setColor(n16);
            }
            if (this.needsRepaint) {
                graphics.drawRect(n20 + 2, n21 + 13, n18 - 4, 0);
            }
            this.clipX = n20;
            this.clipY = n21;
            this.clipWidth = n18;
            this.clipHeight = n19;
            if (this.needsRepaint) {
                this.a(graphics, n20 + 3, n21 + 5, this.m(s + 16), true);
            }
            n25 += 1 + this.spriteData[s + 16];
            n22 = 19;
            n23 = 19;
            n24 = 19;
        }
        int n26 = n20 + 4;
        this.clipX = n20 + 1;
        this.clipY = n21 + n22 - 4;
        this.clipWidth = n18 - 2;
        this.clipHeight = n19 - n22 + 3;
        graphics.setClip(this.clipX, this.clipY, this.clipWidth, this.clipHeight);
        if (this.needsRepaint && this.cursorOverUI) {
            graphics.setColor(n16);
            graphics.drawRect(n20 + n18 - 6, n21 + n24 - 1, 4, 0);
            graphics.drawRect(n20 + n18 - 5, n21 + n24 - 2, 2, 0);
            graphics.drawRect(n20 + n18 - 4, n21 + n24 - 3, 0, 0);
        }
        if (this.needsRepaint && this.cursorAllowZoomOut) {
            graphics.setColor(n16);
            graphics.drawRect(n20 + n18 - 6, n21 + n19 - 5, 4, 0);
            graphics.drawRect(n20 + n18 - 5, n21 + n19 - 4, 2, 0);
            graphics.drawRect(n20 + n18 - 4, n21 + n19 - 3, 0, 0);
        }
        this.cursorSelectY = -2;
        this.cursorSelectX = -2;
        this.cursorOverUI = false;
        this.cursorAllowZoomOut = false;
        this.cursorDragX = -1;
        int n27 = this.spriteData[n25++];
        for (n13 = 0; n13 < n27; ++n13) {
            n12 = n21 + n22 - this.cursorScale;
            if (((by = this.spriteData[n25++]) & 0x80) == 0) {
                int n28;
                n11 = (by & 4) == 4 ? 0 : this.spriteData[n25++];
                n10 = (by & 2) == 2 ? 0 : this.spriteData[n25++];
                n9 = (by & 0x40) == 64 ? 1 : 0;
                n8 = (by & 0x20) == 32 ? 1 : 0;
                n7 = n9 != 0 ? this.spriteData[n25++] : 0;
                n6 = n8 != 0 ? this.spriteData[n25++] : 0;
                n5 = (by & 0x10) == 16 ? this.spriteData[n25++] : 0;
                n4 = (by & 0x10) == 16 ? this.spriteData[n25++] : 0;
                n3 = (by & 0x10) == 16 ? this.spriteData[n25++] : 0;
                n2 = (by & 0x10) == 16 ? this.spriteData[n25++] : 0;
                byte by2 = (by & 0x10) == 16 ? this.spriteData[n25++] : (byte)0;
                byte[] byArray = n8 != 0 ? this.q(this.currentSpriteId, n6) : this.m(n25);
                int n29 = 0;
                if (byArray == null) continue;
                for (n28 = 0; n28 < byArray.length; ++n28) {
                    if (byArray[n28] >= 32 && byArray[n28] <= 39) continue;
                    ++n29;
                }
                if (n8 == 0) {
                    n25 = n25 + this.spriteData[n25] + 1;
                }
                if (n12 + n10 + 6 + by2 >= this.clipY + this.clipHeight) {
                    n28 = n19 + (n12 + n10 + 6 + by2 + n3 + 3 - (this.clipHeight + this.clipY));
                    if (this.dialogContentHeight < n28) {
                        this.dialogContentHeight = n28;
                    }
                    if (this.dialogContentHeight > hs - 20) {
                        this.dialogContentHeight = hs - 20;
                    }
                }
                if (this.needsRepaint && n12 + n10 + 6 + by2 >= this.clipY + this.clipHeight) {
                    this.cursorAllowZoomOut = true;
                }
                if (n9 != 0 && (n9 == 0 || !this.e(this.currentSpriteId, n7))) continue;
                if (n12 + n10 - n3 - 3 <= this.clipY + this.clipHeight && n12 + n10 + 8 + by2 >= this.clipY) {
                    if ((by & 0x10) == 16) {
                        if (this.cursorDragY == -3) {
                            this.cursorDragY = n13;
                        }
                        if (n12 + n10 + 8 + by2 <= this.clipY + this.clipHeight && n12 + n10 - 3 - n3 >= this.clipY) {
                            if (n13 < this.cursorDragY) {
                                this.cursorSelectY = n13;
                            }
                            if (this.cursorSelectX == -2 && n13 > this.cursorDragY) {
                                this.cursorSelectX = n13;
                            }
                        }
                        if (this.needsRepaint && this.cursorDragY == n13) {
                            graphics.setColor(n17);
                            graphics.fillRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                            this.cursorDragX = n5;
                        }
                        if (this.needsRepaint && (by & 8) == 8) {
                            graphics.setColor(n16);
                            graphics.drawRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                            graphics.setColor(n15);
                            graphics.drawRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 0, 10 + n3 + by2);
                            graphics.drawRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 0);
                        }
                    }
                    if (this.needsRepaint && this.needsRepaint) {
                        this.a(graphics, n26 + n11, n12 + n10, byArray, true);
                    }
                }
                n22 += 11 + n10;
                continue;
            }
            n11 = (by & 0x40) == 64 ? 1 : 0;
            n10 = this.spriteData[n25++];
            n9 = (by & 0x20) == 32 ? 0 : this.spriteData[n25++];
            n8 = (by & 0x10) == 16 ? 0 : this.spriteData[n25++];
            n7 = n11 != 0 ? this.spriteData[n25++] : 0;
            Image image = this.u(this.currentSpriteId, n10);
            if (image == null) continue;
            n5 = image.getWidth();
            n4 = image.getHeight();
            if (n11 != 0 && (n11 == 0 || !this.e(this.currentSpriteId, n7))) continue;
            if (this.needsRepaint && n12 + n8 <= this.clipY + this.clipHeight && n12 + n8 + n4 >= this.clipY) {
                graphics.drawImage(image, n26 + n9, n12 + n8, 20);
            }
            if (this.needsRepaint && n12 + n8 + n4 >= this.clipY + this.clipHeight) {
                this.cursorAllowZoomOut = true;
            }
            n22 += n4 + n8;
        }
        n27 = this.spriteData[n25++];
        for (n13 = 0; n13 < n27; ++n13) {
            n12 = n21 + n23 - this.cursorScale;
            by = this.spriteData[n25++];
            n11 = by & 7;
            n10 = 65536 * this.c(this.spriteData[n25++]) + 256 * this.c(this.spriteData[n25++]) + this.c(this.spriteData[n25++]);
            n9 = this.c(this.spriteData[n25++]) + 256 * this.c(this.spriteData[n25++]);
            n8 = this.c(this.spriteData[n25++]) + 256 * this.c(this.spriteData[n25++]);
            n7 = this.c(this.spriteData[n25++]) + 256 * this.c(this.spriteData[n25++]);
            n6 = this.c(this.spriteData[n25++]) + 256 * this.c(this.spriteData[n25++]);
            n9 = (short)((by & 8) == 8 ? this.neighboursMask(this.currentSpriteId, n9) : n9);
            n8 = (short)((by & 0x10) == 16 ? this.neighboursMask(this.currentSpriteId, n8) : n8);
            n7 = (short)((by & 0x20) == 32 ? this.neighboursMask(this.currentSpriteId, n7) : n7);
            n6 = (short)((by & 0x40) == 64 ? this.neighboursMask(this.currentSpriteId, n6) : n6);
            n5 = (by & 0x80) == 128 ? 1 : 0;
            n4 = n5 != 0 ? this.spriteData[n25++] : 0;
            n3 = n12 + n8;
            n2 = n3 + n6;
            if (!this.needsRepaint || n5 != 0 && (n5 == 0 || !this.e(this.currentSpriteId, n4))) continue;
            switch (n11) {
                case 0: {
                    graphics.setColor(n10);
                    graphics.drawLine(n26 + n9, n12 + n8, n26 + n7, n12 + n6);
                    n3 = Math.min(n12 + n8, n12 + n6);
                    n2 = Math.max(n12 + n8, n12 + n6);
                    break;
                }
                case 1: {
                    graphics.setColor(n10);
                    graphics.drawRect(n26 + n9, n12 + n8, n7, n6);
                    break;
                }
                case 2: {
                    graphics.setColor(n10);
                    graphics.fillRect(n26 + n9, n12 + n8, n7, n6);
                    break;
                }
                case 3: {
                    graphics.setColor(n10);
                    graphics.drawArc(n26 + n9, n12 + n8, n7, n6, 0, 360);
                    break;
                }
                case 4: {
                    graphics.setColor(n10);
                    graphics.fillArc(n26 + n9, n12 + n8, n7, n6, 0, 360);
                }
            }
            if (!this.needsRepaint || n2 < this.clipY + this.clipHeight) continue;
            this.cursorAllowZoomOut = true;
        }
        if (this.needsRepaint) {
            if (this.cursorScale == 0 && this.cursorSelectY == -2) {
                this.cursorSelectY = -1;
            }
            if (this.cursorScale > 0) {
                this.cursorOverUI = true;
            }
        }
        this.needsRepaint = true;
        if (this.spriteJustChanged > 0) {
            --this.spriteJustChanged;
        }
    }

    public void a(Graphics graphics, int n, int n2, byte[] byArray, boolean bl) {
        int n3 = n;
        graphics.setColor(0);
        for (int i = 0; i < byArray.length; ++i) {
            byte by = byArray[i];
            if (bl) {
                graphics.setClip(this.clipX, this.clipY, this.clipWidth, this.clipHeight);
                graphics.clipRect(n3, n2 - 4, 5, 12);
            } else {
                graphics.setClip(n3, n2 - 4, 5, 12);
            }
            if (by == 0) {
                n3 += 6;
                continue;
            }
            if (by < 32) {
                graphics.drawImage(this.ca[(by - 1) / 16], n3 - 5 * ((by - 1) % 16), n2, 20);
                n3 += 6;
                continue;
            }
            if (by < 40) {
                byte by2 = (byte)(byArray[i + 1] >= 40 && byArray[i + 1] <= 65 ? 1 : 0);
                switch (by) {
                    case 32: {
                        graphics.drawLine(n3 + 2, n2 - 2 + by2, n3 + 2, n2 - 2 + by2);
                        break;
                    }
                    case 35: {
                        graphics.drawLine(n3 + 2, n2 - 4 + by2, n3 + 2, n2 - 4 + by2);
                    }
                    case 34: {
                        graphics.drawLine(n3 + 2, n2 - 2 + by2, n3 + 1, n2 - 3 + by2);
                    }
                    case 33: {
                        graphics.drawLine(n3 + 2, n2 - 2 + by2, n3 + 3, n2 - 3 + by2);
                        break;
                    }
                    case 37: {
                        graphics.drawLine(n3 + 1, n2 - 2 + by2, n3 + 1, n2 - 2 + by2);
                        graphics.drawLine(n3 + 3, n2 - 2 + by2, n3 + 3, n2 - 2 + by2);
                    }
                }
                continue;
            }
            if (by < 70) {
                graphics.drawImage(this.ca[2 + (by - 40) / 16], n3 - 5 * ((by - 40) % 16), n2, 20);
                n3 += 6;
                continue;
            }
            if (by >= 100) continue;
            graphics.drawImage(this.ca[4 + (by - 70) / 16], n3 - 5 * ((by - 70) % 16), n2, 20);
            n3 += 6;
        }
        if (bl) {
            graphics.setClip(this.clipX, this.clipY, this.clipWidth, this.clipHeight);
        } else {
            graphics.setClip(0, 0, e, hs);
        }
    }

    public void handleMenuAction(byte by) {
        switch (by) {
            case 1: {
                if (this.cursorSelectX != -2) {
                    this.cursorDragY = this.cursorSelectX;
                    break;
                }
                if (!this.cursorAllowZoomOut) break;
                this.cursorScale += 10;
                break;
            }
            case 2: {
                if (this.cursorSelectY > -1) {
                    this.cursorDragY = this.cursorSelectY;
                } else if (this.cursorOverUI) {
                    this.cursorScale -= 10;
                } else {
                    this.cursorDragY = -1;
                }
                if (this.cursorScale >= 0) break;
                this.cursorScale = 0;
                break;
            }
            case 3: {
                this.v(this.currentSpriteId, this.cursorDragX);
            }
        }
    }

    void z(int n, int n2) {
        this.selectionStartX = n - 5;
        this.selectionStartY = n2 - 5;
        if (this.selectionStartX < 0) {
            this.selectionStartX = 0;
        }
        if (this.selectionStartY < 0) {
            this.selectionStartY = 0;
        }
        if (this.selectionStartX + 10 >= this.mapWidth) {
            this.selectionStartX = this.mapWidth - 9;
        }
        if (this.selectionStartY + 10 >= this.mapHeight) {
            this.selectionStartY = this.mapHeight - 9;
        }
        this.terrainEditBuffer = new byte[10][10];
        for (int i = 0; i < 10; ++i) {
            System.arraycopy(this.heightMap[this.selectionStartX + i], this.selectionStartY, this.terrainEditBuffer[i], 0, 10);
        }
        this.terrainSmoothCost = 0;
    }

    boolean a(int n, int n2, int n3, boolean bl) {
        boolean bl2 = true;
        if (n > 0) {
            if (this.tileMap[n - 1][n2] > 33) {
                bl2 = false;
            }
            if (n2 > 0 && this.tileMap[n - 1][n2 - 1] > 33) {
                bl2 = false;
            }
        }
        if (n2 > 0 && this.tileMap[n][n2 - 1] > 33) {
            bl2 = false;
        }
        if (this.tileMap[n][n2] > 33) {
            bl2 = false;
        }
        int n4 = n - this.selectionStartX;
        int n5 = n2 - this.selectionStartY;
        if (bl2) {
            this.terrainEditBuffer[n4][n5] = (byte)n3;
            if (n4 > 0 && (bl ? this.terrainEditBuffer[n4 - 1][n5] < n3 - 1 : this.terrainEditBuffer[n4 - 1][n5] > n3 + 1) && !this.a(n - 1, n2, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
            if (n5 > 0 && (bl ? this.terrainEditBuffer[n4][n5 - 1] < n3 - 1 : this.terrainEditBuffer[n4][n5 - 1] > n3 + 1) && !this.a(n, n2 - 1, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
            if (n4 < 9 && (bl ? this.terrainEditBuffer[n4 + 1][n5] < n3 - 1 : this.terrainEditBuffer[n4 + 1][n5] > n3 + 1) && !this.a(n + 1, n2, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
            if (n5 < 9 && (bl ? this.terrainEditBuffer[n4][n5 + 1] < n3 - 1 : this.terrainEditBuffer[n4][n5 + 1] > n3 + 1) && !this.a(n, n2 + 1, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
        }
        this.terrainSmoothCost += this.costOverhead + (n3 == -1 ? this.costMaintenance : Math.abs(n3) * this.costCrew);
        return bl2;
    }

    void ac() {
        for (int i = 0; i < 10; ++i) {
            System.arraycopy(this.terrainEditBuffer[i], 0, this.heightMap[this.selectionStartX + i], this.selectionStartY, 10);
        }
    }

    void smoothHeightMap(boolean bl) {
        int n;
        int n2;
        int n3;
        int n4;
        if (bl) {
            this.ac();
            this.terrainEditBuffer = null;
            n4 = this.selectionStartX;
            n3 = this.selectionStartY;
            n2 = this.selectionStartX + 10;
            n = this.selectionStartY + 10;
        } else {
            n4 = 0;
            n3 = 0;
            n2 = this.mapWidth;
            n = this.mapHeight;
        }
        for (int i = n4; i < n2; ++i) {
            for (int j = n3; j < n; ++j) {
                byte by = this.heightMap[i][j];
                byte by2 = this.heightMap[i][j + 1];
                byte by3 = this.heightMap[i + 1][j];
                byte by4 = this.heightMap[i + 1][j + 1];
                if (this.tileMap[i][j] >= 34) continue;
                if (by == by2 && by == by3 && by == by4) {
                    this.tileElevation[i][j] = by;
                    if (this.tileMap[i][j] <= 28) {
                        this.tileMap[i][j] = 0;
                    }
                } else if (by == by2 && by == by3 && by4 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = 11;
                }
                if (by == by3 && by == by4 && by2 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = 13;
                }
                if (by == by2 && by == by4 && by3 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = 14;
                }
                if (by2 == by3 && by2 == by4 && by < by2) {
                    this.tileElevation[i][j] = by2;
                    this.tileMap[i][j] = 12;
                }
                if (by == by4 && by2 == by3 && by2 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = 16;
                }
                if (by == by4 && by2 == by3 && by < by2) {
                    this.tileElevation[i][j] = by2;
                    this.tileMap[i][j] = 15;
                }
                if (by == by2 && by3 == by4 && by3 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = (byte)(by3 == -1 && i + 2 <= this.mapWidth && (this.heightMap[i + 2][j] == -1 || this.heightMap[i + 2][j + 1] == -1) ? 9 : 24);
                }
                if (by == by2 && by3 == by4 && by < by3) {
                    this.tileElevation[i][j] = by3;
                    this.tileMap[i][j] = (byte)(by == -1 && i - 1 >= 0 && (this.heightMap[i - 1][j] == -1 || this.heightMap[i - 1][j + 1] == -1) ? 8 : 22);
                }
                if (by == by3 && by2 == by4 && by2 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = (byte)(by2 == -1 && j + 2 <= this.mapHeight && (this.heightMap[i][j + 2] == -1 || this.heightMap[i + 1][j + 2] == -1) ? 10 : 21);
                }
                if (by == by3 && by2 == by4 && by < by2) {
                    this.tileElevation[i][j] = by2;
                    this.tileMap[i][j] = (byte)(by == -1 && j - 1 >= 0 && (this.heightMap[i][j - 1] == -1 || this.heightMap[i + 1][j - 1] == -1) ? 7 : 23);
                }
                if (by == by2 && by == by3 && by < by4) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = (byte)(by == -1 ? 3 : 19);
                }
                if (by == by2 && by == by4 && by < by3) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = (byte)(by == -1 ? 6 : 18);
                }
                if (by == by3 && by == by4 && by < by2) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = (byte)(by == -1 ? 5 : 20);
                }
                if (by2 == by3 && by2 == by4 && by4 < by) {
                    this.tileElevation[i][j] = by4;
                    this.tileMap[i][j] = (byte)(by4 == -1 ? 4 : 17);
                }
                if (by == by4 && by2 < by && by3 > by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = 25;
                }
                if (by3 == by2 && by < by3 && by4 > by3) {
                    this.tileElevation[i][j] = by3;
                    this.tileMap[i][j] = 26;
                }
                if (by == by4 && by2 > by && by3 < by) {
                    this.tileElevation[i][j] = by;
                    this.tileMap[i][j] = 27;
                }
                if (by3 != by2 || by <= by3 || by4 >= by3) continue;
                this.tileElevation[i][j] = by3;
                this.tileMap[i][j] = 28;
            }
        }
    }

    void a(int n, int n2, boolean bl) {
        if (bl && this.heightMap[n][n2] < 3 || !bl && this.heightMap[n][n2] > -1) {
            this.z(n, n2);
            if (this.a(n, n2, this.heightMap[n][n2] + (bl ? 1 : -1), bl)) {
                if (this.initialIndustryPlaced) {
                    if (this.terrainSmoothCost <= this.money) {
                        this.money -= this.terrainSmoothCost;
                        this.p(-this.terrainSmoothCost);
                        this.ho += this.terrainSmoothCost;
                        this.smoothHeightMap(true);
                    } else {
                        this.enterOutOfMoneyState();
                    }
                } else {
                    this.smoothHeightMap(true);
                }
            }
        }
        this.bH = -1;
    }

    void c(int n, int n2, int n3, int n4, int n5) {
        for (int i = n; i < n + n3; ++i) {
            for (int j = n2; j < n2 + n4; ++j) {
                if (i <= 0 || j <= 0 || i > this.mapWidth || j > this.mapHeight) continue;
                this.z(i, j);
                this.a(i, j, n5, true);
                if (this.rng.nextInt() % 2 < 0 && i > 0) {
                    this.a(i - 1, j, n5, true);
                }
                if (this.rng.nextInt() % 2 < 0 && j > 0) {
                    this.a(i, j - 1, n5, true);
                }
                if (this.rng.nextInt() % 2 < 0 && i < this.mapWidth) {
                    this.a(i + 1, j, n5, true);
                }
                if (this.rng.nextInt() % 2 < 0 && j < this.mapHeight) {
                    this.a(i, j + 1, n5, true);
                }
                this.ac();
            }
        }
    }

    void b(int n, int n2, int n3, int n4, int n5) {
        for (int i = n; i < n + n3; ++i) {
            for (int j = n2; j < n2 + n4; ++j) {
                if (i <= 0 || j <= 0 || i > this.mapWidth || j > this.mapHeight) continue;
                for (int k = 0; k < 3; ++k) {
                    this.a(i, j, false);
                }
                if (this.rng.nextInt() % 2 < 0 && i > 0) {
                    this.a(i - 1, j, false);
                }
                if (this.rng.nextInt() % 2 < 0 && j > 0) {
                    this.a(i, j - 1, false);
                }
                if (this.rng.nextInt() % 2 < 0 && i < this.mapWidth) {
                    this.a(i + 1, j, false);
                }
                if (this.rng.nextInt() % 2 >= 0 || j >= this.mapHeight) continue;
                this.a(i, j + 1, false);
            }
        }
    }

    void loadState25Setup(int n) {
        if (n == 0) {
            this.tileMap = new byte[this.mapWidth + 1][this.mapHeight + 1];
            this.trackMap = new byte[this.mapWidth + 1][this.mapHeight + 1];
            this.tileElevation = new byte[this.mapWidth + 1][this.mapHeight + 1];
            this.heightMap = new byte[this.mapWidth + 2][this.mapHeight + 2];
            this.initialIndustryPlaced = false;
            this.mapGenStage = 0;
            this.tilesGeneratedInStage = 0;
            this.totalTilesToGenerate = this.hf + this.archiveOrRecallMethod + this.gG * (this.gG - 1) + this.co + this.weightAccel2 + this.weightAccel + this.bX + this.dK + this.weightAccel4 + this.weightAccel3 + 1;
            this.mapGenTilesDone = 0;
        }
        if (n == 1) {
            int n2;
            int n3;
            int n4;
            if (this.mapGenStage == 0) {
                this.mapGenStage = 1;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 1 && this.tilesGeneratedInStage == this.hf) {
                this.mapGenStage = 2;
                this.tilesGeneratedInStage = 0;
                this.smoothHeightMap(false);
            }
            if (this.mapGenStage == 2 && this.tilesGeneratedInStage == this.archiveOrRecallMethod) {
                this.mapGenStage = 3;
                this.tilesGeneratedInStage = 0;
                this.smoothHeightMap(false);
            }
            if (this.mapGenStage == 3 && this.tilesGeneratedInStage == this.gG * (this.gG - 1) + this.co) {
                this.mapGenStage = 4;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 4 && this.tilesGeneratedInStage == this.weightAccel2) {
                this.mapGenStage = 5;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 5 && this.tilesGeneratedInStage == this.weightAccel) {
                this.mapGenStage = 6;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 6 && this.tilesGeneratedInStage == this.bX) {
                this.mapGenStage = 7;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 7 && this.tilesGeneratedInStage == this.dK) {
                this.mapGenStage = 8;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 8 && this.tilesGeneratedInStage == this.weightAccel4) {
                this.mapGenStage = 9;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 9 && this.tilesGeneratedInStage == this.weightAccel3) {
                this.mapGenStage = 10;
                this.tilesGeneratedInStage = 0;
            }
            if (this.mapGenStage == 1) {
                this.c(Math.abs(this.rng.nextInt() % this.mapWidth), Math.abs(this.rng.nextInt() % this.mapHeight), Math.abs(this.rng.nextInt() % 20) + 1, Math.abs(this.rng.nextInt() % 20) + 1, Math.abs(this.rng.nextInt() % 2) + 1);
            }
            if (this.mapGenStage == 2) {
                this.b(Math.abs(this.rng.nextInt() % this.mapWidth), Math.abs(this.rng.nextInt() % this.mapHeight), Math.abs(this.rng.nextInt() % 20) + 1, Math.abs(this.rng.nextInt() % 20) + 1, -1);
            }
            this.fx = false;
            if (this.mapGenStage == 3) {
                block0: for (n4 = 0; n4 < this.gG; ++n4) {
                    for (n3 = 0; n3 < this.co; ++n3) {
                        if (this.tilesGeneratedInStage != this.gG * n4 + n3) continue;
                        n2 = this.mapWidth / this.gG;
                        int n5 = this.mapHeight / this.co;
                        int n6 = n4 * n2 + Math.abs(this.rng.nextInt() % (n2 / 2));
                        int n7 = n3 * n5 + Math.abs(this.rng.nextInt() % (n5 / 2));
                        this.w(n6, n7);
                        continue block0;
                    }
                }
            }
            if (this.mapGenStage == 4) {
                this.u(0);
            }
            if (this.mapGenStage == 5) {
                this.u(1);
            }
            if (this.mapGenStage == 6) {
                this.u(2);
            }
            if (this.mapGenStage == 7) {
                this.u(3);
            }
            if (this.mapGenStage == 8) {
                this.u(4);
            }
            if (this.mapGenStage == 9) {
                this.u(5);
            }
            if (this.fx) {
                this.loadStep = 0;
            }
            if (this.mapGenStage == 10) {
                for (n4 = 0; n4 < this.dQ; ++n4) {
                    n3 = Math.abs(this.rng.nextInt() % this.mapWidth);
                    if (this.tileMap[n3][n2 = Math.abs(this.rng.nextInt() % this.mapHeight)] != 0 || this.tileElevation[n3][n2] < 0) continue;
                    this.tileMap[n3][n2] = (byte)(29 + Math.abs(this.rng.nextInt() % 5));
                }
                this.initialIndustryPlaced = true;
            }
            ++this.tilesGeneratedInStage;
            ++this.mapGenTilesDone;
        }
    }

    byte p() {
        int n = Math.abs(this.rng.nextInt() % 22);
        if (n < 6) {
            return 34;
        }
        if (n < 11) {
            return 35;
        }
        if (n < 15) {
            return 36;
        }
        if (n < 18) {
            return 37;
        }
        if (n < 20) {
            return 38;
        }
        if (n < 21) {
            return 39;
        }
        if (n < 22) {
            return 40;
        }
        return 0;
    }

    void w(int n, int n2) {
        int n3;
        int n4;
        byte by;
        byte by2;
        byte by3;
        byte by4;
        int n5;
        int n6;
        int n7 = Math.abs(this.rng.nextInt() % 5) + this.cK;
        byte by5 = Math.abs(this.rng.nextInt() % 5) + this.cK;
        byte[][] byArray = new byte[n7][by5];
        boolean bl = false;
        int n8 = Math.abs(this.rng.nextInt() % (n7 - 2)) + 1;
        for (n6 = Math.abs(this.rng.nextInt() % 3); n6 < by5 - Math.abs(this.rng.nextInt() % 3); ++n6) {
            byArray[n8][n6] = 1;
        }
        if (n7 >= 7) {
            do {
                n8 = Math.abs(this.rng.nextInt() % (n7 - 2)) + 1;
                bl = false;
                for (n6 = 0; n6 < by5; ++n6) {
                    if (byArray[n8 - 1][n6] != 1 && byArray[n8 + 1][n6] != 1) continue;
                    bl = true;
                }
            } while (bl);
            for (n6 = Math.abs(this.rng.nextInt() % 3); n6 < by5 - Math.abs(this.rng.nextInt() % 3); ++n6) {
                byArray[n8][n6] = 1;
            }
        }
        n6 = Math.abs(this.rng.nextInt() % (by5 - 2)) + 1;
        for (n5 = Math.abs(this.rng.nextInt() % 3); n5 < n7 - Math.abs(this.rng.nextInt() % 3); ++n5) {
            byArray[n5][n6] = 2;
        }
        if (by5 >= 7) {
            do {
                n6 = Math.abs(this.rng.nextInt() % (by5 - 2)) + 1;
                bl = false;
                for (n5 = 0; n5 < n7; ++n5) {
                    if (byArray[n5][n6 - 1] != 2 && byArray[n5][n6 + 1] != 2) continue;
                    bl = true;
                }
            } while (bl);
        }
        for (n5 = 0; n5 < n7; ++n5) {
            for (by4 = 0; by4 < by5; ++by4) {
                if (byArray[n5][by4] != 1 && byArray[n5][by4] != 2) continue;
                by3 = n5 > 0 && byArray[n5 - 1][by4] > 0 ? (byte)1 : 0;
                n3 = (by3 != 0 ? 1 : 0) + ((by2 = n5 < n7 - 1 && byArray[n5 + 1][by4] > 0 ? (byte)1 : 0) != 0 ? 1 : 0) + ((by = by4 < by5 - 1 && byArray[n5][by4 + 1] > 0 ? (byte)1 : 0) != 0 ? 1 : 0) + ((n4 = by4 > 0 && byArray[n5][by4 - 1] > 0 ? 1 : 0) != 0 ? 1 : 0);
                if (n3 > 2) {
                    byArray[n5][by4] = 44;
                    continue;
                }
                if (by2 != 0 && n4 != 0) {
                    byArray[n5][by4] = 45;
                    continue;
                }
                if (by2 != 0 && by != 0) {
                    byArray[n5][by4] = 46;
                    continue;
                }
                if (by3 != 0 && by != 0) {
                    byArray[n5][by4] = 47;
                    continue;
                }
                if (by3 != 0 && n4 != 0) {
                    byArray[n5][by4] = 48;
                    continue;
                }
                if ((by3 != 0 || by2 != 0) && by == 0 && n4 == 0) {
                    byArray[n5][by4] = 43;
                    continue;
                }
                if (n4 == 0 && by == 0 || by3 != 0 || by2 != 0) continue;
                byArray[n5][by4] = 42;
            }
        }
        for (n5 = 0; n5 < n7; ++n5) {
            for (by4 = 0; by4 < by5; ++by4) {
                if (byArray[n5][by4] != 0) continue;
                by3 = 1;
                by2 = n5 > 0 ? byArray[n5 - 1][by4] : (byte)0;
                n4 = n5 < n7 - 1 ? byArray[n5 + 1][by4] : 0;
                by = by4 > 0 ? byArray[n5][by4 - 1] : (byte)0;
                int n9 = n3 = by4 < by5 - 1 ? byArray[n5][by4 + 1] : 0;
                if (by2 < 42 && n4 < 42 && by < 42 && n3 < 42) {
                    by3 = 0;
                }
                if (by2 == 44 || n4 == 44 || by == 44 || n3 == 44) {
                    by3 = 0;
                }
                if (by2 == 45 || by2 == 46 || by2 == 43) {
                    by3 = 0;
                }
                if (n4 == 47 || n4 == 48 || n4 == 43) {
                    by3 = 0;
                }
                if (by == 46 || by == 47 || by == 42) {
                    by3 = 0;
                }
                if (n3 == 45 || n3 == 48 || n3 == 42) {
                    by3 = 0;
                }
                if (by2 == 44) {
                    byArray[n5][by4] = 43;
                }
                if (n4 == 44) {
                    byArray[n5][by4] = 43;
                }
                if (by == 44) {
                    byArray[n5][by4] = 42;
                }
                if (n3 == 44) {
                    byArray[n5][by4] = 42;
                }
                if (by3 == 0) continue;
                byArray[n5][by4] = this.p();
            }
        }
        n5 = -2;
        for (by4 = 0; by4 < n7; ++by4) {
            for (by3 = 0; by3 < by5; ++by3) {
                if (n + by4 >= this.mapWidth || n2 + by3 >= this.mapHeight || byArray[by4][by3] == 0 && (byArray[by4][by3] != 0 || this.rng.nextInt() % 3 >= 0)) continue;
                if (n5 == -2 && (n5 = this.heightMap[n + by4][n2 + by3]) < 0) {
                    n5 = 0;
                }
                long l = System.currentTimeMillis();
                while (n5 != this.heightMap[n + by4][n2 + by3]) {
                    this.a(n + by4, n2 + by3, n5 > this.heightMap[n + by4][n2 + by3]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
                l = System.currentTimeMillis();
                while (n5 != this.heightMap[n + by4 + 1][n2 + by3]) {
                    this.a(n + by4 + 1, n2 + by3, n5 > this.heightMap[n + by4 + 1][n2 + by3]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
                l = System.currentTimeMillis();
                while (n5 != this.heightMap[n + by4][n2 + by3 + 1]) {
                    this.a(n + by4, n2 + by3 + 1, n5 > this.heightMap[n + by4][n2 + by3 + 1]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
                l = System.currentTimeMillis();
                while (n5 != this.heightMap[n + by4 + 1][n2 + by3 + 1]) {
                    this.a(n + by4 + 1, n2 + by3 + 1, n5 > this.heightMap[n + by4 + 1][n2 + by3 + 1]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
            }
        }
        by4 = 0;
        for (by3 = 0; by3 < n7; ++by3) {
            for (byte by6 = 0; by6 < by5; ++by6) {
                if (byArray[by3][by6] > 40 || byArray[by3][by6] < 34) continue;
                by4 = (byte)(by4 + 1);
            }
        }
        by3 = this.a(by4, n + n7 / 2, n2 + by5 / 2);
        for (int i = 0; i < n7; ++i) {
            for (n4 = 0; n4 < by5; ++n4) {
                if (n + i >= this.mapWidth || n2 + n4 >= this.mapHeight || byArray[i][n4] == 0) continue;
                this.tileMap[n + i][n2 + n4] = byArray[i][n4];
                this.trackMap[n + i][n2 + n4] = (byte)(0x80 | by3);
            }
        }
    }

    void a(int n, int n2, int n3, int n4) {
        byte by = -2;
        for (int i = n; i < n + n3; ++i) {
            for (int j = n2; j < n2 + n4; ++j) {
                if (i >= this.mapWidth || j >= this.mapHeight) continue;
                if (by == -2 && (by = this.heightMap[i][j]) < 0) {
                    by = 0;
                }
                while (by != this.heightMap[i][j]) {
                    this.a(i, j, by > this.heightMap[i][j]);
                }
                while (by != this.heightMap[i + 1][j]) {
                    this.a(i + 1, j, by > this.heightMap[i + 1][j]);
                }
                while (by != this.heightMap[i][j + 1]) {
                    this.a(i, j + 1, by > this.heightMap[i][j + 1]);
                }
                while (by != this.heightMap[i + 1][j + 1]) {
                    this.a(i + 1, j + 1, by > this.heightMap[i + 1][j + 1]);
                }
            }
        }
    }

    void b(int n, int n2, int n3, int n4) {
        this.tileMap[n][n2] = (byte)n3;
        this.trackMap[n][n2] = (byte)(0xC0 | n4);
    }

    void u(int n) {
        int n2;
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        boolean bl;
        if (this.fx) {
            return;
        }
        long l = System.currentTimeMillis();
        do {
            bl = true;
            n7 = Math.abs(this.rng.nextInt() % (this.mapWidth - 8)) + 4;
            n6 = Math.abs(this.rng.nextInt() % (this.mapHeight - 8)) + 4;
            n5 = this.rng.nextInt() % 2 <= 0 ? 3 : 2;
            int n8 = n4 = this.rng.nextInt() % 2 <= 0 ? 3 : 2;
            if (n == 2) {
                n5 = 2;
                n4 = 2;
            }
            for (n3 = n7 - 3; n3 < n7 + n5 + 3; ++n3) {
                for (n2 = n6 - 3; n2 < n6 + n4 + 3; ++n2) {
                    if (this.tileMap[n3][n2] > 33) {
                        bl = false;
                    }
                    if (this.tileElevation[n3][n2] != -1) continue;
                    bl = false;
                }
            }
            if (System.currentTimeMillis() - l <= 1500L) continue;
            this.fx = true;
            break;
        } while (!bl);
        if (!this.fx) {
            this.a(n7, n6, n5, n4);
            n3 = this.f(n);
            switch (n) {
                case 0: {
                    if (n5 == 2 && n4 == 2) {
                        this.b(n7, n6, 52, n3);
                        this.b(n7, n6 + 1, 52, n3);
                        this.b(n7 + 1, n6, 51, n3);
                        this.b(n7 + 1, n6 + 1, 50, n3);
                    }
                    if (n5 == 3 && n4 == 2 || n5 == 3 && n4 == 3) {
                        this.b(n7, n6, 52, n3);
                        this.b(n7, n6 + 1, 52, n3);
                        this.b(n7 + 1, n6, 51, n3);
                        this.b(n7 + 2, n6, 51, n3);
                        this.b(n7 + 1, n6 + 1, 50, n3);
                        this.b(n7 + 2, n6 + 1, 50, n3);
                    }
                    if (n5 != 2 || n4 != 3) break;
                    this.b(n7, n6, 52, n3);
                    this.b(n7 + 1, n6, 52, n3);
                    this.b(n7, n6 + 1, 50, n3);
                    this.b(n7 + 1, n6 + 1, 51, n3);
                    this.b(n7, n6 + 2, 50, n3);
                    this.b(n7 + 1, n6 + 2, 51, n3);
                    break;
                }
                case 1: {
                    if (n5 == 2 && n4 == 2) {
                        this.b(n7, n6, 53, n3);
                        this.b(n7, n6 + 1, 53, n3);
                        this.b(n7 + 1, n6, 55, n3);
                        this.b(n7 + 1, n6 + 1, 54, n3);
                    }
                    if (n5 == 3 && n4 == 2 || n5 == 3 && n4 == 3) {
                        this.b(n7, n6, 53, n3);
                        this.b(n7, n6 + 1, 53, n3);
                        this.b(n7 + 1, n6, 55, n3);
                        this.b(n7 + 2, n6, 55, n3);
                        this.b(n7 + 1, n6 + 1, 54, n3);
                        this.b(n7 + 2, n6 + 1, 54, n3);
                    }
                    if (n5 != 2 || n4 != 3) break;
                    this.b(n7, n6, 53, n3);
                    this.b(n7 + 1, n6, 53, n3);
                    this.b(n7, n6 + 1, 55, n3);
                    this.b(n7 + 1, n6 + 1, 55, n3);
                    this.b(n7, n6 + 2, 54, n3);
                    this.b(n7 + 1, n6 + 2, 54, n3);
                    break;
                }
                case 2: {
                    if (n5 != 2 || n4 != 2) break;
                    this.b(n7, n6, (byte)(57 + Math.abs(this.rng.nextInt() % 2)), n3);
                    this.b(n7, n6 + 1, 56, n3);
                    this.b(n7 + 1, n6, (byte)(57 + Math.abs(this.rng.nextInt() % 2)), n3);
                    this.b(n7 + 1, n6 + 1, (byte)(57 + Math.abs(this.rng.nextInt() % 2)), n3);
                    break;
                }
                case 3: {
                    for (n2 = n7; n2 < n7 + n5; ++n2) {
                        for (int i = n6; i < n6 + n4; ++i) {
                            this.b(n2, i, (byte)(59 + Math.abs(this.rng.nextInt() % 3)), n3);
                        }
                    }
                    break;
                }
                case 4: {
                    if (n5 == 2 && n4 == 2) {
                        this.b(n7, n6, 62, n3);
                        this.b(n7, n6 + 1, 62, n3);
                        this.b(n7 + 1, n6, 64, n3);
                        this.b(n7 + 1, n6 + 1, 63, n3);
                    }
                    if (n5 == 3 && n4 == 2 || n5 == 3 && n4 == 3) {
                        this.b(n7, n6, 62, n3);
                        this.b(n7, n6 + 1, 62, n3);
                        this.b(n7 + 1, n6, 64, n3);
                        this.b(n7 + 2, n6, 64, n3);
                        this.b(n7 + 1, n6 + 1, 62, n3);
                        this.b(n7 + 2, n6 + 1, 63, n3);
                    }
                    if (n5 != 2 || n4 != 3) break;
                    this.b(n7, n6, 62, n3);
                    this.b(n7 + 1, n6, 63, n3);
                    this.b(n7, n6 + 1, 62, n3);
                    this.b(n7 + 1, n6 + 1, 64, n3);
                    this.b(n7, n6 + 2, 62, n3);
                    this.b(n7 + 1, n6 + 2, 64, n3);
                    break;
                }
                case 5: {
                    if (n5 == 2 && n4 == 2) {
                        this.b(n7, n6, 65, n3);
                        this.b(n7 + 1, n6, 66, n3);
                        this.b(n7, n6 + 1, 67, n3);
                    }
                    if (n5 == 3 && n4 == 2 || n5 == 3 && n4 == 3) {
                        this.b(n7, n6, 65, n3);
                        this.b(n7 + 1, n6, 66, n3);
                        this.b(n7 + 2, n6, 67, n3);
                        this.b(n7, n6 + 1, 68, n3);
                    }
                    if (n5 != 2 || n4 != 3) break;
                    this.b(n7, n6, 65, n3);
                    this.b(n7, n6 + 1, 66, n3);
                    this.b(n7, n6 + 2, 67, n3);
                    this.b(n7 + 1, n6, 68, n3);
                }
            }
        }
    }

    int G() {
        if (this.keySubState < 30) {
            this.keySubState += this.hh;
        }
        this.keySubState = (byte)this.keySubState;
        return this.keySubState / 3;
    }

    void viewportPanKey() {
        if (this.lastKeyCode == 56 || this.lastKeyCode == 6) {
            this.viewportCenterY += 30;
        }
        if (this.lastKeyCode == 50 || this.lastKeyCode == 1) {
            this.viewportCenterY -= 30;
        }
        if (this.lastKeyCode == 54 || this.lastKeyCode == 5) {
            this.viewportCenterX += 60;
        }
        if (this.lastKeyCode == 52 || this.lastKeyCode == 2) {
            this.viewportCenterX -= 60;
        }
        if (this.lastKeyCode == 49) {
            this.viewportCenterY -= 30;
            this.viewportCenterX -= 60;
        }
        if (this.lastKeyCode == 51) {
            this.viewportCenterY -= 30;
            this.viewportCenterX += 60;
        }
        if (this.lastKeyCode == 55) {
            this.viewportCenterY += 30;
            this.viewportCenterX -= 60;
        }
        if (this.lastKeyCode == 57) {
            this.viewportCenterY += 30;
            this.viewportCenterX += 60;
        }
        if (this.viewportCenterX < 0) {
            this.viewportCenterX = 0;
        }
        if (this.viewportCenterY < 0) {
            this.viewportCenterY = 0;
        }
        if (this.viewportCenterX > this.mapScreenWidth - e) {
            this.viewportCenterX = this.mapScreenWidth - e;
        }
        if (this.viewportCenterY > this.mapScreenHeight - eN) {
            this.viewportCenterY = this.mapScreenHeight - eN;
        }
        this.viewportBaseX = this.viewportCenterX + e / 2;
        this.viewportBaseY = this.viewportCenterY + eN / 2;
        this.mapScaleX = this.viewportBaseX * 2;
        this.mapScaleY = this.viewportBaseY * 2;
    }

    void viewportZoomKey() {
        int n = this.G();
        if (this.lastKeyCode == 56 || this.lastKeyCode == 6) {
            this.mapScaleY += n * 2;
        }
        if (this.lastKeyCode == 50 || this.lastKeyCode == 1) {
            this.mapScaleY -= n * 2;
        }
        if (this.lastKeyCode == 54 || this.lastKeyCode == 5) {
            this.mapScaleX += n * 2;
        }
        if (this.lastKeyCode == 52 || this.lastKeyCode == 2) {
            this.mapScaleX -= n * 2;
        }
        if (this.lastKeyCode == 49) {
            this.mapScaleY -= n;
            this.mapScaleX -= n * 2;
        }
        if (this.lastKeyCode == 51) {
            this.mapScaleY -= n;
            this.mapScaleX += n * 2;
        }
        if (this.lastKeyCode == 55) {
            this.mapScaleY += n;
            this.mapScaleX -= n * 2;
        }
        if (this.lastKeyCode == 57) {
            this.mapScaleY += n;
            this.mapScaleX += n * 2;
        }
        this.viewportBaseX = this.mapScaleX / 2;
        this.viewportBaseY = this.mapScaleY / 2;
        if (this.viewportBaseX > this.mapScreenWidth - 8) {
            this.viewportBaseX = this.mapScreenWidth - 8;
        }
        if (this.viewportBaseY > this.mapScreenHeight - 4) {
            this.viewportBaseY = this.mapScreenHeight - 4;
        }
        boolean bl = false;
        if (this.viewportCenterX > 0 && this.viewportBaseX - this.viewportCenterX <= 10) {
            bl = true;
            this.viewportCenterX = this.viewportBaseX - 11;
        }
        if (this.viewportCenterY > 0 && this.viewportBaseY - this.viewportCenterY <= 10) {
            bl = true;
            this.viewportCenterY = this.viewportBaseY - 11;
        }
        if (this.viewportBaseX - this.viewportCenterX >= e - 20) {
            bl = true;
            this.viewportCenterX = this.viewportBaseX - e + 21;
        }
        if (this.viewportBaseY - this.viewportCenterY >= eN - 20) {
            bl = true;
            this.viewportCenterY = this.viewportBaseY - eN + 21;
        }
        if (this.viewportBaseX < 0) {
            this.viewportBaseX = 0;
        }
        if (this.viewportBaseY < 0) {
            this.viewportBaseY = 0;
        }
        if (this.viewportCenterX < 0) {
            this.viewportCenterX = 0;
            bl = false;
        }
        if (this.viewportCenterY < 0) {
            this.viewportCenterY = 0;
            bl = false;
        }
        if (this.viewportCenterX > this.mapScreenWidth - e) {
            this.viewportCenterX = this.mapScreenWidth - e;
            bl = false;
        }
        if (this.viewportCenterY > this.mapScreenHeight - eN) {
            this.viewportCenterY = this.mapScreenHeight - eN;
            bl = false;
        }
        if (bl) {
            this.keySubState = 80;
        }
        this.i();
        this.mapScaleX = this.viewportBaseX * 2;
        this.mapScaleY = this.viewportBaseY * 2;
    }

    int a(int n, int n2, int n3, int n4, int n5, int n6) {
        return (n3 - n) * (n2 - n6) - (n4 - n2) * (n - n5);
    }

    void a(int n, int n2, int[] nArray, int[] nArray2, boolean bl) {
        int n3 = n * 16 + n2 * 16 + 16;
        int n4 = this.hj - n * 8 + n2 * 8 + 8;
        nArray[0] = n3 - 16;
        nArray[1] = n3;
        nArray[2] = n3;
        nArray[3] = n3 + 16;
        nArray2[0] = n4 - 4 * this.heightMap[n][n2];
        nArray2[1] = n4 - 8 - 4 * this.heightMap[n + 1][n2];
        nArray2[2] = n4 + 8 - 4 * this.heightMap[n][n2 + 1];
        nArray2[3] = n4 - 4 * this.heightMap[n + 1][n2 + 1];
        if (bl) {
            int n5 = 0;
            while (n5 < 4) {
                int n6 = n5;
                nArray[n6] = nArray[n6] - this.viewportCenterX;
                int n7 = n5++;
                nArray2[n7] = nArray2[n7] - this.viewportCenterY;
            }
        }
    }

    void i() {
        int n;
        int n2;
        int n3;
        int n4;
        int n5 = 0;
        int n6 = 0;
        int n7 = this.viewportBaseX - 2 * this.viewportBaseY + 16 + 2 * this.hj;
        int n8 = this.viewportBaseX + 2 * this.viewportBaseY - 16 - 2 * this.hj;
        n8 /= 32;
        this.blockBuildMenu = true;
        block0: for (n4 = (n7 /= 32) - 1; n4 <= n7 + 1; ++n4) {
            for (n3 = n8 - 1; n3 <= n8 + 1; ++n3) {
                if (n4 < 0 || n4 > this.mapWidth - 1 || n3 < 0 || n3 > this.mapHeight - 1) continue;
                n5 = n3 * 16 + n4 * 16 + 16;
                n6 = this.hj - n4 * 8 + n3 * 8 + 8;
                this.a(n4, n3, this.selectionTileX, this.selectionTileY, false);
                n2 = this.a(this.viewportBaseX, this.viewportBaseY, this.selectionTileX[0], this.selectionTileY[0], this.selectionTileX[1], this.selectionTileY[1]) * this.a(this.viewportBaseX, this.viewportBaseY, this.selectionTileX[2], this.selectionTileY[2], this.selectionTileX[3], this.selectionTileY[3]);
                n = this.a(this.viewportBaseX, this.viewportBaseY, this.selectionTileX[0], this.selectionTileY[0], this.selectionTileX[2], this.selectionTileY[2]) * this.a(this.viewportBaseX, this.viewportBaseY, this.selectionTileX[1], this.selectionTileY[1], this.selectionTileX[3], this.selectionTileY[3]);
                if (n2 > 0 || n > 0) continue;
                n7 = n4;
                n8 = n3;
                this.blockBuildMenu = false;
                break block0;
            }
        }
        if (!this.blockBuildMenu) {
            this.swapMapX = n7;
            this.swapMapY = n8;
            this.tileCursorX = n7;
            this.tileCursorY = n8;
            n4 = Math.abs((this.viewportBaseX - this.selectionTileX[0]) * (this.viewportBaseX - this.selectionTileX[0]) + (this.viewportBaseY - this.selectionTileY[0]) * (this.viewportBaseY - this.selectionTileY[0]));
            n3 = Math.abs((this.viewportBaseX - this.selectionTileX[1]) * (this.viewportBaseX - this.selectionTileX[1]) + (this.viewportBaseY - this.selectionTileY[1]) * (this.viewportBaseY - this.selectionTileY[1]));
            n2 = Math.abs((this.viewportBaseX - this.selectionTileX[2]) * (this.viewportBaseX - this.selectionTileX[2]) + (this.viewportBaseY - this.selectionTileY[2]) * (this.viewportBaseY - this.selectionTileY[2]));
            n = Math.abs((this.viewportBaseX - this.selectionTileX[3]) * (this.viewportBaseX - this.selectionTileX[3]) + (this.viewportBaseY - this.selectionTileY[3]) * (this.viewportBaseY - this.selectionTileY[3]));
            int n9 = Math.min(n4, Math.min(n3, Math.min(n2, n)));
            this.vehicleMenuSelectIndex = n4 < n;
            boolean bl = this.trainMenuSelectIndex = n3 < n2;
            if (n9 == n4) {
                n5 -= 16;
            } else if (n9 == n3) {
                n6 -= 8;
                ++this.swapMapX;
            } else if (n9 == n2) {
                n6 += 8;
                ++this.swapMapY;
            } else if (n9 == n) {
                n5 += 16;
                ++this.swapMapX;
                ++this.swapMapY;
            }
            n6 = n6 - this.viewportCenterY - 4 * this.heightMap[this.swapMapX][this.swapMapY];
            this.cursorScreenX = n5 -= this.viewportCenterX;
            this.cursorScreenY = n6;
            int n10 = 0;
            while (n10 <= 3) {
                int n11 = n10;
                this.selectionTileX[n11] = this.selectionTileX[n11] - this.viewportCenterX;
                int n12 = n10++;
                this.selectionTileY[n12] = this.selectionTileY[n12] - this.viewportCenterY;
            }
        }
    }

    void a(Graphics graphics, int[] nArray, int[] nArray2) {
        graphics.drawLine(nArray[0], nArray2[0] + 15, nArray[1], nArray2[1] + 15);
        graphics.drawLine(nArray[0] + 1, nArray2[0] + 15, nArray[1] + 1, nArray2[1] + 15);
        graphics.drawLine(nArray[0], nArray2[0] + 15, nArray[2], nArray2[2] + 15);
        graphics.drawLine(nArray[0] + 1, nArray2[0] + 15, nArray[2] + 1, nArray2[2] + 15);
        graphics.drawLine(nArray[1], nArray2[1] + 15, nArray[3], nArray2[3] + 15);
        graphics.drawLine(nArray[1] + 1, nArray2[1] + 15, nArray[3] + 1, nArray2[3] + 15);
        graphics.drawLine(nArray[2], nArray2[2] + 15, nArray[3], nArray2[3] + 15);
        graphics.drawLine(nArray[2] + 1, nArray2[2] + 15, nArray[3] + 1, nArray2[3] + 15);
    }

    void c(Graphics graphics) {
        byte by;
        if (!this.blockBuildMenu && (by = this.menuTransitionCode[this.buildMenuMode][this.buildMenuIndex]) != 0) {
            int[] nArray;
            int[] nArray2;
            if (by == 1) {
                if (this.e(this.tileCursorX, this.tileCursorY, 0)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 2) {
                if (this.e(this.tileCursorX, this.tileCursorY, 1)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 3) {
                if (this.e(this.tileCursorX, this.tileCursorY, 2)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 4) {
                if (this.e(this.tileCursorX, this.tileCursorY, 3)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 5) {
                if (this.e(this.tileCursorX, this.tileCursorY, 4)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 6) {
                if (this.e(this.tileCursorX, this.tileCursorY, 5)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
                nArray2 = new int[4];
                nArray = new int[4];
                if (this.tileCursorY > 0) {
                    this.a(this.tileCursorX, this.tileCursorY - 1, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
                if (this.tileCursorY < this.mapWidth - 1) {
                    this.a(this.tileCursorX, this.tileCursorY + 1, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
            }
            if (by == 7) {
                if (this.e(this.tileCursorX, this.tileCursorY, 6)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
                nArray2 = new int[4];
                nArray = new int[4];
                if (this.tileCursorX > 0) {
                    this.a(this.tileCursorX - 1, this.tileCursorY, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
                if (this.tileCursorX < this.mapWidth - 1) {
                    this.a(this.tileCursorX + 1, this.tileCursorY, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
            }
            if (by == 8) {
                if (this.j(this.tileCursorX, this.tileCursorY)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 9) {
                if (this.r(this.tileCursorX, this.tileCursorY)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 10) {
                graphics.setColor(0xFFFFFF);
            }
            if (by == 11) {
                if (this.b(this.tileCursorX, this.tileCursorY)) {
                    int n = this.tileCursorX;
                    int n2 = this.tileCursorY;
                    int n3 = 0;
                    int n4 = 0;
                    if (this.tileMap[n][n2] == 7) {
                        n4 = -1;
                    }
                    if (this.tileMap[n][n2] == 10) {
                        n4 = 1;
                    }
                    if (this.tileMap[n][n2] == 8) {
                        n3 = -1;
                    }
                    if (this.tileMap[n][n2] == 9) {
                        n3 = 1;
                    }
                    graphics.setColor(65280);
                    int[] nArray3 = new int[4];
                    int[] nArray4 = new int[4];
                    while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < this.mapWidth && n2 < this.mapHeight) {
                        this.a(n, n2, nArray3, nArray4, true);
                        this.a(graphics, nArray3, nArray4);
                        if (this.tileMap[n][n2] == 0 && this.trackMap[n][n2] == 0) continue;
                        break;
                    }
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            this.a(graphics, this.selectionTileX, this.selectionTileY);
        }
    }

    void e(Graphics graphics) {
        if (this.menuRowEnabled[this.buildMenuMode][this.buildMenuIndex]) {
            int n = this.viewportBaseX - this.viewportCenterX;
            int n2 = this.viewportBaseY - this.viewportCenterY;
            if (this.blockBuildMenu) {
                this.a(graphics, 200, n, n2 + 15, 20);
                graphics.setColor(0xFF0000);
                graphics.drawArc(n + 5, n2 + 5 + 15, 7, 7, 0, 360);
                graphics.drawLine(n + 7, n2 + 10 + 15, n + 10, n2 + 7 + 15);
            } else {
                if (this.buildMenuMode == 1 && (this.buildMenuIndex == 5 || this.buildMenuIndex == 6)) {
                    this.a(graphics, 249, this.cursorScreenX - 2, this.cursorScreenY - 2 + 15, 20);
                }
                this.a(graphics, 200, n, n2 + 15, 20);
                if (this.buildMenuMode != 0 || this.buildMenuIndex != 0) {
                    this.a(graphics, this.c(this.menuTextIndex[this.buildMenuMode][this.buildMenuIndex]), n + 5, n2 + 4 + 15, 20);
                }
                if (this.buildMenuMode == 3 && this.buildMenuIndex < 4) {
                    int n3;
                    this.a(graphics, this.buildMenuIndex + 230, n + 5, n2 + 4 + 15, 20);
                    int n4 = this.tileCursorX;
                    int n5 = this.tileCursorY;
                    int n6 = n4 - 3;
                    int n7 = n5 - 3;
                    int n8 = n4 + 3;
                    int n9 = n5 + 3;
                    if (n6 < 0) {
                        n6 = 0;
                    }
                    if (n7 < 0) {
                        n7 = 0;
                    }
                    if (n8 >= this.mapWidth) {
                        n8 = this.mapWidth - 1;
                    }
                    if (n9 >= this.mapHeight) {
                        n9 = this.mapHeight - 1;
                    }
                    int n10 = 0;
                    boolean bl = false;
                    boolean bl2 = false;
                    boolean bl3 = false;
                    boolean bl4 = false;
                    boolean bl5 = false;
                    for (n3 = n6; n3 <= n8; ++n3) {
                        for (int i = n7; i <= n9; ++i) {
                            if (this.tileMap[n3][i] >= 50 && this.tileMap[n3][i] <= 55) {
                                bl2 = true;
                            }
                            if (this.tileMap[n3][i] >= 56 && this.tileMap[n3][i] <= 61) {
                                bl3 = true;
                            }
                            if (this.tileMap[n3][i] >= 62 && this.tileMap[n3][i] <= 68) {
                                bl = true;
                            }
                            if (this.tileMap[n3][i] < 34 || this.tileMap[n3][i] > 40) continue;
                            ++n10;
                        }
                    }
                    if (n10 / 3 > 0) {
                        bl4 = true;
                    }
                    if (n10 / 7 > 0) {
                        bl5 = true;
                    }
                    n3 = 0;
                    if (bl4) {
                        this.a(graphics, 190, n3, 15, 20);
                        n3 += 15;
                    }
                    if (bl5) {
                        this.a(graphics, 191, n3, 15, 20);
                        n3 += 15;
                    }
                    if (bl) {
                        this.a(graphics, 192, n3, 15, 20);
                        n3 += 15;
                    }
                    if (bl3) {
                        this.a(graphics, 194, n3, 15, 20);
                        n3 += 15;
                    }
                    if (bl2) {
                        this.a(graphics, 193, n3, 15, 20);
                    }
                }
            }
        }
    }

    void demolishAtCursor() {
        if (!this.blockBuildMenu && this.r(this.tileCursorX, this.tileCursorY)) {
            this.bg = 0;
            if (this.tileMap[this.tileCursorX][this.tileCursorY] >= 81 && this.tileMap[this.tileCursorX][this.tileCursorY] <= 86) {
                int n = this.tileCursorX;
                int n2 = this.tileCursorY;
                this.d(n, n2, -3);
                if (this.tileMap[n][n2] == 81 || this.tileMap[n][n2] == 84 || this.tileMap[n][n2] == 85) {
                    int n3;
                    for (n3 = n; n3 < this.mapWidth && (this.tileMap[n3][n2] == 81 || this.tileMap[n3][n2] == 84 || this.tileMap[n3][n2] == 85); ++n3) {
                        this.a(n3, n2, true, false);
                    }
                    for (n3 = n - 1; n3 >= 0 && (this.tileMap[n3][n2] == 81 || this.tileMap[n3][n2] == 84 || this.tileMap[n3][n2] == 85); --n3) {
                        this.a(n3, n2, true, false);
                    }
                } else {
                    int n4;
                    for (n4 = n2; n4 < this.mapHeight && (this.tileMap[n][n4] == 82 || this.tileMap[n][n4] == 83 || this.tileMap[n][n4] == 86); ++n4) {
                        this.a(n, n4, true, false);
                    }
                    for (n4 = n2 - 1; n4 >= 0 && (this.tileMap[n][n4] == 82 || this.tileMap[n][n4] == 83 || this.tileMap[n][n4] == 86); --n4) {
                        this.a(n, n4, true, false);
                    }
                }
            } else if (this.tileMap[this.tileCursorX][this.tileCursorY] >= 87 && this.tileMap[this.tileCursorX][this.tileCursorY] <= 90) {
                int n = this.spawnOrFindAt(this.tileCursorX, this.tileCursorY);
                if (n != 5000) {
                    int n5 = this.c(this.wagonAJ[n]);
                    int n6 = this.c(this.wagonEQ[n]);
                    if ((this.vehicleFlags[n] & 0x20) == 32) {
                        if ((this.vehicleFlags[n] & 0x40) == 0) {
                            this.trackMap[n5][n6 - 1] = 0;
                            this.trackMap[n5][n6 + 1] = 0;
                            this.a(n5, n6 - 1, true, false);
                            this.a(n5, n6 + 1, true, false);
                        } else {
                            this.trackMap[n5 - 1][n6] = 0;
                            this.trackMap[n5 + 1][n6] = 0;
                            this.a(n5 - 1, n6, true, false);
                            this.a(n5 + 1, n6, true, false);
                        }
                    }
                    this.a(n5, n6, true, false);
                    this.d(n5, n6, -6);
                    this.trackMap[n5][n6] = 0;
                    this.vehicleFlags[n] = 0;
                }
            } else {
                this.a(this.tileCursorX, this.tileCursorY, true, false);
                this.d(this.tileCursorX, this.tileCursorY, -6);
            }
            this.f();
        }
    }

    void pickTrainsAtCursor() {
        if (!this.blockBuildMenu) {
            byte by = this.trackMap[this.tileCursorX][this.tileCursorY];
            if ((by & 0xC0) == 128) {
                this.junctionDirCursor = (byte)(this.trackMap[this.tileCursorX][this.tileCursorY] & 0x3F);
                this.ready = false;
                this.transitionCode = 20;
                this.setState(20);
            }
            if ((by & 0xC0) == 192) {
                this.stationDirCursor = (byte)(this.trackMap[this.tileCursorX][this.tileCursorY] & 0x3F);
                this.ready = false;
                this.transitionCode = 21;
                this.setState(20);
            }
            if (by == 65) {
                this.trainAtCursor = this.cPosLookup(this.tileCursorX, this.tileCursorY);
                if (this.trainAtCursor != 5000) {
                    int n;
                    int n2;
                    this.wagonType = this.trainEraOrStage[this.trainAtCursor];
                    for (n2 = 0; n2 < 5; ++n2) {
                        this.wagonOptionSlots[n2] = -1;
                    }
                    n2 = this.nextCar[this.trainAtCursor];
                    for (n = 0; n < 5 && n2 != -1; ++n) {
                        switch (this.trainFlags[n2] & 0xF) {
                            case 2: {
                                this.wagonOptionSlots[n] = 0;
                                break;
                            }
                            case 3: {
                                this.wagonOptionSlots[n] = 1;
                                break;
                            }
                            case 4: {
                                this.wagonOptionSlots[n] = 2;
                                break;
                            }
                            case 5: {
                                this.wagonOptionSlots[n] = 2;
                                break;
                            }
                            case 6: {
                                this.wagonOptionSlots[n] = 3;
                                break;
                            }
                            case 7: {
                                this.wagonOptionSlots[n] = 3;
                            }
                        }
                        n2 = this.nextCar[n2];
                    }
                    for (n = 0; n < 5; ++n) {
                        this.wagonCountMatch[n] = this.wagonOptionSlots[n];
                    }
                    this.wagonDiscountType = this.wagonType;
                    this.difficultyPct = 100;
                    this.computeTrainCost();
                    this.baseTrainCostSnapshot = this.unitCost;
                    this.difficultyPct = 100 - 3 * this.trainAgeOrTimer[this.trainAtCursor];
                    this.computeTrainCost();
                    this.currentTrainCostSnapshot = this.unitCost;
                    this.ready = false;
                    this.transitionCode = 26;
                    this.setState(20);
                } else {
                    this.aiWagonsLastSeen = this.spawnOrFindAt(this.tileCursorX, this.tileCursorY);
                    if (this.aiWagonsLastSeen != 5000) {
                        this.ready = false;
                        this.transitionCode = 22;
                        this.setState(20);
                    }
                }
            }
        }
    }

    void buildPlaceTrackPiece(int n) {
        switch (n) {
            case 0: {
                int n2;
                if (!this.e(this.tileCursorX, this.tileCursorY, 0) || (n2 = this.a(1, this.tileCursorX, this.tileCursorY, false, false)) == 5000 || !this.q(this.costPower)) break;
                this.tileMap[this.tileCursorX][this.tileCursorY] = 87;
                this.trackMap[this.tileCursorX][this.tileCursorY] = 65;
                this.d(this.tileCursorX, this.tileCursorY, -8);
                this.x(this.costPower);
                break;
            }
            case 1: {
                int n3;
                if (!this.e(this.tileCursorX, this.tileCursorY, 0) || (n3 = this.a(1, this.tileCursorX, this.tileCursorY, true, false)) == 5000 || !this.q(this.costPower)) break;
                this.tileMap[this.tileCursorX][this.tileCursorY] = 88;
                this.trackMap[this.tileCursorX][this.tileCursorY] = 65;
                this.d(this.tileCursorX, this.tileCursorY, -8);
                this.x(this.costPower);
                break;
            }
            case 2: {
                int n4;
                if (!this.e(this.tileCursorX, this.tileCursorY, 5) || (n4 = this.a(1, this.tileCursorX, this.tileCursorY, false, true)) == 5000 || !this.q(this.costStruct)) break;
                this.tileMap[this.tileCursorX][this.tileCursorY] = 87;
                this.tileMap[this.tileCursorX][this.tileCursorY - 1] = 89;
                this.tileMap[this.tileCursorX][this.tileCursorY + 1] = 89;
                this.trackMap[this.tileCursorX][this.tileCursorY] = 65;
                this.trackMap[this.tileCursorX][this.tileCursorY - 1] = 65;
                this.trackMap[this.tileCursorX][this.tileCursorY + 1] = 65;
                this.d(this.tileCursorX, this.tileCursorY, -11);
                this.x(this.costStruct);
                break;
            }
            case 3: {
                int n5;
                if (!this.e(this.tileCursorX, this.tileCursorY, 6) || (n5 = this.a(1, this.tileCursorX, this.tileCursorY, true, true)) == 5000 || !this.q(this.costStruct)) break;
                this.tileMap[this.tileCursorX][this.tileCursorY] = 88;
                this.tileMap[this.tileCursorX - 1][this.tileCursorY] = 90;
                this.tileMap[this.tileCursorX + 1][this.tileCursorY] = 90;
                this.trackMap[this.tileCursorX][this.tileCursorY] = 65;
                this.trackMap[this.tileCursorX - 1][this.tileCursorY] = 65;
                this.trackMap[this.tileCursorX + 1][this.tileCursorY] = 65;
                this.d(this.tileCursorX, this.tileCursorY, -11);
                this.x(this.costStruct);
            }
        }
        this.tickInputOrAi(true);
        this.T();
    }

    void openBuildMenu() {
        byte by = this.buildMenuItems[this.buildMenuMode][this.buildMenuIndex];
        if (!(!this.pauseFlag || this.buildMenuMode == 0 && by == 11 || this.buildMenuMode == 0 && by == 1 || this.buildMenuMode == 0 && by == 9 || this.buildMenuMode == 1 && by == 1 || this.buildMenuMode == 1 && by == 3 || this.buildMenuMode == 1 && by == 9 || this.buildMenuMode == 3 && by == 9 || this.buildMenuMode == 5 && by == 9)) {
            return;
        }
        block0 : switch (this.buildMenuMode) {
            case 0: {
                switch (by) {
                    case 0: {
                        this.pickTrainsAtCursor();
                        break;
                    }
                    case 1: {
                        this.buildMenuMode = 1;
                        this.menuDirty = true;
                        this.buildMenuIndex = 0;
                        break;
                    }
                    case 2: {
                        this.ready = false;
                        this.transitionCode = 30;
                        this.setState(20);
                        break;
                    }
                    case 3: {
                        this.ready = false;
                        this.transitionCode = 31;
                        this.setState(20);
                        break;
                    }
                    case 4: {
                        this.a();
                        this.ready = false;
                        this.transitionCode = 60;
                        this.setState(20);
                        break;
                    }
                    case 5: {
                        this.ready = false;
                        this.transitionCode = 17;
                        this.setState(20);
                        break;
                    }
                    case 8: {
                        this.inputPending = true;
                        break;
                    }
                    case 9: {
                        this.c(this.scenarioStep - 2);
                        this.bC = false;
                        this.ready = false;
                        this.transitionCode = 1;
                        this.setState(20);
                        break;
                    }
                    case 11: {
                        this.pauseFlag = !this.pauseFlag;
                    }
                }
                break;
            }
            case 1: {
                switch (by) {
                    case 0: {
                        if (this.blockBuildMenu || !this.j(this.tileCursorX, this.tileCursorY)) break;
                        this.placementX = this.tileCursorX;
                        this.placementY = this.tileCursorY;
                        this.tileMap[this.placementX][this.placementY] = 127;
                        this.buildMenuMode = 4;
                        this.menuDirty = true;
                        this.buildMenuIndex = 0;
                        break;
                    }
                    case 1: {
                        this.buildMenuMode = 5;
                        this.menuDirty = true;
                        this.buildMenuIndex = 0;
                        break;
                    }
                    case 2: {
                        if (this.blockBuildMenu) break;
                        if (this.tileMap[this.tileCursorX][this.tileCursorY] == 7) {
                            this.a(this.tileCursorX, this.tileCursorY, 0, -1, 10, 83, 86, 82);
                        }
                        if (this.tileMap[this.tileCursorX][this.tileCursorY] == 10) {
                            this.a(this.tileCursorX, this.tileCursorY, 0, 1, 7, 86, 83, 82);
                        }
                        if (this.tileMap[this.tileCursorX][this.tileCursorY] == 8) {
                            this.a(this.tileCursorX, this.tileCursorY, -1, 0, 9, 84, 85, 81);
                        }
                        if (this.tileMap[this.tileCursorX][this.tileCursorY] != 9) break;
                        this.a(this.tileCursorX, this.tileCursorY, 1, 0, 8, 85, 84, 81);
                        break;
                    }
                    case 3: {
                        this.buildMenuMode = 3;
                        this.menuDirty = true;
                        this.buildMenuIndex = 0;
                        break;
                    }
                    case 4: {
                        this.demolishAtCursor();
                        break;
                    }
                    case 5: {
                        if (this.blockBuildMenu) break;
                        this.a(this.swapMapX, this.swapMapY, false);
                        break;
                    }
                    case 6: {
                        if (this.blockBuildMenu) break;
                        this.a(this.swapMapX, this.swapMapY, true);
                        break;
                    }
                    case 9: {
                        this.buildMenuMode = 0;
                        this.menuDirty = true;
                        this.buildMenuIndex = 1;
                    }
                }
                break;
            }
            case 2: {
                switch (by) {
                    case 0: {
                        if (this.selectionCost <= this.money) {
                            this.n(1);
                            this.money -= this.selectionCost;
                            this.totalSpent += this.selectionCost;
                            this.p(-this.selectionCost);
                            this.x(this.selectionCost);
                            break;
                        }
                        this.normalizeTrackBits();
                        this.enterOutOfMoneyState();
                        break;
                    }
                    case 9: {
                        if (this.aC) {
                            this.normalizeTrackBits();
                            break;
                        }
                        this.r();
                    }
                }
                this.buildMenuMode = 1;
                this.buildMenuIndex = this.aC ? 0 : 2;
                this.menuDirty = true;
                this.bH = -1;
                break;
            }
            case 3: {
                switch (by) {
                    default: {
                        this.buildPlaceTrackPiece(by);
                        break block0;
                    }
                    case 9: 
                }
                this.buildMenuMode = 1;
                this.menuDirty = true;
                this.buildMenuIndex = 3;
                break;
            }
            case 4: {
                switch (by) {
                    case 0: {
                        if (this.blockBuildMenu || !this.j(this.tileCursorX, this.tileCursorY)) break;
                        this.demolishX = this.tileCursorX;
                        this.demolishY = this.tileCursorY;
                        this.tileMap[this.placementX][this.placementY] = 0;
                        this.setState(45);
                        break;
                    }
                    case 9: {
                        this.tileMap[this.placementX][this.placementY] = 0;
                        this.buildMenuMode = 1;
                        this.menuDirty = true;
                        this.buildMenuIndex = 0;
                    }
                }
                break;
            }
            case 5: {
                switch (by) {
                    case 0: {
                        this.a(this.vehicleMenuSelectIndex, 75, 74, -1, -1, -1, -1, false, 0);
                        break block0;
                    }
                    case 1: {
                        this.a(this.trainMenuSelectIndex, 72, 73, -1, -1, -1, -1, false, 0);
                        break block0;
                    }
                    case 2: {
                        this.a(true, 70, 0, 21, 23, 77, 79, true, 71);
                        break block0;
                    }
                    case 3: {
                        this.a(true, 71, 0, 22, 24, 78, 80, true, 70);
                        break block0;
                    }
                    case 9: {
                        this.buildMenuMode = 1;
                        this.menuDirty = true;
                        this.buildMenuIndex = 1;
                    }
                }
            }
        }
    }

    boolean q(int n) {
        if (n <= this.money) {
            this.money -= n;
            this.bH = -1;
            this.totalSpent += n;
            this.p(-n);
            return true;
        }
        this.enterOutOfMoneyState();
        return false;
    }

    void a(int n, int n2, int n3, int n4, int n5, int n6, int n7, int n8) {
        int n9 = n;
        int n10 = n2;
        boolean bl = false;
        int n11 = this.moneyBaseA;
        this.trackStartX = n;
        this.trackStartY = n2;
        this.trackStepX = n3;
        this.trackStepY = n4;
        while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < this.mapWidth && n2 < this.mapHeight) {
            n11 += this.moneyBaseB;
            if (this.tileMap[n][n2] == n5) {
                bl = true;
            }
            if (this.tileMap[n][n2] == 0 && this.trackMap[n][n2] == 0) continue;
        }
        if (bl) {
            this.selectionCost = n11;
            this.buildMenuMode = 2;
            this.menuDirty = true;
            this.buildMenuIndex = 0;
            this.aC = false;
            this.tileMap[n][n2] = (byte)n7;
            this.trackMap[n][n2] = 65;
            this.tileMap[n9][n10] = (byte)n6;
            this.trackMap[n9][n10] = 65;
            while ((n9 += n3) != n || (n10 += n4) != n2) {
                this.tileMap[n9][n10] = (byte)n8;
                this.d(n9, n10, -4);
                this.trackMap[n9][n10] = 65;
            }
            this.bH = -1;
        }
    }

    void r() {
        byte by;
        int n = this.trackStartX;
        int n2 = this.trackStartY;
        while ((by = this.tileMap[n][n2]) == 81 || by == 82 || by == 83 || by == 84 || by == 85 || by == 86) {
            if (by == 83) {
                this.tileMap[n][n2] = 7;
            }
            if (by == 86) {
                this.tileMap[n][n2] = 10;
            }
            if (by == 84) {
                this.tileMap[n][n2] = 8;
            }
            if (by == 85) {
                this.tileMap[n][n2] = 9;
            }
            if (by == 81 || by == 82) {
                this.tileMap[n][n2] = 0;
            }
            this.trackMap[n][n2] = 0;
            if ((n += this.trackStepX) >= 0 && (n2 += this.trackStepY) >= 0 && n < this.mapWidth && n2 < this.mapHeight) continue;
            break;
        }
    }

    void a(boolean bl, int n, int n2, int n3, int n4, int n5, int n6, boolean bl2, int n7) {
        if (!this.blockBuildMenu && this.tileElevation[this.tileCursorX][this.tileCursorY] >= 0 && (this.trackMap[this.tileCursorX][this.tileCursorY] == 0 || bl2 && this.tileMap[this.tileCursorX][this.tileCursorY] == n7) && (this.tileMap[this.tileCursorX][this.tileCursorY] == n7 || this.tileMap[this.tileCursorX][this.tileCursorY] == n3 || this.tileMap[this.tileCursorX][this.tileCursorY] == n4 || this.tileMap[this.tileCursorX][this.tileCursorY] == 0 || this.tileMap[this.tileCursorX][this.tileCursorY] >= 29 && this.tileMap[this.tileCursorX][this.tileCursorY] <= 33)) {
            int n8 = 0;
            if (this.tileMap[this.tileCursorX][this.tileCursorY] != 0) {
                n8 += this.baseTrainSpeed;
            }
            n8 += this.accel1;
            if (bl2 && this.tileMap[this.tileCursorX][this.tileCursorY] == n7) {
                if ((n8 += this.brake1) <= this.money) {
                    this.tileMap[this.tileCursorX][this.tileCursorY] = 76;
                    int n9 = this.trackMap[this.tileCursorX][this.tileCursorY] & 0x3F;
                    n9 = n7 == 70 ? 0x48 | n9 : 0x41 | n9 << 3;
                    this.trackMap[this.tileCursorX][this.tileCursorY] = (byte)n9;
                    this.money -= n8;
                    this.bH = -1;
                    this.p(-n8);
                    this.totalSpent += n8;
                    this.x(n8);
                    this.d(this.tileCursorX, this.tileCursorY, -4);
                } else {
                    this.enterOutOfMoneyState();
                }
            } else if (n8 <= this.money) {
                this.tileMap[this.tileCursorX][this.tileCursorY] = this.tileMap[this.tileCursorX][this.tileCursorY] == n3 ? (byte)n5 : (this.tileMap[this.tileCursorX][this.tileCursorY] == n4 ? (byte)n6 : (byte)(bl ? n : n2));
                this.trackMap[this.tileCursorX][this.tileCursorY] = 65;
                this.money -= n8;
                this.totalSpent += n8;
                this.bH = -1;
                this.p(-n8);
                this.x(n8);
                this.d(this.tileCursorX, this.tileCursorY, -4);
            } else {
                this.enterOutOfMoneyState();
            }
        }
    }

    void a(int n, int n2, boolean bl, boolean bl2) {
        int n3;
        boolean bl3;
        boolean bl4 = bl3 = !bl;
        if (bl) {
            int n4 = n3 = this.tileMap[n][n2] >= 29 && this.tileMap[n][n2] <= 33 ? this.baseTrainSpeed2 : -this.accel2;
            if (this.money >= n3 || this.money < 0) {
                this.bg += n3;
                bl3 = true;
                this.ck = true;
            } else {
                this.ck = false;
            }
        }
        for (n3 = 49; n3 >= 0; --n3) {
            if (this.tileQueue[n3][0] == 0 || this.c(this.tileQueue[n3][1]) != n || this.c(this.tileQueue[n3][2]) != n2) continue;
            bl3 = false;
        }
        if (bl3) {
            if (this.c(this.tileMap[n][n2]) < 128) {
                this.tileMap[n][n2] = (byte)(this.tileMap[n][n2] + 128);
            }
            n3 = 0;
            for (int i = 49; i >= 0; --i) {
                if (this.tileQueue[i][0] != 0) continue;
                n3 = i;
                break;
            }
            this.tileQueue[n3][0] = 14;
            this.go[n3] = bl2;
            this.tileQueue[n3][1] = (byte)n;
            this.tileQueue[n3][2] = (byte)n2;
        }
    }

    void f() {
        if (this.ck) {
            this.ho += this.bg;
            this.money -= this.bg;
            this.p(-this.bg);
        } else {
            this.enterOutOfMoneyState();
        }
    }

    void d(int n, int n2) {
        this.buildMenuItems[n] = new byte[n2];
        this.buildMenuResultCodes[n] = new byte[n2];
        this.menuItemCode[n] = new byte[n2];
        this.menuTextIndex[n] = new byte[n2];
        this.menuTransitionCode[n] = new byte[n2];
        this.menuRowEnabled[n] = new boolean[n2];
        this.eY = 0;
        this.fN = n;
        this.cf = 0;
    }

    void a(int n, int n2, int n3, boolean bl, boolean bl2, int n4) {
        this.buildMenuItems[this.fN][this.eY] = (byte)n2;
        this.buildMenuResultCodes[this.fN][this.eY] = (byte)n3;
        this.menuItemCode[this.fN][this.eY] = (byte)this.cf;
        this.menuTextIndex[this.fN][this.eY] = (byte)n;
        this.menuTransitionCode[this.fN][this.eY] = (byte)n4;
        this.menuRowEnabled[this.fN][this.eY] = bl2;
        this.cf += 15 + (bl ? 2 : 0);
        ++this.eY;
    }

    void v(int n) {
        if (n != -1) {
            this.scenarioBits = (byte)n;
        }
        this.buildMenuItems = new byte[6][];
        this.buildMenuResultCodes = new byte[6][];
        this.menuItemCode = new byte[6][];
        this.menuTextIndex = new byte[6][];
        this.menuTransitionCode = new byte[6][];
        this.menuRowEnabled = new boolean[6][];
        int n2 = 5;
        if ((this.scenarioBits & 1) == 1) {
            ++n2;
        }
        if ((this.scenarioBits & 2) == 2) {
            ++n2;
        }
        if ((this.scenarioBits & 4) == 4) {
            ++n2;
        }
        if (this.scenarioStep >= 2) {
            ++n2;
        }
        this.d(0, n2);
        this.a(202, 0, 0, true, true, 10);
        this.a(205, 1, 3, true, false, 0);
        if ((this.scenarioBits & 1) == 1) {
            this.a(208, 2, 25, false, false, 0);
        }
        if ((this.scenarioBits & 2) == 2) {
            this.a(218, 3, 26, false, false, 0);
        }
        if (this.scenarioStep >= 2) {
            this.a(207, 5, 28, (this.scenarioBits & 4) != 4, false, 0);
        }
        if ((this.scenarioBits & 4) == 4) {
            this.a(206, 4, 27, true, false, 0);
        }
        this.a(236, 11, 31, false, false, 0);
        this.a(234, 8, 29, true, false, 0);
        this.a(210, 9, 5, true, false, 0);
        n2 = 4;
        if ((this.scenarioBits & 8) == 8) {
            ++n2;
        }
        if ((this.scenarioBits & 0x10) == 16) {
            ++n2;
        }
        if ((this.scenarioBits & 0x20) == 32) {
            n2 += 2;
        }
        this.d(1, n2);
        if ((this.scenarioBits & 8) == 8) {
            this.a(211, 0, 8, true, true, 8);
        }
        this.a(205, 1, 3, false, false, 0);
        if ((this.scenarioBits & 0x10) == 16) {
            this.a(216, 2, 13, false, true, 11);
        }
        this.a(217, 3, 15, true, false, 0);
        this.a(219, 4, 14, true, true, 9);
        if ((this.scenarioBits & 0x20) == 32) {
            this.a(203, 5, 2, false, true, 0);
        }
        if ((this.scenarioBits & 0x20) == 32) {
            this.a(204, 6, 1, true, true, 0);
        }
        this.a(201, 9, 17, true, false, 0);
        this.d(2, 2);
        this.a(225, 0, 18, true, true, 0);
        this.a(226, 9, 19, true, true, 0);
        n2 = 3;
        if ((this.scenarioBits & 0x40) == 64) {
            n2 += 2;
        }
        this.d(3, n2);
        this.a(230, 0, 20, false, true, 1);
        this.a(231, 1, 21, true, true, 1);
        if ((this.scenarioBits & 0x40) == 64) {
            this.a(232, 2, 22, false, true, 6);
        }
        if ((this.scenarioBits & 0x40) == 64) {
            this.a(233, 3, 23, true, true, 7);
        }
        this.a(201, 9, 17, true, false, 0);
        this.d(4, 2);
        this.a(211, 0, 24, true, true, 8);
        this.a(226, 9, 19, true, false, 0);
        this.d(5, 5);
        this.a(212, 0, 9, false, true, 2);
        this.a(213, 1, 10, true, true, 3);
        this.a(214, 2, 11, false, true, 4);
        this.a(215, 3, 12, true, true, 5);
        this.a(201, 9, 17, true, false, 0);
    }

    void cycleBuildMenuIndex(boolean bl) {
        this.buildMenuIndex = bl ? ++this.buildMenuIndex : --this.buildMenuIndex;
        if (this.buildMenuIndex < 0) {
            this.buildMenuIndex = this.buildMenuItems[this.buildMenuMode].length - 1;
        }
        if (this.buildMenuIndex == this.buildMenuItems[this.buildMenuMode].length) {
            this.buildMenuIndex = 0;
        }
        this.confirmTargetState = this.buildMenuResultCodes[this.buildMenuMode][this.buildMenuIndex];
        this.menuDirty = true;
        this.confirmCounter = 20;
    }

    int a(Graphics graphics, int n, int n2, int n3, boolean bl) {
        this.a(graphics, n, n3, fX, 20);
        if (this.buildMenuIndex == n2) {
            this.gj = n3;
        }
        return n3 + 15 + (bl ? 3 : 0);
    }

    void b(Graphics graphics) {
        this.h(graphics);
        graphics.setColor(0x606060);
        graphics.fillRect(0, fX, e, 15);
        this.a(graphics, 201, 10, 0, false);
        this.a(graphics, 235, 10, 18, false);
        graphics.setColor(0xFFFFFF);
        graphics.drawRect(18 * this.confirmToggle, fX, 14, 14);
        graphics.drawRect(18 * this.confirmToggle + 1, fX + 1, 12, 12);
    }

    void h(Graphics graphics) {
        if (this.fr != this.dayOfMonth || this.fd != this.monthOfYear || this.dN != this.year || this.cy != this.money || this.menuDirty) {
            int n;
            graphics.setColor(0x909090);
            graphics.fillRect(0, 0, e, 14);
            graphics.setColor(0xBBBBBB);
            graphics.drawRect(0, 0, e, 14);
            graphics.setColor(0x222222);
            graphics.drawLine(1, 14, e, 14);
            graphics.drawLine(e - 1, 1, e - 1, 14);
            StringBuffer stringBuffer = new StringBuffer();
            stringBuffer.append(Math.abs(this.money));
            stringBuffer = stringBuffer.reverse();
            int n2 = (stringBuffer.length() - 1) / 3;
            for (int i = 1; i <= n2; ++i) {
                stringBuffer.insert(4 * i - 1, ':');
            }
            stringBuffer = stringBuffer.reverse();
            String string = (this.money < 0 ? "-" : "") + stringBuffer.toString();
            byte[] byArray = new byte[string.length() + 1];
            byArray[0] = 24;
            for (n = 0; n < string.length(); ++n) {
                byArray[n + 1] = string.charAt(n) == '-' ? 22 : (byte)(string.charAt(n) - 47);
            }
            this.a(graphics, 1, 5, byArray, false);
            string = (String.valueOf(this.dayOfMonth).length() == 1 ? "0" : "") + this.dayOfMonth + "." + (String.valueOf(this.monthOfYear).length() == 1 ? "0" : "") + this.monthOfYear + "." + (e < 140 ? this.year - 1900 : this.year);
            byArray = new byte[string.length()];
            for (n = 0; n < string.length(); ++n) {
                if (string.charAt(n) >= '0' && string.charAt(n) <= '9') {
                    byArray[n] = (byte)(string.charAt(n) - 47);
                }
                if (string.charAt(n) != '.') continue;
                byArray[n] = 11;
            }
            this.a(graphics, e - (e < 140 ? 49 : 61), 5, byArray, false);
            this.cy = this.money;
            this.fr = this.dayOfMonth;
            this.fd = this.monthOfYear;
            this.dN = this.year;
        }
    }

    void a(Graphics graphics) {
        this.h(this.backbufferG);
        if (this.menuDirty) {
            boolean bl = false;
            this.backbufferG.setColor(0x606060);
            this.backbufferG.fillRect(0, fX, e, 15);
            int n = this.c(this.menuItemCode[this.buildMenuMode][this.buildMenuIndex]);
            int n2 = n + 15 - e;
            if (n2 < 0) {
                n2 = 0;
            }
            for (int i = 0; i < this.buildMenuItems[this.buildMenuMode].length; ++i) {
                this.a(this.backbufferG, this.c(this.menuTextIndex[this.buildMenuMode][i]), this.c(this.menuItemCode[this.buildMenuMode][i]) - n2, fX, 20);
                if (i != this.buildMenuIndex) continue;
                this.backbufferG.setColor(0xFFFFFF);
                this.backbufferG.drawRect(this.c(this.menuItemCode[this.buildMenuMode][i]) - n2, fX, 14, 14);
                this.backbufferG.drawRect(this.c(this.menuItemCode[this.buildMenuMode][i]) + 1 - n2, fX + 1, 12, 12);
            }
            if (this.buildMenuMode == 2) {
                String string = String.valueOf(Math.abs(this.selectionCost));
                byte[] byArray = new byte[string.length() + this.n[this.ds[10]].length + 1];
                for (int i = 0; i < string.length(); ++i) {
                    byArray[i + 1] = (byte)(string.charAt(i) - 47);
                }
                byArray[0] = 24;
                System.arraycopy(this.n[this.ds[10]], 0, byArray, string.length() + 1, this.n[this.ds[10]].length);
                this.a(this.backbufferG, 35, fX + 5, byArray, false);
            }
            this.menuDirty = false;
        }
    }

    void p(int n) {
        this.K[2] = this.K[1];
        this.K[1] = this.K[0];
        this.K[0] = n;
        this.fJ[2] = this.fJ[1];
        this.fJ[1] = this.fJ[0];
        this.fJ[0] = 80;
    }

    int a(Graphics graphics, int n, int n2) {
        String string = String.valueOf(Math.abs(n));
        byte[] byArray = new byte[string.length() + 1];
        for (int i = 0; i < string.length(); ++i) {
            byArray[i + 1] = (byte)(string.charAt(i) - 47);
        }
        byArray[0] = 24;
        this.a(graphics, e, 15 + n2, n < 0 ? 1 : 2, 4, byArray);
        return n2 + 15;
    }

    void g(Graphics graphics) {
        graphics.setColor(0xFF0000);
        int n = 0;
        if (this.fJ[2] > 0) {
            this.fJ[2] = (byte)(this.fJ[2] - 1);
            n = this.a(graphics, this.K[2], n);
        }
        if (this.fJ[1] > 0) {
            this.fJ[1] = (byte)(this.fJ[1] - 1);
            n = this.a(graphics, this.K[1], n);
        }
        if (this.fJ[0] > 0) {
            this.fJ[0] = (byte)(this.fJ[0] - 1);
            this.a(graphics, this.K[0], n);
        }
    }

    void d(Graphics graphics) {
        if (this.confirmCounter > 0) {
            if (this.confirmTargetState >= 0) {
                this.a(graphics, 10, 10, 0, 37, this.n[this.ds[120 + this.confirmTargetState]]);
            }
            --this.confirmCounter;
        }
    }

    byte a(byte by, int n, int n2) {
        boolean bl;
        this.constructionTimers[this.numSelectedTrains] = 50;
        this.zonePopulation[this.numSelectedTrains] = by;
        this.trainY2[this.numSelectedTrains] = (short)n;
        this.trainX2[this.numSelectedTrains] = (short)n2;
        short s = 0;
        do {
            byte by2 = (byte)Math.abs(this.rng.nextInt() % 6);
            byte by3 = (byte)Math.abs(this.rng.nextInt() % 14);
            s = (short)(256 * by2 + by3);
            bl = true;
            for (int i = 0; i < this.numSelectedTrains; ++i) {
                if (s != this.di[i]) continue;
                bl = false;
            }
        } while (!bl);
        this.di[this.numSelectedTrains] = s;
        this.numSelectedTrains = (byte)(this.numSelectedTrains + 1);
        return (byte)(this.numSelectedTrains - 1);
    }

    void dayResetP() {
        if (this.monthOfYear % 4 == 0) {
            for (int i = 0; i < 64; ++i) {
                if (this.constructionTimers[i] <= 0) continue;
                int n = i;
                this.constructionTimers[n] = (byte)(this.constructionTimers[n] - 1);
            }
        }
    }

    void d(int n, int n2, int n3) {
        Vector<Integer> vector = new Vector<Integer>();
        for (int i = n - 5; i <= n + 5; ++i) {
            for (int j = n2 - 5; j <= n2 + 5; ++j) {
                int n4;
                if (i < 0 || j < 0 || i >= this.mapWidth || j >= this.mapHeight || (this.trackMap[i][j] & 0xC0) != 128 || vector.indexOf(new Integer(n4 = this.trackMap[i][j] & 0x3F)) != -1) continue;
                vector.addElement(new Integer(n4));
                int n5 = 0;
                if (n3 < 0) {
                    n5 = this.constructionTimers[n4] * n3 * 3 / 200 - 1;
                    if (n5 < n3) {
                        n3 = n5;
                    }
                } else {
                    n5 = (100 - this.constructionTimers[n4]) / 30 * n3 / 3 + 1;
                    if (n5 > n3) {
                        n5 = n3;
                    }
                }
                int n6 = n4;
                this.constructionTimers[n6] = (byte)(this.constructionTimers[n6] + n5);
                if (this.constructionTimers[n4] < 0) {
                    this.constructionTimers[n4] = 0;
                }
                if (this.constructionTimers[n4] <= 100) continue;
                this.constructionTimers[n4] = 100;
            }
        }
    }

    void maybeAITick() {
        int n;
        for (n = 0; n < 64; ++n) {
            this.zonePopulation[n] = 0;
        }
        for (n = 0; n < this.mapWidth; ++n) {
            for (int i = 0; i < this.mapHeight; ++i) {
                if (this.tileMap[n][i] < 34 || this.tileMap[n][i] > 40) continue;
                int n2 = this.trackMap[n][i] & 0x3F;
                this.zonePopulation[n2] = (short)(this.zonePopulation[n2] + 1);
            }
        }
    }

    void a(int n, int n2, int n3) {
        if (this.tileMap[n][n2] == 0 || this.tileMap[n][n2] >= 29 && this.tileMap[n][n2] <= 33) {
            this.tileMap[n][n2] = this.p();
            this.trackMap[n][n2] = (byte)n3;
        }
    }

    boolean v() {
        int n;
        int n2 = Math.abs(this.rng.nextInt() % (this.mapWidth - 2)) + 1;
        if (this.tileMap[n2][n = Math.abs(this.rng.nextInt() % (this.mapHeight - 2)) + 1] == 0 || this.tileMap[n2][n] >= 29 && this.tileMap[n2][n] <= 33) {
            boolean bl;
            byte by = this.tileMap[n2 - 1][n];
            byte by2 = this.tileMap[n2 + 1][n];
            byte by3 = this.tileMap[n2][n - 1];
            byte by4 = this.tileMap[n2][n + 1];
            boolean bl2 = by == 43 || by == 44 || by == 45 || by == 46;
            boolean bl3 = by2 == 43 || by2 == 44 || by2 == 47 || by2 == 48;
            boolean bl4 = by3 == 42 || by3 == 44 || by3 == 46 || by3 == 47;
            boolean bl5 = bl = by4 == 42 || by4 == 44 || by4 == 45 || by4 == 48;
            if (bl2 || bl3 || bl4 || bl) {
                int n3 = -1;
                if (bl2) {
                    n3 = this.trackMap[n2 - 1][n];
                } else if (bl3) {
                    n3 = this.trackMap[n2 + 1][n];
                } else if (bl4) {
                    n3 = this.trackMap[n2][n - 1];
                } else if (bl) {
                    n3 = this.trackMap[n2][n + 1];
                }
                int n4 = n3 & 0x3F;
                if (n4 > -1) {
                    int n5;
                    if (bl2 && bl3 && bl4 && bl) {
                        this.tileMap[n2][n] = 44;
                    } else if (bl2 && bl3 && bl4) {
                        this.tileMap[n2][n] = 44;
                    } else if (bl2 && bl3 && bl) {
                        this.tileMap[n2][n] = 44;
                    } else if (bl2 && bl4 && bl) {
                        this.tileMap[n2][n] = 44;
                    } else if (bl3 && bl4 && bl) {
                        this.tileMap[n2][n] = 44;
                    } else if (bl2 && bl4) {
                        this.tileMap[n2][n] = 48;
                    } else if (bl2 && bl) {
                        this.tileMap[n2][n] = 47;
                    } else if (bl3 && bl4) {
                        this.tileMap[n2][n] = 45;
                    } else if (bl3 && bl) {
                        this.tileMap[n2][n] = 46;
                    } else {
                        n5 = Math.abs(this.rng.nextInt() % 13);
                        if (n5 == 12) {
                            this.tileMap[n2][n] = 44;
                        } else if (n5 == 11) {
                            if (bl2) {
                                this.tileMap[n2][n] = 47;
                            }
                            if (bl3) {
                                this.tileMap[n2][n] = 46;
                            }
                            if (bl4) {
                                this.tileMap[n2][n] = 45;
                            }
                            if (bl) {
                                this.tileMap[n2][n] = 46;
                            }
                        } else if (n5 == 10) {
                            if (bl2) {
                                this.tileMap[n2][n] = 48;
                            }
                            if (bl3) {
                                this.tileMap[n2][n] = 45;
                            }
                            if (bl4) {
                                this.tileMap[n2][n] = 48;
                            }
                            if (bl) {
                                this.tileMap[n2][n] = 47;
                            }
                        } else {
                            if (bl2 || bl3) {
                                this.tileMap[n2][n] = 43;
                            }
                            if (bl4 || bl) {
                                this.tileMap[n2][n] = 42;
                            }
                        }
                    }
                    n5 = this.tileMap[n2][n];
                    switch (n5) {
                        case 42: {
                            this.a(n2 - 1, n, n3);
                            this.a(n2 + 1, n, n3);
                            break;
                        }
                        case 43: {
                            this.a(n2, n - 1, n3);
                            this.a(n2, n + 1, n3);
                            break;
                        }
                        case 45: {
                            this.a(n2 - 1, n, n3);
                            this.a(n2, n + 1, n3);
                            this.a(n2 - 1, n + 1, n3);
                            break;
                        }
                        case 46: {
                            this.a(n2 - 1, n, n3);
                            this.a(n2, n - 1, n3);
                            this.a(n2 - 1, n - 1, n3);
                            break;
                        }
                        case 47: {
                            this.a(n2 + 1, n, n3);
                            this.a(n2, n - 1, n3);
                            this.a(n2 + 1, n - 1, n3);
                            break;
                        }
                        case 48: {
                            this.a(n2 + 1, n, n3);
                            this.a(n2, n + 1, n3);
                            this.a(n2 + 1, n + 1, n3);
                        }
                    }
                    this.trackMap[n2][n] = (byte)n3;
                    return true;
                }
            }
        }
        return false;
    }

    byte f(int n) {
        this.da[this.aE] = (byte)n;
        this.cityPopulation[this.aE] = (byte)(50 + this.rng.nextInt() % 20);
        this.be[this.aE] = 0;
        this.aE = (byte)(this.aE + 1);
        return (byte)(this.aE - 1);
    }

    void dayResetAg() {
        for (int i = 0; i < 48; ++i) {
            int n = i;
            this.cityPopulation[n] = (byte)(this.cityPopulation[n] + this.rng.nextInt() % 8);
            if (this.cityPopulation[i] < 10) {
                this.cityPopulation[i] = 10;
            }
            if (this.cityPopulation[i] <= 90) continue;
            this.cityPopulation[i] = 90;
        }
    }

    void T() {
        for (int i = 0; i < 48; ++i) {
            this.be[i] = 0;
        }
        Vector<String> vector = new Vector<String>();
        for (int i = 0; i < this.mapWidth; ++i) {
            for (int j = 0; j < this.mapHeight; ++j) {
                byte by = this.trackMap[i][j];
                if ((by & 0xC0) != 192) continue;
                int n = by & 0x3F;
                int n2 = i - 3;
                int n3 = j - 3;
                int n4 = i + 3;
                int n5 = j + 3;
                if (n2 < 0) {
                    n2 = 0;
                }
                if (n3 < 0) {
                    n3 = 0;
                }
                if (n4 >= this.mapWidth) {
                    n4 = this.mapWidth - 1;
                }
                if (n5 >= this.mapHeight) {
                    n5 = this.mapHeight - 1;
                }
                for (int k = n2; k <= n4; ++k) {
                    for (int i2 = n3; i2 <= n5; ++i2) {
                        if (this.tileMap[k][i2] != 87 && this.tileMap[k][i2] != 88 || vector.indexOf(n + "_" + k + "_" + i2) != -1) continue;
                        vector.addElement(n + "_" + k + "_" + i2);
                        int n6 = n;
                        this.be[n6] = (byte)(this.be[n6] + 1);
                    }
                }
            }
        }
    }

    byte[] b(int n) {
        StringBuffer stringBuffer = new StringBuffer();
        stringBuffer.append(Math.abs(n));
        stringBuffer = stringBuffer.reverse();
        int n2 = (stringBuffer.length() - 1) / 3;
        for (int i = 1; i <= n2; ++i) {
            stringBuffer.insert(4 * i - 1, ':');
        }
        stringBuffer = stringBuffer.reverse();
        String string = (n < 0 ? "-" : "") + stringBuffer.toString();
        byte[] byArray = new byte[string.length() + 1];
        byArray[0] = 24;
        for (int i = 0; i < string.length(); ++i) {
            byArray[i + 1] = string.charAt(i) == '-' ? 22 : (byte)(string.charAt(i) - 47);
        }
        return byArray;
    }

    byte[] p(int n, int n2) {
        String string = String.valueOf(n);
        byte[] byArray = new byte[string.length() + 1];
        for (int i = 0; i < string.length(); ++i) {
            byArray[i] = string.charAt(i) == '-' ? 22 : (byte)(string.charAt(i) - 47);
        }
        byArray[byArray.length - 1] = (byte)n2;
        return byArray;
    }

    byte[] d(String string) {
        byte[] byArray = new byte[string.length()];
        for (int i = 0; i < byArray.length; ++i) {
            byArray[i] = string.charAt(i) == '-' ? 22 : (string.charAt(i) == '.' ? 11 : (string.charAt(i) == '/' ? 20 : (string.charAt(i) == '%' ? 29 : (byte)(string.charAt(i) - 47))));
        }
        return byArray;
    }

    Image u(int n, int n2) {
        if (n == 23) {
            if (n2 == 0 && this.wagonType >= 0 && this.wagonType < 3) {
                return this.tileSprites[this.c(this.tileSpriteIndex[136])];
            }
            if (n2 == 0 && this.wagonType >= 3 && this.wagonType < 6) {
                return this.tileSprites[this.c(this.tileSpriteIndex[146])];
            }
            if (n2 > 0) {
                switch (this.wagonOptionSlots[n2 - 1]) {
                    case -1: {
                        return null;
                    }
                    case 0: {
                        return this.tileSprites[this.c(this.tileSpriteIndex[162])];
                    }
                    case 1: {
                        return this.tileSprites[this.c(this.tileSpriteIndex[167])];
                    }
                    case 2: {
                        return this.tileSprites[this.c(this.tileSpriteIndex[172])];
                    }
                    case 3: {
                        return this.tileSprites[this.c(this.tileSpriteIndex[182])];
                    }
                }
            }
        }
        if (n == 26 && this.A(this.trainAtCursor) > n2) {
            int n3;
            int n4 = this.trainAtCursor;
            for (n3 = 0; n3 < n2; ++n3) {
                n4 = this.nextCar[n4];
            }
            n3 = this.trainFlags[n4] & 0xF;
            switch (n3) {
                case -1: {
                    return null;
                }
                case 0: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[136])];
                }
                case 1: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[146])];
                }
                case 2: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[162])];
                }
                case 3: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[167])];
                }
                case 4: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[172])];
                }
                case 5: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[177])];
                }
                case 6: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[182])];
                }
                case 7: {
                    return this.tileSprites[this.c(this.tileSpriteIndex[187])];
                }
            }
        }
        return null;
    }

    boolean e(int n, int n2) {
        if (n != 1) {
            this.S = false;
        }
        if (n == 1) {
            if (!this.S) {
                this.cH = this.X();
                this.S = true;
            }
            if (n2 == 0 && this.dP) {
                return false;
            }
            if (n2 == 0 && !this.ready) {
                return true;
            }
            if (n2 == 1 && this.cH) {
                return true;
            }
        }
        if (n == 5 && this.aM > n2) {
            return true;
        }
        if (n == 21) {
            if (n2 == 0 && this.da[this.stationDirCursor] % 2 == 0) {
                return true;
            }
            if (n2 == 1 && this.da[this.stationDirCursor] % 2 == 1) {
                return true;
            }
            if (n2 == 2 && this.da[this.stationDirCursor] % 2 == 1) {
                return true;
            }
        }
        if (n == 22) {
            if (n2 == 0 && this.trainOptionSlots[this.aiWagonsLastSeen][0] == 0 && this.trainOptionSlots[this.aiWagonsLastSeen][1] == 0 && this.trainOptionSlots[this.aiWagonsLastSeen][2] == 0 && this.trainOptionSlots[this.aiWagonsLastSeen][3] == 0) {
                return true;
            }
            if (n2 == 1 && this.trainOptionSlots[this.aiWagonsLastSeen][0] > 0) {
                return true;
            }
            if (n2 == 2 && this.trainOptionSlots[this.aiWagonsLastSeen][1] > 0) {
                return true;
            }
            if (n2 == 3 && this.trainOptionSlots[this.aiWagonsLastSeen][2] > 0) {
                return true;
            }
            if (n2 == 4 && this.trainOptionSlots[this.aiWagonsLastSeen][3] > 0) {
                return true;
            }
            if (n2 == 5 && this.trainOptionSlots[this.aiWagonsLastSeen][4] > 0) {
                return true;
            }
            if (n2 == 6 && this.trainFlagBits[this.aiWagonsLastSeen] == 0) {
                return true;
            }
            if (n2 == 7 && (this.trainFlagBits[this.aiWagonsLastSeen] & 1) == 1) {
                return true;
            }
            if (n2 == 8 && (this.trainFlagBits[this.aiWagonsLastSeen] & 2) == 2) {
                return true;
            }
            if (n2 == 9 && (this.trainFlagBits[this.aiWagonsLastSeen] & 4) == 4) {
                return true;
            }
            if (n2 == 10 && (this.trainFlagBits[this.aiWagonsLastSeen] & 8) == 8) {
                return true;
            }
            if (n2 == 11 && (this.trainFlagBits[this.aiWagonsLastSeen] & 0x10) == 16) {
                return true;
            }
        }
        if (n == 24 && n2 <= this.era) {
            return true;
        }
        if (n == 26) {
            if (n2 < 6 && this.A(this.trainAtCursor) > n2) {
                return true;
            }
            if (n2 == 6 && this.eL[this.trainAtCursor] == 7) {
                return true;
            }
            if (n2 == 7 && this.eL[this.trainAtCursor] != 7) {
                return true;
            }
            if (n2 == 8 && this.trainAgeOrTimer[this.trainAtCursor] > 0) {
                return true;
            }
        }
        return n == 60 && (n2 != 0 ? n2 <= this.aO : this.aO == 0);
    }

    int neighboursMask(int n, int n2) {
        if (n == 34) {
            if (this.W == 0 || this.W == 2) {
                if (n2 == 65) {
                    return 65;
                }
                if (n2 >= 20) {
                    return 64 + this.fA[n2 - 20];
                }
                return 65 + this.fA[n2];
            }
            if (n2 == 65) {
                return 35;
            }
            if (n2 >= 20) {
                return 34 + this.fA[n2 - 20];
            }
            return 35 + this.fA[n2];
        }
        return 0;
    }

    byte[] q(int n, int n2) {
        switch (n) {
            case 14: {
                switch (this.screenParam) {
                    case 0: {
                        return this.n[this.ds[11]];
                    }
                    case 1: {
                        return this.n[this.ds[12]];
                    }
                    case 2: {
                        return this.n[this.ds[13]];
                    }
                    case 3: {
                        return this.n[this.ds[14]];
                    }
                    case 4: {
                        return this.n[this.ds[15]];
                    }
                }
                break;
            }
            case 4: {
                switch (n2) {
                    case 1: {
                        return this.n[this.ds[25 + this.ae]];
                    }
                    case 2: {
                        return this.n[this.ds[30 + this.difficultyA]];
                    }
                    case 3: {
                        return this.n[this.ds[35 + this.difficultySlot]];
                    }
                    case 4: {
                        return this.n[this.ds[40 + this.difficultyB]];
                    }
                    case 5: {
                        return this.n[this.ds[40 + this.difficultyC]];
                    }
                    case 6: {
                        return this.p(this.T, 0);
                    }
                }
                break;
            }
            case 20: {
                switch (n2) {
                    case 0: {
                        byte[] byArray = this.n[this.ds[80 + (this.di[this.junctionDirCursor] >> 8 & 0xFF)]];
                        byte[] byArray2 = this.n[this.ds[86 + (this.di[this.junctionDirCursor] & 0xFF)]];
                        byte[] byArray3 = new byte[byArray2.length + byArray.length];
                        System.arraycopy(byArray2, 0, byArray3, 0, byArray2.length);
                        System.arraycopy(byArray, 0, byArray3, byArray2.length, byArray.length);
                        return byArray3;
                    }
                    case 1: {
                        return this.p(this.zonePopulation[this.junctionDirCursor] * 13, 0);
                    }
                    case 2: {
                        return this.p(this.constructionTimers[this.junctionDirCursor], 29);
                    }
                }
                break;
            }
            case 21: {
                switch (n2) {
                    case 0: {
                        return this.n[this.ds[100 + this.da[this.stationDirCursor]]];
                    }
                    case 1: {
                        return this.n[this.ds[106 + this.da[this.stationDirCursor] / 2]];
                    }
                    case 2: {
                        return this.p(this.prevStationRevenue[this.stationDirCursor], 0);
                    }
                }
                break;
            }
            case 17: {
                return this.p(this.U(), 29);
            }
            case 10: {
                switch (n2) {
                    case 1: {
                        return !ch ? this.n[this.ds[20]] : this.n[this.ds[21]];
                    }
                    case 2: {
                        return !selectionHidden ? this.n[this.ds[20]] : this.n[this.ds[21]];
                    }
                    case 3: {
                        return cc ? this.n[this.ds[20]] : this.n[this.ds[21]];
                    }
                    case 4: {
                        return q ? this.n[this.ds[20]] : this.n[this.ds[21]];
                    }
                    case 5: {
                        return this.ac ? this.n[this.ds[20]] : this.n[this.ds[21]];
                    }
                    case 6: {
                        return this.p(this.gh, 0);
                    }
                }
                break;
            }
            case 24: {
                switch (n2) {
                    case 0: {
                        return this.b(this.gWagonCost);
                    }
                    case 1: {
                        return this.b(this.gWagonCost2);
                    }
                    case 2: {
                        return this.b(this.gWagonCost3);
                    }
                    case 3: {
                        return this.b(this.gWagonCost4);
                    }
                    case 4: {
                        return this.b(this.gWagonCost5);
                    }
                    case 5: {
                        return this.b(this.gWagonCost6);
                    }
                }
                break;
            }
            case 23: {
                return this.b(this.unitCost - this.currentTrainCostSnapshot);
            }
            case 22: {
                return this.p(this.trainOptionSlots[this.aiWagonsLastSeen][n2], 0);
            }
            case 26: {
                int n3;
                if (n2 == 0) {
                    return this.n[this.ds[60 + this.trainEraOrStage[this.trainAtCursor]]];
                }
                if (n2 == 6) {
                    return this.b(this.trainRevenue[this.trainAtCursor]);
                }
                if (n2 == 7) {
                    return this.b(this.prevTrainRevenue[this.trainAtCursor]);
                }
                if (n2 == 8) {
                    return this.p(this.trainAgeOrTimer[this.trainAtCursor], 0);
                }
                if (n2 == 9) {
                    return this.b(this.currentTrainCostSnapshot * 70 / 100);
                }
                if (n2 == 10) {
                    return this.b(this.baseTrainCostSnapshot - this.currentTrainCostSnapshot);
                }
                if (n2 == 20) {
                    return this.bz[this.trainAtCursor] == 0 ? this.n[this.ds[21]] : this.n[this.ds[20]];
                }
                if (n2 <= 0 || n2 >= 6) break;
                int n4 = 0;
                if (this.A(this.trainAtCursor) > n2) {
                    n4 = this.trainAtCursor;
                    for (n3 = 0; n3 < n2; ++n3) {
                        n4 = this.nextCar[n4];
                    }
                }
                n3 = this.trainFlags[n4] & 0xF;
                String string = String.valueOf(this.trainEnergyOrCount[n4]);
                if (n3 == 2) {
                    string = string + "/" + this.wagonEnergyRemaining[n4];
                }
                return this.d(string);
            }
            case 30: {
                switch (n2) {
                    case 0: {
                        return this.b(this.loanBalance);
                    }
                    case 1: {
                        return this.b(this.loanCeiling);
                    }
                    case 2: {
                        return this.p(this.aD, 29);
                    }
                }
                break;
            }
            case 32: {
                switch (n2) {
                    case 0: {
                        return this.b(this.y ? this.gx : this.totalSpent);
                    }
                    case 1: {
                        return this.b(this.y ? this.eM : this.aiMoneySpent);
                    }
                    case 2: {
                        return this.b(this.y ? this.prevFuelCost : this.fuelCostAccrued);
                    }
                    case 3: {
                        return this.b(this.y ? this.prevSubsidyCost : this.ho);
                    }
                    case 4: {
                        return this.b(this.y ? this.prevTerrainTotal : this.eg);
                    }
                    case 5: {
                        return this.b(this.y ? this.prevEnterpriseTotal : this.eJ);
                    }
                    case 6: {
                        return this.b(this.y ? this.prevTotalRevenue : this.totalRevenue);
                    }
                    case 7: {
                        return this.b(this.y ? this.prevXIndicator : this.eX);
                    }
                }
                break;
            }
            case 35: {
                switch (n2) {
                    case 0: {
                        return this.b(this.y ? this.bG : this.br);
                    }
                    case 1: {
                        return this.b(this.y ? this.he : this.gV);
                    }
                    case 2: {
                        return this.b(this.y ? this.bP : this.cj);
                    }
                    case 3: {
                        return this.b(this.y ? this.ht : this.aA);
                    }
                    case 4: {
                        return this.b(this.y ? this.cg : this.gs);
                    }
                    case 5: {
                        return this.b(this.y ? this.cm : this.dq);
                    }
                    case 6: {
                        return this.b(this.y ? this.cr : this.dg);
                    }
                    case 7: {
                        return this.b(this.y ? this.gq : this.bM);
                    }
                }
                break;
            }
            case 33: {
                if (n2 == 65) {
                    return this.n[this.ds[57 + this.W]];
                }
                if (n2 >= 40) {
                    if (!this.bR) {
                        if (this.W == 0) {
                            return this.b(this.yrN[n2 - 40]);
                        }
                        if (this.W == 1) {
                            return this.b(this.yrCx[n2 - 40]);
                        }
                        if (this.W == 2) {
                            return this.b(this.yrZ[n2 - 40]);
                        }
                    } else {
                        if (this.W == 0) {
                            return this.b(this.dG[n2 - 40]);
                        }
                        if (this.W == 1) {
                            return this.b(this.ef[n2 - 40]);
                        }
                        if (this.W == 2) {
                            return this.b(this.L[n2 - 40]);
                        }
                    }
                } else {
                    if (n2 >= 20) {
                        return !this.bR ? this.p(this.dm[n2 - 20], 0) : null;
                    }
                    return !this.bR ? this.p(this.o[n2] + 1940, 20) : this.p(this.dm[n2] + 1940, 0);
                }
            }
            case 34: {
                if (n2 == 65) {
                    return this.n[this.ds[54 + this.W]];
                }
                if (n2 >= 20) {
                    return !this.bR ? this.p(this.dm[n2 - 20], 0) : null;
                }
                return !this.bR ? this.p(this.o[n2] + 40, 20) : this.p(this.dm[n2] + 1940, 0);
            }
            case 41: 
            case 42: 
            case 44: 
            case 45: 
            case 46: 
            case 47: 
            case 48: 
            case 49: 
            case 50: 
            case 51: 
            case 52: 
            case 53: 
            case 54: {
                if (n2 == 1) {
                    byte[] byArray = this.p(this.bY[this.cZ], 11);
                    byte[] byArray4 = this.p(this.dE[this.cZ], 11);
                    byte[] byArray5 = this.p(this.fF[this.cZ] + 1950, 0);
                    byte[] byArray6 = new byte[byArray.length + byArray4.length + byArray5.length];
                    System.arraycopy(byArray, 0, byArray6, 0, byArray.length);
                    System.arraycopy(byArray4, 0, byArray6, byArray.length, byArray4.length);
                    System.arraycopy(byArray5, 0, byArray6, byArray.length + byArray4.length, byArray5.length);
                    return byArray6;
                }
                if (n2 == 2) {
                    return this.b(this.menuRowCost[this.cZ]);
                }
                if (n2 == 3) {
                    return this.p(this.a(this.cZ), 29);
                }
                if (n2 != 4) break;
                return this.p(this.menuRowCost[this.cZ], 0);
            }
            case 60: {
                if (n2 % 2 == 1) {
                    int n5 = n2 / 2;
                    return this.n[this.c(this.ds[this.menuRowType[this.gP[n5]] + 160])];
                }
                int n6 = (n2 - 1) / 2;
                return this.p(this.a(this.gP[n6]), 29);
            }
            case 61: 
            case 62: 
            case 63: 
            case 64: 
            case 65: 
            case 66: 
            case 67: 
            case 68: 
            case 69: 
            case 70: 
            case 71: 
            case 72: {
                if (n2 == 1) {
                    return this.p(this.menuRowEra[this.cZ], 29);
                }
                if (n2 == 2) {
                    return this.b(this.menuRowRevenue[this.cZ]);
                }
                if (n2 != 3) break;
                return this.p(this.menuRowEra[this.cZ], 0);
            }
            case 27: {
                if (n2 == 1) {
                    return this.bE;
                }
                if (n2 != 0) break;
                this.cD = new byte[10];
                this.cD[this.cu] = 28;
                return this.cD;
            }
            case 28: {
                if (n2 == 0) {
                    return this.p(this.H, 0);
                }
                if (n2 >= 1 && n2 <= 13) {
                    return this.p(this.gk[n2 - 1], 0);
                }
                if (n2 < 20 || n2 > 33) break;
                return this.p(this.bd[n2 - 20], 0);
            }
            case 6: {
                return this.p(this.gJ, 0);
            }
        }
        return null;
    }

    void v(int n, int n2) {
        block0 : switch (n) {
            case 1: {
                switch (n2) {
                    case 1: {
                        if (this.ready) {
                            this.transitionCode = 4;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 4;
                        this.setState(30);
                        break;
                    }
                    case 2: {
                        this.screenParam = 1;
                        if (this.ready) {
                            this.transitionCode = 14;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 14;
                        this.setState(30);
                        break;
                    }
                    case 3: {
                        this.screenParam = 0;
                        if (this.ready) {
                            this.transitionCode = 14;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 14;
                        this.setState(30);
                        break;
                    }
                    case 4: {
                        if (this.ready) {
                            this.transitionCode = 10;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 10;
                        this.setState(30);
                        break;
                    }
                    case 5: {
                        if (this.ready) {
                            this.transitionCode = 2;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 2;
                        this.setState(30);
                        break;
                    }
                    case 6: {
                        if (this.ready) {
                            this.transitionCode = 3;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 3;
                        this.setState(30);
                        break;
                    }
                    case 7: {
                        this.setState(30);
                        break;
                    }
                    case 8: {
                        if (this.ready) {
                            this.transitionCode = 5;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 5;
                        this.setState(30);
                        break;
                    }
                    case 9: {
                        if (this.ready) {
                            this.transitionCode = 28;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 28;
                        this.setState(30);
                        break;
                    }
                    case 99: {
                        this.screenParam = 2;
                        if (this.ready) {
                            this.transitionCode = 14;
                            this.o();
                            break;
                        }
                        this.pendingTransitionCode = 14;
                        this.setState(30);
                    }
                }
                break;
            }
            case 14: {
                if (n2 == 2) {
                    if (this.screenParam == 4) {
                        this.pendingTransitionCode = 26;
                        this.setState(30);
                        break;
                    }
                    if (this.screenParam == 3) {
                        this.setState(10);
                        break;
                    }
                    if (this.ready) {
                        this.transitionCode = 1;
                        this.o();
                        break;
                    }
                    this.pendingTransitionCode = 1;
                    this.setState(30);
                    break;
                }
                switch (this.screenParam) {
                    case 0: {
                        this.setState(55);
                        break;
                    }
                    case 1: {
                        this.setState(65);
                        break;
                    }
                    case 2: {
                        this.stopRequested = true;
                        break;
                    }
                    case 3: {
                        this.musicAutoStart = true;
                        this.ac = true;
                        this.setState(10);
                        break;
                    }
                    case 4: {
                        this.money += this.currentTrainCostSnapshot * 70 / 100;
                        this.aiMoneySpent -= this.currentTrainCostSnapshot * 70 / 100;
                        if (this.currentTrainCostSnapshot != 0) {
                            this.p(this.currentTrainCostSnapshot * 70 / 100);
                        }
                        short s = this.nextCar[this.trainAtCursor];
                        this.o(s);
                        this.trainFlags[this.trainAtCursor] = 0;
                        while (s != -1) {
                            this.trainFlags[s] = 0;
                            s = this.nextCar[s];
                        }
                        this.setState(30);
                    }
                }
                break;
            }
            case 36: {
                this.setState(40);
                break;
            }
            case 17: {
                if (n2 == 1) {
                    this.pendingTransitionCode = 80 + this.ao;
                }
                this.setState(30);
                break;
            }
            case 2: 
            case 3: {
                if (this.ready) {
                    this.transitionCode = 1;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = 1;
                this.setState(30);
                break;
            }
            case 6: {
                this.setState(30);
                break;
            }
            case 7: {
                this.ready = false;
                this.dP = true;
                this.transitionCode = 1;
                this.o();
                this.setState(20);
                break;
            }
            case 5: {
                if (n2 >= 2) {
                    this.J = n2;
                    this.setState(25);
                    break;
                }
                if (this.ready) {
                    this.transitionCode = 1;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = 1;
                this.setState(30);
                break;
            }
            case 4: {
                switch (n2) {
                    case 1: {
                        if (this.ae == 0) {
                            this.ae = 1;
                            break block0;
                        }
                        this.ae = 0;
                        break block0;
                    }
                    case 2: {
                        if (this.difficultyA == 0) {
                            this.difficultyA = 1;
                            break block0;
                        }
                        this.difficultyA = 0;
                        break block0;
                    }
                    case 3: {
                        if (this.difficultySlot == 0) {
                            this.difficultySlot = 1;
                            break block0;
                        }
                        this.difficultySlot = 0;
                        break block0;
                    }
                    case 4: {
                        if (this.difficultyB == 0) {
                            this.difficultyB = 1;
                            break block0;
                        }
                        this.difficultyB = 0;
                        break block0;
                    }
                    case 5: {
                        if (this.difficultyC == 0) {
                            this.difficultyC = 1;
                            break block0;
                        }
                        this.difficultyC = 0;
                        break block0;
                    }
                    case 6: {
                        this.T = (byte)(this.T + 1);
                        if (this.T != 4) break block0;
                        this.T = 1;
                        break block0;
                    }
                    case 10: {
                        this.J = this.ae;
                        this.setState(25);
                        break block0;
                    }
                }
                if (this.ready) {
                    this.transitionCode = 1;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = 1;
                this.setState(30);
                break;
            }
            case 20: {
                switch (n2) {
                    default: 
                }
                this.setState(30);
                break;
            }
            case 21: {
                switch (n2) {
                    default: 
                }
                this.setState(30);
                break;
            }
            case 22: {
                switch (n2) {
                    case 1: {
                        int n3;
                        this.wagonType = 0;
                        this.currentTrainCostSnapshot = 0;
                        this.wagonDiscountType = this.wagonType;
                        for (n3 = 0; n3 < 5; ++n3) {
                            this.wagonOptionSlots[n3] = -1;
                        }
                        for (n3 = 0; n3 < 5; ++n3) {
                            this.wagonCountMatch[n3] = -1;
                        }
                        this.difficultyPct = 100;
                        this.discountApplied = false;
                        this.computeTrainCost();
                        this.pendingTransitionCode = 23;
                        this.setState(30);
                        break block0;
                    }
                }
                this.setState(30);
                break;
            }
            case 23: {
                if (n2 == 0) {
                    this.pendingTransitionCode = 24;
                    this.setState(30);
                    break;
                }
                if (n2 == 1) {
                    this.pendingTransitionCode = 25;
                    this.setState(30);
                    break;
                }
                if (n2 == 2) {
                    if (!this.discountApplied) {
                        if (!this.y()) break;
                        this.setState(30);
                        break;
                    }
                    if (!this.a(this.trainAtCursor, this.wagonType, this.wagonOptionSlots)) break;
                    this.pendingTransitionCode = 26;
                    this.setState(30);
                    break;
                }
                this.pendingTransitionCode = !this.discountApplied ? 22 : 26;
                this.setState(30);
                break;
            }
            case 24: {
                if (n2 >= 0 && n2 < 6) {
                    this.wagonType = n2;
                    this.computeTrainCost();
                }
                this.pendingTransitionCode = 23;
                this.setState(30);
                break;
            }
            case 25: {
                if (n2 == 3) {
                    this.addSelectedOption(0);
                } else if (n2 == 1) {
                    this.addSelectedOption(1);
                } else if (n2 == 0) {
                    this.addSelectedOption(2);
                } else if (n2 == 2) {
                    this.addSelectedOption(3);
                } else if (n2 == 9) {
                    this.popSelectedOption();
                }
                this.computeTrainCost();
                this.pendingTransitionCode = 23;
                this.setState(30);
                break;
            }
            case 26: {
                if (n2 == 1) {
                    this.discountApplied = true;
                    this.pendingTransitionCode = 23;
                    this.setState(30);
                } else if (n2 == 2) {
                    int n4 = this.baseTrainCostSnapshot - this.currentTrainCostSnapshot;
                    if (n4 <= this.money) {
                        this.trainAgeOrTimer[this.trainAtCursor] = 0;
                        this.money -= n4;
                        this.p(-n4);
                        this.aiMoneySpent += n4;
                        this.difficultyPct = 100 - 3 * this.trainAgeOrTimer[this.trainAtCursor];
                        this.computeTrainCost();
                        this.currentTrainCostSnapshot = this.unitCost;
                        this.pendingTransitionCode = 26;
                        this.setState(30);
                    } else {
                        this.enterOutOfMoneyState();
                    }
                } else if (n2 == 3) {
                    this.screenParam = 4;
                    this.pendingTransitionCode = 14;
                    this.setState(30);
                }
                if (n2 == 4) {
                    this.bz[this.trainAtCursor] = (byte)(this.bz[this.trainAtCursor] == 0 ? 1 : 0);
                    break;
                }
                this.setState(30);
                break;
            }
            case 10: {
                switch (n2) {
                    case 1: {
                        ch = !ch;
                        break block0;
                    }
                    case 2: {
                        selectionHidden = !selectionHidden;
                        break block0;
                    }
                    case 3: {
                        cc = !cc;
                        break block0;
                    }
                    case 4: {
                        q = !q;
                        break block0;
                    }
                    case 5: {
                        boolean bl = this.ac = !this.ac;
                        if (this.ac) {
                            this.playMusic();
                            break block0;
                        }
                        this.stopMusic();
                        break block0;
                    }
                    case 6: {
                        ++this.gh;
                        if (this.gh == 6) {
                            this.gh = 1;
                        }
                        this.C(this.gh);
                        break block0;
                    }
                }
                this.O();
                if (this.ready) {
                    this.transitionCode = 1;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = 1;
                this.setState(30);
                break;
            }
            case 18: 
            case 19: {
                if (this.ready) {
                    this.transitionCode = 1;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = 1;
                this.setState(30);
                break;
            }
            case 8: 
            case 9: {
                if (n2 == 1) {
                    this.pendingTransitionCode = this.fY;
                    this.setState(30);
                    break;
                }
                if (n2 == 2) {
                    this.pendingTransitionCode = this.dR;
                    this.setState(30);
                    break;
                }
                this.setState(30);
                break;
            }
            case 11: 
            case 12: 
            case 13: 
            case 16: {
                this.setState(30);
                break;
            }
            case 15: {
                if (n2 == 1) {
                    this.musicAutoStart = true;
                }
                this.setState(10);
                break;
            }
            case 30: {
                switch (n2) {
                    case 1: {
                        if (this.loanCeiling - 20000 < this.loanBalance) break;
                        this.loanBalance += 20000;
                        this.money += 20000;
                        this.z(-20000);
                        break;
                    }
                    case 2: {
                        if (this.loanBalance < 20000 || this.money < 20000) break;
                        this.loanBalance -= 20000;
                        this.money -= 20000;
                        this.z(20000);
                        break;
                    }
                    default: {
                        this.setState(30);
                        this.menuDirty = true;
                    }
                }
                this.mapRedrawPending = true;
                break;
            }
            case 31: {
                this.dS = false;
                switch (n2) {
                    case 0: {
                        this.y = false;
                        this.archiveOrRecall();
                        this.pendingTransitionCode = 32;
                        this.setState(30);
                        break block0;
                    }
                    case 1: {
                        this.y = true;
                        this.pendingTransitionCode = 32;
                        this.setState(30);
                        break block0;
                    }
                    case 2: {
                        this.b(0, false);
                        this.pendingTransitionCode = 33;
                        this.setState(30);
                        break block0;
                    }
                    case 3: {
                        this.b(0, false);
                        this.pendingTransitionCode = 34;
                        this.setState(30);
                        break block0;
                    }
                    case 4: {
                        this.b(1, false);
                        this.pendingTransitionCode = 33;
                        this.setState(30);
                        break block0;
                    }
                    case 5: {
                        this.b(1, false);
                        this.pendingTransitionCode = 34;
                        this.setState(30);
                        break block0;
                    }
                    case 6: {
                        this.b(2, false);
                        this.pendingTransitionCode = 33;
                        this.setState(30);
                        break block0;
                    }
                    case 7: {
                        this.b(2, false);
                        this.pendingTransitionCode = 34;
                        this.setState(30);
                        break block0;
                    }
                    case 8: {
                        this.b(0, true);
                        this.pendingTransitionCode = 33;
                        this.setState(30);
                        break block0;
                    }
                    case 9: {
                        this.b(0, true);
                        this.pendingTransitionCode = 34;
                        this.setState(30);
                        break block0;
                    }
                    case 10: {
                        this.b(1, true);
                        this.pendingTransitionCode = 33;
                        this.setState(30);
                        break block0;
                    }
                    case 11: {
                        this.b(1, true);
                        this.pendingTransitionCode = 34;
                        this.setState(30);
                        break block0;
                    }
                    case 12: {
                        this.b(2, true);
                        this.pendingTransitionCode = 33;
                        this.setState(30);
                        break block0;
                    }
                    case 13: {
                        this.b(2, true);
                        this.pendingTransitionCode = 34;
                        this.setState(30);
                        break block0;
                    }
                    case 14: {
                        this.y = false;
                        this.archiveOrRecall();
                        this.pendingTransitionCode = 35;
                        this.setState(30);
                        break block0;
                    }
                    case 15: {
                        this.y = true;
                        this.pendingTransitionCode = 35;
                        this.setState(30);
                        break block0;
                    }
                }
                this.setState(30);
                this.menuDirty = true;
                break;
            }
            case 32: 
            case 35: {
                if (!this.dS) {
                    this.pendingTransitionCode = 31;
                    this.setState(30);
                    break;
                }
                this.setState(30);
                this.menuDirty = true;
                break;
            }
            case 33: 
            case 34: {
                this.pendingTransitionCode = 31;
                this.setState(30);
                break;
            }
            case 40: {
                switch (n2) {
                    case 1: {
                        this.pendingTransitionCode = this.menuRowType[this.cZ] + 40;
                        this.cW = (byte)2;
                        break;
                    }
                    case 2: {
                        this.pendingTransitionCode = this.menuRowAltType[this.cZ] + 60;
                        this.cW = (byte)2;
                    }
                }
                this.setState(30);
                break;
            }
            case 41: 
            case 42: 
            case 44: 
            case 45: 
            case 46: 
            case 47: 
            case 48: 
            case 49: 
            case 50: 
            case 51: 
            case 52: 
            case 53: 
            case 54: {
                if (this.cW == 1) {
                    this.pendingTransitionCode = 60;
                }
                if (this.cW == 2) {
                    this.pendingTransitionCode = 40;
                }
                if (this.cW == 3) {
                    this.pendingTransitionCode = 9;
                }
                if (this.cW == 4) {
                    this.pendingTransitionCode = 8;
                }
                this.setState(30);
                break;
            }
            case 60: {
                if (n2 % 2 == 1) {
                    this.cZ = this.gP[n2 / 2];
                    this.pendingTransitionCode = this.menuRowType[this.cZ] + 40;
                }
                if (n2 % 2 == 0) {
                    this.cZ = this.gP[(n2 - 1) / 2];
                    this.pendingTransitionCode = this.menuRowAltType[this.cZ] + 60;
                }
                this.cW = 1;
                this.setState(30);
                break;
            }
            case 61: 
            case 62: 
            case 63: 
            case 64: 
            case 65: 
            case 66: 
            case 67: 
            case 68: 
            case 69: 
            case 70: 
            case 71: 
            case 72: {
                if (this.cW == 1) {
                    this.pendingTransitionCode = 60;
                }
                if (this.cW == 2) {
                    this.pendingTransitionCode = 40;
                }
                if (this.cW == 3) {
                    this.pendingTransitionCode = 9;
                }
                if (this.cW == 4) {
                    this.pendingTransitionCode = 8;
                }
                this.setState(30);
                break;
            }
            case 28: {
                if (n2 == 41) {
                    this.ad = false;
                    this.setState(120);
                    break;
                }
                if (this.ready) {
                    this.transitionCode = n2 == 40 ? 27 : 1;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = n2 == 40 ? 27 : 1;
                this.setState(30);
                break;
            }
            case 27: {
                if (n2 == 40) {
                    this.ad = true;
                    this.setState(120);
                    break;
                }
                if (n2 == 0) {
                    this.cu = (byte)(this.cu - 1);
                }
                if (n2 == 1) {
                    this.cu = (byte)(this.cu + 1);
                }
                if (this.cu < 0) {
                    this.cu = (byte)9;
                }
                if (this.cu == 10) {
                    this.cu = 0;
                }
                this.fi = this.bE[this.cu];
                if (n2 == 2) {
                    this.fi = (byte)(this.fi - 1);
                    if (this.fi < 0) {
                        this.fi = (byte)95;
                    }
                    if (this.fi == 69) {
                        this.fi = (byte)65;
                    }
                    if (this.fi == 39) {
                        this.fi = (byte)17;
                    }
                    this.bE[this.cu] = this.fi;
                }
                if (n2 == 3) {
                    this.fi = (byte)(this.fi + 1);
                    if (this.fi == 18) {
                        this.fi = (byte)40;
                    }
                    if (this.fi == 66) {
                        this.fi = (byte)70;
                    }
                    if (this.fi == 96) {
                        this.fi = 0;
                    }
                    this.bE[this.cu] = this.fi;
                }
                if (n2 >= 0) break;
                this.O();
                if (this.ready) {
                    this.transitionCode = 28;
                    this.o();
                    break;
                }
                this.pendingTransitionCode = 28;
                this.setState(30);
            }
        }
        if (n > 80 && n < 100) {
            this.setState(30);
        }
    }

    boolean K() {
        this.aQ = new short[this.mapWidth][this.mapHeight];
        this.fl = 10;
        this.selectionCost = 0;
        this.b();
        this.aQ[this.placementX][this.placementY] = 20;
        this.C();
        if (this.aQ[this.demolishX][this.demolishY] < 20) {
            this.aQ = null;
            return false;
        }
        this.S();
        this.aQ = null;
        this.N();
        if (this.et == 0) {
            return false;
        }
        return this.selectionCost != 0;
    }

    void S() {
        int n;
        int n2;
        int n3;
        boolean bl;
        int n4 = this.demolishX;
        int n5 = this.demolishY;
        boolean bl2 = true;
        do {
            bl = true;
            this.dX = n4;
            this.gO = n5;
            n3 = this.tileMap[n4][n5];
            if (n3 == 7 || n3 == 10 || n3 == 21 || n3 == 23 || n3 == 71) {
                this.o(n4 - 1, n5);
                this.o(n4 + 1, n5);
            }
            if (n3 == 8 || n3 == 9 || n3 == 22 || n3 == 24 || n3 == 70) {
                this.o(n4, n5 - 1);
                this.o(n4, n5 + 1);
            }
            if (n4 == this.placementX && n5 == this.placementY) {
                bl = false;
            } else if (this.ed % 2 == 0) {
                if (!(this.b(n4 + 1, n5, this.aQ[n4][n5]) || this.b(n4 - 1, n5, this.aQ[n4][n5]) || this.b(n4, n5 + 1, this.aQ[n4][n5]) || this.b(n4, n5 - 1, this.aQ[n4][n5]))) {
                    bl = false;
                    bl2 = false;
                }
            } else if (!(this.b(n4, n5 + 1, this.aQ[n4][n5]) || this.b(n4, n5 - 1, this.aQ[n4][n5]) || this.b(n4 + 1, n5, this.aQ[n4][n5]) || this.b(n4 - 1, n5, this.aQ[n4][n5]))) {
                bl = false;
                bl2 = false;
            }
            ++this.ed;
            this.aQ[n4][n5] = 9;
            n4 = this.dX;
            n5 = this.gO;
            n2 = 0;
            n = 0;
            if (this.tileMap[n4][n5] == 7) {
                n2 = -1;
            }
            if (this.tileMap[n4][n5] == 8) {
                n = -1;
            }
            if (this.tileMap[n4][n5] == 9) {
                n = 1;
            }
            if (this.tileMap[n4][n5] == 10) {
                n2 = 1;
            }
            if (n == 0 && n2 == 0) continue;
            do {
                this.aQ[n4][n5] = 9;
            } while (this.aQ[n4 += n][n5 += n2] == 3);
        } while (bl);
        for (n3 = 0; n3 < this.mapWidth; ++n3) {
            for (n2 = 0; n2 < this.mapHeight; ++n2) {
                if (this.aQ[n3][n2] == 9) continue;
                this.aQ[n3][n2] = 0;
            }
        }
        if (bl2) {
            for (n3 = 0; n3 < this.mapWidth; ++n3) {
                for (n2 = 0; n2 < this.mapHeight; ++n2) {
                    boolean bl3;
                    boolean bl4;
                    boolean bl5;
                    if (this.aQ[n3][n2] != 9) continue;
                    int n6 = n3 > 0 ? (this.aQ[n3 - 1][n2] == 9 ? 1 : 0) : (n = 0);
                    boolean bl6 = n3 < this.mapWidth - 1 ? this.aQ[n3 + 1][n2] == 9 : (bl5 = false);
                    boolean bl7 = n2 > 0 ? this.aQ[n3][n2 - 1] == 9 : (bl4 = false);
                    boolean bl8 = n2 < this.mapHeight - 1 ? this.aQ[n3][n2 + 1] == 9 : (bl3 = false);
                    if ((n != 0 ? 1 : 0) + (bl5 ? 1 : 0) + (bl4 ? 1 : 0) + (bl3 ? 1 : 0) == 1) {
                        n = this.a(n != 0, n3 - 1, n2, 71, 72, 74, 78, 80, 84, 88, 90, true);
                        bl5 = this.a(bl5, n3 + 1, n2, 71, 73, 75, 78, 80, 85, 88, 90, true);
                        bl4 = this.a(bl4, n3, n2 - 1, 70, 73, 74, 77, 79, 83, 87, 89, false);
                        bl3 = this.a(bl3, n3, n2 + 1, 70, 72, 75, 77, 79, 86, 87, 89, false);
                    }
                    byte by = this.tileMap[n3][n2];
                    boolean bl9 = false;
                    if (this.tileMap[n3][n2] != 0 && this.tileMap[n3][n2] > 28) {
                        this.selectionCost += this.baseTrainSpeed;
                    }
                    int n7 = this.selectionCost;
                    if (n != 0 && bl5) {
                        if (this.tileMap[n3][n2] == 22) {
                            this.tileMap[n3][n2] = 78;
                        } else if (this.tileMap[n3][n2] == 24) {
                            this.tileMap[n3][n2] = 80;
                        } else if (this.tileMap[n3][n2] == 9) {
                            this.tileMap[n3][n2] = 85;
                            this.selectionCost += this.moneyBaseA;
                        } else if (this.tileMap[n3][n2] == 8) {
                            this.tileMap[n3][n2] = 84;
                            this.selectionCost += this.moneyBaseA;
                        } else if (this.tileMap[n3][n2] == 70) {
                            this.tileMap[n3][n2] = 76;
                            bl9 = true;
                            this.trackMap[n3][n2] = (byte)(0x40 | this.trackMap[n3][n2] & 0x3F | this.bW << 3);
                        } else if (this.tileElevation[n3][n2] == -1) {
                            this.tileMap[n3][n2] = 81;
                            this.selectionCost += this.moneyBaseB;
                        } else {
                            this.tileMap[n3][n2] = 71;
                        }
                    } else if (bl4 && bl3) {
                        if (this.tileMap[n3][n2] == 21) {
                            this.tileMap[n3][n2] = 77;
                        } else if (this.tileMap[n3][n2] == 23) {
                            this.tileMap[n3][n2] = 79;
                        } else if (this.tileMap[n3][n2] == 7) {
                            this.tileMap[n3][n2] = 83;
                            this.selectionCost += this.moneyBaseA;
                        } else if (this.tileMap[n3][n2] == 10) {
                            this.tileMap[n3][n2] = 86;
                            this.selectionCost += this.moneyBaseA;
                        } else if (this.tileMap[n3][n2] == 71) {
                            this.tileMap[n3][n2] = 76;
                            bl9 = true;
                            this.trackMap[n3][n2] = (byte)(0x40 | (this.trackMap[n3][n2] & 0x3F) << 3 | this.bW);
                        } else if (this.tileElevation[n3][n2] == -1) {
                            this.tileMap[n3][n2] = 82;
                            this.selectionCost += this.moneyBaseB;
                        } else {
                            this.tileMap[n3][n2] = 70;
                        }
                    } else if (n != 0 && bl4) {
                        this.tileMap[n3][n2] = 75;
                    } else if (bl5 && bl4) {
                        this.tileMap[n3][n2] = 72;
                    } else if (n != 0 && bl3) {
                        this.tileMap[n3][n2] = 73;
                    } else if (bl5 && bl3) {
                        this.tileMap[n3][n2] = 74;
                    }
                    if (!bl9 && by != this.tileMap[n3][n2]) {
                        this.trackMap[n3][n2] = (byte)(0x40 | this.bW);
                        if (n7 == this.selectionCost) {
                            this.selectionCost += this.accel1;
                        }
                    }
                    if (!bl9) continue;
                    this.selectionCost += this.brake1 + this.accel1 + this.baseTrainSpeed;
                }
            }
        }
    }

    boolean a(boolean bl, int n, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, boolean bl2) {
        if (!bl && n >= 0 && n <= this.mapWidth - 1 && n2 >= 0 && n2 <= this.mapHeight - 1) {
            byte by = this.tileMap[n][n2];
            int n11 = this.trackMap[n][n2] & 0x3F;
            n11 = by == 76 && bl2 ? (n11 >>= 3) : (n11 &= 7);
            if (n11 == this.df && (by == n3 || by == n4 || by == n5 || by == n6 || by == n7 || by == n8 || by == n9 || by == n10 || by == 76)) {
                bl = true;
            }
        }
        return bl;
    }

    boolean b(int n, int n2, int n3) {
        boolean bl = false;
        if (n >= 0 && n < this.mapWidth && n2 >= 0 && n2 < this.mapHeight && this.aQ[n][n2] >= 20 && this.aQ[n][n2] < n3) {
            bl = true;
            this.dX = n;
            this.gO = n2;
        }
        return bl;
    }

    void o(int n, int n2) {
        if (n > 0 && n < this.mapWidth && n2 > 0 && n2 < this.mapHeight && (this.tileMap[n][n2] >= 7 && this.tileMap[n][n2] <= 10 || this.tileMap[n][n2] >= 21 && this.tileMap[n][n2] <= 24 || this.tileMap[n][n2] == 70 || this.tileMap[n][n2] == 71) && this.aQ[n][n2] > 0) {
            this.aQ[n][n2] = 0;
        }
    }

    void C() {
        boolean bl = true;
        boolean bl2 = false;
        for (int i = 50; bl && !bl2 && i > 0; --i) {
            int n;
            int n2;
            int n3;
            bl = false;
            for (n3 = this.placementX; n3 >= 0; --n3) {
                for (n2 = this.placementY; n2 >= 0; --n2) {
                    n = this.hPath(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
            for (n3 = this.placementX; n3 < this.mapWidth; ++n3) {
                for (n2 = this.placementY; n2 < this.mapHeight; ++n2) {
                    n = this.hPath(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
            for (n3 = this.placementX; n3 >= 0; --n3) {
                for (n2 = this.placementY; n2 < this.mapHeight; ++n2) {
                    n = this.hPath(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
            for (n3 = this.placementX; n3 < this.mapWidth; ++n3) {
                for (n2 = this.placementY; n2 >= 0; --n2) {
                    n = this.hPath(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
        }
    }

    int hPath(int n, int n2) {
        boolean bl = false;
        if (this.aQ[n][n2] > 0 && this.aQ[n][n2] != 3 && this.aQ[n][n2] != 7) {
            int n3;
            int n4 = n3 = this.aQ[n][n2] < 20 ? Short.MAX_VALUE : this.aQ[n][n2];
            n4 = this.h(n - 1, n2, n4);
            n4 = this.h(n + 1, n2, n4);
            n4 = this.h(n, n2 - 1, n4);
            if ((n4 = (int)this.h(n, n2 + 1, n4)) < n3) {
                n4 = (short)(n4 + 1);
                if (this.aQ[n][n2] == 2) {
                    int n5 = 0;
                    int n6 = 0;
                    int n7 = n;
                    int n8 = n2;
                    if (this.tileMap[n][n2] == 7) {
                        n5 = -1;
                    }
                    if (this.tileMap[n][n2] == 8) {
                        n6 = -1;
                    }
                    if (this.tileMap[n][n2] == 9) {
                        n6 = 1;
                    }
                    if (this.tileMap[n][n2] == 10) {
                        n5 = 1;
                    }
                    if (n6 != 0 || n5 != 0) {
                        int n9 = n4;
                        do {
                            n9 = (short)(n9 + 1);
                        } while (this.aQ[n7 += n6][n8 += n5] == 3);
                        this.aQ[n7][n8] = n9;
                        bl = true;
                    }
                }
                this.aQ[n][n2] = n4;
            }
        } else {
            bl = true;
        }
        return this.aQ[this.demolishX][this.demolishY] >= 20 ? 2 : (bl ? 0 : 1);
    }

    short h(int n, int n2, int n3) {
        short s = (short)n3;
        if (n >= 0 && n < this.mapWidth && n2 >= 0 && n2 < this.mapHeight && this.aQ[n][n2] >= 20 && this.aQ[n][n2] < s) {
            s = this.aQ[n][n2];
        }
        return s;
    }

    void normalizeTrackBits() {
        for (int i = 0; i < this.mapWidth; ++i) {
            for (int j = 0; j < this.mapHeight; ++j) {
                if (this.trackMap[i][j] == 64) {
                    this.tileMap[i][j] = this.tileMap[i][j] == 78 ? 22 : (this.tileMap[i][j] == 80 ? 24 : (this.tileMap[i][j] == 85 ? 9 : (this.tileMap[i][j] == 84 ? 8 : (this.tileMap[i][j] == 77 ? 21 : (this.tileMap[i][j] == 79 ? 23 : (this.tileMap[i][j] == 83 ? 7 : (this.tileMap[i][j] == 86 ? 10 : 0)))))));
                    this.trackMap[i][j] = 0;
                }
                if (this.tileMap[i][j] != 76) continue;
                int n = this.trackMap[i][j] & 7;
                int n2 = (this.trackMap[i][j] & 0x3F) >> 3;
                if (n == 0 && n2 == 0) {
                    this.tileMap[i][j] = 0;
                    this.trackMap[i][j] = 0;
                    continue;
                }
                if (n == 0) {
                    this.tileMap[i][j] = 71;
                    this.trackMap[i][j] = (byte)(0x40 | n2);
                    continue;
                }
                if (n2 != 0) continue;
                this.tileMap[i][j] = 70;
                this.trackMap[i][j] = (byte)(0x40 | n);
            }
        }
    }

    void N() {
        this.et = 0;
        for (int i = 0; i < this.mapWidth; ++i) {
            for (int j = 0; j < this.mapHeight; ++j) {
                if (this.trackMap[i][j] == 64) {
                    ++this.et;
                }
                if (this.tileMap[i][j] != 76) continue;
                int n = this.trackMap[i][j] & 7;
                int n2 = (this.trackMap[i][j] & 0x3F) >> 3;
                if (n == 0) {
                    ++this.et;
                }
                if (n2 != 0) continue;
                ++this.et;
            }
        }
    }

    void n(int n) {
        for (int i = 0; i < this.mapWidth; ++i) {
            for (int j = 0; j < this.mapHeight; ++j) {
                if (this.trackMap[i][j] == 64) {
                    if (n == 1) {
                        this.d(i, j, -4);
                    }
                    this.trackMap[i][j] = (byte)(0x40 | n);
                }
                if (this.tileMap[i][j] != 76) continue;
                int n2 = this.trackMap[i][j] & 7;
                int n3 = (this.trackMap[i][j] & 0x3F) >> 3;
                if (n2 == 0) {
                    n2 = n;
                }
                if (n3 == 0) {
                    n3 = n;
                }
                this.trackMap[i][j] = (byte)(0x40 | n2 | n3 << 3);
            }
        }
    }

    void b() {
        int n;
        int n2;
        for (n2 = 0; n2 < this.mapWidth; ++n2) {
            for (n = 0; n < this.mapHeight; ++n) {
                this.aQ[n2][n] = 0;
            }
        }
        for (n2 = 0; n2 < this.mapWidth; ++n2) {
            for (n = 0; n < this.mapHeight; ++n) {
                int n3;
                boolean bl;
                boolean bl2;
                int n4;
                if (this.aQ[n2][n] != 0) continue;
                short s = 0;
                if (this.trackMap[n2][n] == 0) {
                    s = 1;
                }
                byte by = this.tileMap[n2][n];
                if (s == 1) {
                    if (this.tileElevation[n2][n] == -1) {
                        s = 0;
                    }
                    if (by != 0 && by != 21 && by != 22 && by != 23 && by != 24 && by != 29 && by != 30 && by != 31 && by != 32 && by != 33) {
                        s = 0;
                    }
                }
                if (by == 71 && n2 > 0 && n2 < this.mapWidth - 1 && this.trackMap[n2][n] == this.trackMap[n2 - 1][n] && this.trackMap[n2][n] == this.trackMap[n2 + 1][n]) {
                    s = 1;
                }
                if (by == 70 && n > 0 && n < this.mapHeight - 1 && this.trackMap[n2][n] == this.trackMap[n2][n - 1] && this.trackMap[n2][n] == this.trackMap[n2][n + 1]) {
                    s = 1;
                }
                this.aQ[n2][n] = s;
                if (this.tileMap[n2][n] == 7) {
                    n4 = n;
                    bl2 = false;
                    do {
                        bl = false;
                        if (this.tileElevation[n2][--n4] != -1 || this.tileMap[n2][n4] != 0 || this.trackMap[n2][n4] != 0) {
                            bl = true;
                        }
                        if (n4 == 0) {
                            bl = true;
                        }
                        if (this.tileMap[n2][n4] != 10) continue;
                        bl = true;
                        bl2 = true;
                    } while (!bl);
                    if (bl2) {
                        for (n3 = n4; n3 <= n; ++n3) {
                            this.aQ[n2][n3] = 3;
                        }
                        this.aQ[n2][n4] = 2;
                        this.aQ[n2][n] = 2;
                    }
                }
                if (this.tileMap[n2][n] != 8) continue;
                n4 = n2;
                bl2 = false;
                do {
                    bl = false;
                    if (this.tileElevation[--n4][n] != -1 || this.tileMap[n4][n] != 0 || this.trackMap[n4][n] != 0) {
                        bl = true;
                    }
                    if (n4 == 0) {
                        bl = true;
                    }
                    if (this.tileMap[n4][n] != 9) continue;
                    bl = true;
                    bl2 = true;
                } while (!bl);
                if (!bl2) continue;
                for (n3 = n4; n3 <= n2; ++n3) {
                    this.aQ[n3][n] = 3;
                }
                this.aQ[n4][n] = 2;
                this.aQ[n2][n] = 2;
            }
        }
    }

    boolean a(int n, int n2, int n3, int n4, int n5) {
        boolean bl = false;
        while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < this.mapWidth && n2 < this.mapHeight) {
            if (this.tileMap[n][n2] == n5) {
                bl = true;
            }
            if (this.tileMap[n][n2] == 0 && this.trackMap[n][n2] == 0) continue;
            break;
        }
        return bl;
    }

    boolean b(int n, int n2) {
        byte by = this.tileMap[n][n2];
        if (by < 7 || by > 10) {
            return false;
        }
        boolean bl = false;
        if (by == 7) {
            bl |= this.a(n, n2, 0, -1, 10);
        }
        if (by == 10) {
            bl |= this.a(n, n2, 0, 1, 7);
        }
        if (by == 8) {
            bl |= this.a(n, n2, -1, 0, 9);
        }
        if (by == 9) {
            bl |= this.a(n, n2, 1, 0, 8);
        }
        return bl;
    }

    boolean j(int n, int n2) {
        return this.tileElevation[n][n2] >= 0 && (this.tileMap[n][n2] == 0 || this.tileMap[n][n2] >= 29 && this.tileMap[n][n2] <= 33) && this.trackMap[n][n2] == 0;
    }

    boolean r(int n, int n2) {
        if (this.tileMap[n][n2] >= 29 && this.tileMap[n][n2] <= 33 || this.tileMap[n][n2] >= 70 && this.tileMap[n][n2] <= 80 && this.trackMap[n][n2] == 65 || this.tileMap[n][n2] == 76 && (this.trackMap[n][n2] & 0x47) == 65 || this.tileMap[n][n2] == 76 && (this.trackMap[n][n2] & 0x78) == 72 || (this.tileMap[n][n2] == 87 || this.tileMap[n][n2] == 88 || this.tileMap[n][n2] == 89 || this.tileMap[n][n2] == 90) && this.trackMap[n][n2] == 65) {
            int n3 = this.spawnOrFindAt(n, n2);
            for (int i = 0; i < 500; ++i) {
                if ((this.trainFlags[i] & 0x80) != 128) continue;
                int n4 = this.c(this.trainX[i]);
                int n5 = this.c(this.trainY[i]);
                if (n == n4 && n2 == n5) {
                    return false;
                }
                if (n3 == 5000 || n3 != this.spawnOrFindAt(n4, n5)) continue;
                return false;
            }
            return true;
        }
        if (this.tileMap[n][n2] >= 81 && this.tileMap[n][n2] <= 86 && this.trackMap[n][n2] == 65) {
            for (int i = 0; i < 500; ++i) {
                int n6;
                if ((this.trainFlags[i] & 0x80) != 128) continue;
                if (this.tileMap[n][n2] == 81 || this.tileMap[n][n2] == 84 || this.tileMap[n][n2] == 85) {
                    for (n6 = n; n6 < this.mapWidth && (this.tileMap[n6][n2] == 81 || this.tileMap[n6][n2] == 84 || this.tileMap[n6][n2] == 85); ++n6) {
                        if (this.c(this.trainX[i]) != n6 || this.c(this.trainY[i]) != n2) continue;
                        return false;
                    }
                    for (n6 = n - 1; n6 >= 0 && (this.tileMap[n6][n2] == 81 || this.tileMap[n6][n2] == 84 || this.tileMap[n6][n2] == 85); --n6) {
                        if (this.c(this.trainX[i]) != n6 || this.c(this.trainY[i]) != n2) continue;
                        return false;
                    }
                    continue;
                }
                for (n6 = n2; n6 < this.mapHeight && (this.tileMap[n][n6] == 82 || this.tileMap[n][n6] == 83 || this.tileMap[n][n6] == 86); ++n6) {
                    if (this.c(this.trainX[i]) != n || this.c(this.trainY[i]) != n6) continue;
                    return false;
                }
                for (n6 = n2 - 1; n6 >= 0 && (this.tileMap[n][n6] == 82 || this.tileMap[n][n6] == 83 || this.tileMap[n][n6] == 86); --n6) {
                    if (this.c(this.trainX[i]) != n || this.c(this.trainY[i]) != n6) continue;
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    boolean e(int n, int n2, int n3) {
        if (n3 == 5) {
            return this.e(n, n2, 0) && this.e(n, n2 - 1, 0) && this.e(n, n2 + 1, 0);
        }
        if (n3 == 6) {
            return this.e(n, n2, 0) && this.e(n - 1, n2, 0) && this.e(n + 1, n2, 0);
        }
        if (n < 0 || n2 < 0 || n >= this.mapWidth || n2 >= this.mapHeight) {
            return false;
        }
        byte by = this.tileMap[n][n2];
        boolean bl = false;
        if (this.tileElevation[n][n2] >= 0 && (by == 0 || by >= 29 && by <= 33) && this.trackMap[n][n2] == 0) {
            bl = true;
        }
        if (n3 == 3 && (by == 71 || by == 21 || by == 23)) {
            bl = true;
        }
        if (n3 == 4 && (by == 70 || by == 22 || by == 24)) {
            bl = true;
        }
        return bl;
    }

    int spawnOrFindAt(int n, int n2) {
        for (int i = 0; i < 250; ++i) {
            if ((this.vehicleFlags[i] & 0x80) != 128) continue;
            if (this.c(this.wagonAJ[i]) == n && this.c(this.wagonEQ[i]) == n2) {
                return i;
            }
            if ((this.c(this.wagonAJ[i]) == n - 1 || this.c(this.wagonAJ[i]) == n + 1) && this.c(this.wagonEQ[i]) == n2 && (this.vehicleFlags[i] & 0xE0) == 224) {
                return i;
            }
            if (this.c(this.wagonEQ[i]) != n2 - 1 && this.c(this.wagonEQ[i]) != n2 + 1 || this.c(this.wagonAJ[i]) != n || (this.vehicleFlags[i] & 0xE0) != 160) continue;
            return i;
        }
        return 5000;
    }

    int a(int n, int n2, int n3, boolean bl, boolean bl2) {
        int n4;
        int n5 = 5000;
        for (n4 = 0; n4 < 250; ++n4) {
            if ((this.vehicleFlags[n4] & 0x80) != 0) continue;
            n5 = n4;
            break;
        }
        if (n5 != 5000) {
            this.vehicleFlags[n5] = 0;
            int n6 = n5;
            this.vehicleFlags[n6] = (byte)(this.vehicleFlags[n6] | 0x80);
            int n7 = n5;
            this.vehicleFlags[n7] = (byte)(this.vehicleFlags[n7] | (bl ? 64 : 0));
            int n8 = n5;
            this.vehicleFlags[n8] = (byte)(this.vehicleFlags[n8] | (bl2 ? 32 : 0));
            int n9 = n5;
            this.vehicleFlags[n9] = (byte)(this.vehicleFlags[n9] | n);
            this.wagonAJ[n5] = (byte)n2;
            this.wagonEQ[n5] = (byte)n3;
            this.trainFlagBits[n5] = 0;
            for (n4 = 0; n4 < 5; ++n4) {
                this.trainOptionSlots[n5][n4] = 0;
            }
        }
        return n5;
    }

    void computeTrainCost() {
        switch (this.wagonType) {
            case 0: {
                this.unitCost = this.gWagonCost;
                break;
            }
            case 1: {
                this.unitCost = this.gWagonCost2;
                break;
            }
            case 2: {
                this.unitCost = this.gWagonCost3;
                break;
            }
            case 3: {
                this.unitCost = this.gWagonCost4;
                break;
            }
            case 4: {
                this.unitCost = this.gWagonCost5;
                break;
            }
            case 5: {
                this.unitCost = this.gWagonCost6;
            }
        }
        if (this.wagonType == this.wagonDiscountType) {
            this.unitCost = this.unitCost * this.difficultyPct / 100;
        }
        block14: for (int i = 0; i < 5; ++i) {
            switch (this.wagonOptionSlots[i]) {
                case 0: {
                    this.unitCost += this.costGroupC1 * (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? this.difficultyPct : 1) / (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? 100 : 1);
                    continue block14;
                }
                case 1: {
                    this.unitCost += this.costGroupC2 * (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? this.difficultyPct : 1) / (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? 100 : 1);
                    continue block14;
                }
                case 2: {
                    this.unitCost += this.costGroupC3 * (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? this.difficultyPct : 1) / (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? 100 : 1);
                    continue block14;
                }
                case 3: {
                    this.unitCost += this.costGroupC4 * (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? this.difficultyPct : 1) / (this.wagonOptionSlots[i] == this.wagonCountMatch[i] ? 100 : 1);
                }
            }
        }
    }

    void addSelectedOption(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.wagonOptionSlots[i] != -1) continue;
            this.wagonOptionSlots[i] = n;
            break;
        }
    }

    void popSelectedOption() {
        for (int i = 4; i >= 0; --i) {
            if (this.wagonOptionSlots[i] == -1) continue;
            this.wagonOptionSlots[i] = -1;
            break;
        }
    }

    boolean y() {
        if (this.money >= this.unitCost) {
            int n = this.b(this.tileCursorX, this.tileCursorY, this.wagonType < 3 ? 0 : 1, this.wagonType, (this.vehicleFlags[this.aiWagonsLastSeen] & 0x40) == 64 ? 2 : 0, 8 - this.wagonType);
            for (int i = 0; i < 5; ++i) {
                if (this.wagonOptionSlots[i] < 0) continue;
                int n2 = 2 + this.wagonOptionSlots[i];
                if (n2 == 5) {
                    ++n2;
                }
                n = this.spawnTrainCar(n, n2);
                this.w(n2);
            }
            this.money -= this.unitCost;
            this.aiMoneySpent += this.unitCost;
            this.p(-this.unitCost);
            ++this.aiWagonsBought;
            return true;
        }
        this.enterOutOfMoneyState();
        return false;
    }

    void tickInputOrAi(boolean bl) {
        for (int i = 0; i < 250; ++i) {
            int n;
            int n2;
            if ((this.vehicleFlags[i] & 0x80) != 128) continue;
            int n3 = this.c(this.wagonAJ[i]);
            int n4 = this.c(this.wagonEQ[i]);
            int n5 = n3 - 3;
            int n6 = n4 - 3;
            int n7 = n3 + 3;
            int n8 = n4 + 3;
            if (n5 < 0) {
                n5 = 0;
            }
            if (n6 < 0) {
                n6 = 0;
            }
            if (n7 >= this.mapWidth) {
                n7 = this.mapWidth - 1;
            }
            if (n8 >= this.mapHeight) {
                n8 = this.mapHeight - 1;
            }
            int n9 = 0;
            boolean bl2 = false;
            boolean bl3 = false;
            boolean bl4 = false;
            Vector<Integer> vector = new Vector<Integer>();
            Vector<Integer> vector2 = new Vector<Integer>();
            Vector<Integer> vector3 = new Vector<Integer>();
            Vector<Integer> vector4 = new Vector<Integer>();
            for (n2 = n5; n2 <= n7; ++n2) {
                for (n = n6; n <= n8; ++n) {
                    int n10;
                    int n11;
                    if (this.tileMap[n2][n] >= 34 && this.tileMap[n2][n] <= 40) {
                        if ((this.trackMap[n2][n] & 0xC0) == 128 && !bl && vector.indexOf(new Integer(n11 = this.trackMap[n2][n] & 0x3F)) == -1) {
                            vector.addElement(new Integer(n11));
                            if ((this.trainFlagBits[i] & 1) == 1) {
                                short[] sArray = this.trainOptionSlots[i];
                                sArray[0] = (short)(sArray[0] + this.zonePopulation[n11] * (this.constructionTimers[n11] + 10 + this.zonePopulation[n11] / 10 + this.rng.nextInt() % 40) / 170);
                            }
                            if ((this.trainFlagBits[i] & 0x10) == 16) {
                                short[] sArray = this.trainOptionSlots[i];
                                sArray[4] = (short)(sArray[4] + this.zonePopulation[n11] / 2 * (this.constructionTimers[n11] + 10 + this.zonePopulation[n11] / 10 + Math.abs(this.rng.nextInt() % 40)) / 170);
                            }
                            int n12 = n11;
                            this.zonePopulation[n12] = (short)(this.zonePopulation[n12] + this.rng.nextInt() % 2);
                        }
                        ++n9;
                    }
                    if (this.tileMap[n2][n] >= 65 && this.tileMap[n2][n] <= 68 && !bl && (this.trackMap[n2][n] & 0xC0) == 192 && vector2.indexOf(new Integer(n11 = this.trackMap[n2][n] & 0x3F)) == -1) {
                        vector2.addElement(new Integer(n11));
                        n10 = this.cityPopulation[n11] * 4 / 30;
                        n10 = n10 / this.be[n11] == 0 ? 1 : n10 / this.be[n11];
                        short[] sArray = this.trainOptionSlots[i];
                        sArray[1] = (short)(sArray[1] + n10);
                        int n13 = n11;
                        this.bb[n13] = (short)(this.bb[n13] + n10);
                        int n14 = i;
                        this.wagonServiceCounter[n14] = (byte)(this.wagonServiceCounter[n14] | 4);
                    }
                    if (this.tileMap[n2][n] >= 53 && this.tileMap[n2][n] <= 55 && !bl && (this.trackMap[n2][n] & 0xC0) == 192 && vector3.indexOf(new Integer(n11 = this.trackMap[n2][n] & 0x3F)) == -1) {
                        vector3.addElement(new Integer(n11));
                        n10 = this.cityPopulation[n11] * 4 / 30;
                        n10 = n10 / this.be[n11] == 0 ? 1 : n10 / this.be[n11];
                        short[] sArray = this.trainOptionSlots[i];
                        sArray[2] = (short)(sArray[2] + n10);
                        int n15 = n11;
                        this.bb[n15] = (short)(this.bb[n15] + n10);
                        int n16 = i;
                        this.wagonServiceCounter[n16] = (byte)(this.wagonServiceCounter[n16] | 8);
                    }
                    if (this.tileMap[n2][n] >= 59 && this.tileMap[n2][n] <= 61 && !bl && (this.trackMap[n2][n] & 0xC0) == 192 && vector4.indexOf(new Integer(n11 = this.trackMap[n2][n] & 0x3F)) == -1) {
                        vector4.addElement(new Integer(n11));
                        n10 = this.cityPopulation[n11] * 4 / 30;
                        n10 = n10 / this.be[n11] == 0 ? 1 : n10 / this.be[n11];
                        short[] sArray = this.trainOptionSlots[i];
                        sArray[3] = (short)(sArray[3] + n10);
                        int n17 = n11;
                        this.bb[n17] = (short)(this.bb[n17] + n10);
                        int n18 = i;
                        this.wagonServiceCounter[n18] = (byte)(this.wagonServiceCounter[n18] | 2);
                    }
                    if (this.tileMap[n2][n] >= 50 && this.tileMap[n2][n] <= 52) {
                        int n19 = i;
                        this.trainFlagBits[n19] = (byte)(this.trainFlagBits[n19] | 4);
                    }
                    if (this.tileMap[n2][n] >= 56 && this.tileMap[n2][n] <= 58) {
                        int n20 = i;
                        this.trainFlagBits[n20] = (byte)(this.trainFlagBits[n20] | 8);
                    }
                    if (this.tileMap[n2][n] < 62 || this.tileMap[n2][n] > 64) continue;
                    int n21 = i;
                    this.trainFlagBits[n21] = (byte)(this.trainFlagBits[n21] | 2);
                }
            }
            if (n9 / 3 > 0) {
                int n22 = i;
                this.trainFlagBits[n22] = (byte)(this.trainFlagBits[n22] | 1);
                int n23 = i;
                this.wagonServiceCounter[n23] = (byte)(this.wagonServiceCounter[n23] | 1);
            }
            if (n9 / 7 > 0) {
                int n24 = i;
                this.trainFlagBits[n24] = (byte)(this.trainFlagBits[n24] | 0x10);
                int n25 = i;
                this.wagonServiceCounter[n25] = (byte)(this.wagonServiceCounter[n25] | 0x10);
            }
            n2 = (this.vehicleFlags[i] & 0x20) == 32 ? 1 : 0;
            for (n = 0; n < 5; ++n) {
                if (n2 != 0) {
                    if (n == 0) {
                        if (this.trainOptionSlots[i][n] <= 250) continue;
                        this.trainOptionSlots[i][n] = 250;
                        continue;
                    }
                    if (n == 4) {
                        if (this.trainOptionSlots[i][n] <= 325) continue;
                        this.trainOptionSlots[i][n] = 325;
                        continue;
                    }
                    if (this.trainOptionSlots[i][n] <= 450) continue;
                    this.trainOptionSlots[i][n] = 450;
                    continue;
                }
                if (n == 0) {
                    if (this.trainOptionSlots[i][n] <= 100) continue;
                    this.trainOptionSlots[i][n] = 100;
                    continue;
                }
                if (n == 4) {
                    if (this.trainOptionSlots[i][n] <= 150) continue;
                    this.trainOptionSlots[i][n] = 150;
                    continue;
                }
                if (this.trainOptionSlots[i][n] <= 200) continue;
                this.trainOptionSlots[i][n] = 200;
            }
        }
    }

    int spawnTrainCar(int n, int n2) {
        int n3 = 500;
        for (int i = 0; i < 500; ++i) {
            if ((this.trainFlags[i] & 0x80) != 0) continue;
            n3 = i;
            break;
        }
        if (n3 != 500) {
            this.nextCar[n] = (short)n3;
            this.trainFlags[n3] = -128;
            int n4 = n3;
            this.trainFlags[n4] = (byte)(this.trainFlags[n4] | n2);
            this.nextCar[n3] = -1;
            this.trainX[n3] = this.trainX[n];
            this.trainY[n3] = this.trainY[n];
            this.trainDir[n3] = this.trainDir[n];
            this.trainPhase[n3] = (byte)(this.trainPhase[n] - 8);
            this.trainEnergyOrCount[n3] = 0;
            this.wagonEnergyRemaining[n3] = 0;
            this.wagonIorHost[n3] = -1;
        }
        this.bH = -1;
        return n3;
    }

    int b(int n, int n2, int n3, int n4, int n5, int n6) {
        int n7 = 500;
        for (int i = 0; i < 500; ++i) {
            if ((this.trainFlags[i] & 0x80) != 0) continue;
            n7 = i;
            break;
        }
        if (n7 != 500) {
            this.trainFlags[n7] = -64;
            int n8 = n7;
            this.trainFlags[n8] = (byte)(this.trainFlags[n8] | n3);
            this.nextCar[n7] = -1;
            this.trainX[n7] = (byte)n;
            this.trainY[n7] = (byte)n2;
            this.trainDir[n7] = (byte)n5;
            this.wagonEnergyRemaining[n7] = (byte)n6;
            this.trainPhase[n7] = 0;
            this.trainEnergyOrCount[n7] = 1;
            this.wagonIorHost[n7] = -1;
            this.trainEraOrStage[n7] = (byte)n4;
            this.trainAgeOrTimer[n7] = 0;
            this.eL[n7] = 7;
            this.bz[n7] = 0;
        }
        this.bH = -1;
        return n7;
    }

    void revenueOrYear() {
        int n = 0;
        while (n < 500) {
            int n2 = n++;
            this.trainAgeOrTimer[n2] = (byte)(this.trainAgeOrTimer[n2] + 1);
        }
    }

    void everyTwoTick() {
        if (this.eA[0] == 0) {
            for (int i = 0; i < 500; ++i) {
                int n;
                int n2;
                if ((this.trainFlags[i] & 0x80) != 128 || (this.trainFlags[i] & 0x40) != 64 || this.trackMap[n2 = this.c(this.trainX[i])][n = this.c(this.trainY[i])] != 65) continue;
                int n3 = this.A(i) * 2;
                if ((this.trainFlags[i] & 0xF) == 0) {
                    n3 += 3;
                } else if ((this.trainFlags[i] & 0xF) == 1) {
                    n3 += 7;
                }
                int n4 = i;
                this.trainRevenue[n4] = this.trainRevenue[n4] - n3;
                this.money -= n3;
                this.fuelCostAccrued += n3;
            }
        }
    }

    boolean i(int n, int n2) {
        short s = this.nextCar[n];
        if (s != -1) {
            if ((this.trainFlags[s] & 0x40) == 0) {
                int n3 = this.trainFlags[s] & 0xF;
                switch (n3) {
                    case 2: {
                        if ((this.wagonServiceCounter[n2] & 1) == 1 && this.trainEnergyOrCount[s] < 25) {
                            return true;
                        }
                        if ((this.wagonServiceCounter[n2] & 0x10) != 16 || this.wagonEnergyRemaining[s] >= 15) break;
                        return true;
                    }
                    case 3: {
                        if ((this.wagonServiceCounter[n2] & 4) != 4 || this.trainEnergyOrCount[s] >= 25) break;
                        return true;
                    }
                    case 4: 
                    case 5: {
                        if ((this.wagonServiceCounter[n2] & 8) != 8 || this.trainEnergyOrCount[s] >= 30) break;
                        return true;
                    }
                    case 6: 
                    case 7: {
                        if ((this.wagonServiceCounter[n2] & 2) != 2 || this.trainEnergyOrCount[s] >= 30) break;
                        return true;
                    }
                }
            }
            if (this.i(s, n2)) {
                return true;
            }
        }
        return false;
    }

    void updateTrain(int n) {
        int n2;
        if ((this.trainFlags[n] & 0x40) == 0 || this.c(this.trainEnergyOrCount[n]) <= 0) {
            int n3;
            int n4;
            int n5;
            int n6;
            int n7;
            int n8;
            n2 = 0;
            int n9 = n;
            this.trainFlags[n9] = (byte)(this.trainFlags[n9] & 0xEF);
            if ((this.trainFlags[n] & 0x40) == 64) {
                this.trainEnergyOrCount[n] = (byte)(this.wagonEnergyRemaining[n] + this.B(n) / 2 + this.A(n) / 2 + this.trainAgeOrTimer[n] / 3);
            }
            int n10 = this.c(this.trainX[n]);
            int n11 = this.c(this.trainY[n]);
            if ((this.trainFlags[n] & 0x40) == 64 && this.trainPhase[n] == 4 && (this.tileMap[n10][n11] == 87 || this.tileMap[n10][n11] == 88) && this.trackMap[n10][n11] == 65) {
                n8 = this.spawnOrFindAt(n10, n11);
                n7 = 0;
                if (n8 != this.wagonIorHost[n]) {
                    n7 = this.xPlaceSpawn(n, n8);
                }
                if (this.eL[n] != 7) {
                    this.trainFlags[n] = -64;
                    int n12 = n;
                    this.trainFlags[n12] = (byte)(this.trainFlags[n12] | (byte)((this.eL[n] & 7) >= 3 ? 1 : 0));
                    this.wagonEnergyRemaining[n] = (byte)(8 - (this.eL[n] & 7));
                    this.trainEraOrStage[n] = (byte)(this.eL[n] & 7);
                    this.trainAgeOrTimer[n] = 0;
                    n6 = this.nextCar[n];
                    while (n6 != -1) {
                        this.trainFlags[n6] = 0;
                        n6 = this.nextCar[n6];
                    }
                    n5 = n;
                    for (n4 = 0; n4 < 5; ++n4) {
                        n3 = this.eL[n] >> 3 + 3 * n4 & 7;
                        if (n3 >= 7) continue;
                        int n13 = 2 + n3;
                        if (n13 == 5) {
                            ++n13;
                        }
                        n5 = this.spawnTrainCar(n5, n13);
                    }
                    this.eL[n] = 7;
                    this.bH = -1;
                }
                this.b(n, n8, false);
                if (this.bz[n] == 1 && this.i(n, n8)) {
                    this.trainPhase[n] = 3;
                    n2 = 1;
                }
                if (n8 != this.wagonIorHost[n]) {
                    if (n7 > 0) {
                        this.money += n7;
                        this.p(n7);
                        this.totalRevenue += n7;
                        int n14 = n;
                        this.trainRevenue[n14] = this.trainRevenue[n14] + n7;
                        this.h(n7);
                    }
                    this.trainEnergyOrCount[n] = (byte)(this.A(n) * 40);
                }
                this.wagonIorHost[n] = (byte)n8;
                int n15 = n;
                this.trainFlags[n15] = (byte)(this.trainFlags[n15] | 0x10);
            }
            if ((this.trainFlags[n] & 0x40) == 64 && this.trainPhase[n] == 4 && (this.tileMap[n10][n11] == 87 || this.tileMap[n10][n11] == 88) && this.trackMap[n10][n11] != 65) {
                this.trainEnergyOrCount[n] = (byte)(this.A(n) * 40);
            }
            n8 = 0;
            if (this.trainPhase[n] < 8) {
                int n16 = n;
                this.trainPhase[n16] = (byte)(this.trainPhase[n16] + 1);
            } else {
                byte[] byArray;
                if (this.c(this.trainServiceCounter[n]) < 200) {
                    int n17 = n;
                    this.trainServiceCounter[n17] = (byte)(this.trainServiceCounter[n17] + 1);
                }
                n10 = this.trainX[n];
                n11 = this.trainY[n];
                n7 = this.trainDir[n];
                this.trainPhase[n] = 0;
                n6 = this.c(this.trainX[n]);
                n5 = this.c(this.trainY[n]);
                n4 = this.trackMap[n6][n5];
                n3 = this.tileMap[n6][n5] - 70;
                if (n3 == 6 && (this.trainDir[n] == 0 || this.trainDir[n] == 1)) {
                    n4 = 0x40 | n4 & 7;
                    n3 = 0;
                }
                if (n3 == 6 && (this.trainDir[n] == 2 || this.trainDir[n] == 3)) {
                    n4 = 0x40 | (n4 & 0x38) >> 3;
                    n3 = 1;
                }
                if (n3 == 7) {
                    n3 = 0;
                }
                if (n3 == 8) {
                    n3 = 1;
                }
                if (n3 == 9) {
                    n3 = 0;
                }
                if (n3 == 10) {
                    n3 = 1;
                }
                if (n3 == 11) {
                    n3 = 1;
                }
                if (n3 == 12) {
                    n3 = 0;
                }
                if (n3 == 13) {
                    n3 = 0;
                }
                if (n3 == 14) {
                    n3 = 1;
                }
                if (n3 == 15) {
                    n3 = 1;
                }
                if (n3 == 16) {
                    n3 = 0;
                }
                if (n3 == 17) {
                    n3 = 0;
                }
                if (n3 == 18) {
                    n3 = 1;
                }
                if (n3 == 19) {
                    n3 = 0;
                }
                if (n3 == 20) {
                    n3 = 1;
                }
                if ((n6 += (byArray = this.eR[n3][0] == this.trainDir[n] ? this.eR[n3] : this.aH[n3])[2]) >= 0 && (n5 += byArray[3]) >= 0 && n6 < this.mapWidth && n5 < this.mapHeight) {
                    int n18;
                    boolean bl = false;
                    int n19 = this.trackMap[n6][n5];
                    int n20 = this.tileMap[n6][n5] - 70;
                    if (n20 == 7) {
                        n20 = 0;
                    }
                    if (n20 == 8) {
                        n20 = 1;
                    }
                    if (n20 == 9) {
                        n20 = 0;
                    }
                    if (n20 == 10) {
                        n20 = 1;
                    }
                    if (n20 == 11) {
                        n20 = 1;
                    }
                    if (n20 == 12) {
                        n20 = 0;
                    }
                    if (n20 == 13) {
                        n20 = 0;
                    }
                    if (n20 == 14) {
                        n20 = 1;
                    }
                    if (n20 == 15) {
                        n20 = 1;
                    }
                    if (n20 == 16) {
                        n20 = 0;
                    }
                    if (n20 == 17) {
                        n20 = 0;
                    }
                    if (n20 == 18) {
                        n20 = 1;
                    }
                    if (n20 == 19) {
                        n20 = 0;
                    }
                    if (n20 == 20) {
                        n20 = 1;
                    }
                    for (n18 = 2; n18 < byArray.length / 2; ++n18) {
                        if (byArray[2 * n18] != n20) continue;
                        if (n20 == 6) {
                            if (byArray[2 * n18 + 1] == 0 || byArray[2 * n18 + 1] == 1) {
                                n19 = 0x40 | n19 & 7;
                            }
                            if (byArray[2 * n18 + 1] == 2 || byArray[2 * n18 + 1] == 3) {
                                n19 = 0x40 | (n19 & 0x38) >> 3;
                            }
                        }
                        if (n4 != n19) break;
                        this.trainX[n] = (byte)n6;
                        this.trainY[n] = (byte)n5;
                        this.trainDir[n] = byArray[2 * n18 + 1];
                        bl = true;
                    }
                    if (!bl) {
                        this.trainDir[n] = byArray[1];
                        this.f(n, this.nextCar[n], 0);
                    }
                    if (bl && (this.trainFlags[n] & 0x40) == 64 && n20 == 6) {
                        n18 = 0;
                        for (int i = 0; i < 500; ++i) {
                            if ((this.trainFlags[i] & 0x80) != 128 || i == n || this.trainX[i] != this.trainX[n] || this.trainY[i] != this.trainY[n]) continue;
                            n18 = 1;
                            break;
                        }
                        if (n18 != 0) {
                            this.trainX[n] = (byte)n10;
                            this.trainY[n] = (byte)n11;
                            this.trainDir[n] = (byte)n7;
                            this.trainPhase[n] = 8;
                            n8 = 1;
                        }
                    }
                } else {
                    if (this.eR[n3][0] == this.trainDir[n]) {
                        this.trainDir[n] = this.eR[n3][1];
                    } else if (this.aH[n3][0] == this.trainDir[n]) {
                        this.trainDir[n] = this.aH[n3][1];
                    }
                    this.f(n, this.nextCar[n], 0);
                }
            }
            if (n2 == 0 && n8 == 0 && this.nextCar[n] != -1) {
                this.updateTrain(this.nextCar[n]);
            }
        }
        if ((this.trainFlags[n] & 0x40) == 64) {
            n2 = this.c(this.trainEnergyOrCount[n]);
            if ((n2 -= 3) < 0) {
                n2 = 0;
            }
            this.trainEnergyOrCount[n] = (byte)n2;
        }
    }

    void f(int n, int n2, int n3) {
        if (n2 != -1) {
            this.f(n, this.nextCar[n2], n3 - 8);
            this.trainX[n2] = this.trainX[n];
            this.trainY[n2] = this.trainY[n];
            this.trainDir[n2] = this.trainDir[n];
            this.trainPhase[n2] = (byte)(n3 - 8);
        }
    }

    int A(int n) {
        if (n != -1) {
            return this.A(this.nextCar[n]) + 1;
        }
        return 0;
    }

    int B(int n) {
        if (n != -1) {
            byte by = this.tileMap[this.c(this.trainX[n])][this.c(this.trainY[n])];
            int n2 = 0;
            if (by == 77) {
                int n3 = n2 = this.trainDir[n] == 0 ? -1 : 1;
            }
            if (by == 79) {
                int n4 = n2 = this.trainDir[n] == 0 ? 1 : -1;
            }
            if (by == 78) {
                int n5 = n2 = this.trainDir[n] == 2 ? 1 : -1;
            }
            if (by == 80) {
                n2 = this.trainDir[n] == 2 ? -1 : 1;
            }
            return this.B(this.nextCar[n]) + n2;
        }
        return 0;
    }

    void a(int n, boolean bl) {
        if (n != -1) {
            this.a((int)this.nextCar[n], bl);
            int n2 = n;
            this.trainFlags[n2] = (byte)(this.trainFlags[n2] | 0xE0);
            this.nextCar[n] = -1;
            this.trainEnergyOrCount[n] = 7;
            if (bl) {
                this.a(this.c(this.trainX[n]), this.c(this.trainY[n]), false, true);
            }
            this.bH = -1;
            this.ready = false;
            this.transitionCode = 16;
            this.setState(20);
        }
    }

    void b(Graphics graphics, int n) {
        int n2;
        int n3;
        int n4;
        short[] sArray = new short[8];
        System.arraycopy(this.cs[this.trainFlags[n] & 0xF], 0, sArray, 0, 8);
        int n5 = this.c(this.trainX[n]);
        int n6 = this.c(this.trainY[n]);
        if ((this.trainFlags[n] & 0x40) == 64) {
            int n7;
            n4 = this.tileMap[n5][n6];
            n3 = this.c(this.trackMap[n5][n6]);
            n2 = 0;
            if (n4 != 76 && n3 > 65) {
                n2 = 1;
            }
            if (n4 == 76) {
                n7 = n3 & 7;
                int n8 = n3 >> 3 & 7;
                if ((this.trainDir[n] == 0 || this.trainDir[n] == 1) && n7 > 1) {
                    n2 = 1;
                }
                if ((this.trainDir[n] == 2 || this.trainDir[n] == 3) && n8 > 1) {
                    n2 = 1;
                }
            }
            if (n2 != 0) {
                n7 = 0;
                while (n7 < sArray.length) {
                    int n9 = n7++;
                    sArray[n9] = (short)(sArray[n9] - 30);
                }
            }
        }
        n4 = this.c(this.tileMap[n5][n6]);
        n3 = n6 * 16 + n5 * 16 + 16 - this.viewportCenterX;
        n2 = this.hj - n5 * 8 + n6 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n5][n6] + 15;
        if (this.trainPhase[n] >= 0) {
            switch (this.trainDir[n]) {
                case 0: {
                    if (n4 == 82) {
                        n2 -= 4;
                    }
                    if (n4 == 79) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[3]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 11 + 4 + this.trainPhase[n] - this.trainPhase[n] / 2, 33);
                        break;
                    }
                    if (n4 == 77) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[3]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 11 + this.trainPhase[n] + this.trainPhase[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[3]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 11 + this.trainPhase[n], 33);
                    break;
                }
                case 1: {
                    if (n4 == 82) {
                        n2 -= 4;
                    }
                    if (n4 == 79) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[7]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 3 - this.trainPhase[n] + this.trainPhase[n] / 2, 33);
                        break;
                    }
                    if (n4 == 77) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[7]])], n3 + 8 - this.trainPhase[n] * 2, n2 + 1 - this.trainPhase[n] - this.trainPhase[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[7]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 3 - this.trainPhase[n], 33);
                    break;
                }
                case 2: {
                    if (n4 == 81) {
                        n2 -= 4;
                    }
                    if (n4 == 78) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[1]])], n3 - 8 + this.trainPhase[n] * 2, n2 + 1 - this.trainPhase[n] - this.trainPhase[n] / 2, 33);
                        break;
                    }
                    if (n4 == 80) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[1]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 3 - this.trainPhase[n] + this.trainPhase[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[1]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 3 - this.trainPhase[n], 33);
                    break;
                }
                case 3: {
                    if (n4 == 81) {
                        n2 -= 4;
                    }
                    if (n4 == 78) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[5]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 11 + this.trainPhase[n] + this.trainPhase[n] / 2, 33);
                        break;
                    }
                    if (n4 == 80) {
                        graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[5]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 11 + 4 + this.trainPhase[n] - this.trainPhase[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[5]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 11 + this.trainPhase[n], 33);
                    break;
                }
                case 4: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[2]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 12, 33);
                    break;
                }
                case 5: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[6]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 12, 33);
                    break;
                }
                case 6: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[2]])], n3 - 8 + this.trainPhase[n] * 2, n2 - 4, 33);
                    break;
                }
                case 7: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[6]])], n3 + 8 - this.trainPhase[n] * 2, n2 - 4, 33);
                    break;
                }
                case 8: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[0]])], n3 + 8, n2 - 11 + this.trainPhase[n], 33);
                    break;
                }
                case 9: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[4]])], n3 + 8, n2 - 3 - this.trainPhase[n], 33);
                    break;
                }
                case 10: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[0]])], n3 - 8, n2 - 11 + this.trainPhase[n], 33);
                    break;
                }
                case 11: {
                    graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[sArray[4]])], n3 - 8, n2 - 3 - this.trainPhase[n], 33);
                }
            }
        }
    }

    boolean j(int n, int n2, int n3) {
        if (n != -1) {
            if (this.c(this.trainX[n]) == n2 && this.c(this.trainY[n]) == n3) {
                return true;
            }
            return this.j(this.nextCar[n], n2, n3);
        }
        return false;
    }

    void i(int n) {
        int n2 = -1;
        for (int i = 0; i < 500; ++i) {
            if (n == i || (this.trainFlags[i] & 0x80) != 128 || (this.trainFlags[i] & 0x40) != 64 || !this.j(i, this.c(this.trainX[n]), this.c(this.trainY[n]))) continue;
            this.a(i, true);
            this.a(n, true);
        }
    }

    boolean i(int n, int n2, int n3) {
        if (n != -1) {
            if (this.c(this.trainX[n]) == n2 && this.c(this.trainY[n]) == n3) {
                return true;
            }
            if (this.i(this.nextCar[n], n2, n3)) {
                return true;
            }
        }
        return false;
    }

    int cPosLookup(int n, int n2) {
        for (int i = 0; i < 500; ++i) {
            if ((this.trainFlags[i] & 0x80) != 128 || (this.trainFlags[i] & 0x40) != 64 || !this.i(i, n, n2)) continue;
            return i;
        }
        return 5000;
    }

    int xPlaceSpawn(int n, int n2) {
        if (n != -1) {
            int n3 = 0;
            if ((this.trainFlags[n] & 0x40) == 0) {
                int n4 = this.trainFlags[n] & 0xF;
                if (this.wagonIorHost[n] != n2 && (this.trainEnergyOrCount[n] > 0 || this.wagonEnergyRemaining[n] > 0)) {
                    switch (n4) {
                        case 2: {
                            int n5;
                            if ((this.trainFlagBits[n2] & 1) == 1) {
                                n5 = this.trainEnergyOrCount[n] * 3 * (this.c(this.trainServiceCounter[n]) / 15 + 1);
                                if (this.eA[4] > 0) {
                                    n5 *= 2;
                                }
                                n3 += n5;
                                this.t(4, this.trainEnergyOrCount[n]);
                                this.trainEnergyOrCount[n] = 0;
                            }
                            if ((this.trainFlagBits[n2] & 0x10) != 16) break;
                            n5 = this.wagonEnergyRemaining[n] * 8 * (this.c(this.trainServiceCounter[n]) / 15 + 1);
                            if (this.eA[5] > 0) {
                                n5 *= 2;
                            }
                            n3 += n5;
                            this.t(5, this.wagonEnergyRemaining[n]);
                            this.wagonEnergyRemaining[n] = 0;
                            break;
                        }
                        case 3: {
                            if ((this.trainFlagBits[n2] & 2) != 2) break;
                            int n6 = this.trainEnergyOrCount[n] * 10 * (this.c(this.trainServiceCounter[n]) / 15 + 1);
                            if (this.eA[3] > 0) {
                                n6 *= 2;
                            }
                            n3 += n6;
                            this.t(3, this.trainEnergyOrCount[n]);
                            this.trainEnergyOrCount[n] = 0;
                            break;
                        }
                        case 4: 
                        case 5: {
                            if ((this.trainFlagBits[n2] & 4) != 4) break;
                            int n7 = this.trainEnergyOrCount[n] * 12 * (this.c(this.trainServiceCounter[n]) / 15 + 1);
                            if (this.eA[2] > 0) {
                                n7 *= 2;
                            }
                            n3 += n7;
                            this.t(2, this.trainEnergyOrCount[n]);
                            this.trainEnergyOrCount[n] = 0;
                            break;
                        }
                        case 6: 
                        case 7: {
                            if ((this.trainFlagBits[n2] & 8) != 8) break;
                            int n8 = this.trainEnergyOrCount[n] * 13 * (this.c(this.trainServiceCounter[n]) / 15 + 1);
                            if (this.eA[1] > 0) {
                                n8 *= 2;
                            }
                            n3 += n8;
                            this.t(1, this.trainEnergyOrCount[n]);
                            this.trainEnergyOrCount[n] = 0;
                        }
                    }
                }
            }
            return this.xPlaceSpawn(this.nextCar[n], n2) + n3;
        }
        return 0;
    }

    int g(int n, int n2, int n3) {
        int n4 = this.trainOptionSlots[n][n2] > n3 ? n3 : this.trainOptionSlots[n][n2];
        short[] sArray = this.trainOptionSlots[n];
        int n5 = n2;
        sArray[n5] = (short)(sArray[n5] - n4);
        return n4;
    }

    int c(int n, int n2, int n3) {
        int n4 = this.g(n, n2, n3);
        for (int i = 0; i < 250; ++i) {
            if (n == i || (this.vehicleFlags[i] & 0x80) != 128 || (this.vehicleFlags[i] & 7) != (this.vehicleFlags[n] & 7) || Math.abs(this.wagonAJ[n] - this.wagonAJ[i]) >= 3 || Math.abs(this.wagonEQ[n] - this.wagonEQ[i]) >= 3) continue;
            n4 += this.g(i, n2, n3 - n4);
        }
        return n4;
    }

    void b(int n, int n2, boolean bl) {
        boolean bl2 = bl;
        if (n != -1) {
            boolean bl3 = false;
            if ((this.trainFlags[n] & 0x40) == 0) {
                int n3 = this.trainFlags[n] & 0xF;
                switch (n3) {
                    case 2: {
                        int n4 = n;
                        this.trainEnergyOrCount[n4] = (byte)(this.trainEnergyOrCount[n4] + this.c(n2, 0, 25 - this.trainEnergyOrCount[n]));
                        int n5 = n;
                        this.wagonEnergyRemaining[n5] = (byte)(this.wagonEnergyRemaining[n5] + this.c(n2, 4, 15 - this.wagonEnergyRemaining[n]));
                        this.trainServiceCounter[n] = 0;
                        if (n2 == this.wagonIorHost[n] || bl2) break;
                        this.d(this.c(this.trainX[n]), this.c(this.trainY[n]), 4);
                        bl2 = true;
                        break;
                    }
                    case 3: {
                        int n6 = n;
                        this.trainEnergyOrCount[n6] = (byte)(this.trainEnergyOrCount[n6] + this.c(n2, 1, 25 - this.trainEnergyOrCount[n]));
                        this.trainServiceCounter[n] = 0;
                        break;
                    }
                    case 4: 
                    case 5: {
                        int n7 = n;
                        this.trainEnergyOrCount[n7] = (byte)(this.trainEnergyOrCount[n7] + this.c(n2, 2, 30 - this.trainEnergyOrCount[n]));
                        this.trainServiceCounter[n] = 0;
                        break;
                    }
                    case 6: 
                    case 7: {
                        int n8 = n;
                        this.trainEnergyOrCount[n8] = (byte)(this.trainEnergyOrCount[n8] + this.c(n2, 3, 30 - this.trainEnergyOrCount[n]));
                        this.trainServiceCounter[n] = 0;
                    }
                }
                if (n3 == 4 || n3 == 5) {
                    this.trainFlags[n] = this.trainEnergyOrCount[n] >= 10 ? (byte)(this.trainFlags[n] & 0xF0 | 5) : (byte)(this.trainFlags[n] & 0xF0 | 4);
                }
                if (n3 == 6 || n3 == 7) {
                    this.trainFlags[n] = this.trainEnergyOrCount[n] >= 10 ? (byte)(this.trainFlags[n] & 0xF0 | 7) : (byte)(this.trainFlags[n] & 0xF0 | 6);
                }
                this.wagonIorHost[n] = (byte)n2;
            } else {
                this.trainServiceCounter[n] = 0;
            }
            this.b((int)this.nextCar[n], n2, bl2);
        }
    }

    void o(int n) {
        if (n != -1) {
            this.decCountsForOption(this.trainFlags[n] & 7);
            this.o(this.nextCar[n]);
        }
    }

    boolean a(int n, int n2, int[] nArray) {
        if (this.money >= this.unitCost - this.currentTrainCostSnapshot) {
            this.eL[n] = (short)n2;
            this.o(this.nextCar[n]);
            for (int i = 0; i < 5; ++i) {
                if (nArray[i] > -1) {
                    int n3 = nArray[i] + 2;
                    if (n3 == 5) {
                        ++n3;
                    }
                    this.w(n3);
                }
                int n4 = n;
                this.eL[n4] = this.eL[n4] | (nArray[i] & 7) << 3 + 3 * i;
            }
            this.money -= this.unitCost - this.currentTrainCostSnapshot;
            this.aiMoneySpent += this.unitCost - this.currentTrainCostSnapshot;
            if (this.unitCost - this.currentTrainCostSnapshot != 0) {
                this.p(-(this.unitCost - this.currentTrainCostSnapshot));
            }
            return true;
        }
        this.enterOutOfMoneyState();
        return false;
    }

    void loadGame(int n, int n2) {
        this.saveSlot = n;
        this.savePendingMode = n2;
        this.saveReadOffset = 0;
        this.snapshotSize = 0;
        this.snapshotLoaded = false;
        if (this.savePendingMode == 1) {
            try {
                RecordStore recordStore = RecordStore.openRecordStore((String)"TTStore", (boolean)true);
                RecordEnumeration recordEnumeration = recordStore.enumerateRecords(null, null, false);
                for (int i = 0; i < recordEnumeration.numRecords(); ++i) {
                    int n3 = recordEnumeration.nextRecordId();
                    byte by = recordStore.getRecord(n3)[0];
                    if (by != n) continue;
                    this.saveBuffer = new byte[recordStore.getRecordSize(n3) - 1];
                    System.arraycopy(recordStore.getRecord(n3), 1, this.saveBuffer, 0, recordStore.getRecordSize(n3) - 1);
                    this.snapshotSize = recordStore.getRecordSize(n3) - 1;
                    this.snapshotLoaded = true;
                }
                recordEnumeration.destroy();
                recordStore.closeRecordStore();
            }
            catch (Exception exception) {}
        } else {
            this.snapshotLoaded = true;
        }
    }

    void k(int n) {
        if (this.snapshotSize > 0) {
            byte[] byArray = new byte[this.snapshotSize + n];
            System.arraycopy(this.saveBuffer, 0, byArray, 0, this.snapshotSize);
            this.snapshotSize += n;
            this.saveBuffer = new byte[this.snapshotSize];
            System.arraycopy(byArray, 0, this.saveBuffer, 0, this.snapshotSize);
        } else {
            this.saveBuffer = new byte[n];
            this.snapshotSize += n;
        }
    }

    private int s() {
        return this.saveBuffer[this.saveReadOffset] >= 0 ? this.saveBuffer[this.saveReadOffset++] : 256 + this.saveBuffer[this.saveReadOffset++];
    }

    public void saveGame() {
        if (this.savePendingMode == 0) {
            try {
                RecordStore recordStore = RecordStore.openRecordStore((String)"TTStore", (boolean)true);
                RecordEnumeration recordEnumeration = recordStore.enumerateRecords(null, null, false);
                for (int i = 0; i < recordEnumeration.numRecords(); ++i) {
                    int n = recordEnumeration.nextRecordId();
                    byte by = recordStore.getRecord(n)[0];
                    if (by != this.saveSlot) continue;
                    recordStore.deleteRecord(n);
                }
                byte[] byArray = new byte[this.snapshotSize + 1];
                byArray[0] = (byte)this.saveSlot;
                System.arraycopy(this.saveBuffer, 0, byArray, 1, this.snapshotSize);
                recordStore.addRecord(byArray, 0, byArray.length);
                recordEnumeration.destroy();
                recordStore.closeRecordStore();
            }
            catch (Exception exception) {
                // empty catch block
            }
        }
    }

    void a(ByteArrayOutputStream byteArrayOutputStream, int n, int n2) {
        if (n2 == 1) {
            if (n == -1) {
                byteArrayOutputStream.write(255);
                byteArrayOutputStream.write(1);
            }
            byteArrayOutputStream.write(n);
        } else {
            byteArrayOutputStream.write(255);
            byteArrayOutputStream.write(n2);
            byteArrayOutputStream.write(n);
        }
    }

    void c(String string) {
        long l = 0L;
        for (int i = 0; i < this.snapshotSize; ++i) {
            l += (long)this.c(this.saveBuffer[i]);
        }
    }

    void Y() {
        this.c("Compress pre");
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        byte by = this.saveBuffer[0];
        int n = 1;
        for (int i = 1; i < this.snapshotSize; ++i) {
            if (by != this.saveBuffer[i] || n == 254) {
                this.a(byteArrayOutputStream, (int)by, n);
                by = this.saveBuffer[i];
                n = 1;
                continue;
            }
            ++n;
        }
        this.a(byteArrayOutputStream, (int)by, n);
        this.bm += this.snapshotSize;
        this.saveBuffer = byteArrayOutputStream.toByteArray();
        this.snapshotSize = this.saveBuffer.length;
        this.fw += this.snapshotSize;
        try {
            byteArrayOutputStream.close();
        }
        catch (Exception exception) {
            // empty catch block
        }
        this.c("Compress post");
    }

    void j() {
        this.c("Decompress pre");
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        for (int i = 0; i < this.snapshotSize; ++i) {
            if (this.saveBuffer[i] == -1) {
                for (int j = 0; j < this.c(this.saveBuffer[i + 1]); ++j) {
                    byteArrayOutputStream.write(this.saveBuffer[i + 2]);
                }
                i += 2;
                continue;
            }
            byteArrayOutputStream.write(this.saveBuffer[i]);
        }
        this.fw += this.snapshotSize;
        this.saveBuffer = byteArrayOutputStream.toByteArray();
        this.snapshotSize = this.saveBuffer.length;
        this.bm += this.snapshotSize;
        try {
            byteArrayOutputStream.close();
        }
        catch (Exception exception) {
            // empty catch block
        }
        this.c("Decompress post");
    }

    boolean d(boolean bl) {
        return this.b((byte)(bl ? 1 : 0)) == 1;
    }

    byte b(byte by) {
        if (this.savePendingMode == 2) {
            return 0;
        }
        byte by2 = by;
        if (this.savePendingMode == 0) {
            this.k(1);
            this.saveBuffer[this.saveReadOffset++] = by;
        } else if (this.snapshotLoaded) {
            by2 = this.saveBuffer[this.saveReadOffset++];
        }
        return by2;
    }

    int s(int n) {
        if (this.savePendingMode == 2) {
            return 0;
        }
        int n2 = n;
        if (this.savePendingMode == 0) {
            this.k(4);
            this.saveBuffer[this.saveReadOffset++] = (byte)(n & 0xFF);
            this.saveBuffer[this.saveReadOffset++] = (byte)(n >> 8 & 0xFF);
            this.saveBuffer[this.saveReadOffset++] = (byte)(n >> 16 & 0xFF);
            this.saveBuffer[this.saveReadOffset++] = (byte)(n >> 24 & 0xFF);
        } else if (this.snapshotLoaded) {
            n2 = this.s() + 256 * this.s() + 65536 * this.s() + 0x1000000 * this.s();
        }
        return n2;
    }

    void a(byte[][] byArray) {
        for (int i = 0; i < byArray.length; ++i) {
            this.a(byArray[i]);
        }
    }

    void a(short[][] sArray) {
        for (int i = 0; i < sArray.length; ++i) {
            this.a(sArray[i]);
        }
    }

    void a(byte[] byArray) {
        if (this.savePendingMode == 2) {
            for (int i = 0; i < byArray.length; ++i) {
                byArray[i] = 0;
            }
        } else if (this.savePendingMode == 0) {
            this.k(byArray.length);
            System.arraycopy(byArray, 0, this.saveBuffer, this.saveReadOffset, byArray.length);
            this.saveReadOffset += byArray.length;
        } else if (this.snapshotLoaded) {
            System.arraycopy(this.saveBuffer, this.saveReadOffset, byArray, 0, byArray.length);
            this.saveReadOffset += byArray.length;
        }
    }

    void a(short[] sArray) {
        block4: {
            block5: {
                block3: {
                    if (this.savePendingMode != 2) break block3;
                    for (int i = 0; i < sArray.length; ++i) {
                        sArray[i] = 0;
                    }
                    break block4;
                }
                if (this.savePendingMode != 0) break block5;
                this.k(2 * sArray.length);
                for (int i = 0; i < sArray.length; ++i) {
                    this.saveBuffer[this.saveReadOffset++] = (byte)(sArray[i] & 0xFF);
                    this.saveBuffer[this.saveReadOffset++] = (byte)(sArray[i] >> 8 & 0xFF);
                }
                break block4;
            }
            if (!this.snapshotLoaded) break block4;
            for (int i = 0; i < sArray.length; ++i) {
                sArray[i] = (short)(this.s() + 256 * this.s());
            }
        }
    }

    void a(int[] nArray) {
        block4: {
            block5: {
                block3: {
                    if (this.savePendingMode != 2) break block3;
                    for (int i = 0; i < nArray.length; ++i) {
                        nArray[i] = 0;
                    }
                    break block4;
                }
                if (this.savePendingMode != 0) break block5;
                this.k(4 * nArray.length);
                for (int i = 0; i < nArray.length; ++i) {
                    this.saveBuffer[this.saveReadOffset++] = (byte)(nArray[i] & 0xFF);
                    this.saveBuffer[this.saveReadOffset++] = (byte)(nArray[i] >> 8 & 0xFF);
                    this.saveBuffer[this.saveReadOffset++] = (byte)(nArray[i] >> 16 & 0xFF);
                    this.saveBuffer[this.saveReadOffset++] = (byte)(nArray[i] >> 24 & 0xFF);
                }
                break block4;
            }
            if (!this.snapshotLoaded) break block4;
            for (int i = 0; i < nArray.length; ++i) {
                nArray[i] = this.s() + 256 * this.s() + 65536 * this.s() + 0x1000000 * this.s();
            }
        }
    }

    void g(int n) {
        if (n == 0) {
            this.loadGame(0, 0);
        }
        if (n == 1) {
            this.loadGame(0, 1);
        }
        if (n == 2) {
            this.savePendingMode = 2;
        }
        if (n == 4) {
            this.snapshotSize = 0;
            this.saveBuffer = null;
            this.savePendingMode = 0;
        }
        if (n == 5) {
            this.saveReadOffset = 0;
            this.savePendingMode = 1;
            this.snapshotLoaded = true;
        }
        if (n == 1) {
            this.j();
        }
        this.mapWidth = this.s(this.mapWidth);
        this.mapHeight = this.s(this.mapHeight);
        this.viewportCenterX = this.s(this.viewportCenterX);
        this.viewportCenterY = this.s(this.viewportCenterY);
        this.money = this.s(this.money);
        this.dayOfMonth = this.s(this.dayOfMonth);
        this.monthOfYear = this.s(this.monthOfYear);
        this.year = this.s(this.year);
        this.bI = this.s(this.bI);
        this.viewportBaseX = this.s(this.viewportBaseX);
        this.viewportBaseY = this.s(this.viewportBaseY);
        this.mapScaleX = this.s(this.mapScaleX);
        this.mapScaleY = this.s(this.mapScaleY);
        this.a(this.visibleTrains);
        this.visibleTrainCount = this.s(this.visibleTrainCount);
        this.viewportX0 = this.s(this.viewportX0);
        this.viewportY0 = this.s(this.viewportY0);
        this.viewportX1 = this.s(this.viewportX1);
        this.viewportY1 = this.s(this.viewportY1);
        this.loanBalance = this.s(this.loanBalance);
        this.loanCeiling = this.s(this.loanCeiling);
        this.aD = this.b(this.aD);
        this.eC = this.s(this.eC);
        this.a(this.yrN);
        this.a(this.yrCx);
        this.a(this.yrZ);
        this.a(this.dG);
        this.a(this.ef);
        this.a(this.L);
        this.a(this.trainRevenue);
        this.a(this.prevTrainRevenue);
        this.a(this.bb);
        this.a(this.prevStationRevenue);
        this.fe = this.s(this.fe);
        this.decadeOtherXTotal = this.s(this.decadeOtherXTotal);
        this.totalSpent = this.s(this.totalSpent);
        this.aiMoneySpent = this.s(this.aiMoneySpent);
        this.fuelCostAccrued = this.s(this.fuelCostAccrued);
        this.ho = this.s(this.ho);
        this.totalRevenue = this.s(this.totalRevenue);
        this.eg = this.s(this.eg);
        this.eJ = this.s(this.eJ);
        this.eX = this.s(this.eX);
        this.gx = this.s(this.gx);
        this.eM = this.s(this.eM);
        this.prevFuelCost = this.s(this.prevFuelCost);
        this.prevSubsidyCost = this.s(this.prevSubsidyCost);
        this.prevTotalRevenue = this.s(this.prevTotalRevenue);
        this.prevTerrainTotal = this.s(this.prevTerrainTotal);
        this.prevEnterpriseTotal = this.s(this.prevEnterpriseTotal);
        this.prevXIndicator = this.s(this.prevXIndicator);
        this.bG = this.s(this.bG);
        this.he = this.s(this.he);
        this.bP = this.s(this.bP);
        this.ht = this.s(this.ht);
        this.cr = this.s(this.cr);
        this.cg = this.s(this.cg);
        this.cm = this.s(this.cm);
        this.gq = this.s(this.gq);
        this.decadeCostTotal = this.s(this.decadeCostTotal);
        this.decadeSpentTotal = this.s(this.decadeSpentTotal);
        this.decadeFuelCost = this.s(this.decadeFuelCost);
        this.decadeSubsidyTotal = this.s(this.decadeSubsidyTotal);
        this.decadeRevenueTotal = this.s(this.decadeRevenueTotal);
        this.eb = this.s(this.eb);
        this.decadeEnterpriseTotal = this.s(this.decadeEnterpriseTotal);
        this.decadeXTotal = this.s(this.decadeXTotal);
        this.aE = this.b(this.aE);
        this.a(this.da);
        this.a(this.cityPopulation);
        this.a(this.be);
        this.a(this.vehicleFlags);
        this.a(this.wagonAJ);
        this.a(this.wagonEQ);
        this.a(this.trainFlagBits);
        this.a(this.trainOptionSlots);
        this.numSelectedTrains = this.b(this.numSelectedTrains);
        this.flagsA = this.d(this.flagsA);
        this.a(this.constructionTimers);
        this.a(this.zonePopulation);
        this.a(this.di);
        this.a(this.trainY2);
        this.a(this.trainX2);
        this.era = this.s(this.era);
        this.a(this.trainX);
        this.a(this.trainY);
        this.a(this.trainDir);
        this.a(this.trainPhase);
        this.a(this.trainFlags);
        this.a(this.wagonEnergyRemaining);
        this.a(this.trainEnergyOrCount);
        this.a(this.wagonIorHost);
        this.a(this.trainServiceCounter);
        this.a(this.nextCar);
        this.a(this.trainEraOrStage);
        this.a(this.trainAgeOrTimer);
        this.a(this.eL);
        this.a(this.bz);
        this.cA = this.s(this.cA);
        this.aY = this.d(this.aY);
        this.hu = this.s(this.hu);
        if (this.hu == 0) {
            this.hu = 66;
        }
        this.cd = this.b(this.cd);
        this.fP = this.b(this.fP);
        if (n != 2) {
            this.T = this.b(this.T);
        }
        this.a(this.menuRowType);
        this.a(this.bh);
        this.a(this.bY);
        this.a(this.dn);
        this.a(this.dE);
        this.a(this.fW);
        this.a(this.fF);
        this.a(this.menuRowCost);
        this.a(this.menuRowValue);
        this.a(this.menuRowAltType);
        this.a(this.menuRowEra);
        this.a(this.menuRowRevenue);
        this.a(this.dU);
        this.er = this.s(this.er);
        this.bq = this.s(this.bq);
        this.bv = this.s(this.bv);
        this.flagsB = this.d(this.flagsB);
        this.scenarioBits = this.b(this.scenarioBits);
        this.scenarioMilestoneIndex = this.b(this.scenarioMilestoneIndex);
        this.milestonesEnabled = this.d(this.milestonesEnabled);
        this.currentMilestoneScore = this.s(this.currentMilestoneScore);
        this.milestoneCoThreshold2 = this.s(this.milestoneCoThreshold2);
        this.bD = this.s(this.bD);
        this.milestoneThreshold1 = this.s(this.milestoneThreshold1);
        this.milestoneThreshold2 = this.s(this.milestoneThreshold2);
        this.milestoneThreshold3 = this.s(this.milestoneThreshold3);
        this.milestoneThreshold4 = this.s(this.milestoneThreshold4);
        this.milestoneThreshold5 = this.s(this.milestoneThreshold5);
        this.aiWagonsBought = this.s(this.aiWagonsBought);
        this.by = this.s(this.by);
        this.scenarioStep = this.s(this.scenarioStep);
        this.ao = this.s(this.ao);
        this.moneyBaseA = this.s(this.moneyBaseA);
        this.moneyBaseB = this.s(this.moneyBaseB);
        this.baseTrainSpeed = this.s(this.baseTrainSpeed);
        this.accel1 = this.s(this.accel1);
        this.brake1 = this.s(this.brake1);
        this.baseTrainSpeed2 = this.s(this.baseTrainSpeed2);
        this.accel2 = this.s(this.accel2);
        this.costPower = this.s(this.costPower);
        this.costStruct = this.s(this.costStruct);
        this.costOverhead = this.s(this.costOverhead);
        this.costMaintenance = this.s(this.costMaintenance);
        this.costCrew = this.s(this.costCrew);
        this.gWagonCost = this.s(this.gWagonCost);
        this.gWagonCost2 = this.s(this.gWagonCost2);
        this.gWagonCost3 = this.s(this.gWagonCost3);
        this.gWagonCost4 = this.s(this.gWagonCost4);
        this.gWagonCost5 = this.s(this.gWagonCost5);
        this.gWagonCost6 = this.s(this.gWagonCost6);
        this.costGroupC1 = this.s(this.costGroupC1);
        this.costGroupC2 = this.s(this.costGroupC2);
        this.costGroupC3 = this.s(this.costGroupC3);
        this.costGroupC4 = this.s(this.costGroupC4);
        if (n == 0) {
            this.Y();
            this.saveGame();
        }
    }

    void A() {
        ch = this.d(ch);
        selectionHidden = this.d(selectionHidden);
        cc = this.d(cc);
        q = this.d(q);
        this.aM = this.s(this.aM);
        this.H = this.s(this.H);
        this.gh = this.s(this.gh);
        this.a(this.gk);
        this.a(this.bd);
        this.a(this.bE);
    }

    void O() {
        this.loadGame(5, 0);
        this.A();
        this.saveGame();
    }

    void Z() {
        this.loadGame(5, 1);
        this.A();
        this.C(this.gh);
    }

    void t() {
        this.bm = 0;
        this.fw = 0;
        this.g(0);
        this.loadGame(1, 0);
        this.a(this.tileMap);
        this.Y();
        this.saveGame();
        this.loadGame(2, 0);
        this.a(this.trackMap);
        this.Y();
        this.saveGame();
        this.loadGame(3, 0);
        this.a(this.tileElevation);
        this.Y();
        this.saveGame();
        this.loadGame(4, 0);
        this.a(this.heightMap);
        this.Y();
        this.saveGame();
    }

    boolean X() {
        this.loadGame(0, 1);
        return this.snapshotLoaded;
    }

    void u() {
        this.bm = 0;
        this.fw = 0;
        this.g(1);
        this.tileMap = new byte[this.mapWidth + 1][this.mapHeight + 1];
        this.trackMap = new byte[this.mapWidth + 1][this.mapHeight + 1];
        this.tileElevation = new byte[this.mapWidth + 1][this.mapHeight + 1];
        this.heightMap = new byte[this.mapWidth + 2][this.mapHeight + 2];
        this.hj = (this.mapWidth - 1) * 8;
        this.mapScreenWidth = (this.mapWidth + this.mapHeight) * 16;
        this.mapScreenHeight = (this.mapWidth + this.mapHeight) * 8;
        this.gU = this.mapScreenWidth / 2;
        this.fO = this.mapScreenHeight / 2;
        this.v(-1);
        this.loadGame(1, 1);
        this.j();
        this.a(this.tileMap);
        this.loadGame(2, 1);
        this.j();
        this.a(this.trackMap);
        this.loadGame(3, 1);
        this.j();
        this.a(this.tileElevation);
        this.loadGame(4, 1);
        this.j();
        this.a(this.heightMap);
        this.bH = -1;
    }

    void everyTenTick() {
        this.yrZ[0] = this.h();
        this.L[0] = this.yrZ[0];
    }

    void l() {
        this.archiveOrRecall();
        this.yrN[0] = this.eX;
        this.yrCx[0] = this.totalRevenue;
        this.dG[0] = this.decadeOtherXTotal + this.eX;
        this.ef[0] = this.fe + this.totalRevenue;
        this.br = this.decadeCostTotal + this.totalSpent;
        this.gV = this.decadeSpentTotal + this.aiMoneySpent;
        this.cj = this.decadeFuelCost + this.fuelCostAccrued;
        this.aA = this.decadeSubsidyTotal + this.ho;
        this.dg = this.decadeRevenueTotal + this.totalRevenue;
        this.bM = this.decadeXTotal + this.eX;
        this.dq = this.decadeEnterpriseTotal + this.eJ;
    }

    void newYearNotify() {
        int n;
        for (n = 0; n < 500; ++n) {
            this.prevTrainRevenue[n] = this.trainRevenue[n];
            this.trainRevenue[n] = 0;
        }
        for (n = 0; n < 48; ++n) {
            this.prevStationRevenue[n] = this.bb[n];
            this.bb[n] = 0;
        }
        this.archiveOrRecall();
        this.gx = this.totalSpent;
        this.eM = this.aiMoneySpent;
        this.prevFuelCost = this.fuelCostAccrued;
        this.prevSubsidyCost = this.ho;
        this.prevTotalRevenue = this.totalRevenue;
        this.prevTerrainTotal = this.eg;
        this.prevEnterpriseTotal = this.eJ;
        this.prevXIndicator = this.eX;
        for (n = 10; n >= 0; --n) {
            this.yrN[n + 1] = this.yrN[n];
            this.yrCx[n + 1] = this.yrCx[n];
            this.yrZ[n + 1] = this.yrZ[n];
        }
        this.yrN[0] = 0;
        this.yrCx[0] = 0;
        this.yrZ[0] = 0;
        this.decadeCostTotal += this.totalSpent;
        this.decadeSpentTotal += this.aiMoneySpent;
        this.decadeFuelCost += this.fuelCostAccrued;
        this.decadeSubsidyTotal += this.ho;
        this.decadeRevenueTotal += this.totalRevenue;
        this.decadeXTotal += this.eX;
        this.decadeEnterpriseTotal += this.eJ;
        this.decadeOtherXTotal += this.eX;
        this.fe += this.totalRevenue;
        if (this.monthOfYear == 13) {
            this.bG = this.br;
            this.he = this.gV;
            this.bP = this.cj;
            this.ht = this.aA;
            this.cr = this.dg;
            this.cg = this.gs;
            this.cm = this.dq;
            this.gq = this.bM;
            this.decadeCostTotal = 0;
            this.decadeSpentTotal = 0;
            this.decadeFuelCost = 0;
            this.decadeSubsidyTotal = 0;
            this.decadeRevenueTotal = 0;
            this.decadeXTotal = 0;
            this.decadeEnterpriseTotal = 0;
            for (n = 10; n >= 0; --n) {
                this.dG[n + 1] = this.dG[n];
                this.ef[n + 1] = this.ef[n];
                this.L[n + 1] = this.L[n];
            }
            this.decadeOtherXTotal = 0;
            this.fe = 0;
            this.dG[0] = 0;
            this.ef[0] = 0;
            this.L[0] = 0;
        }
        this.L[0] = this.h();
        this.money -= this.eg;
        if (this.eg > 0) {
            this.p(-this.eg);
        }
        this.money -= this.eJ;
        if (this.eJ > 0) {
            this.p(-this.eJ);
        }
        this.totalSpent = 0;
        this.aiMoneySpent = 0;
        this.fuelCostAccrued = 0;
        this.ho = 0;
        this.totalRevenue = 0;
        this.eX = 0;
        this.eJ = 0;
        this.mapRedrawPending = true;
        this.y = true;
        this.dS = true;
        if (this.monthOfYear == 13) {
            if (q) {
                this.ready = false;
                this.transitionCode = 35;
                this.setState(20);
            }
        } else if (cc) {
            this.ready = false;
            this.transitionCode = 32;
            this.setState(20);
        }
    }

    void archiveOrRecall() {
        this.eg = this.loanBalance * this.aD / 1200;
        this.eJ = this.totalRevenue * this.eC / 100;
        this.eX = this.totalRevenue - this.eg - this.totalSpent - this.aiMoneySpent - this.fuelCostAccrued - this.ho - this.eJ;
    }

    int h() {
        int n;
        int n2;
        int n3;
        int n4 = 0;
        for (n3 = 0; n3 < this.mapWidth; ++n3) {
            for (n2 = 0; n2 < this.mapHeight; ++n2) {
                if (this.trackMap[n3][n2] != 65) continue;
                n = this.tileMap[n3][n2];
                if (n >= 70 && n <= 80) {
                    if (n > 76) {
                        n4 += 94;
                    }
                    n4 += 196;
                }
                if (n == 76) {
                    n4 += 168;
                }
                if (n >= 81 && n <= 86) {
                    n4 += this.accel1;
                }
                if (n == 87 || n == 88) {
                    n4 += this.costPower;
                }
                if (n != 89 && n != 90) continue;
                n4 += this.costPower / 2;
            }
        }
        for (n3 = 0; n3 < 500; ++n3) {
            if ((this.trainFlags[n3] & 0x80) != 128 || this.trackMap[n2 = this.c(this.trainX[n3])][n = this.c(this.trainY[n3])] != 65) continue;
            if ((this.trainFlags[n3] & 0x40) == 64) {
                switch (this.trainEraOrStage[n3]) {
                    case 0: {
                        n4 += this.gWagonCost;
                        break;
                    }
                    case 1: {
                        n4 += this.gWagonCost2;
                        break;
                    }
                    case 2: {
                        n4 += this.gWagonCost3;
                        break;
                    }
                    case 3: {
                        n4 += this.gWagonCost4;
                        break;
                    }
                    case 4: {
                        n4 += this.gWagonCost5;
                        break;
                    }
                    case 5: {
                        n4 += this.gWagonCost6;
                    }
                }
                continue;
            }
            n4 += (this.costGroupC1 + this.costGroupC2 + this.costGroupC3 + this.costGroupC4) / 4;
        }
        return n4 -= this.loanBalance;
    }

    void b(int n, boolean bl) {
        block29: {
            int n2;
            int n3;
            block28: {
                int n4;
                this.W = n;
                this.bR = bl;
                int n5 = 0;
                int n6 = 0;
                for (n3 = 0; n3 < 12; ++n3) {
                    if (!bl) {
                        this.o[n3] = (byte)(this.year - 1940);
                        this.dm[n3] = (byte)(this.monthOfYear - n3);
                        if (this.dm[n3] < 1) {
                            this.dm[n3] = (byte)(12 + this.dm[n3]);
                            int n7 = n3;
                            this.o[n7] = (byte)(this.o[n7] - 1);
                        }
                        if (n == 0) {
                            if (this.yrN[n3] > n6) {
                                n6 = this.yrN[n3];
                            }
                            if (this.yrN[n3] < n5) {
                                n5 = this.yrN[n3];
                            }
                        }
                        if (n == 1) {
                            if (this.yrCx[n3] > n6) {
                                n6 = this.yrCx[n3];
                            }
                            if (this.yrCx[n3] < n5) {
                                n5 = this.yrCx[n3];
                            }
                        }
                        if (n != 2) continue;
                        if (this.yrZ[n3] > n6) {
                            n6 = this.yrZ[n3];
                        }
                        if (this.yrZ[n3] >= n5) continue;
                        n5 = this.yrZ[n3];
                        continue;
                    }
                    this.dm[n3] = (byte)(this.year - n3 - 1940);
                    if (n == 0) {
                        if (this.dG[n3] > n6) {
                            n6 = this.dG[n3];
                        }
                        if (this.dG[n3] < n5) {
                            n5 = this.dG[n3];
                        }
                    }
                    if (n == 1) {
                        if (this.ef[n3] > n6) {
                            n6 = this.ef[n3];
                        }
                        if (this.ef[n3] < n5) {
                            n5 = this.ef[n3];
                        }
                    }
                    if (n != 2) continue;
                    if (this.L[n3] > n6) {
                        n6 = this.L[n3];
                    }
                    if (this.L[n3] >= n5) continue;
                    n5 = this.L[n3];
                }
                n3 = Math.max(n6, -n5);
                if (bl) break block28;
                if (n == 0) {
                    for (n4 = 0; n4 < 12; ++n4) {
                        this.fA[n4] = this.yrN[n4] == 0 ? (byte)0 : (byte)(30 * this.yrN[n4] / n3);
                    }
                }
                if (n == 1) {
                    for (n4 = 0; n4 < 12; ++n4) {
                        this.fA[n4] = this.yrCx[n4] == 0 ? (byte)0 : (byte)(60 * this.yrCx[n4] / n3);
                    }
                }
                if (n != 2) break block29;
                for (n4 = 0; n4 < 12; ++n4) {
                    this.fA[n4] = this.yrN[n4] == 0 ? (byte)0 : (byte)(30 * this.yrZ[n4] / n3);
                }
                break block29;
            }
            if (n == 0) {
                for (n2 = 0; n2 < 12; ++n2) {
                    this.fA[n2] = this.dG[n2] == 0 ? (byte)0 : (byte)(30 * this.dG[n2] / n3);
                }
            }
            if (n == 1) {
                for (n2 = 0; n2 < 12; ++n2) {
                    this.fA[n2] = this.ef[n2] == 0 ? (byte)0 : (byte)(60 * this.ef[n2] / n3);
                }
            }
            if (n == 2) {
                for (n2 = 0; n2 < 12; ++n2) {
                    this.fA[n2] = this.dG[n2] == 0 ? (byte)0 : (byte)(30 * this.L[n2] / n3);
                }
            }
        }
    }

    void g() {
        if (this.buildMenuMode != 2 && this.buildMenuMode != 4 && this.rng.nextInt() % this.cd > this.fP) {
            this.setState(105);
        }
    }

    void c() {
        int n = 0;
        int n2 = 0;
        do {
            this.F();
        } while (++n != 50 && this.cA == -1);
        if (n < 50) {
            do {
                this.D();
            } while (++n2 != 50 && !this.aY);
            if (n2 < 50) {
                this.placementX = this.gS;
                this.placementY = this.bp;
                this.demolishX = this.dD;
                this.demolishY = this.k;
                if (this.K()) {
                    this.g(this.gS, this.bp);
                    this.g(this.dD, this.k);
                    this.m(this.gS, this.bp);
                    this.n(this.hu - 64);
                    ++this.hu;
                    if (this.hu == 69) {
                        this.hu = 66;
                    }
                }
            }
        }
    }

    void dayResetAd() {
        for (int i = 0; i < 500; ++i) {
            int n;
            int n2;
            if ((this.trainFlags[i] & 0x80) != 128 || (this.trainFlags[i] & 0x40) != 64 || this.trackMap[n2 = this.c(this.trainX[i])][n = this.c(this.trainY[i])] != 66 && this.trackMap[n2][n] != 67 && this.trackMap[n2][n] != 68 || this.trainEraOrStage[i] >= this.era) continue;
            this.trainEraOrStage[i] = (byte)this.era;
            this.trainFlags[i] = -64;
            int n3 = i;
            this.trainFlags[n3] = (byte)(this.trainFlags[n3] | this.era / 3);
            this.wagonEnergyRemaining[i] = (byte)(8 - this.era);
            break;
        }
    }

    void postRevenueReset() {
        for (int i = 0; i < 500; ++i) {
            int n;
            int n2;
            if ((this.trainFlags[i] & 0x80) != 128 || this.trackMap[n2 = this.c(this.trainX[i])][n = this.c(this.trainY[i])] != 66 && this.trackMap[n2][n] != 67 && this.trackMap[n2][n] != 68) continue;
            this.trainAgeOrTimer[i] = 0;
        }
    }

    void m(int n, int n2) {
        int n3 = this.era / 3;
        int n4 = this.era;
        int n5 = this.selectionCost / 8000 + 1;
        if (n5 > 5) {
            n5 = 5;
        }
        int n6 = 2;
        if (this.cA == 1) {
            n6 = 3;
        }
        if (this.cA == 2) {
            n6 = 4;
        }
        if (this.cA == 3) {
            n6 = 6;
        }
        int n7 = this.b(n, n2, n3, n4, this.tileMap[n][n2] == 87 ? 0 : 2, 8 - n4);
        for (int i = 0; i < n5; ++i) {
            n7 = this.spawnTrainCar(n7, n6);
        }
    }

    void g(int n, int n2) {
        boolean bl;
        boolean bl2;
        boolean bl3;
        boolean bl4;
        boolean bl5 = n > 0 ? this.trackMap[n - 1][n2] == 64 : (bl4 = false);
        boolean bl6 = n < this.mapWidth - 1 ? this.trackMap[n + 1][n2] == 64 : (bl3 = false);
        boolean bl7 = n2 > 0 ? this.trackMap[n][n2 - 1] == 64 : (bl2 = false);
        boolean bl8 = n2 < this.mapHeight - 1 ? this.trackMap[n][n2 + 1] == 64 : (bl = false);
        if (bl4 || bl3) {
            this.tileMap[n][n2] = 88;
            this.trackMap[n][n2] = (byte)this.hu;
            if (bl3 && n > 1 && (this.tileMap[n - 1][n2] == 0 || this.tileMap[n - 1][n2] >= 29 && this.tileMap[n - 1][n2] <= 33) && (this.tileMap[n - 2][n2] == 0 || this.tileMap[n - 2][n2] >= 29 && this.tileMap[n - 2][n2] <= 33)) {
                this.tileMap[n][n2] = 90;
                this.tileMap[n - 1][n2] = 88;
                this.tileMap[n - 2][n2] = 90;
                this.trackMap[n - 1][n2] = (byte)this.hu;
                this.trackMap[n - 2][n2] = (byte)this.hu;
            }
            if (bl4 && n < this.mapWidth - 2 && (this.tileMap[n + 1][n2] == 0 || this.tileMap[n + 1][n2] >= 29 && this.tileMap[n + 1][n2] <= 33) && (this.tileMap[n + 2][n2] == 0 || this.tileMap[n + 2][n2] >= 29 && this.tileMap[n + 2][n2] <= 33)) {
                this.tileMap[n][n2] = 90;
                this.tileMap[n + 1][n2] = 88;
                this.tileMap[n + 2][n2] = 90;
                this.trackMap[n + 1][n2] = (byte)this.hu;
                this.trackMap[n + 2][n2] = (byte)this.hu;
            }
        }
        if (bl2 || bl) {
            this.tileMap[n][n2] = 87;
            this.trackMap[n][n2] = (byte)this.hu;
            if (bl2 && n2 > 1 && (this.tileMap[n][n2 - 1] == 0 || this.tileMap[n][n2 - 1] >= 29 && this.tileMap[n][n2 - 1] <= 33) && (this.tileMap[n][n2 - 2] == 0 || this.tileMap[n][n2 - 2] >= 29 && this.tileMap[n][n2 - 2] <= 33)) {
                this.tileMap[n][n2] = 89;
                this.tileMap[n][n2 - 1] = 87;
                this.tileMap[n][n2 - 2] = 89;
                this.trackMap[n][n2 - 1] = (byte)this.hu;
                this.trackMap[n][n2 - 2] = (byte)this.hu;
            }
            if (bl2 && n2 < this.mapHeight - 2 && (this.tileMap[n][n2 + 1] == 0 || this.tileMap[n][n2 + 1] >= 29 && this.tileMap[n][n2 + 1] <= 33) && (this.tileMap[n][n2 + 2] == 0 || this.tileMap[n][n2 + 2] >= 29 && this.tileMap[n][n2 + 2] <= 33)) {
                this.tileMap[n][n2] = 89;
                this.tileMap[n][n2 + 1] = 87;
                this.tileMap[n][n2 + 2] = 89;
                this.trackMap[n][n2 + 1] = (byte)this.hu;
                this.trackMap[n][n2 + 2] = (byte)this.hu;
            }
        }
    }

    void F() {
        int n = Math.abs(this.rng.nextInt() % this.mapWidth);
        int n2 = Math.abs(this.rng.nextInt() % this.mapHeight);
        this.cA = -1;
        if (this.tileMap[n][n2] == 0 || this.tileMap[n][n2] >= 29 && this.tileMap[n][n2] <= 33) {
            int n3 = n - 2;
            int n4 = n2 - 2;
            int n5 = n + 2;
            int n6 = n2 + 2;
            if (n3 < 0) {
                n3 = 0;
            }
            if (n4 < 0) {
                n4 = 0;
            }
            if (n5 >= this.mapWidth) {
                n5 = this.mapWidth - 1;
            }
            if (n6 >= this.mapHeight) {
                n6 = this.mapHeight - 1;
            }
            block0: for (int i = n3; i <= n5; ++i) {
                for (int j = n4; j <= n6; ++j) {
                    byte by = this.tileMap[i][j];
                    if (by >= 34 && by <= 40) {
                        this.cA = 0;
                    }
                    if (by >= 65 && by <= 68) {
                        this.cA = 1;
                    }
                    if (by >= 53 && by <= 55) {
                        this.cA = 2;
                    }
                    if (by >= 59 && by <= 61) {
                        this.cA = 3;
                    }
                    if (this.cA > 0) break block0;
                }
            }
            if (this.cA >= 0) {
                this.gS = n;
                this.bp = n2;
            }
        }
    }

    void D() {
        int n = Math.abs(this.rng.nextInt() % this.mapWidth);
        int n2 = Math.abs(this.rng.nextInt() % this.mapHeight);
        this.aY = false;
        if (this.tileMap[n][n2] == 0 || this.tileMap[n][n2] >= 29 && this.tileMap[n][n2] <= 33) {
            int n3 = n - 2;
            int n4 = n2 - 2;
            int n5 = n + 2;
            int n6 = n2 + 2;
            if (n3 < 0) {
                n3 = 0;
            }
            if (n4 < 0) {
                n4 = 0;
            }
            if (n5 >= this.mapWidth) {
                n5 = this.mapWidth - 1;
            }
            if (n6 >= this.mapHeight) {
                n6 = this.mapHeight - 1;
            }
            block0: for (int i = n3; i <= n5; ++i) {
                for (int j = n4; j <= n6; ++j) {
                    byte by = this.tileMap[i][j];
                    if (by >= 34 && by <= 40 && this.cA == 0) {
                        this.aY = true;
                    }
                    if (by >= 62 && by <= 64 && this.cA == 1) {
                        this.aY = true;
                    }
                    if (by >= 50 && by <= 52 && this.cA == 2) {
                        this.aY = true;
                    }
                    if (by >= 56 && by <= 58 && this.cA == 3) {
                        this.aY = true;
                    }
                    if (this.aY) break block0;
                }
            }
            if (this.aY) {
                this.dD = n;
                this.k = n2;
            }
        }
    }

    int U() {
        int n = 0;
        int n2 = 0;
        int n3 = this.currentMilestoneScore;
        if (this.scenarioMilestoneIndex >= 6 && this.scenarioMilestoneIndex <= 11) {
            n3 += this.milestoneCoThreshold2;
        }
        switch (this.scenarioMilestoneIndex) {
            case 1: {
                n2 = this.milestoneThreshold1;
                break;
            }
            case 2: {
                n2 = this.milestoneThreshold2;
                break;
            }
            case 3: {
                n2 = this.milestoneThreshold3;
                break;
            }
            case 4: {
                n2 = this.milestoneThreshold4;
                break;
            }
            case 5: {
                n2 = this.milestoneThreshold5;
                break;
            }
            case 12: {
                n2 = this.bD;
                break;
            }
            case 15: {
                n2 = this.aiWagonsBought;
                break;
            }
            case 14: {
                n2 = this.year - 1950;
                n3 = this.currentMilestoneScore - 1950;
                break;
            }
            case 13: {
                n2 = this.loanCeiling - this.loanBalance;
                n3 = this.loanCeiling;
                break;
            }
            case 6: {
                n2 = this.milestoneThreshold1 + this.milestoneThreshold2;
                break;
            }
            case 7: {
                n2 = this.milestoneThreshold1 + this.milestoneThreshold3;
                break;
            }
            case 8: {
                n2 = this.milestoneThreshold1 + this.milestoneThreshold4;
                break;
            }
            case 9: {
                n2 = this.milestoneThreshold2 + this.milestoneThreshold3;
                break;
            }
            case 10: {
                n2 = this.milestoneThreshold2 + this.milestoneThreshold4;
                break;
            }
            case 11: {
                n2 = this.milestoneThreshold4 + this.milestoneThreshold5;
            }
        }
        n = 100 * n2 / n3;
        if (n < 0) {
            n = 0;
        }
        if (n > 100) {
            n = 100;
        }
        return n;
    }

    void everyTwoTickZ() {
        boolean bl = false;
        if (!this.milestonesEnabled) {
            switch (this.scenarioMilestoneIndex) {
                case 1: {
                    if (this.currentMilestoneScore > this.milestoneThreshold1) break;
                    bl = true;
                    break;
                }
                case 2: {
                    if (this.currentMilestoneScore > this.milestoneThreshold2) break;
                    bl = true;
                    break;
                }
                case 3: {
                    if (this.currentMilestoneScore > this.milestoneThreshold3) break;
                    bl = true;
                    break;
                }
                case 4: {
                    if (this.currentMilestoneScore > this.milestoneThreshold4) break;
                    bl = true;
                    break;
                }
                case 5: {
                    if (this.currentMilestoneScore > this.milestoneThreshold5) break;
                    bl = true;
                    break;
                }
                case 6: {
                    if (this.currentMilestoneScore > this.milestoneThreshold1 || this.milestoneCoThreshold2 > this.milestoneThreshold2) break;
                    bl = true;
                    break;
                }
                case 7: {
                    if (this.currentMilestoneScore > this.milestoneThreshold1 || this.milestoneCoThreshold2 > this.milestoneThreshold3) break;
                    bl = true;
                    break;
                }
                case 8: {
                    if (this.currentMilestoneScore > this.milestoneThreshold1 || this.milestoneCoThreshold2 > this.milestoneThreshold4) break;
                    bl = true;
                    break;
                }
                case 9: {
                    if (this.currentMilestoneScore > this.milestoneThreshold2 || this.milestoneCoThreshold2 > this.milestoneThreshold3) break;
                    bl = true;
                    break;
                }
                case 10: {
                    if (this.currentMilestoneScore > this.milestoneThreshold2 || this.milestoneCoThreshold2 > this.milestoneThreshold4) break;
                    bl = true;
                    break;
                }
                case 11: {
                    if (this.currentMilestoneScore > this.milestoneThreshold4 || this.milestoneCoThreshold2 > this.milestoneThreshold5) break;
                    bl = true;
                    break;
                }
                case 12: {
                    if (this.currentMilestoneScore > this.bD) break;
                    bl = true;
                    break;
                }
                case 13: {
                    if (this.loanBalance != 0) break;
                    bl = true;
                    break;
                }
                case 14: {
                    if (this.year != this.currentMilestoneScore) break;
                    bl = true;
                    break;
                }
                case 15: {
                    if (this.currentMilestoneScore > this.aiWagonsBought) break;
                    bl = true;
                }
            }
        }
        if (bl) {
            this.ready = false;
            this.transitionCode = 6;
            this.milestonesEnabled = true;
            if (this.scenarioStep >= 2) {
                if (this.aM < this.scenarioStep) {
                    this.aM = this.scenarioStep;
                }
                this.O();
                this.gJ = this.c(this.scenarioStep - 2);
            }
            this.setState(20);
        }
    }

    void a() {
        this.aO = 0;
        for (int i = 0; i < 5; ++i) {
            if (this.menuRowType[i] == 0) continue;
            this.gP[this.aO] = (byte)i;
            ++this.aO;
        }
    }

    byte a(int n) {
        if (this.menuRowType[n] == 1) {
            int n2 = 100 * this.menuRowValue[n] / this.menuRowCost[n];
            if (n2 > 100) {
                n2 = 100;
            }
            if (n2 < 0) {
                n2 = 0;
            }
            return (byte)n2;
        }
        if (this.menuRowType[n] == 2) {
            int n3 = this.money > 0 ? 100 * this.money / this.menuRowCost[n] : 100 * this.menuRowCost[n] / this.money;
            if (n3 > 100) {
                n3 = 100;
            }
            if (n3 < 0) {
                n3 = 0;
            }
            return (byte)n3;
        }
        if (this.menuRowType[n] >= 4 && this.menuRowType[n] <= 14) {
            int n4 = 100 * this.menuRowValue[n] / this.menuRowCost[n];
            if (n4 > 100) {
                n4 = 100;
            }
            if (n4 < 0) {
                n4 = 0;
            }
            return (byte)n4;
        }
        return 0;
    }

    void f(int n, int n2) {
        int n3 = n * 60 + 20 + this.rng.nextInt() % 20;
        int n4 = this.bh[n2] + n3;
        int n5 = this.dn[n2];
        int n6 = this.fW[n2];
        while (n4 > 28) {
            n4 -= 28;
            ++n5;
        }
        while (n5 > 12) {
            n5 -= 12;
            ++n6;
        }
        if (n4 == 1) {
            ++n4;
        }
        this.bY[n2] = (byte)n4;
        this.dE[n2] = (byte)n5;
        this.fF[n2] = (byte)n6;
    }

    void D(int n) {
        if (this.menuRowAltType[n] == 1 || this.menuRowAltType[n] == 2) {
            this.money += this.menuRowRevenue[n];
            this.p(this.menuRowRevenue[n]);
        }
        if (this.menuRowAltType[n] == 3) {
            this.loanCeiling += this.menuRowRevenue[n];
        }
        if (this.menuRowAltType[n] == 4 && this.era < 5) {
            ++this.era;
        }
        if (this.menuRowAltType[n] >= 5 && this.menuRowAltType[n] <= 10) {
            int n2 = this.menuRowAltType[n] - 5;
            this.eA[n2] = (byte)(this.eA[n2] + this.menuRowEra[n]);
        }
        if (this.menuRowAltType[n] == 11) {
            this.loanBalance -= this.loanBalance * this.menuRowEra[n] / 100;
            if (this.loanBalance < 0) {
                this.loanBalance = 0;
            }
            if (this.loanBalance % 20000 != 0) {
                this.loanBalance = this.loanBalance % 20000 < 10000 ? (this.loanBalance -= this.loanBalance % 20000) : (this.loanBalance += 20000 - this.loanBalance % 20000);
            }
        }
        if (this.menuRowAltType[n] == 12 && this.aD > 1) {
            this.aD = (byte)(this.aD - 1);
        }
        this.fY = this.menuRowType[n] + 40;
        this.dR = this.menuRowAltType[n] + 60;
        this.cW = (byte)3;
        this.cZ = (byte)5;
        this.menuRowType[5] = this.menuRowType[n];
        this.menuRowCost[5] = this.menuRowCost[n];
        this.menuRowValue[5] = this.menuRowValue[n];
        this.menuRowAltType[5] = this.menuRowAltType[n];
        this.menuRowEra[5] = this.menuRowEra[n];
        this.menuRowRevenue[5] = this.menuRowRevenue[n];
        this.bh[5] = this.bh[n];
        this.dn[5] = this.dn[n];
        this.fW[5] = this.fW[n];
        this.bY[5] = this.bY[n];
        this.dE[5] = this.dE[n];
        this.fF[5] = this.fF[n];
        this.menuRowType[n] = 0;
        this.menuRowCost[n] = 0;
        this.menuRowValue[n] = 0;
        this.menuRowAltType[n] = 0;
        this.menuRowEra[n] = 0;
        this.menuRowRevenue[n] = 0;
        this.ready = false;
        this.transitionCode = 9;
        this.setState(20);
    }

    void e(int n) {
        this.fY = this.menuRowType[n] + 40;
        this.dR = this.menuRowAltType[n] + 60;
        this.cW = (byte)4;
        this.cZ = (byte)5;
        this.menuRowType[5] = this.menuRowType[n];
        this.menuRowCost[5] = this.menuRowCost[n];
        this.menuRowValue[5] = this.menuRowValue[n];
        this.menuRowAltType[5] = this.menuRowAltType[n];
        this.menuRowEra[5] = this.menuRowEra[n];
        this.menuRowRevenue[5] = this.menuRowRevenue[n];
        this.bh[5] = this.bh[n];
        this.dn[5] = this.dn[n];
        this.fW[5] = this.fW[n];
        this.bY[5] = this.bY[n];
        this.dE[5] = this.dE[n];
        this.fF[5] = this.fF[n];
        this.menuRowType[n] = 0;
        this.menuRowCost[n] = 0;
        this.menuRowValue[n] = 0;
        this.menuRowAltType[n] = 0;
        this.menuRowEra[n] = 0;
        this.menuRowRevenue[n] = 0;
        this.ready = false;
        this.transitionCode = 8;
        this.setState(20);
    }

    void Q() {
        int n;
        if (!this.flagsB) {
            return;
        }
        for (n = 0; n < 6; ++n) {
            if (this.eA[n] <= 0) continue;
            int n2 = n;
            this.eA[n2] = (byte)(this.eA[n2] - 1);
        }
        for (n = 0; n < 5; ++n) {
            if ((this.menuRowType[n] == 1 || this.menuRowType[n] >= 4 && this.menuRowType[n] <= 14) && this.menuRowCost[n] <= this.menuRowValue[n]) {
                this.D(n);
            }
            if (this.bY[n] != this.dayOfMonth || this.dE[n] != this.monthOfYear || this.fF[n] != this.year - 1950) continue;
            if (this.menuRowType[n] == 2 && this.menuRowCost[n] <= this.money) {
                this.D(n);
            }
            if (this.menuRowType[n] == 0) continue;
            this.e(n);
        }
        n = this.dayOfMonth + 32 * this.monthOfYear + 370 * (this.year - 1950);
        if (n - this.bv > 150 + this.rng.nextInt() % 20) {
            int n3;
            int n4 = -1;
            for (n3 = 0; n3 < 5; ++n3) {
                if (this.menuRowType[n3] != 0) continue;
                n4 = n3;
                break;
            }
            if (n4 > -1) {
                int n5;
                this.bh[n4] = (byte)this.dayOfMonth;
                this.dn[n4] = (byte)this.monthOfYear;
                this.fW[n4] = (byte)(this.year - 1950);
                n3 = 4 + Math.abs(this.rng.nextInt() % 12);
                this.f(n3, n4);
                int n6 = Math.abs(this.rng.nextInt() % 16);
                int n7 = 0;
                int n8 = 0;
                int n9 = 0;
                int n10 = 0;
                int n11 = 0;
                int n12 = 0;
                block9: for (n5 = 0; n5 < 500; ++n5) {
                    if ((this.trainFlags[n5] & 0x80) != 128 || this.trackMap[this.c(this.trainX[n5])][this.c(this.trainY[n5])] != 65) continue;
                    ++n8;
                    if ((this.trainFlags[n5] & 0x40) == 64) {
                        ++n7;
                        continue;
                    }
                    switch (this.trainFlags[n5] & 7) {
                        case 2: {
                            ++n12;
                            continue block9;
                        }
                        case 3: {
                            ++n11;
                            continue block9;
                        }
                        case 4: 
                        case 5: {
                            ++n10;
                            continue block9;
                        }
                        case 6: 
                        case 7: {
                            ++n12;
                        }
                    }
                }
                this.menuRowType[n4] = n6 > 0 && n6 < 15 ? (byte)n6 : (byte)0;
                if (n6 == 1) {
                    this.menuRowCost[n4] = n3 * (n7 + 2) * (3000 + this.rng.nextInt() % 1500);
                }
                if (n6 == 2) {
                    this.menuRowCost[n4] = this.money + n3 * (n7 + 2) * (2000 + this.rng.nextInt() % 1000);
                }
                if (n6 == 3) {
                    this.menuRowType[n4] = 0;
                }
                if (n6 == 4) {
                    this.menuRowCost[n4] = n3 * (n9 * 2 + 5) * (25 + this.rng.nextInt() % 3);
                }
                if (n6 == 5) {
                    this.menuRowCost[n4] = n3 * (n10 * 2 + 5) * (25 + this.rng.nextInt() % 3);
                }
                if (n6 == 6) {
                    this.menuRowCost[n4] = n3 * (n11 * 2 + 5) * (25 + this.rng.nextInt() % 3);
                }
                if (n6 == 7) {
                    this.menuRowCost[n4] = n3 * (n12 * 2 + 5) * (18 + this.rng.nextInt() % 3);
                }
                if (n6 == 8) {
                    this.menuRowCost[n4] = n3 * (n12 * 2 + 5) * (10 + this.rng.nextInt() % 3);
                }
                if (n6 == 9) {
                    this.menuRowCost[n4] = 3 + Math.abs(this.rng.nextInt() % 8);
                }
                if (n6 == 10) {
                    this.menuRowCost[n4] = 3 + Math.abs(this.rng.nextInt() % 8);
                }
                if (n6 == 11) {
                    this.menuRowCost[n4] = 3 + Math.abs(this.rng.nextInt() % 8);
                }
                if (n6 == 12) {
                    this.menuRowCost[n4] = 3 + Math.abs(this.rng.nextInt() % 8);
                }
                if (n6 == 13) {
                    this.menuRowCost[n4] = (13324 + Math.abs(this.rng.nextInt() % 5000)) * n3;
                }
                if (n6 == 14) {
                    if (this.loanBalance > 40000) {
                        this.menuRowCost[n4] = this.loanBalance * (75 + Math.abs(this.rng.nextInt() % 15)) / 100;
                    } else {
                        this.menuRowType[n4] = 0;
                    }
                }
                if (n6 != 0) {
                    n5 = 0;
                    while (n5 == 0) {
                        n5 = 1 + Math.abs(this.rng.nextInt() % 12);
                        byte[] byArray = this.aK[n5 - 1];
                        boolean bl = false;
                        for (int i = 0; i < byArray.length; ++i) {
                            if (byArray[i] != n6) continue;
                            bl = true;
                        }
                        if (n5 == 12 && this.aD <= 1) {
                            bl = false;
                        }
                        if (n5 == 3 && this.loanCeiling >= 1000000) {
                            bl = false;
                        }
                        if (n5 == 4 && this.era >= 5) {
                            bl = false;
                        }
                        if (this.er < this.a[n5 - 1][0]) {
                            bl = false;
                        }
                        if (this.er < this.dU[n5 - 1]) {
                            bl = false;
                        }
                        if (bl) continue;
                        n5 = 0;
                    }
                    int n13 = n5 - 1;
                    this.dU[n13] = (short)(this.dU[n13] + this.c(this.a[n5 - 1][1]));
                    if (n5 == 1) {
                        this.menuRowEra[n4] = (byte)(30 + Math.abs(this.rng.nextInt() % 50));
                        this.menuRowRevenue[n4] = this.menuRowCost[n4] * this.menuRowEra[n4] / 100;
                    }
                    if (n5 == 2) {
                        this.menuRowRevenue[n4] = 5000 * (6 + Math.abs(this.rng.nextInt() % 17));
                    }
                    if (n5 == 3) {
                        this.menuRowRevenue[n4] = 20000 * (3 + Math.abs(this.rng.nextInt() % 3));
                    }
                    if (n5 >= 5 && n5 <= 10) {
                        this.menuRowEra[n4] = (byte)(25 + Math.abs(this.rng.nextInt() % 45));
                    }
                    if (n5 == 11) {
                        this.menuRowEra[n4] = (byte)(15 + Math.abs(this.rng.nextInt() % 30));
                    }
                    this.menuRowAltType[n4] = (byte)n5;
                }
                if (this.menuRowType[n4] != 0) {
                    this.ready = false;
                    this.transitionCode = 40;
                    this.cZ = (byte)n4;
                    this.setState(20);
                    ++this.er;
                }
            }
            this.bv = n;
        }
    }

    void h(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.menuRowType[i] != 1) continue;
            int n2 = i;
            this.menuRowValue[n2] = this.menuRowValue[n2] + n;
        }
        this.bD += n;
        this.by += n / 73;
    }

    void t(int n, int n2) {
        for (int i = 0; i < 5; ++i) {
            if (this.menuRowType[i] != 3 + n) continue;
            int n3 = i;
            this.menuRowValue[n3] = this.menuRowValue[n3] + n2;
        }
        switch (n) {
            case 1: {
                this.milestoneThreshold1 += n2;
                break;
            }
            case 2: {
                this.milestoneThreshold2 += n2;
                break;
            }
            case 3: {
                this.milestoneThreshold3 += n2;
                break;
            }
            case 4: {
                this.milestoneThreshold4 += n2;
                break;
            }
            case 5: {
                this.milestoneThreshold5 += n2;
            }
        }
    }

    void w(int n) {
        for (int i = 0; i < 5; ++i) {
            if (n == 6 && this.menuRowType[i] == 9) {
                int n2 = i;
                this.menuRowValue[n2] = this.menuRowValue[n2] + 1;
            }
            if (n == 4 && this.menuRowType[i] == 10) {
                int n3 = i;
                this.menuRowValue[n3] = this.menuRowValue[n3] + 1;
            }
            if (n == 3 && this.menuRowType[i] == 11) {
                int n4 = i;
                this.menuRowValue[n4] = this.menuRowValue[n4] + 1;
            }
            if (n != 2 || this.menuRowType[i] != 12) continue;
            int n5 = i;
            this.menuRowValue[n5] = this.menuRowValue[n5] + 1;
        }
    }

    void decCountsForOption(int n) {
        for (int i = 0; i < 5; ++i) {
            if (n == 7 && this.menuRowType[i] == 9) {
                int n2 = i;
                this.menuRowValue[n2] = this.menuRowValue[n2] - 1;
            }
            if (n == 6 && this.menuRowType[i] == 9) {
                int n3 = i;
                this.menuRowValue[n3] = this.menuRowValue[n3] - 1;
            }
            if (n == 5 && this.menuRowType[i] == 10) {
                int n4 = i;
                this.menuRowValue[n4] = this.menuRowValue[n4] - 1;
            }
            if (n == 4 && this.menuRowType[i] == 10) {
                int n5 = i;
                this.menuRowValue[n5] = this.menuRowValue[n5] - 1;
            }
            if (n == 3 && this.menuRowType[i] == 11) {
                int n6 = i;
                this.menuRowValue[n6] = this.menuRowValue[n6] - 1;
            }
            if (n != 2 || this.menuRowType[i] != 12) continue;
            int n7 = i;
            this.menuRowValue[n7] = this.menuRowValue[n7] - 1;
        }
    }

    void x(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.menuRowType[i] != 13) continue;
            int n2 = i;
            this.menuRowValue[n2] = this.menuRowValue[n2] + n;
        }
    }

    void z(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.menuRowType[i] != 14) continue;
            int n2 = i;
            this.menuRowValue[n2] = this.menuRowValue[n2] + n;
        }
    }

    void loadMusic() {
        if (this.musicPlayer != null) {
            this.musicPlayer.deallocate();
            this.musicPlayer = null;
        }
        try {
            InputStream inputStream = this.getClass().getResourceAsStream("/6");
            this.musicPlayer = Manager.createPlayer((InputStream)inputStream, (String)"audio/midi");
            this.musicPlayer.prefetch();
            Control[] controlArray = this.musicPlayer.getControls();
            for (int i = 0; i < controlArray.length; ++i) {
                if (!(controlArray[i] instanceof VolumeControl)) continue;
                this.volumeControl = (VolumeControl)controlArray[i];
            }
            this.volumeControl.setLevel(20 * this.gh);
            this.musicPlaying = false;
            this.ay = true;
        }
        catch (Exception exception) {
            this.ay = false;
        }
    }

    void playMusic() {
        try {
            if (this.ay && this.musicPlayer != null && !this.musicPlaying && this.ac) {
                this.musicPlayer.prefetch();
                this.musicPlayer.setMediaTime(0L);
                this.musicPlayer.setLoopCount(-1);
                this.musicPlayer.start();
                this.musicPlaying = true;
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    void C(int n) {
        try {
            if (this.volumeControl != null) {
                this.volumeControl.setLevel(n * 20);
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    void stopMusic() {
        try {
            if (this.ay && this.musicPlayer != null && this.musicPlaying) {
                this.musicPlayer.stop();
                this.musicPlaying = false;
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    int c(int n) {
        int n2 = this.by;
        for (int i = 0; i < this.mapWidth; ++i) {
            for (int j = 0; j < this.mapHeight; ++j) {
                if (this.trackMap[i][j] != 65) continue;
                n2 += 3;
                for (int k = 0; k < 500; ++k) {
                    if ((this.trainFlags[k] & 0x80) != 128 || this.c(this.trainX[k]) != i || this.c(this.trainY[k]) != j) continue;
                    if ((this.trainFlags[k] & 0x40) == 64) {
                        n2 += 400;
                        continue;
                    }
                    n2 += 55;
                }
            }
        }
        n2 += this.dayOfMonth + 30 * this.monthOfYear + 365 * (this.year - 1950);
        if (n < 0) {
            if (n2 > this.H) {
                this.H = n2;
            }
        } else {
            this.gk[n] = n2;
        }
        this.O();
        return n2;
    }

    String decodeString(byte[] byArray) {
        byte[] byArray2 = new byte[byArray.length / 2];
        for (int i = 0; i < byArray2.length; ++i) {
            byArray2[i] = (byte)((byArray[2 * i + 1] ^ 0x57) + byArray[2 * i]);
        }
        return new String(byArray2);
    }

    String n() {
        StringBuffer stringBuffer = new StringBuffer();
        for (int i = 0; i < this.bE.length; ++i) {
            byte by = this.bE[i];
            if (by == 0) {
                stringBuffer.append(' ');
            }
            if (by >= 1 && by <= 10) {
                stringBuffer.append((char)(by + 47));
            }
            if (by == 11) {
                stringBuffer.append('.');
            }
            if (by == 12) {
                stringBuffer.append(',');
            }
            if (by == 13) {
                stringBuffer.append('?');
            }
            if (by == 14) {
                stringBuffer.append('!');
            }
            if (by == 15) {
                stringBuffer.append(';');
            }
            if (by == 16) {
                stringBuffer.append('(');
            }
            if (by == 17) {
                stringBuffer.append(')');
            }
            if (by >= 40 && by <= 65) {
                stringBuffer.append((char)(by + 57));
            }
            if (by < 70 || by > 95) continue;
            stringBuffer.append((char)(by - 5));
        }
        return stringBuffer.toString();
    }

    boolean syncHighScores(boolean bl) {
        try {
            int n;
            int n2;
            HttpConnection httpConnection = (HttpConnection)Connector.open((String)this.decodeString(ec));
            httpConnection.setRequestMethod("GET");
            httpConnection.setRequestProperty(this.decodeString(dO), this.decodeString(i));
            httpConnection.setRequestProperty(this.decodeString(el), this.decodeString(viewportZoomStep));
            httpConnection.setRequestProperty(this.decodeString(fz), "en");
            httpConnection.setRequestProperty(this.decodeString(dM), this.n());
            httpConnection.setRequestProperty(this.decodeString(cQ), bl ? "S" : "D");
            StringBuffer stringBuffer = new StringBuffer(String.valueOf(this.H));
            for (n2 = 0; n2 < this.gk.length; ++n2) {
                stringBuffer.append(",");
                stringBuffer.append(this.gk[n2]);
            }
            n2 = 0;
            int n3 = 0;
            int n4 = 0;
            String string = stringBuffer.toString();
            httpConnection.setRequestProperty(this.decodeString(l), string);
            for (int i = 0; i < string.length(); ++i) {
                n2 += string.charAt(i);
                if (i > 0) {
                    n3 += string.charAt(i) ^ string.charAt(i - 1);
                }
                n4 += (i + 1) * string.charAt(i);
            }
            httpConnection.setRequestProperty(this.decodeString(ag), String.valueOf(n2 += n3 + n4));
            if (httpConnection.getResponseCode() != 200) {
                return false;
            }
            InputStream inputStream = httpConnection.openInputStream();
            int n5 = -1;
            StringBuffer stringBuffer2 = new StringBuffer();
            int n6 = 0;
            int n7 = 0;
            while ((n = inputStream.read()) != -1) {
                if (n < 10) {
                    n5 = n;
                    break;
                }
                if (n >= 48 && n <= 57) {
                    n6 *= 10;
                    n6 += n - 48;
                }
                if (n != 124) continue;
                this.bd[n7] = n6;
                n6 = 0;
                ++n7;
            }
            inputStream.close();
            httpConnection.close();
            if (n5 != 0) {
                return false;
            }
        }
        catch (Exception exception) {
            return false;
        }
        this.O();
        return true;
    }

    private void a(ByteArrayOutputStream byteArrayOutputStream, ByteArrayOutputStream byteArrayOutputStream2, int n, int n2) {
        byteArrayOutputStream.write(n);
        byteArrayOutputStream2.write(n2);
    }

    public void k(int n, int n2) {
        try {
            int n3;
            int n4;
            int n5;
            ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
            ByteArrayOutputStream byteArrayOutputStream2 = new ByteArrayOutputStream();
            n2 += n2 % 8;
            int n6 = (n2 -= 30) / 8 + 6;
            int n7 = n / 32 + 2;
            for (n5 = 0; n5 < n6; ++n5) {
                for (n4 = -1; n4 < n7; ++n4) {
                    n3 = n4 - n5 / 2;
                    int n8 = n5 % 2 == 0 ? 0 + n4 + n5 / 2 : -1 + n4 + n5 / 2;
                    this.a(byteArrayOutputStream, byteArrayOutputStream2, n3, n8);
                }
            }
            this.tileOffsetX = byteArrayOutputStream.toByteArray();
            this.tileOffsetY = byteArrayOutputStream2.toByteArray();
            this.numActiveTileAnimations = this.tileOffsetX.length;
            byteArrayOutputStream.close();
            byteArrayOutputStream2.close();
            do {
                n5 = 0;
                for (n4 = 0; n4 < this.numActiveTileAnimations - 1; ++n4) {
                    if (this.tileOffsetY[n4] <= this.tileOffsetY[n4 + 1]) continue;
                    n3 = this.tileOffsetY[n4];
                    this.tileOffsetY[n4] = this.tileOffsetY[n4 + 1];
                    this.tileOffsetY[n4 + 1] = n3;
                    n3 = this.tileOffsetX[n4];
                    this.tileOffsetX[n4] = this.tileOffsetX[n4 + 1];
                    this.tileOffsetX[n4 + 1] = n3;
                    n5 = 1;
                }
            } while (n5 != 0);
            do {
                n5 = 0;
                for (n4 = 1; n4 < this.numActiveTileAnimations; ++n4) {
                    if (this.tileOffsetX[n4] <= this.tileOffsetX[n4 - 1] || this.tileOffsetY[n4] != this.tileOffsetY[n4 - 1]) continue;
                    n3 = this.tileOffsetY[n4];
                    this.tileOffsetY[n4] = this.tileOffsetY[n4 - 1];
                    this.tileOffsetY[n4 - 1] = n3;
                    n3 = this.tileOffsetX[n4];
                    this.tileOffsetX[n4] = this.tileOffsetX[n4 - 1];
                    this.tileOffsetX[n4 - 1] = n3;
                    n5 = 1;
                }
            } while (n5 != 0);
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    void a(Graphics graphics, int n, int n2, int n3, int n4, byte[] byArray) {
        int n5 = 0;
        for (int i = 0; i < byArray.length; ++i) {
            if (byArray[i] >= 32 && byArray[i] <= 39) continue;
            ++n5;
        }
        n5 *= 6;
        if ((n4 & 1) == 1) {
            n -= n5 / 2 + 3;
        }
        if ((n4 & 2) == 2) {
            n2 -= 5;
        }
        if ((n4 & 4) == 4) {
            n -= n5 + 6;
        }
        if (n4 == 37) {
            n = 0;
            n2 = 15 + eN - 15;
        }
        graphics.setColor(this.dc[3 * n3]);
        graphics.fillRect(n, n2, n5 + 5, 13);
        graphics.setColor(this.dc[3 * n3 + 1]);
        graphics.drawRect(n, n2, n5 + 5, 13);
        graphics.setColor(this.dc[3 * n3 + 2]);
        graphics.drawRect(n, n2 + 13, n5 + 5, 0);
        graphics.drawRect(n + n5 + 5, n2, 0, 13);
        this.a(graphics, n + 4, n2 + 5, byArray, false);
    }

    void enterOutOfMoneyState() {
        this.ready = false;
        this.transitionCode = 11;
        this.setState(20);
    }

    void a(Graphics graphics, int n, int n2, byte by) {
        byte[] byArray = this.n[this.ds[80 + (this.di[by] >> 8 & 0xFF)]];
        byte[] byArray2 = this.n[this.ds[86 + (this.di[by] & 0xFF)]];
        byte[] byArray3 = new byte[byArray2.length + byArray.length];
        System.arraycopy(byArray2, 0, byArray3, 0, byArray2.length);
        System.arraycopy(byArray, 0, byArray3, byArray2.length, byArray.length);
        int n3 = 0;
        for (int i = 0; i < byArray3.length; ++i) {
            if (byArray3[i] >= 32 && byArray3[i] <= 39) continue;
            ++n3;
        }
        graphics.setColor(255, 176, 98);
        graphics.fillRect(n -= (n3 *= 6) / 2 + 3, n2 -= 5, n3 + 5, 11);
        graphics.setColor(255, 210, 170);
        graphics.drawRect(n, n2, n3 + 5, 11);
        graphics.setColor(255, 130, 0);
        graphics.drawRect(n, n2 + 11, n3 + 5, 0);
        graphics.drawRect(n + n3 + 5, n2, 0, 11);
        this.clipX = 0;
        this.clipY = 15;
        this.clipWidth = e;
        this.clipHeight = eN;
        this.a(graphics, n + 4, n2 + 3, byArray3, true);
    }

    void a(int n, int n2) {
        if (this.eD != n || this.aJ != n2) {
            e = n;
            hs = n2;
            this.backbuffer = Image.createImage((int)e, (int)hs);
            this.backbufferG = this.backbuffer.getGraphics();
            eN = hs - 30;
            fX = hs - 15;
            this.k(n, n2);
            this.eD = n;
            this.aJ = n2;
            this.menuDirty = true;
            this.bH = -1;
            if (this.gameState == 20 && !this.ready) {
                if (this.hx == 30) {
                    this.pendingTransitionCode = this.transitionCode;
                    this.gameState = 30;
                }
                if (this.hx == 40) {
                    this.pendingTransitionCode = this.transitionCode;
                    this.gameState = 40;
                }
            }
        }
    }

    public void paint(Graphics graphics) {
        this.setFullScreenMode(true);
        int n = this.getWidth();
        int n2 = this.getHeight();
        if (!this.ce) {
            this.a(n, n2);
            if (this.gameState != 40 && this.gameState != 35 && this.gameState != 45 && this.gameState != 55 && this.gameState != 65 && this.gameState != 105 && this.gameState != 120 && (this.gameState != 20 || this.ready)) {
                graphics.setColor(0);
                graphics.fillRect(0, 0, n, n2);
            }
            graphics.setClip(0, 0, e, hs);
            switch (this.gameState) {
                case 0: 
                case 1: 
                case 2: 
                case 3: {
                    graphics.setColor(0xFFFFFF);
                    if (this.gameState == 0) {
                        graphics.setColor(0);
                    }
                    graphics.fillRect(0, 0, e, hs);
                    int n3 = e / 2;
                    int n4 = hs / 2;
                    int n5 = this.P.getWidth() / 2;
                    int n6 = this.P.getHeight();
                    int n7 = n6 / 10 + 1;
                    for (int i = 0; i < 10; ++i) {
                        int n8 = (n3 + n5 + 30) * (this.loadingTimer - 5 * (i / 2)) / 10;
                        if (n8 < 0) {
                            n8 = 0;
                        }
                        if (n8 > n3 + n5 + 30) {
                            n8 = n3 + n5 + 30;
                        }
                        int n9 = n3 - (n3 + n5 + 30 - n8);
                        if (i % 2 == 1) {
                            n9 = n3 + (n3 + n5 + 30 - n8);
                        }
                        graphics.setClip(n9 - n5, n4 - n6 / 2 + i * n7, n5 * 2, n7);
                        graphics.drawImage(this.P, n9, n4, 3);
                    }
                    graphics.setClip(0, 0, e, hs);
                    break;
                }
                case 10: {
                    graphics.setColor(0xFFFFFF);
                    graphics.fillRect(0, 0, e, hs);
                    graphics.drawImage(this.P, e / 2, hs / 2, 3);
                    graphics.setColor(215, 222, 238);
                    graphics.fillRect(10, hs - 13, e - 21, 9);
                    int n10 = this.eW * (e - 20) / (this.al - 1);
                    graphics.setColor(60, 105, 144);
                    graphics.fillRect(10, hs - 13, n10, 9);
                    graphics.setColor(49, 64, 90);
                    graphics.drawRect(10, hs - 13, e - 21, 9);
                    break;
                }
                case 20: {
                    if (this.ready) {
                        graphics.setColor(0xFFFFFF);
                        graphics.fillRect(0, 0, e, hs);
                        if (this.P != null) {
                            graphics.drawImage(this.P, e / 2, hs / 2, 3);
                        }
                    }
                    if (this.transitionCode == 36) {
                        this.gameState = 40;
                        this.paint(graphics);
                        this.gameState = 20;
                    }
                    this.a(graphics, this.transitionCode);
                    break;
                }
                case 25: {
                    graphics.setColor(0);
                    graphics.fillRect(0, 0, e, hs);
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[0]]);
                    graphics.setColor(215, 222, 238);
                    graphics.fillRect(10, hs - 23, e - 21, 9);
                    int n11 = this.totalTilesToGenerate < 2 ? 2 : this.totalTilesToGenerate;
                    int n12 = this.mapGenTilesDone * (e - 20) / (n11 - 1);
                    graphics.setColor(60, 105, 144);
                    graphics.fillRect(10, hs - 23, n12, 9);
                    graphics.setColor(0xFFFFFF);
                    graphics.drawRect(10, hs - 23, e - 21, 9);
                    break;
                }
                case 30: {
                    this.tickStart = System.currentTimeMillis();
                    this.renderMap(graphics);
                    this.tickEnd = System.currentTimeMillis();
                    if (this.inputPending) {
                        int n13;
                        graphics.setColor(0);
                        graphics.setClip(0, 15, e, fX - 15);
                        for (n13 = 0; n13 < e; ++n13) {
                            graphics.drawLine(n13 * 2, 0, hs + n13 * 2, hs);
                        }
                        for (n13 = 0; n13 < hs; ++n13) {
                            graphics.drawLine(0, n13 * 2, e, e + n13 * 2);
                        }
                        graphics.setClip(0, 0, e, hs);
                        this.a(graphics, 234, 0, 15, 20);
                    }
                    this.g(graphics);
                    if (this.pauseFlag) {
                        this.a(graphics, 236, 0, 15, 0);
                    }
                    if (this.pendingTransitionCode > -1) {
                        this.transitionCode = this.pendingTransitionCode;
                        this.o();
                        this.pendingTransitionCode = -1;
                        this.a(graphics, this.transitionCode);
                        this.setState(20);
                    }
                    if (this.fZ <= 0) break;
                    this.fZ = (byte)(this.fZ - 1);
                    if (this.fZ != 0) break;
                    this.transitionCode = 80 + this.ff;
                    this.o();
                    this.ready = false;
                    this.a(graphics, this.transitionCode);
                    this.setState(20);
                    break;
                }
                case 35: {
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[50]]);
                    break;
                }
                case 40: {
                    int n14;
                    graphics.setClip(0, 15, e, eN);
                    graphics.setColor(0);
                    graphics.fillRect(0, 15, e, eN);
                    int n15 = this.viewportCenterX / 16;
                    int n16 = this.viewportCenterY / 8;
                    int n17 = this.mapWidth + this.mapHeight;
                    int n18 = e / 2 - n15 - e / 32;
                    if (e - n18 > n17) {
                        n18 = e - n17;
                    }
                    if (n18 > 0) {
                        n18 = 0;
                    }
                    if (eN - (n14 = eN / 2 - n16 - eN / 16) > n17) {
                        n14 = eN - n17;
                    }
                    if (n14 > 0) {
                        n14 = 0;
                    }
                    int n19 = n17 < e ? (e - n17) / 2 : 0;
                    int n20 = n17 < eN ? (eN - n17) / 2 : 0;
                    graphics.drawImage(this.backbuffer, n18 + n19, 15 + n14 + n20, 20);
                    int n21 = this.viewportCenterX - 2 * this.viewportCenterY + 16 + 2 * this.hj;
                    int n22 = this.viewportCenterX + 2 * this.viewportCenterY - 16 - 2 * this.hj;
                    int n23 = this.mapWidth - (n21 /= 32) + (n22 /= 32);
                    int n24 = n21 + n22;
                    graphics.setColor(0xFFFFFF);
                    graphics.drawRect(n24 + n18 + n19, 15 + n23 + n14 + n20, e / 16, eN / 8);
                    graphics.setClip(0, 0, e, hs);
                    this.b(graphics);
                    this.d(graphics);
                    if (this.ee) {
                        this.ready = false;
                        this.transitionCode = 36;
                        this.setState(20);
                        this.ee = false;
                    }
                    if (this.pendingTransitionCode <= -1) break;
                    this.transitionCode = this.pendingTransitionCode;
                    this.o();
                    this.pendingTransitionCode = -1;
                    this.a(graphics, this.transitionCode);
                    this.setState(20);
                    break;
                }
                case 45: {
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[51]]);
                    break;
                }
                case 55: {
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[52]]);
                    break;
                }
                case 65: {
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[53]]);
                    break;
                }
                case 105: {
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[49]]);
                    break;
                }
                case 120: {
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[66]]);
                }
            }
        }
    }

    public void setState(int n) {
        this.hx = this.gameState;
        if (!this.ready) {
            this.P = null;
        }
        this.S = false;
        this.menuDirty = true;
        switch (n) {
            case 0: {
                this.a("/1");
                this.b("/4");
                this.loadingTimer = 0;
            }
            case 1: 
            case 2: {
                this.P = this.aa();
                this.loadingStartTime = System.currentTimeMillis();
                this.loadingTimeout = false;
                this.loadingTimer = 0;
                break;
            }
            case 4: {
                try {
                    XorResourceInput xorResourceInput = new XorResourceInput("/3");
                    int n2 = xorResourceInput.readByte();
                    int n3 = xorResourceInput.readByte();
                    this.n = new byte[n2][];
                    this.ds = new byte[n3 + 1];
                    for (int i = 0; i < n2; ++i) {
                        int n4 = xorResourceInput.readByte();
                        int n5 = xorResourceInput.readByte();
                        this.n[i] = new byte[n5];
                        this.ds[n4] = (byte)i;
                        for (int j = 0; j < n5; ++j) {
                            this.n[i][j] = (byte)xorResourceInput.readByte();
                        }
                    }
                    xorResourceInput.close();
                }
                catch (Exception exception) {}
                break;
            }
            case 10: {
                this.eW = 0;
                try {
                    XorResourceInput xorResourceInput = new XorResourceInput("/2");
                    this.al = xorResourceInput.readByte();
                    for (int i = 0; i < this.al; ++i) {
                        int n6 = xorResourceInput.readByte();
                        this.tileSpriteIndex[n6] = (byte)(i - 8);
                    }
                    xorResourceInput.close();
                }
                catch (Exception exception) {
                    // empty catch block
                }
                this.al -= 8;
                this.tileSprites = new Image[this.al];
                this.tileSpriteIndex[68] = this.tileSpriteIndex[66];
                break;
            }
            case 20: {
                this.o();
                if (!this.bC) {
                    this.cursorDragY = 2;
                }
                this.bC = true;
                break;
            }
            case 25: {
                this.loadStep = 0;
                break;
            }
            case 30: {
                this.backbuffer = null;
                System.gc();
                this.backbuffer = Image.createImage((int)e, (int)hs);
                this.backbufferG = this.backbuffer.getGraphics();
                this.bH = -1;
                this.menuDirty = true;
                break;
            }
            case 35: {
                this.loadStep = 4;
                this.confirmToggle = 0;
                break;
            }
            case 45: {
                this.loadStep = 4;
                break;
            }
            case 55: {
                this.loadStep = 4;
                break;
            }
            case 65: {
                this.loadStep = 4;
                break;
            }
            case 105: {
                this.loadStep = 4;
                break;
            }
            case 120: {
                this.loadStep = 4;
            }
        }
        this.gameState = n;
    }

    public void update() {
        switch (this.gameState) {
            case 0: 
            case 1: 
            case 2: {
                if (System.currentTimeMillis() - this.loadingStartTime > 5000L) {
                    this.loadingTimeout = true;
                }
                ++this.loadingTimer;
                if (!this.loadingTimeout) break;
                if (this.gameState == 1) {
                    for (int i = 0; i < 6; ++i) {
                        this.ca[i] = this.aa();
                    }
                    this.setState(4);
                    break;
                }
                this.setState(this.gameState + 1);
                break;
            }
            case 4: {
                this.ready = true;
                this.screenParam = 3;
                this.transitionCode = 14;
                this.o();
                this.setState(20);
                break;
            }
            case 10: {
                this.tileSprites[this.eW] = this.aa();
                ++this.eW;
                if (this.eW == this.al / 2) {
                    this.loadMusic();
                    if (this.musicAutoStart) {
                        this.playMusic();
                        this.ac = true;
                    } else {
                        this.ac = false;
                    }
                }
                if (this.eW != this.al) break;
                this.ready = true;
                this.transitionCode = 1;
                this.setState(20);
                break;
            }
            case 25: {
                ++this.loadStep;
                if (this.loadStep == 2) {
                    this.g(2);
                    this.dayOfMonth = 1;
                    this.monthOfYear = 1;
                    this.year = 1950;
                    this.milestonesEnabled = false;
                    this.pauseFlag = false;
                    this.dP = false;
                    this.moneyBaseA = 974;
                    this.moneyBaseB = 974;
                    this.baseTrainSpeed = 173;
                    this.accel1 = 342;
                    this.brake1 = 311;
                    this.baseTrainSpeed2 = 173;
                    this.accel2 = 234;
                    this.costPower = 4759;
                    this.costStruct = 9845;
                    this.costOverhead = 347;
                    this.costMaintenance = 5982;
                    this.costCrew = 481;
                    this.gWagonCost = 37894;
                    this.gWagonCost2 = 43247;
                    this.gWagonCost3 = 51872;
                    this.gWagonCost4 = 60149;
                    this.gWagonCost5 = 71423;
                    this.gWagonCost6 = 82675;
                    this.costGroupC1 = 4578;
                    this.costGroupC2 = 5196;
                    this.costGroupC3 = 4314;
                    this.costGroupC4 = 5072;
                    try {
                        XorResourceInput xorResourceInput = new XorResourceInput("/5");
                        int n = xorResourceInput.readByte();
                        for (int i = 0; i < n * 36; ++i) {
                            int n2 = xorResourceInput.readByte();
                            int n3 = xorResourceInput.readByte();
                            int n4 = xorResourceInput.readByte();
                            int n5 = xorResourceInput.readByte();
                            int n6 = xorResourceInput.readByte() + 256 * xorResourceInput.readByte();
                            int n7 = xorResourceInput.readByte();
                            int n8 = xorResourceInput.readByte();
                            int n9 = xorResourceInput.readByte();
                            int n10 = xorResourceInput.readByte();
                            int n11 = xorResourceInput.readByte();
                            int n12 = xorResourceInput.readByte();
                            int n13 = xorResourceInput.readByte();
                            int n14 = xorResourceInput.readByte();
                            int n15 = xorResourceInput.readByte();
                            int n16 = xorResourceInput.readByte() + 256 * xorResourceInput.readByte() + 65536 * xorResourceInput.readByte();
                            int n17 = xorResourceInput.readByte() + 256 * xorResourceInput.readByte() + 65536 * xorResourceInput.readByte();
                            int n18 = xorResourceInput.readByte() + 256 * xorResourceInput.readByte() + 65536 * xorResourceInput.readByte();
                            int n19 = xorResourceInput.readByte();
                            int n20 = xorResourceInput.readByte();
                            int n21 = xorResourceInput.readByte();
                            int n22 = xorResourceInput.readByte();
                            int n23 = xorResourceInput.readByte() + 256 * xorResourceInput.readByte() + 65536 * xorResourceInput.readByte();
                            int n24 = xorResourceInput.readByte() + 256 * xorResourceInput.readByte() + 65536 * xorResourceInput.readByte();
                            int n25 = xorResourceInput.readByte();
                            int n26 = xorResourceInput.readByte();
                            if (n2 != this.J) continue;
                            this.scenarioStep = n2;
                            this.mapWidth = this.mapHeight = n3;
                            this.hj = (this.mapWidth - 1) * 8;
                            this.mapScreenWidth = (this.mapWidth + this.mapHeight) * 16;
                            this.mapScreenHeight = (this.mapWidth + this.mapHeight) * 8;
                            this.gU = this.mapScreenWidth / 2;
                            this.fO = this.mapScreenHeight / 2;
                            this.viewportCenterX = this.gU;
                            this.viewportCenterY = this.fO;
                            this.viewportBaseX = this.viewportCenterX;
                            this.viewportBaseY = this.viewportCenterY;
                            this.mapScaleX = this.viewportBaseX * 2;
                            this.mapScaleY = this.viewportBaseY * 2;
                            this.dQ = n6;
                            this.hf = n4;
                            this.archiveOrRecallMethod = n5;
                            this.cK = n7;
                            this.gG = n8;
                            this.co = n9;
                            this.dK = n10;
                            this.bX = n11;
                            this.weightAccel = n12;
                            this.weightAccel2 = n13;
                            this.weightAccel3 = n14;
                            this.weightAccel4 = n15;
                            this.money = n16;
                            this.loanCeiling = n17;
                            this.loanBalance = n18;
                            this.aD = (byte)n19;
                            this.era = n20 - 1;
                            this.scenarioMilestoneIndex = (byte)n22;
                            this.currentMilestoneScore = n23;
                            this.milestoneCoThreshold2 = n24;
                            this.v(n25);
                            this.flagsA = (n26 & 2) == 2;
                            this.flagsB = (n26 & 4) == 4;
                            if ((n26 & 1) != 1) {
                                this.cd = (byte)25;
                                this.fP = (byte)100;
                                System.out.println("AI OFF");
                            } else {
                                this.cd = (byte)32;
                                this.fP = (byte)29;
                            }
                            this.ao = n21;
                            if (n21 != 0) {
                                this.fZ = (byte)5;
                                this.ff = (byte)n21;
                                continue;
                            }
                            this.fZ = 0;
                        }
                        xorResourceInput.close();
                    }
                    catch (Exception exception) {
                        // empty catch block
                    }
                    if (this.J <= 1) {
                        if (this.difficultySlot == 0) {
                            this.cd = (byte)32;
                            this.fP = (byte)29;
                        }
                        if (this.difficultySlot == 1) {
                            this.cd = (byte)25;
                            this.fP = (byte)22;
                        }
                        if (this.difficultyA == 0) {
                            this.aD = (byte)4;
                            this.eC = 2;
                        }
                        if (this.difficultyA == 1) {
                            this.aD = (byte)8;
                            this.eC = 5;
                        }
                        if (this.difficultyB == 0) {
                            this.gG /= 2;
                            this.co /= 2;
                        }
                        if (this.difficultyC == 0) {
                            this.dK /= 2;
                            this.bX /= 2;
                            this.weightAccel /= 2;
                            this.weightAccel2 /= 2;
                            this.weightAccel3 /= 2;
                            this.weightAccel4 /= 2;
                        }
                    }
                }
                if (this.loadStep == 3) {
                    this.buildMenuIndex = 0;
                    this.buildMenuMode = 0;
                    this.loadState25Setup(0);
                }
                if (this.loadStep <= 3) break;
                this.loadState25Setup(1);
                if (this.mapGenTilesDone != this.totalTilesToGenerate) break;
                this.mapGenTilesDone = 0;
                this.setState(30);
                break;
            }
            case 30: {
                int n;
                int n27;
                this.tickStart = System.currentTimeMillis();
                ++this.timerA1;
                if (this.timerA1 == 2) {
                    this.timerA1 = 0;
                    ++this.timerA2;
                    if (this.timerA2 == 3) {
                        this.timerA2 = 0;
                    }
                }
                ++this.timerB1;
                if (this.timerB1 == 2) {
                    this.timerB1 = 0;
                    ++this.timerB2;
                    if (this.timerB2 == 3) {
                        this.timerB2 = 0;
                    }
                }
                ++this.timerC1;
                if (this.timerC1 == 3) {
                    this.timerC1 = 0;
                    ++this.timerC2;
                    if (this.timerC2 == 3) {
                        this.timerC2 = 0;
                    }
                }
                ++this.woodAgeTimer;
                if (this.woodAgeTimer == 150) {
                    for (n27 = 0; n27 < this.mapWidth; ++n27) {
                        for (n = 0; n < this.mapHeight; ++n) {
                            if (this.tileMap[n27][n] < 59 || this.tileMap[n27][n] > 61 || this.rng.nextInt() % 3 != 0) continue;
                            byte[] byArray = this.tileMap[n27];
                            int n28 = n;
                            byArray[n28] = (byte)(byArray[n28] + 1);
                            if (this.tileMap[n27][n] != 62) continue;
                            this.tileMap[n27][n] = 59;
                        }
                    }
                    this.woodAgeTimer = 0;
                }
                ++this.phaseTimer;
                if (this.phaseTimer == 10) {
                    this.phaseTimer = 0;
                }
                for (n27 = 0; n27 < 50; ++n27) {
                    int n29;
                    if (this.tileQueue[n27][0] <= 0) continue;
                    byte[] byArray = this.tileQueue[n27];
                    byArray[0] = (byte)(byArray[0] - 1);
                    if (this.tileQueue[n27][0] == 0) {
                        n = this.c(this.tileQueue[n27][1]);
                        n29 = this.c(this.tileQueue[n27][2]);
                        if (n >= 0 && n29 >= 0 && n < this.mapWidth && n29 < this.mapHeight) {
                            this.tileMap[n][n29] = (byte)(this.c(this.tileMap[n][n29]) - 128);
                        }
                    }
                    if (this.tileQueue[n27][0] != 7) continue;
                    n = this.c(this.tileQueue[n27][1]);
                    n29 = this.c(this.tileQueue[n27][2]);
                    if (n < 0 || n29 < 0 || n >= this.mapWidth || n29 >= this.mapHeight) continue;
                    int n30 = this.c(this.tileMap[n][n29]) - 128;
                    if (this.go[n27]) continue;
                    if (n30 >= 29 && n30 <= 33) {
                        this.tileMap[n][n29] = -128;
                    }
                    if (n30 >= 70 && n30 <= 75 || n30 >= 87 && n30 <= 90) {
                        this.trackMap[n][n29] = 0;
                        this.tileMap[n][n29] = -128;
                        this.bH = -1;
                    }
                    if (n30 >= 77 && n30 <= 80) {
                        this.tileMap[n][n29] = (byte)(128 + n30 - 56);
                        this.trackMap[n][n29] = 0;
                        this.bH = -1;
                    }
                    if (n30 == 81 || n30 == 82) {
                        this.tileMap[n][n29] = -128;
                        this.trackMap[n][n29] = 0;
                        this.bH = -1;
                    }
                    if (n30 >= 83 && n30 <= 86) {
                        this.tileMap[n][n29] = (byte)(128 + n30 - 76);
                        this.trackMap[n][n29] = 0;
                        this.bH = -1;
                    }
                    if (n30 != 76) continue;
                    int n31 = this.trackMap[n][n29] & 7;
                    int n32 = (this.trackMap[n][n29] & 0x3F) >> 3;
                    if (n31 == 1 && n32 == 1) {
                        this.tileMap[n][n29] = -128;
                        this.trackMap[n][n29] = 0;
                    } else if (n31 == 1) {
                        this.tileMap[n][n29] = -57;
                        this.trackMap[n][n29] = (byte)(0x40 | n32);
                    } else if (n32 == 1) {
                        this.tileMap[n][n29] = -58;
                        this.trackMap[n][n29] = (byte)(0x40 | n31);
                    }
                    this.bH = -1;
                }
                for (n27 = 0; n27 < 500; ++n27) {
                    if ((this.trainFlags[n27] & 0x80) != 128 || (this.trainFlags[n27] & 0x40) != 64) continue;
                    if ((this.trainFlags[n27] & 0x20) == 32) {
                        if (this.trainEnergyOrCount[n27] > 0) {
                            int n33 = n27;
                            this.trainEnergyOrCount[n33] = (byte)(this.trainEnergyOrCount[n33] - 1);
                            continue;
                        }
                        this.trainPhase[n27] = 0;
                        this.trainFlags[n27] = 0;
                        continue;
                    }
                    this.updateTrain(n27);
                    this.updateTrain(n27);
                    if (this.trainPhase[n27] != 0) continue;
                    this.i(n27);
                }
                if (this.bI >= this.dH) {
                    this.l();
                    if (this.gg > 0) {
                        --this.gg;
                    }
                    this.bI = 0;
                    ++this.dayOfMonth;
                    this.Q();
                    if (this.dayOfMonth % 4 == 0) {
                        this.g();
                    }
                    if (this.dayOfMonth % 2 == 0) {
                        if (this.dayOfMonth % 10 == 0) {
                            this.everyTenTick();
                        }
                        this.everyTwoTick();
                        this.everyTwoTickZ();
                        if (this.flagsA) {
                            n27 = 0;
                            for (n = 0; n < 30; ++n) {
                                n27 = n27 != 0 || this.v() ? 1 : 0;
                            }
                            if (n27 != 0) {
                                this.maybeAITick();
                            }
                        }
                        this.tickInputOrAi(false);
                    }
                    n27 = this.monthOfYear;
                    switch (this.monthOfYear) {
                        case 1: 
                        case 3: 
                        case 5: 
                        case 7: 
                        case 8: 
                        case 10: 
                        case 12: {
                            if (this.dayOfMonth != 32) break;
                            ++this.monthOfYear;
                            break;
                        }
                        case 2: {
                            if ((this.year % 4 != 0 || this.dayOfMonth != 30) && (this.year % 4 == 0 || this.dayOfMonth != 29)) break;
                            ++this.monthOfYear;
                            break;
                        }
                        case 4: 
                        case 6: 
                        case 9: 
                        case 11: {
                            if (this.dayOfMonth != 31) break;
                            ++this.monthOfYear;
                        }
                    }
                    if (n27 != this.monthOfYear) {
                        this.dayOfMonth = 1;
                        this.newYearNotify();
                        this.dayResetP();
                        this.dayResetAg();
                        this.dayResetAd();
                        n = Math.abs(this.rng.nextInt() % 7);
                        switch (n) {
                            case 0: {
                                this.moneyBaseA += 9;
                                this.moneyBaseB += 9;
                                break;
                            }
                            case 1: {
                                ++this.baseTrainSpeed;
                                this.accel1 += 3;
                                this.brake1 += 3;
                                break;
                            }
                            case 2: {
                                ++this.baseTrainSpeed2;
                                this.accel2 += 2;
                                break;
                            }
                            case 3: {
                                this.costPower += 47;
                                this.costStruct += 98;
                                break;
                            }
                            case 4: {
                                this.costOverhead += 3;
                                this.costMaintenance += 59;
                                this.costCrew += 4;
                                break;
                            }
                            case 5: {
                                this.gWagonCost += 378;
                                this.gWagonCost2 += 432;
                                this.gWagonCost3 += 518;
                                this.gWagonCost4 += 601;
                                this.gWagonCost5 += 714;
                                this.gWagonCost6 += 826;
                                break;
                            }
                            case 6: {
                                this.costGroupC1 += 45;
                                this.costGroupC2 += 51;
                                this.costGroupC3 += 43;
                                this.costGroupC4 += 50;
                            }
                        }
                    }
                    if (this.monthOfYear == 13) {
                        this.revenueOrYear();
                        this.postRevenueReset();
                        this.monthOfYear = 1;
                        ++this.year;
                        if (this.money < 0) {
                            this.ready = false;
                            this.transitionCode = 7;
                            this.setState(20);
                        }
                    }
                    n = this.era;
                    if (this.dayOfMonth == 5 && this.monthOfYear == 3 && this.year == 1951 && this.era < 2) {
                        this.era = 2;
                    }
                    if (this.dayOfMonth == 17 && this.monthOfYear == 7 && this.year == 1953 && this.era < 3) {
                        this.era = 3;
                    }
                    if (this.dayOfMonth == 4 && this.monthOfYear == 12 && this.year == 1957 && this.era < 4) {
                        this.era = 4;
                    }
                    if (this.dayOfMonth == 26 && this.monthOfYear == 2 && this.year == 1962 && this.era < 5) {
                        this.era = 5;
                    }
                    if (n != this.era) {
                        this.ready = false;
                        this.transitionCode = 13;
                        this.setState(20);
                    }
                } else {
                    ++this.bI;
                }
                this.tickEnd = System.currentTimeMillis();
                break;
            }
            case 35: {
                if (this.loadStep > 0) {
                    --this.loadStep;
                }
                if (this.loadStep != 2) break;
                this.backbuffer = null;
                System.gc();
                int n = this.mapHeight + this.mapWidth;
                this.backbuffer = Image.createImage((int)n, (int)n);
                this.backbufferG = this.backbuffer.getGraphics();
                this.backbufferG.setColor(0);
                this.backbufferG.fillRect(0, 0, n, n);
                int n34 = -1;
                int n35 = 0;
                for (int i = 0; i < this.mapWidth; ++i) {
                    for (int j = 0; j < this.mapHeight; ++j) {
                        int n36 = this.mapWidth - i + j;
                        int n37 = i + j;
                        byte by = this.tileMap[i][j];
                        byte by2 = this.trackMap[i][j];
                        if (by == 0) {
                            n35 = 1;
                        }
                        if (by >= 3 && by <= 28) {
                            n35 = 2;
                        }
                        if (by >= 29 && by <= 33) {
                            n35 = 3;
                        }
                        if (by >= 34 && by <= 41) {
                            n35 = 4;
                        }
                        if (by >= 42 && by <= 48) {
                            n35 = 5;
                        }
                        if (by >= 50 && by <= 52) {
                            n35 = 6;
                        }
                        if (by >= 53 && by <= 55) {
                            n35 = 7;
                        }
                        if (by >= 56 && by <= 58) {
                            n35 = 8;
                        }
                        if (by >= 59 && by <= 61) {
                            n35 = 9;
                        }
                        if (by >= 62 && by <= 64) {
                            n35 = 10;
                        }
                        if (by >= 65 && by <= 68) {
                            n35 = 11;
                        }
                        if (by >= 70 && by <= 90) {
                            if (by2 == 64) {
                                n35 = 12;
                            }
                            if (by2 == 65) {
                                n35 = 12;
                            }
                            if (by2 == 66) {
                                n35 = 13;
                            }
                            if (by2 == 67) {
                                n35 = 14;
                            }
                            if (by2 == 68) {
                                n35 = 15;
                            }
                        }
                        if (n35 != n34) {
                            switch (n35) {
                                case 1: {
                                    if (this.tileElevation[i][j] == -1) {
                                        this.backbufferG.setColor(3167360);
                                    }
                                    if (this.tileElevation[i][j] == 0) {
                                        this.backbufferG.setColor(5274640);
                                    }
                                    if (this.tileElevation[i][j] == 1) {
                                        this.backbufferG.setColor(6327328);
                                    }
                                    if (this.tileElevation[i][j] == 2) {
                                        this.backbufferG.setColor(7380016);
                                    }
                                    if (this.tileElevation[i][j] != 3) break;
                                    this.backbufferG.setColor(8432704);
                                    break;
                                }
                                case 2: {
                                    this.backbufferG.setColor(6327312);
                                    break;
                                }
                                case 3: {
                                    this.backbufferG.setColor(0x909040);
                                    break;
                                }
                                case 4: {
                                    this.backbufferG.setColor(0x606060);
                                    break;
                                }
                                case 5: {
                                    this.backbufferG.setColor(0x404040);
                                    break;
                                }
                                case 6: {
                                    this.backbufferG.setColor(252, 0, 0);
                                    break;
                                }
                                case 7: {
                                    this.backbufferG.setColor(16, 16, 16);
                                    break;
                                }
                                case 8: {
                                    this.backbufferG.setColor(252, 156, 0);
                                    break;
                                }
                                case 9: {
                                    this.backbufferG.setColor(120, 220, 30);
                                    break;
                                }
                                case 10: {
                                    this.backbufferG.setColor(252, 252, 0);
                                    break;
                                }
                                case 11: {
                                    this.backbufferG.setColor(128, 196, 252);
                                    break;
                                }
                                case 12: {
                                    this.backbufferG.setColor(0);
                                    break;
                                }
                                case 13: {
                                    this.backbufferG.setColor(14287063);
                                    break;
                                }
                                case 14: {
                                    this.backbufferG.setColor(16738557);
                                    break;
                                }
                                case 15: {
                                    this.backbufferG.setColor(16040947);
                                }
                            }
                        }
                        n34 = n35;
                        this.backbufferG.drawLine(n37, n36, n37 + 1, n36);
                    }
                    n34 = -1;
                }
                this.setState(40);
                break;
            }
            case 40: {
                this.viewportPanKey();
                break;
            }
            case 45: {
                if (this.loadStep > 0) {
                    --this.loadStep;
                }
                if (this.loadStep != 2) break;
                if (this.K()) {
                    this.setState(30);
                    this.buildMenuIndex = 0;
                    this.aC = true;
                    this.buildMenuMode = 2;
                    this.menuDirty = true;
                    break;
                }
                this.buildMenuIndex = 0;
                this.aC = true;
                this.buildMenuMode = 1;
                this.menuDirty = true;
                this.ready = false;
                this.transitionCode = 12;
                this.setState(20);
                break;
            }
            case 55: {
                if (this.loadStep > 0) {
                    --this.loadStep;
                }
                if (this.loadStep != 2) break;
                this.t();
                this.buildMenuIndex = 0;
                this.menuDirty = true;
                this.setState(30);
                break;
            }
            case 65: {
                if (this.loadStep > 0) {
                    --this.loadStep;
                }
                if (this.loadStep != 2) break;
                this.u();
                this.pauseFlag = false;
                this.buildMenuIndex = 0;
                this.menuDirty = true;
                this.setState(30);
                break;
            }
            case 105: {
                if (this.loadStep > 0) {
                    --this.loadStep;
                }
                if (this.loadStep != 2) break;
                if (this.hu - 66 < this.T) {
                    this.c();
                    this.setState(30);
                    break;
                }
                ++this.hu;
                if (this.hu == 69) {
                    this.hu = 66;
                }
                this.setState(30);
                break;
            }
            case 120: {
                if (this.loadStep > 0) {
                    --this.loadStep;
                }
                if (this.loadStep != 2) break;
                this.transitionCode = this.syncHighScores(this.ad) ? 19 : 18;
                this.setState(20);
            }
        }
    }

    public void renderMap(Graphics graphics) {
        int n;
        int n2;
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        int n8;
        int n9 = this.viewportCenterX - 2 * this.viewportCenterY + 16 + 2 * this.hj;
        int n10 = this.viewportCenterX + 2 * this.viewportCenterY - 16 - 2 * this.hj;
        n9 /= 32;
        n10 /= 32;
        if (this.viewportCenterX != this.bH || this.viewportCenterY != this.viewportCenterYCached) {
            this.backbufferG.setClip(0, 15, e, eN);
            this.backbufferG.setColor(0);
            this.backbufferG.fillRect(0, 15, e, eN);
            this.visibleTrainCount = 0;
            this.viewportX0 = 2 * this.mapWidth;
            this.viewportY0 = 2 * this.mapHeight;
            this.viewportX1 = -this.mapWidth;
            this.viewportY1 = -this.mapWidth;
            for (n8 = 0; n8 < this.numActiveTileAnimations; ++n8) {
                n7 = n9 + this.tileOffsetX[n8];
                int n11 = n10 + this.tileOffsetY[n8];
                if (n7 >= 0 && n7 < this.mapWidth && n11 >= 0 && n11 < this.mapHeight) {
                    n6 = this.c(this.tileMap[n7][n11]);
                    if (n6 >= 128) {
                        n6 -= 128;
                    }
                    if (!(n6 != 0 && n6 <= 28 || n6 >= 77 && n6 <= 86)) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.viewportCenterX;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n7][n11] + 15;
                        if (this.tileElevation[n7][n11] > -1) {
                            if (n6 >= 42 && n6 <= 48 || n6 >= 70 && n6 <= 76) {
                                this.backbufferG.drawImage(this.tileSprites[this.tileSpriteIndex[n6]], n5, n4, 33);
                            } else {
                                this.backbufferG.drawImage(this.tileSprites[this.tileSpriteIndex[1]], n5, n4, 33);
                            }
                        } else {
                            this.backbufferG.drawImage(this.tileSprites[this.tileSpriteIndex[2]], n5, n4, 33);
                        }
                    }
                    if (n6 >= 3 && n6 <= 28) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.viewportCenterX;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n7][n11] + 15;
                        this.backbufferG.drawImage(this.tileSprites[this.tileSpriteIndex[n6]], n5, n4 + this.tileSpriteYOffsetByCode[n6 - 3], 33);
                    }
                    if (n6 >= 77 && n6 <= 80) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.viewportCenterX;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n7][n11] + 15;
                        this.backbufferG.drawImage(this.tileSprites[this.tileSpriteIndex[n6]], n5, n4 + this.tileSpriteYOffsetByCode[n6 - 56 - 3], 33);
                    }
                    if (n6 >= 80 && n6 <= 86) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.viewportCenterX;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n7][n11] + 15;
                        n3 = 0;
                        switch (n6) {
                            case 84: {
                                n3 = 4;
                                break;
                            }
                            case 86: {
                                n3 = 4;
                            }
                        }
                        this.backbufferG.drawImage(this.tileSprites[this.tileSpriteIndex[n6]], n5, n4 + n3, 33);
                    }
                }
                if (n7 > this.viewportX1) {
                    this.viewportX1 = n7;
                }
                if (n11 > this.viewportY1) {
                    this.viewportY1 = n11;
                }
                if (n7 < this.viewportX0) {
                    this.viewportX0 = n7;
                }
                if (n11 >= this.viewportY0) continue;
                this.viewportY0 = n11;
            }
            if (this.viewportX0 < 0) {
                this.viewportX0 = 0;
            }
            if (this.viewportY0 < 0) {
                this.viewportY0 = 0;
            }
            if (this.viewportX1 >= this.mapWidth) {
                this.viewportX1 = this.mapWidth - 1;
            }
            if (this.viewportY1 >= this.mapHeight) {
                this.viewportY1 = this.mapHeight - 1;
            }
            this.bH = this.viewportCenterX;
            this.viewportCenterYCached = this.viewportCenterY;
            this.backbufferG.setClip(0, 0, e, hs);
        }
        this.a(graphics);
        graphics.drawImage(this.backbuffer, 0, 0, 20);
        graphics.setClip(0, 15, e, eN);
        this.c(graphics);
        n8 = this.viewportX1 - this.viewportX0 + 1;
        n7 = this.viewportY1 - this.viewportY0 + 1;
        if (n8 < 1) {
            n8 = 1;
        }
        if (n7 < 1) {
            n7 = 1;
        }
        boolean[][] blArray = new boolean[n8][n7];
        this.visibleTrainCount = 0;
        for (n6 = 0; n6 < 500; ++n6) {
            if ((this.trainFlags[n6] & 0x80) != 128) continue;
            n5 = this.c(this.trainX[n6]);
            n4 = this.c(this.trainY[n6]);
            if (n5 < this.viewportX0 || n4 < this.viewportY0 || n5 > this.viewportX1 || n4 > this.viewportY1) continue;
            this.visibleTrains[this.visibleTrainCount++] = (short)n6;
            blArray[n5 - this.viewportX0][n4 - this.viewportY0] = true;
        }
        for (n6 = 0; n6 < this.numActiveTileAnimations; ++n6) {
            int n12;
            n5 = n9 + this.tileOffsetX[n6];
            n4 = n10 + this.tileOffsetY[n6];
            if (n5 < 0 || n5 >= this.mapWidth || n4 < 0 || n4 >= this.mapHeight) continue;
            n3 = this.c(this.tileMap[n5][n4]);
            n2 = n3;
            if (n2 >= 128) {
                n2 -= 128;
            }
            if (n2 > 28 && n2 < 128 && ((n2 < 42 || n2 > 48) && n2 < 70 || n2 > 86)) {
                n = n4 * 16 + n5 * 16 + 16 - this.viewportCenterX;
                n12 = this.hj - n5 * 8 + n4 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n5][n4] + 15;
                if (!ch) {
                    if (n2 == 54 || n2 == 55) {
                        graphics.drawImage(this.tileSprites[this.tileSpriteIndex[53]], n, n12, 33);
                    }
                    if (this.phaseTimer == 1 && n2 >= 65 && n2 <= 68) {
                        byte[] byArray = this.tileMap[n5];
                        int n13 = n4;
                        byArray[n13] = (byte)(byArray[n13] + 1);
                        if (this.tileMap[n5][n4] == 69) {
                            this.tileMap[n5][n4] = 65;
                        }
                    }
                    if (n2 != 55) {
                        graphics.drawImage(this.tileSprites[this.tileSpriteIndex[n2]], n, n12, 33);
                    } else {
                        graphics.drawImage(this.tileSprites[this.tileSpriteIndex[243 + this.timerC2]], n, n12, 33);
                    }
                    if (n2 == 50) {
                        graphics.drawImage(this.tileSprites[this.tileSpriteIndex[240 + this.timerA2]], n + 12, n12 - 28, 33);
                    }
                    if (n2 == 64) {
                        graphics.drawImage(this.tileSprites[this.tileSpriteIndex[246 + this.timerB2]], n + 3, n12 - 39, 33);
                    }
                } else {
                    graphics.drawImage(this.tileSprites[this.tileSpriteIndex[41]], n, n12, 33);
                }
            }
            if (blArray[n5 - this.viewportX0][n4 - this.viewportY0]) {
                for (n = 0; n < this.visibleTrainCount; ++n) {
                    n12 = this.visibleTrains[n];
                    if ((this.trainFlags[n12] & 0x80) != 128 || n5 != this.c(this.trainX[n12]) || n4 != this.c(this.trainY[n12])) continue;
                    this.b(graphics, n12);
                }
            }
            if (n3 < 128) continue;
            n = n4 * 16 + n5 * 16 + 16 - this.viewportCenterX;
            n12 = this.hj - n5 * 8 + n4 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n5][n4] + 15;
            int n14 = -1;
            for (int i = 0; i < 50; ++i) {
                if (this.c(this.tileQueue[i][1]) != n5 || this.c(this.tileQueue[i][2]) != n4) continue;
                n14 = this.tileQueue[i][0];
            }
            if (n14 <= 0) continue;
            graphics.drawImage(this.tileSprites[this.c(this.tileSpriteIndex[250 + n14 / 3])], n, n12 - 3, 33);
        }
        if (!selectionHidden) {
            for (n6 = 0; n6 < this.numActiveTileAnimations; ++n6) {
                n5 = n9 + this.tileOffsetX[n6];
                n4 = n10 + this.tileOffsetY[n6];
                for (n3 = 0; n3 < this.numSelectedTrains; ++n3) {
                    if (this.trainY2[n3] != n5 || this.trainX2[n3] != n4) continue;
                    n2 = n4 * 16 + n5 * 16 + 16 - this.viewportCenterX;
                    n = this.hj - n5 * 8 + n4 * 8 + 16 - this.viewportCenterY - 4 * this.tileElevation[n5][n4] + 15;
                    this.a(graphics, n2, n - 16, (byte)n3);
                }
            }
        }
        this.e(graphics);
        graphics.setClip(0, 0, e, hs);
        this.d(graphics);
    }

    public void hideNotify() {
        this.paused = true;
        if (this.gameState > 10 && this.gameState != 120) {
            this.stopMusic();
        }
        if (this.gameState == 30) {
            this.ready = false;
            this.transitionCode = 1;
            this.setState(20);
        }
    }

    public void showNotify() {
        if (this.gameState > 10 && this.gameState != 120) {
            this.playMusic();
        }
        this.paused = false;
    }
}
