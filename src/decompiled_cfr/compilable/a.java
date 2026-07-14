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

public class a
extends Canvas
implements Runnable {
    GameMIDlet ex;
    public boolean gQ = false;
    final int at;
    final int cB;
    final int ar;
    final int D;
    int hw;
    int hr;
    int hj = (this.hw - 1) * 8;
    int eV = (this.hw + this.hr) * 16;
    int bQ = (this.hw + this.hr) * 8;
    int gU = this.eV / 2;
    int fO = this.bQ / 2;
    int dQ;
    int hf;
    int ab;
    int gG;
    int co;
    int cK;
    int dK;
    int bX;
    int gf;
    int fo;
    int ew;
    int aB;
    int fb;
    int ba;
    int cw;
    int eH;
    int cz;
    int bO;
    int gT;
    int aW;
    int hi;
    int c;
    int hp;
    int fK;
    int aG;
    int eI;
    int au;
    int bZ;
    int fC;
    int ai;
    int fH;
    int F;
    int eP;
    int fB;
    Thread gn;
    int aZ = 1;
    long cX;
    long cp;
    int cS;
    int ge = 0;
    int gg = 0;
    boolean ev = false;
    boolean bJ = false;
    boolean cv = false;
    long dL;
    boolean ce = false;
    long bT = System.currentTimeMillis();
    int aF = 0;
    int z = 0;
    boolean gy = false;
    int dw = 0;
    int aS = 0;
    boolean ee = false;
    int dp = 255;
    static int e = 1;
    static int hs = 1;
    static int eN = hs - 30;
    static int fX = hs - 15;
    b aU = null;
    Image[] ca = new Image[6];
    short[] dY;
    byte[] gz;
    int eB = 0;
    int gF = -1;
    int x = -1;
    int fk = -1;
    boolean gZ;
    boolean hd;
    int cE = -1;
    boolean ga = false;
    boolean bC = true;
    boolean fI = false;
    int eZ;
    boolean m = false;
    int ft;
    int bf;
    int bl;
    int r;
    int Q;
    int ci = -1;
    int gA = 0;
    byte[][] bK;
    byte[][] cL;
    int as = 0;
    boolean hl = true;
    byte[][] dt;
    int fv;
    int bn;
    boolean fx = false;
    int dr = 0;
    int O = 0;
    int dh = 0;
    int bV = 0;
    int db = this.gU;
    int aR = this.fO;
    int t = this.gU * 2;
    int fj = this.fO * 2;
    int bk = 0;
    int ej = 0;
    int hh = 3;
    int an;
    int am;
    int fg;
    int aL;
    int aT = 0;
    int[] w = new int[4];
    int[] dz = new int[4];
    int de;
    int gX;
    int bA;
    int fQ;
    int ek;
    int I;
    boolean fM;
    boolean cN;
    boolean fn = false;
    int bg;
    boolean ck;
    boolean bu = true;
    int bt = 0;
    int gj = 0;
    byte[][] az;
    byte[][] gp;
    byte[][] aa;
    byte[][] gI;
    byte[][] R;
    boolean[][] cP;
    int eY;
    int fN;
    int cf;
    byte aX;
    int cR = 0;
    int[] K = new int[3];
    byte[] fJ = new byte[3];
    int dB = 0;
    int cb = -1;
    boolean fp = true;
    byte gL = 0;
    byte bw = 0;
    byte[] dC = new byte[64];
    short[] gN = new short[64];
    short[] di = new short[64];
    short[] gi = new short[64];
    short[] bL = new short[64];
    byte du = 0;
    byte aE = 0;
    byte[] da = new byte[48];
    byte[] cV = new byte[48];
    byte[] be = new byte[48];
    int gH = -1;
    boolean fq = false;
    int gv = 0;
    byte ae = 0;
    byte eG = 0;
    byte dJ = 0;
    byte b = 0;
    byte hy = 0;
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
    int cn;
    short[][] aQ;
    int fV;
    int bN;
    int fm;
    int ax;
    int fl;
    int dX;
    int gO;
    int ed = 0;
    byte bW = 0;
    byte df = 1;
    int dx = 0;
    boolean aC = true;
    int et = 0;
    int[] dk = new int[]{-1, 1, 0, 0, -1, 1, 0};
    int[] j = new int[]{0, 0, -1, 1, 0, 0, -1};
    int fD = 0;
    int gY = 0;
    int hg = 0;
    int[] gC = new int[5];
    int[] U = new int[5];
    int s = 0;
    int cfr_renamed_0 = 0;
    int eS = 100;
    byte[] hv = new byte[250];
    byte[] aj = new byte[250];
    byte[] eq = new byte[250];
    short[][] cO = new short[250][5];
    byte[] hq = new byte[250];
    byte[] gW = new byte[250];
    byte[] eo = new byte[500];
    byte[] af = new byte[500];
    byte[] ey = new byte[500];
    byte[] u = new byte[500];
    byte[] ak = new byte[500];
    byte[] g = new byte[500];
    byte[] aw = new byte[500];
    byte[] eT = new byte[500];
    byte[] p = new byte[500];
    byte[] aI = new byte[500];
    byte[] A = new byte[500];
    short[] Y = new short[500];
    int[] eL = new int[500];
    byte[] bz = new byte[500];
    int aN;
    int ez = 1;
    byte[][] eR = new byte[][]{{0, 1, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}, {2, 3, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}, {4, 5, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}, {6, 7, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}, {8, 9, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}, {10, 11, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}};
    byte[][] aH = new byte[][]{{1, 0, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}, {3, 2, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}, {5, 4, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}, {7, 6, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}, {9, 8, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}, {11, 10, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}};
    int dI = 5;
    short[][] cs = new short[][]{{130, 131, 132, 133, 134, 135, 136, 137}, {140, 141, 142, 143, 144, 145, 146, 147}, {160, 161, 162, 163, 160, 161, 162, 163}, {165, 166, 167, 168, 165, 166, 167, 168}, {170, 171, 172, 173, 170, 171, 172, 173}, {175, 176, 177, 178, 175, 176, 177, 178}, {180, 181, 182, 183, 180, 181, 182, 183}, {185, 186, 187, 188, 185, 186, 187, 188}};
    byte[] dv = null;
    int dl = 0;
    int fc = 0;
    int fL = 0;
    int eK = 0;
    boolean dd = false;
    int bm = 0;
    int fw = 0;
    int[] dT = new int[500];
    int[] fS = new int[500];
    short[] bb = new short[48];
    short[] fs = new short[48];
    boolean y = false;
    int W = 0;
    boolean bR = false;
    boolean dS = false;
    int gK = 0;
    int gE = 0;
    int cF = 0;
    int ho = 0;
    int gr = 0;
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
    int V = 0;
    int f = 0;
    int eu = 0;
    int dA = 0;
    int v = 0;
    int cM = 0;
    int bG = 0;
    int he = 0;
    int bP = 0;
    int ht = 0;
    int cr = 0;
    int cg = 0;
    int cm = 0;
    int gq = 0;
    int h = 0;
    int B = 0;
    int hn = 0;
    int ep = 0;
    int bo = 0;
    int eb = 0;
    int dF = 0;
    int aq = 0;
    int fe = 0;
    int eh = 0;
    int[] N = new int[12];
    int[] cx = new int[12];
    int[] Z = new int[12];
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
    byte[] hm = new byte[6];
    byte[] bh = new byte[6];
    byte[] bY = new byte[6];
    byte[] dn = new byte[6];
    byte[] dE = new byte[6];
    byte[] fW = new byte[6];
    byte[] fF = new byte[6];
    int[] aV = new int[6];
    int[] M = new int[6];
    byte[] fa = new byte[6];
    byte[] X = new byte[6];
    int[] cJ = new int[6];
    int er = 0;
    short[] dU = new short[12];
    boolean dy;
    int bq;
    int bv = 0;
    int aO = 0;
    byte[] gP = new byte[5];
    byte ap;
    int bs;
    int fT;
    int bD = 0;
    int cC = 0;
    int hb = 0;
    int dV = 0;
    int fu = 0;
    int cT = 0;
    int gu = 0;
    boolean en;
    boolean cG = false;
    boolean ay = false;
    boolean gR = false;
    boolean ac = false;
    int gh = 2;
    Player bi = null;
    VolumeControl E = null;
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
    static byte[] G = new byte[]{-53, -63, -55, 57, 13, 124, -53, -50, -65, 45, 26, 29};
    static byte[] cQ = new byte[]{-30, 61, -6, 10, -3, 53, 20, 122, 57, 93, 44, 127, -2, 12, -31, 56, -58, 40};
    static byte[] ec = new byte[]{18, 1, -28, -57, -10, 41, -37, -62, -40, 53, 12, 116, 53, -83, 36, 19, 38, 20, -33, -33, 36, 19, 68, 120, -25, 43, 6, 62, 29, 2, -25, 41, -68, 37, 66, 125, 0, 62, 62, 127, -54, -52, 17, 49, 17, 3, -59, -54, -30, 28, 5, 51, 24, 1, -65, -30, -8, 58, -6, 47, 11, 1, 17, 5, 47, 18, 53, 99, 42, 27, 57, 123, -9, 96, -54, -50, 17, 9, 34, 28, -41, 15, 3, 39, 46, 100, 64, 97, -44, -58, -19, 22, -38, -63, 18, 1, 8, 63};
    byte[] gl = new byte[1];
    byte[] av = new byte[1];
    int bS = 0;
    static boolean ch = false;
    static boolean aP = false;
    static boolean cc = true;
    static boolean q = true;
    int bF;
    byte[][] cI;
    byte[][] ea;
    int cU = 0;
    int hc = 0;
    int eU = 200000;
    int cy = 0;
    int hk = 0;
    int cY = 600000;
    byte aD = (byte)4;
    int bB = 1;
    int eQ = 1;
    int bj = 1950;
    int fr = 0;
    int fd = 0;
    int dN = 0;
    int bI = 0;
    int eC = 2;
    int aM = 1;
    int eF = 0;
    int ao = 0;
    Random gm = new Random(System.currentTimeMillis());
    Image fG = Image.createImage((int)e, (int)hs);
    Graphics C = this.fG.getGraphics();
    Image[] fU;
    byte[] gt = new byte[255];
    Image P;
    long gD;
    boolean dZ;
    int eW;
    int al = 0;
    int cq = 0;
    boolean d = true;
    int ct = 0;
    byte[] ds;
    byte[][] n;
    int fy = 0;
    int fh = 0;
    int gB = 0;
    int gb = 0;
    int ei = 0;
    int bc = 0;
    int eE = 0;
    int dj = 0;
    byte[][] ha = new byte[50][3];
    boolean[] go = new boolean[50];
    int[] dc = new int[]{16756834, 16765610, 0xC08000, 0xFF9696, 0xFFC8C8, 0xC80000, 0xA0FFA0, 0xC8FFC8, 51200};
    int bx = 0;
    int fR = 0;
    byte fZ;
    byte ff;
    int eD = -1;
    int aJ = -1;
    int hx = 0;
    int gd = 0;
    int dH = 50;
    long em;
    long ah;
    long gw = 0L;
    byte[] gc = new byte[]{0, 0, -4, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 0, -4, 4, 4, 0, 0, 4, 0, -4, 0};
    int bH = -1;
    int dW = -1;
    short[] bU = new short[500];
    int es = 0;
    int fE;
    int eO;
    int cl;
    int gM;

    public a() {
        this.at = 3;
        this.cB = 33;
        this.ar = 20;
        this.D = 24;
    }

    public int c(byte n) {
        return n < 0 ? 256 + n : n;
    }

    public void a(GameMIDlet gameMIDlet) {
        this.ex = gameMIDlet;
        this.Z();
        this.t(0);
        Display.getDisplay((MIDlet)this.ex).setCurrent((Displayable)this);
        this.gn = new Thread(this);
        this.gn.start();
    }

    public void run() {
        while (!this.gQ) {
            int n;
            if (this.cv) continue;
            this.cX = System.currentTimeMillis();
            if (this.aF == 0) {
                if (this.z != 0) {
                    this.aS = this.z;
                }
            } else {
                --this.aF;
            }
            if (this.aS != 0 && this.aS != -999) {
                this.j(this.aS);
            }
            if (this.aS == -999) {
                this.d(this.aS);
            }
            if (this.dw > 0) {
                --this.dw;
            }
            ++this.ge;
            if (this.ev) {
                this.gg = 1;
                while (this.gg > 0) {
                    this.m();
                    if (this.bF != 20 && this.bF != 40) continue;
                    this.ev = false;
                    break;
                }
                this.gg = 0;
            }
            if (this.bF == 10) {
                for (n = 0; n < 5; ++n) {
                    this.m();
                }
            }
            if (this.bF == 30) {
                for (n = 0; n < this.aZ; ++n) {
                    if (!this.bJ) {
                        this.m();
                    }
                    if (!this.cP[this.bt][this.aT]) continue;
                    this.B();
                }
            } else {
                this.m();
            }
            this.dL = System.currentTimeMillis() - this.cX;
            if (!this.gy) {
                Display.getDisplay((MIDlet)this.ex).setCurrent((Displayable)this);
                this.gy = true;
            }
            if (!this.cv) {
                this.repaint();
                this.serviceRepaints();
                this.cS = (int)(35L - (System.currentTimeMillis() - this.cX));
                if (this.cS < 0) {
                    this.cS = 0;
                }
                this.cp = System.currentTimeMillis();
                while (System.currentTimeMillis() - this.cp < (long)this.cS) {
                    try {
                        Thread.sleep(1L);
                    }
                    catch (Exception exception) {
                        // empty catch block
                    }
                    a a2 = this;
                    a2.gn.yield();
                }
            }
            a a3 = this;
            a3.gn.yield();
        }
        if (!this.cv) {
            this.E();
            this.ex.notifyDestroyed();
        }
    }

    public void pointerPressed(int n, int n2) {
        if (this.ev) {
            return;
        }
    }

    public void pointerReleased(int n, int n2) {
        if (this.ev) {
            this.ev = false;
            return;
        }
    }

    public void pointerDragged(int n, int n2) {
        if (this.ev) {
            return;
        }
    }

    public void keyPressed(int n) {
        if (this.ev) {
            this.ev = false;
            return;
        }
        this.aS = n;
    }

    public void keyReleased(int n) {
        this.aS = -999;
    }

    void a(Graphics graphics, int n, int n2, int n3, int n4) {
        graphics.drawImage(this.fU[this.c(this.gt[n])], n2, n3, n4);
    }

    void j(int n) {
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
        this.aS = 0;
        this.aF = 0;
        this.z = 0;
        switch (this.bF) {
            case 0: 
            case 1: 
            case 2: 
            case 3: {
                this.dZ = true;
                break;
            }
            case 20: {
                if (n == 56 || n == 54) {
                    this.a((byte)1);
                    this.aF = 4;
                    this.z = n;
                }
                if (n == 50 || n == 52) {
                    this.a((byte)2);
                    this.aF = 4;
                    this.z = n;
                }
                if (n == 53) {
                    this.a((byte)3);
                }
                if (!this.fI || n != 42) break;
                this.gF = -3;
                this.cE = -1;
                this.a((byte)3);
                break;
            }
            case 30: {
                this.ej = n;
                this.bk = 0;
                if (this.dw == 0) {
                    if (n == 42) {
                        this.c(false);
                        this.dw = 7;
                        this.aS = n;
                    }
                    if (n == 35) {
                        this.c(true);
                        this.dw = 7;
                        this.aS = n;
                    }
                    if (n == 53) {
                        this.ah();
                    }
                    if (n != 48 || this.bF != 30) break;
                    this.t(35);
                    break;
                }
                if (this.dw == 1) {
                    if (n == 42 && this.bt != 4 && this.bt != 2) {
                        this.bt = 0;
                        this.aT = 0;
                        this.bu = true;
                    }
                    if (n != 35) break;
                    if (this.bt == 1) {
                        this.bt = 0;
                        this.aT = 1;
                    }
                    if (this.bt == 3) {
                        this.bt = 1;
                        this.aT = 3;
                    }
                    if (this.bt == 5) {
                        this.bt = 1;
                        this.aT = 1;
                    }
                    this.bu = true;
                    break;
                }
                if (n != 42 && n != 35) break;
                this.aS = n;
                break;
            }
            case 40: {
                this.ej = n;
                this.bk = 40;
                if (n == 53) {
                    if (this.cR == 0) {
                        this.t(30);
                        this.aT = 0;
                        this.bu = true;
                    } else {
                        this.d = false;
                        this.cq = 36;
                        this.t(20);
                    }
                }
                if (n != 42 && n != 35) break;
                if (this.cR == 0) {
                    this.cR = 1;
                    this.cb = 30;
                } else {
                    this.cR = 0;
                    this.cb = 17;
                }
                this.bu = true;
                this.dB = 20;
            }
        }
    }

    void d(int n) {
        this.aS = 0;
        this.ej = 0;
        this.bk = 0;
        this.aF = 0;
        this.z = 0;
        this.dw = 0;
    }

    public void a(String string) {
        this.aU = new b(string);
    }

    public Image aa() {
        byte[] byArray = null;
        int n = 0;
        try {
            int n2;
            int n3 = this.aU.b();
            if (n3 == 2) {
                n = 256 * this.aU.b() + this.aU.b();
                byArray = new byte[n];
                byArray[0] = -119;
                byArray[1] = 80;
                byArray[2] = 78;
                byArray[3] = 71;
                for (n2 = 4; n2 < n; ++n2) {
                    byArray[n2] = (byte)this.aU.b();
                }
            }
            if (n3 == 1) {
                int n4;
                n2 = 256 * this.aU.b() + this.aU.b();
                int n5 = this.aU.b() * 3;
                int n6 = this.aU.b();
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
                byArray[18] = (byte)this.aU.b();
                byArray[19] = (byte)this.aU.b();
                byArray[22] = (byte)this.aU.b();
                byArray[23] = (byte)this.aU.b();
                byArray[24] = (byte)this.aU.b();
                byArray[25] = 3;
                byArray[29] = (byte)this.aU.b();
                byArray[30] = (byte)this.aU.b();
                byArray[31] = (byte)this.aU.b();
                byArray[32] = (byte)this.aU.b();
                byArray[35] = (byte)(n5 >> 8 & 0xFF);
                byArray[36] = (byte)(n5 & 0xFF);
                byArray[37] = 80;
                byArray[38] = 76;
                byArray[39] = 84;
                byArray[40] = 69;
                for (n4 = 0; n4 < n5 + 4; ++n4) {
                    byArray[41 + n4] = (byte)this.aU.b();
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
                    byArray[n4++] = (byte)this.aU.b();
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
        this.eB = 0;
        this.gF = -1;
        this.x = -1;
        this.fk = -1;
        this.gZ = false;
        this.hd = false;
        this.eZ = 0;
        this.m = false;
    }

    public void b(String string) {
        try {
            b b2 = new b(string);
            int n = b2.b();
            int n2 = b2.b();
            int n3 = 256 * b2.b() + b2.b();
            int n4 = 0;
            this.dY = new short[n2 + 1];
            this.gz = new byte[n3];
            for (int i = 0; i < n; ++i) {
                int n5 = b2.b();
                int n6 = 256 * b2.b() + b2.b();
                this.dY[n5] = (short)n4;
                for (int j = 0; j < n6; ++j) {
                    this.gz[n4] = (byte)b2.b();
                    ++n4;
                }
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    byte[] m(int n) {
        byte by = this.gz[n];
        byte[] byArray = new byte[by];
        System.arraycopy(this.gz, n + 1, byArray, 0, by);
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
        if (this.ci != n) {
            this.gA = 1;
            this.ci = n;
        }
        if (this.gA != 0) {
            this.m = false;
        }
        if (this.ga) {
            this.bu = true;
            this.f(graphics);
            this.ga = false;
        }
        short s = this.dY[n];
        int n14 = 65536 * this.c(this.gz[s + 1]) + 256 * this.c(this.gz[s + 2]) + this.c(this.gz[s + 3]);
        int n15 = 65536 * this.c(this.gz[s + 4]) + 256 * this.c(this.gz[s + 5]) + this.c(this.gz[s + 6]);
        int n16 = 65536 * this.c(this.gz[s + 7]) + 256 * this.c(this.gz[s + 8]) + this.c(this.gz[s + 9]);
        int n17 = 65536 * this.c(this.gz[s + 10]) + 256 * this.c(this.gz[s + 11]) + this.c(this.gz[s + 12]);
        int n18 = this.c(this.gz[s + 13]);
        int n19 = this.c(this.gz[s + 14]);
        if (this.eZ < n19) {
            this.eZ = n19;
        } else {
            n19 = this.eZ;
        }
        if (this.gZ || this.hd) {
            n18 += 6;
        }
        this.Q = this.gz[s + 15];
        int n20 = (e - n18) / 2;
        int n21 = (hs - n19) / 2;
        int n22 = 6;
        int n23 = 6;
        int n24 = 6;
        if (this.m) {
            graphics.setColor(n14);
            graphics.fillRect(n20, n21, n18, n19);
            graphics.setColor(n16);
            graphics.drawRect(n20, n21, n18, n19);
            graphics.setColor(n15);
            graphics.drawRect(n20, n21, 0, n19);
            graphics.drawRect(n20, n21, n18, 0);
        }
        this.fI = false;
        if ((this.gz[s] & 1) == 1) {
            if (this.m && this.gF == -1) {
                graphics.setColor(n17);
                graphics.fillRect(n20 + n18 - 10, n21 + 3, 7, 7);
            }
            if (this.m) {
                graphics.setColor(n16);
                graphics.drawRect(n20 + n18 - 10, n21 + 3, 7, 7);
                graphics.drawLine(n20 + n18 - 8, n21 + 5, n20 + n18 - 5, n21 + 8);
                graphics.drawLine(n20 + n18 - 8, n21 + 8, n20 + n18 - 5, n21 + 5);
            }
            this.fI = true;
            n24 = 19;
        } else if (this.gF == -1) {
            this.gF = -3;
        }
        int n25 = s + 16;
        if ((this.gz[s] & 2) == 2) {
            if (this.m) {
                graphics.setColor(n16);
            }
            if (this.m) {
                graphics.drawRect(n20 + 2, n21 + 13, n18 - 4, 0);
            }
            this.ft = n20;
            this.bf = n21;
            this.bl = n18;
            this.r = n19;
            if (this.m) {
                this.a(graphics, n20 + 3, n21 + 5, this.m(s + 16), true);
            }
            n25 += 1 + this.gz[s + 16];
            n22 = 19;
            n23 = 19;
            n24 = 19;
        }
        int n26 = n20 + 4;
        this.ft = n20 + 1;
        this.bf = n21 + n22 - 4;
        this.bl = n18 - 2;
        this.r = n19 - n22 + 3;
        graphics.setClip(this.ft, this.bf, this.bl, this.r);
        if (this.m && this.gZ) {
            graphics.setColor(n16);
            graphics.drawRect(n20 + n18 - 6, n21 + n24 - 1, 4, 0);
            graphics.drawRect(n20 + n18 - 5, n21 + n24 - 2, 2, 0);
            graphics.drawRect(n20 + n18 - 4, n21 + n24 - 3, 0, 0);
        }
        if (this.m && this.hd) {
            graphics.setColor(n16);
            graphics.drawRect(n20 + n18 - 6, n21 + n19 - 5, 4, 0);
            graphics.drawRect(n20 + n18 - 5, n21 + n19 - 4, 2, 0);
            graphics.drawRect(n20 + n18 - 4, n21 + n19 - 3, 0, 0);
        }
        this.x = -2;
        this.fk = -2;
        this.gZ = false;
        this.hd = false;
        this.cE = -1;
        int n27 = this.gz[n25++];
        for (n13 = 0; n13 < n27; ++n13) {
            n12 = n21 + n22 - this.eB;
            if (((by = this.gz[n25++]) & 0x80) == 0) {
                int n28;
                n11 = (by & 4) == 4 ? 0 : this.gz[n25++];
                n10 = (by & 2) == 2 ? 0 : this.gz[n25++];
                n9 = (by & 0x40) == 64 ? 1 : 0;
                n8 = (by & 0x20) == 32 ? 1 : 0;
                n7 = n9 != 0 ? this.gz[n25++] : 0;
                n6 = n8 != 0 ? this.gz[n25++] : 0;
                n5 = (by & 0x10) == 16 ? this.gz[n25++] : 0;
                n4 = (by & 0x10) == 16 ? this.gz[n25++] : 0;
                n3 = (by & 0x10) == 16 ? this.gz[n25++] : 0;
                n2 = (by & 0x10) == 16 ? this.gz[n25++] : 0;
                byte by2 = (by & 0x10) == 16 ? this.gz[n25++] : (byte)0;
                byte[] byArray = n8 != 0 ? this.q(this.Q, n6) : this.m(n25);
                int n29 = 0;
                if (byArray == null) continue;
                for (n28 = 0; n28 < byArray.length; ++n28) {
                    if (byArray[n28] >= 32 && byArray[n28] <= 39) continue;
                    ++n29;
                }
                if (n8 == 0) {
                    n25 = n25 + this.gz[n25] + 1;
                }
                if (n12 + n10 + 6 + by2 >= this.bf + this.r) {
                    n28 = n19 + (n12 + n10 + 6 + by2 + n3 + 3 - (this.r + this.bf));
                    if (this.eZ < n28) {
                        this.eZ = n28;
                    }
                    if (this.eZ > hs - 20) {
                        this.eZ = hs - 20;
                    }
                }
                if (this.m && n12 + n10 + 6 + by2 >= this.bf + this.r) {
                    this.hd = true;
                }
                if (n9 != 0 && (n9 == 0 || !this.e(this.Q, n7))) continue;
                if (n12 + n10 - n3 - 3 <= this.bf + this.r && n12 + n10 + 8 + by2 >= this.bf) {
                    if ((by & 0x10) == 16) {
                        if (this.gF == -3) {
                            this.gF = n13;
                        }
                        if (n12 + n10 + 8 + by2 <= this.bf + this.r && n12 + n10 - 3 - n3 >= this.bf) {
                            if (n13 < this.gF) {
                                this.x = n13;
                            }
                            if (this.fk == -2 && n13 > this.gF) {
                                this.fk = n13;
                            }
                        }
                        if (this.m && this.gF == n13) {
                            graphics.setColor(n17);
                            graphics.fillRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                            this.cE = n5;
                        }
                        if (this.m && (by & 8) == 8) {
                            graphics.setColor(n16);
                            graphics.drawRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                            graphics.setColor(n15);
                            graphics.drawRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 0, 10 + n3 + by2);
                            graphics.drawRect(n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 0);
                        }
                    }
                    if (this.m && this.m) {
                        this.a(graphics, n26 + n11, n12 + n10, byArray, true);
                    }
                }
                n22 += 11 + n10;
                continue;
            }
            n11 = (by & 0x40) == 64 ? 1 : 0;
            n10 = this.gz[n25++];
            n9 = (by & 0x20) == 32 ? 0 : this.gz[n25++];
            n8 = (by & 0x10) == 16 ? 0 : this.gz[n25++];
            n7 = n11 != 0 ? this.gz[n25++] : 0;
            Image image = this.u(this.Q, n10);
            if (image == null) continue;
            n5 = image.getWidth();
            n4 = image.getHeight();
            if (n11 != 0 && (n11 == 0 || !this.e(this.Q, n7))) continue;
            if (this.m && n12 + n8 <= this.bf + this.r && n12 + n8 + n4 >= this.bf) {
                graphics.drawImage(image, n26 + n9, n12 + n8, 20);
            }
            if (this.m && n12 + n8 + n4 >= this.bf + this.r) {
                this.hd = true;
            }
            n22 += n4 + n8;
        }
        n27 = this.gz[n25++];
        for (n13 = 0; n13 < n27; ++n13) {
            n12 = n21 + n23 - this.eB;
            by = this.gz[n25++];
            n11 = by & 7;
            n10 = 65536 * this.c(this.gz[n25++]) + 256 * this.c(this.gz[n25++]) + this.c(this.gz[n25++]);
            n9 = this.c(this.gz[n25++]) + 256 * this.c(this.gz[n25++]);
            n8 = this.c(this.gz[n25++]) + 256 * this.c(this.gz[n25++]);
            n7 = this.c(this.gz[n25++]) + 256 * this.c(this.gz[n25++]);
            n6 = this.c(this.gz[n25++]) + 256 * this.c(this.gz[n25++]);
            n9 = (short)((by & 8) == 8 ? this.n(this.Q, n9) : n9);
            n8 = (short)((by & 0x10) == 16 ? this.n(this.Q, n8) : n8);
            n7 = (short)((by & 0x20) == 32 ? this.n(this.Q, n7) : n7);
            n6 = (short)((by & 0x40) == 64 ? this.n(this.Q, n6) : n6);
            n5 = (by & 0x80) == 128 ? 1 : 0;
            n4 = n5 != 0 ? this.gz[n25++] : 0;
            n3 = n12 + n8;
            n2 = n3 + n6;
            if (!this.m || n5 != 0 && (n5 == 0 || !this.e(this.Q, n4))) continue;
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
            if (!this.m || n2 < this.bf + this.r) continue;
            this.hd = true;
        }
        if (this.m) {
            if (this.eB == 0 && this.x == -2) {
                this.x = -1;
            }
            if (this.eB > 0) {
                this.gZ = true;
            }
        }
        this.m = true;
        if (this.gA > 0) {
            --this.gA;
        }
    }

    public void a(Graphics graphics, int n, int n2, byte[] byArray, boolean bl) {
        int n3 = n;
        graphics.setColor(0);
        for (int i = 0; i < byArray.length; ++i) {
            byte by = byArray[i];
            if (bl) {
                graphics.setClip(this.ft, this.bf, this.bl, this.r);
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
            graphics.setClip(this.ft, this.bf, this.bl, this.r);
        } else {
            graphics.setClip(0, 0, e, hs);
        }
    }

    public void a(byte by) {
        switch (by) {
            case 1: {
                if (this.fk != -2) {
                    this.gF = this.fk;
                    break;
                }
                if (!this.hd) break;
                this.eB += 10;
                break;
            }
            case 2: {
                if (this.x > -1) {
                    this.gF = this.x;
                } else if (this.gZ) {
                    this.eB -= 10;
                } else {
                    this.gF = -1;
                }
                if (this.eB >= 0) break;
                this.eB = 0;
                break;
            }
            case 3: {
                this.v(this.Q, this.cE);
            }
        }
    }

    void z(int n, int n2) {
        this.fv = n - 5;
        this.bn = n2 - 5;
        if (this.fv < 0) {
            this.fv = 0;
        }
        if (this.bn < 0) {
            this.bn = 0;
        }
        if (this.fv + 10 >= this.hw) {
            this.fv = this.hw - 9;
        }
        if (this.bn + 10 >= this.hr) {
            this.bn = this.hr - 9;
        }
        this.dt = new byte[10][10];
        for (int i = 0; i < 10; ++i) {
            System.arraycopy(this.cL[this.fv + i], this.bn, this.dt[i], 0, 10);
        }
        this.as = 0;
    }

    boolean a(int n, int n2, int n3, boolean bl) {
        boolean bl2 = true;
        if (n > 0) {
            if (this.cI[n - 1][n2] > 33) {
                bl2 = false;
            }
            if (n2 > 0 && this.cI[n - 1][n2 - 1] > 33) {
                bl2 = false;
            }
        }
        if (n2 > 0 && this.cI[n][n2 - 1] > 33) {
            bl2 = false;
        }
        if (this.cI[n][n2] > 33) {
            bl2 = false;
        }
        int n4 = n - this.fv;
        int n5 = n2 - this.bn;
        if (bl2) {
            this.dt[n4][n5] = (byte)n3;
            if (n4 > 0 && (bl ? this.dt[n4 - 1][n5] < n3 - 1 : this.dt[n4 - 1][n5] > n3 + 1) && !this.a(n - 1, n2, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
            if (n5 > 0 && (bl ? this.dt[n4][n5 - 1] < n3 - 1 : this.dt[n4][n5 - 1] > n3 + 1) && !this.a(n, n2 - 1, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
            if (n4 < 9 && (bl ? this.dt[n4 + 1][n5] < n3 - 1 : this.dt[n4 + 1][n5] > n3 + 1) && !this.a(n + 1, n2, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
            if (n5 < 9 && (bl ? this.dt[n4][n5 + 1] < n3 - 1 : this.dt[n4][n5 + 1] > n3 + 1) && !this.a(n, n2 + 1, n3 - (bl ? 1 : -1), bl)) {
                bl2 = false;
            }
        }
        this.as += this.c + (n3 == -1 ? this.hp : Math.abs(n3) * this.fK);
        return bl2;
    }

    void ac() {
        for (int i = 0; i < 10; ++i) {
            System.arraycopy(this.dt[i], 0, this.cL[this.fv + i], this.bn, 10);
        }
    }

    void a(boolean bl) {
        int n;
        int n2;
        int n3;
        int n4;
        if (bl) {
            this.ac();
            this.dt = null;
            n4 = this.fv;
            n3 = this.bn;
            n2 = this.fv + 10;
            n = this.bn + 10;
        } else {
            n4 = 0;
            n3 = 0;
            n2 = this.hw;
            n = this.hr;
        }
        for (int i = n4; i < n2; ++i) {
            for (int j = n3; j < n; ++j) {
                byte by = this.cL[i][j];
                byte by2 = this.cL[i][j + 1];
                byte by3 = this.cL[i + 1][j];
                byte by4 = this.cL[i + 1][j + 1];
                if (this.cI[i][j] >= 34) continue;
                if (by == by2 && by == by3 && by == by4) {
                    this.bK[i][j] = by;
                    if (this.cI[i][j] <= 28) {
                        this.cI[i][j] = 0;
                    }
                } else if (by == by2 && by == by3 && by4 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = 11;
                }
                if (by == by3 && by == by4 && by2 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = 13;
                }
                if (by == by2 && by == by4 && by3 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = 14;
                }
                if (by2 == by3 && by2 == by4 && by < by2) {
                    this.bK[i][j] = by2;
                    this.cI[i][j] = 12;
                }
                if (by == by4 && by2 == by3 && by2 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = 16;
                }
                if (by == by4 && by2 == by3 && by < by2) {
                    this.bK[i][j] = by2;
                    this.cI[i][j] = 15;
                }
                if (by == by2 && by3 == by4 && by3 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = (byte)(by3 == -1 && i + 2 <= this.hw && (this.cL[i + 2][j] == -1 || this.cL[i + 2][j + 1] == -1) ? 9 : 24);
                }
                if (by == by2 && by3 == by4 && by < by3) {
                    this.bK[i][j] = by3;
                    this.cI[i][j] = (byte)(by == -1 && i - 1 >= 0 && (this.cL[i - 1][j] == -1 || this.cL[i - 1][j + 1] == -1) ? 8 : 22);
                }
                if (by == by3 && by2 == by4 && by2 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = (byte)(by2 == -1 && j + 2 <= this.hr && (this.cL[i][j + 2] == -1 || this.cL[i + 1][j + 2] == -1) ? 10 : 21);
                }
                if (by == by3 && by2 == by4 && by < by2) {
                    this.bK[i][j] = by2;
                    this.cI[i][j] = (byte)(by == -1 && j - 1 >= 0 && (this.cL[i][j - 1] == -1 || this.cL[i + 1][j - 1] == -1) ? 7 : 23);
                }
                if (by == by2 && by == by3 && by < by4) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = (byte)(by == -1 ? 3 : 19);
                }
                if (by == by2 && by == by4 && by < by3) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = (byte)(by == -1 ? 6 : 18);
                }
                if (by == by3 && by == by4 && by < by2) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = (byte)(by == -1 ? 5 : 20);
                }
                if (by2 == by3 && by2 == by4 && by4 < by) {
                    this.bK[i][j] = by4;
                    this.cI[i][j] = (byte)(by4 == -1 ? 4 : 17);
                }
                if (by == by4 && by2 < by && by3 > by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = 25;
                }
                if (by3 == by2 && by < by3 && by4 > by3) {
                    this.bK[i][j] = by3;
                    this.cI[i][j] = 26;
                }
                if (by == by4 && by2 > by && by3 < by) {
                    this.bK[i][j] = by;
                    this.cI[i][j] = 27;
                }
                if (by3 != by2 || by <= by3 || by4 >= by3) continue;
                this.bK[i][j] = by3;
                this.cI[i][j] = 28;
            }
        }
    }

    void a(int n, int n2, boolean bl) {
        if (bl && this.cL[n][n2] < 3 || !bl && this.cL[n][n2] > -1) {
            this.z(n, n2);
            if (this.a(n, n2, this.cL[n][n2] + (bl ? 1 : -1), bl)) {
                if (this.hl) {
                    if (this.as <= this.eU) {
                        this.eU -= this.as;
                        this.p(-this.as);
                        this.ho += this.as;
                        this.a(true);
                    } else {
                        this.M();
                    }
                } else {
                    this.a(true);
                }
            }
        }
        this.bH = -1;
    }

    void c(int n, int n2, int n3, int n4, int n5) {
        for (int i = n; i < n + n3; ++i) {
            for (int j = n2; j < n2 + n4; ++j) {
                if (i <= 0 || j <= 0 || i > this.hw || j > this.hr) continue;
                this.z(i, j);
                this.a(i, j, n5, true);
                if (this.gm.nextInt() % 2 < 0 && i > 0) {
                    this.a(i - 1, j, n5, true);
                }
                if (this.gm.nextInt() % 2 < 0 && j > 0) {
                    this.a(i, j - 1, n5, true);
                }
                if (this.gm.nextInt() % 2 < 0 && i < this.hw) {
                    this.a(i + 1, j, n5, true);
                }
                if (this.gm.nextInt() % 2 < 0 && j < this.hr) {
                    this.a(i, j + 1, n5, true);
                }
                this.ac();
            }
        }
    }

    void b(int n, int n2, int n3, int n4, int n5) {
        for (int i = n; i < n + n3; ++i) {
            for (int j = n2; j < n2 + n4; ++j) {
                if (i <= 0 || j <= 0 || i > this.hw || j > this.hr) continue;
                for (int k = 0; k < 3; ++k) {
                    this.a(i, j, false);
                }
                if (this.gm.nextInt() % 2 < 0 && i > 0) {
                    this.a(i - 1, j, false);
                }
                if (this.gm.nextInt() % 2 < 0 && j > 0) {
                    this.a(i, j - 1, false);
                }
                if (this.gm.nextInt() % 2 < 0 && i < this.hw) {
                    this.a(i + 1, j, false);
                }
                if (this.gm.nextInt() % 2 >= 0 || j >= this.hr) continue;
                this.a(i, j + 1, false);
            }
        }
    }

    void F(int n) {
        if (n == 0) {
            this.cI = new byte[this.hw + 1][this.hr + 1];
            this.ea = new byte[this.hw + 1][this.hr + 1];
            this.bK = new byte[this.hw + 1][this.hr + 1];
            this.cL = new byte[this.hw + 2][this.hr + 2];
            this.hl = false;
            this.dr = 0;
            this.O = 0;
            this.dh = this.hf + this.ab + this.gG * (this.gG - 1) + this.co + this.fo + this.gf + this.bX + this.dK + this.aB + this.ew + 1;
            this.bV = 0;
        }
        if (n == 1) {
            int n2;
            int n3;
            int n4;
            if (this.dr == 0) {
                this.dr = 1;
                this.O = 0;
            }
            if (this.dr == 1 && this.O == this.hf) {
                this.dr = 2;
                this.O = 0;
                this.a(false);
            }
            if (this.dr == 2 && this.O == this.ab) {
                this.dr = 3;
                this.O = 0;
                this.a(false);
            }
            if (this.dr == 3 && this.O == this.gG * (this.gG - 1) + this.co) {
                this.dr = 4;
                this.O = 0;
            }
            if (this.dr == 4 && this.O == this.fo) {
                this.dr = 5;
                this.O = 0;
            }
            if (this.dr == 5 && this.O == this.gf) {
                this.dr = 6;
                this.O = 0;
            }
            if (this.dr == 6 && this.O == this.bX) {
                this.dr = 7;
                this.O = 0;
            }
            if (this.dr == 7 && this.O == this.dK) {
                this.dr = 8;
                this.O = 0;
            }
            if (this.dr == 8 && this.O == this.aB) {
                this.dr = 9;
                this.O = 0;
            }
            if (this.dr == 9 && this.O == this.ew) {
                this.dr = 10;
                this.O = 0;
            }
            if (this.dr == 1) {
                this.c(Math.abs(this.gm.nextInt() % this.hw), Math.abs(this.gm.nextInt() % this.hr), Math.abs(this.gm.nextInt() % 20) + 1, Math.abs(this.gm.nextInt() % 20) + 1, Math.abs(this.gm.nextInt() % 2) + 1);
            }
            if (this.dr == 2) {
                this.b(Math.abs(this.gm.nextInt() % this.hw), Math.abs(this.gm.nextInt() % this.hr), Math.abs(this.gm.nextInt() % 20) + 1, Math.abs(this.gm.nextInt() % 20) + 1, -1);
            }
            this.fx = false;
            if (this.dr == 3) {
                block0: for (n4 = 0; n4 < this.gG; ++n4) {
                    for (n3 = 0; n3 < this.co; ++n3) {
                        if (this.O != this.gG * n4 + n3) continue;
                        n2 = this.hw / this.gG;
                        int n5 = this.hr / this.co;
                        int n6 = n4 * n2 + Math.abs(this.gm.nextInt() % (n2 / 2));
                        int n7 = n3 * n5 + Math.abs(this.gm.nextInt() % (n5 / 2));
                        this.w(n6, n7);
                        continue block0;
                    }
                }
            }
            if (this.dr == 4) {
                this.u(0);
            }
            if (this.dr == 5) {
                this.u(1);
            }
            if (this.dr == 6) {
                this.u(2);
            }
            if (this.dr == 7) {
                this.u(3);
            }
            if (this.dr == 8) {
                this.u(4);
            }
            if (this.dr == 9) {
                this.u(5);
            }
            if (this.fx) {
                this.ct = 0;
            }
            if (this.dr == 10) {
                for (n4 = 0; n4 < this.dQ; ++n4) {
                    n3 = Math.abs(this.gm.nextInt() % this.hw);
                    if (this.cI[n3][n2 = Math.abs(this.gm.nextInt() % this.hr)] != 0 || this.bK[n3][n2] < 0) continue;
                    this.cI[n3][n2] = (byte)(29 + Math.abs(this.gm.nextInt() % 5));
                }
                this.hl = true;
            }
            ++this.O;
            ++this.bV;
        }
    }

    byte p() {
        int n = Math.abs(this.gm.nextInt() % 22);
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
        int n7 = Math.abs(this.gm.nextInt() % 5) + this.cK;
        byte by5 = (byte)(Math.abs(this.gm.nextInt() % 5) + this.cK);
        byte[][] byArray = new byte[n7][by5];
        boolean bl = false;
        int n8 = Math.abs(this.gm.nextInt() % (n7 - 2)) + 1;
        for (n6 = Math.abs(this.gm.nextInt() % 3); n6 < by5 - Math.abs(this.gm.nextInt() % 3); ++n6) {
            byArray[n8][n6] = 1;
        }
        if (n7 >= 7) {
            do {
                n8 = Math.abs(this.gm.nextInt() % (n7 - 2)) + 1;
                bl = false;
                for (n6 = 0; n6 < by5; ++n6) {
                    if (byArray[n8 - 1][n6] != 1 && byArray[n8 + 1][n6] != 1) continue;
                    bl = true;
                }
            } while (bl);
            for (n6 = Math.abs(this.gm.nextInt() % 3); n6 < by5 - Math.abs(this.gm.nextInt() % 3); ++n6) {
                byArray[n8][n6] = 1;
            }
        }
        n6 = Math.abs(this.gm.nextInt() % (by5 - 2)) + 1;
        for (n5 = Math.abs(this.gm.nextInt() % 3); n5 < n7 - Math.abs(this.gm.nextInt() % 3); ++n5) {
            byArray[n5][n6] = 2;
        }
        if (by5 >= 7) {
            do {
                n6 = Math.abs(this.gm.nextInt() % (by5 - 2)) + 1;
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
                if (n + by4 >= this.hw || n2 + by3 >= this.hr || byArray[by4][by3] == 0 && (byArray[by4][by3] != 0 || this.gm.nextInt() % 3 >= 0)) continue;
                if (n5 == -2 && (n5 = this.cL[n + by4][n2 + by3]) < 0) {
                    n5 = 0;
                }
                long l = System.currentTimeMillis();
                while (n5 != this.cL[n + by4][n2 + by3]) {
                    this.a(n + by4, n2 + by3, n5 > this.cL[n + by4][n2 + by3]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
                l = System.currentTimeMillis();
                while (n5 != this.cL[n + by4 + 1][n2 + by3]) {
                    this.a(n + by4 + 1, n2 + by3, n5 > this.cL[n + by4 + 1][n2 + by3]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
                l = System.currentTimeMillis();
                while (n5 != this.cL[n + by4][n2 + by3 + 1]) {
                    this.a(n + by4, n2 + by3 + 1, n5 > this.cL[n + by4][n2 + by3 + 1]);
                    if (System.currentTimeMillis() - l <= 1000L) continue;
                    this.fx = true;
                    return;
                }
                l = System.currentTimeMillis();
                while (n5 != this.cL[n + by4 + 1][n2 + by3 + 1]) {
                    this.a(n + by4 + 1, n2 + by3 + 1, n5 > this.cL[n + by4 + 1][n2 + by3 + 1]);
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
                if (n + i >= this.hw || n2 + n4 >= this.hr || byArray[i][n4] == 0) continue;
                this.cI[n + i][n2 + n4] = byArray[i][n4];
                this.ea[n + i][n2 + n4] = (byte)(0x80 | by3);
            }
        }
    }

    void a(int n, int n2, int n3, int n4) {
        byte by = -2;
        for (int i = n; i < n + n3; ++i) {
            for (int j = n2; j < n2 + n4; ++j) {
                if (i >= this.hw || j >= this.hr) continue;
                if (by == -2 && (by = this.cL[i][j]) < 0) {
                    by = 0;
                }
                while (by != this.cL[i][j]) {
                    this.a(i, j, by > this.cL[i][j]);
                }
                while (by != this.cL[i + 1][j]) {
                    this.a(i + 1, j, by > this.cL[i + 1][j]);
                }
                while (by != this.cL[i][j + 1]) {
                    this.a(i, j + 1, by > this.cL[i][j + 1]);
                }
                while (by != this.cL[i + 1][j + 1]) {
                    this.a(i + 1, j + 1, by > this.cL[i + 1][j + 1]);
                }
            }
        }
    }

    void b(int n, int n2, int n3, int n4) {
        this.cI[n][n2] = (byte)n3;
        this.ea[n][n2] = (byte)(0xC0 | n4);
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
            n7 = Math.abs(this.gm.nextInt() % (this.hw - 8)) + 4;
            n6 = Math.abs(this.gm.nextInt() % (this.hr - 8)) + 4;
            n5 = this.gm.nextInt() % 2 <= 0 ? 3 : 2;
            int n8 = n4 = this.gm.nextInt() % 2 <= 0 ? 3 : 2;
            if (n == 2) {
                n5 = 2;
                n4 = 2;
            }
            for (n3 = n7 - 3; n3 < n7 + n5 + 3; ++n3) {
                for (n2 = n6 - 3; n2 < n6 + n4 + 3; ++n2) {
                    if (this.cI[n3][n2] > 33) {
                        bl = false;
                    }
                    if (this.bK[n3][n2] != -1) continue;
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
                    this.b(n7, n6, (byte)(57 + Math.abs(this.gm.nextInt() % 2)), n3);
                    this.b(n7, n6 + 1, 56, n3);
                    this.b(n7 + 1, n6, (byte)(57 + Math.abs(this.gm.nextInt() % 2)), n3);
                    this.b(n7 + 1, n6 + 1, (byte)(57 + Math.abs(this.gm.nextInt() % 2)), n3);
                    break;
                }
                case 3: {
                    for (n2 = n7; n2 < n7 + n5; ++n2) {
                        for (int i = n6; i < n6 + n4; ++i) {
                            this.b(n2, i, (byte)(59 + Math.abs(this.gm.nextInt() % 3)), n3);
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
        if (this.bk < 30) {
            this.bk += this.hh;
        }
        this.bk = (byte)this.bk;
        return this.bk / 3;
    }

    void k() {
        if (this.ej == 56 || this.ej == 6) {
            this.hc += 30;
        }
        if (this.ej == 50 || this.ej == 1) {
            this.hc -= 30;
        }
        if (this.ej == 54 || this.ej == 5) {
            this.cU += 60;
        }
        if (this.ej == 52 || this.ej == 2) {
            this.cU -= 60;
        }
        if (this.ej == 49) {
            this.hc -= 30;
            this.cU -= 60;
        }
        if (this.ej == 51) {
            this.hc -= 30;
            this.cU += 60;
        }
        if (this.ej == 55) {
            this.hc += 30;
            this.cU -= 60;
        }
        if (this.ej == 57) {
            this.hc += 30;
            this.cU += 60;
        }
        if (this.cU < 0) {
            this.cU = 0;
        }
        if (this.hc < 0) {
            this.hc = 0;
        }
        if (this.cU > this.eV - e) {
            this.cU = this.eV - e;
        }
        if (this.hc > this.bQ - eN) {
            this.hc = this.bQ - eN;
        }
        this.db = this.cU + e / 2;
        this.aR = this.hc + eN / 2;
        this.t = this.db * 2;
        this.fj = this.aR * 2;
    }

    void B() {
        int n = this.G();
        if (this.ej == 56 || this.ej == 6) {
            this.fj += n * 2;
        }
        if (this.ej == 50 || this.ej == 1) {
            this.fj -= n * 2;
        }
        if (this.ej == 54 || this.ej == 5) {
            this.t += n * 2;
        }
        if (this.ej == 52 || this.ej == 2) {
            this.t -= n * 2;
        }
        if (this.ej == 49) {
            this.fj -= n;
            this.t -= n * 2;
        }
        if (this.ej == 51) {
            this.fj -= n;
            this.t += n * 2;
        }
        if (this.ej == 55) {
            this.fj += n;
            this.t -= n * 2;
        }
        if (this.ej == 57) {
            this.fj += n;
            this.t += n * 2;
        }
        this.db = this.t / 2;
        this.aR = this.fj / 2;
        if (this.db > this.eV - 8) {
            this.db = this.eV - 8;
        }
        if (this.aR > this.bQ - 4) {
            this.aR = this.bQ - 4;
        }
        boolean bl = false;
        if (this.cU > 0 && this.db - this.cU <= 10) {
            bl = true;
            this.cU = this.db - 11;
        }
        if (this.hc > 0 && this.aR - this.hc <= 10) {
            bl = true;
            this.hc = this.aR - 11;
        }
        if (this.db - this.cU >= e - 20) {
            bl = true;
            this.cU = this.db - e + 21;
        }
        if (this.aR - this.hc >= eN - 20) {
            bl = true;
            this.hc = this.aR - eN + 21;
        }
        if (this.db < 0) {
            this.db = 0;
        }
        if (this.aR < 0) {
            this.aR = 0;
        }
        if (this.cU < 0) {
            this.cU = 0;
            bl = false;
        }
        if (this.hc < 0) {
            this.hc = 0;
            bl = false;
        }
        if (this.cU > this.eV - e) {
            this.cU = this.eV - e;
            bl = false;
        }
        if (this.hc > this.bQ - eN) {
            this.hc = this.bQ - eN;
            bl = false;
        }
        if (bl) {
            this.bk = 80;
        }
        this.i();
        this.t = this.db * 2;
        this.fj = this.aR * 2;
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
        nArray2[0] = n4 - 4 * this.cL[n][n2];
        nArray2[1] = n4 - 8 - 4 * this.cL[n + 1][n2];
        nArray2[2] = n4 + 8 - 4 * this.cL[n][n2 + 1];
        nArray2[3] = n4 - 4 * this.cL[n + 1][n2 + 1];
        if (bl) {
            int n5 = 0;
            while (n5 < 4) {
                int n6 = n5;
                nArray[n6] = nArray[n6] - this.cU;
                int n7 = n5++;
                nArray2[n7] = nArray2[n7] - this.hc;
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
        int n7 = this.db - 2 * this.aR + 16 + 2 * this.hj;
        int n8 = this.db + 2 * this.aR - 16 - 2 * this.hj;
        n8 /= 32;
        this.fn = true;
        block0: for (n4 = (n7 /= 32) - 1; n4 <= n7 + 1; ++n4) {
            for (n3 = n8 - 1; n3 <= n8 + 1; ++n3) {
                if (n4 < 0 || n4 > this.hw - 1 || n3 < 0 || n3 > this.hr - 1) continue;
                n5 = n3 * 16 + n4 * 16 + 16;
                n6 = this.hj - n4 * 8 + n3 * 8 + 8;
                this.a(n4, n3, this.w, this.dz, false);
                n2 = this.a(this.db, this.aR, this.w[0], this.dz[0], this.w[1], this.dz[1]) * this.a(this.db, this.aR, this.w[2], this.dz[2], this.w[3], this.dz[3]);
                n = this.a(this.db, this.aR, this.w[0], this.dz[0], this.w[2], this.dz[2]) * this.a(this.db, this.aR, this.w[1], this.dz[1], this.w[3], this.dz[3]);
                if (n2 > 0 || n > 0) continue;
                n7 = n4;
                n8 = n3;
                this.fn = false;
                break block0;
            }
        }
        if (!this.fn) {
            this.bA = n7;
            this.fQ = n8;
            this.ek = n7;
            this.I = n8;
            n4 = Math.abs((this.db - this.w[0]) * (this.db - this.w[0]) + (this.aR - this.dz[0]) * (this.aR - this.dz[0]));
            n3 = Math.abs((this.db - this.w[1]) * (this.db - this.w[1]) + (this.aR - this.dz[1]) * (this.aR - this.dz[1]));
            n2 = Math.abs((this.db - this.w[2]) * (this.db - this.w[2]) + (this.aR - this.dz[2]) * (this.aR - this.dz[2]));
            n = Math.abs((this.db - this.w[3]) * (this.db - this.w[3]) + (this.aR - this.dz[3]) * (this.aR - this.dz[3]));
            int n9 = Math.min(n4, Math.min(n3, Math.min(n2, n)));
            this.fM = n4 < n;
            boolean bl = this.cN = n3 < n2;
            if (n9 == n4) {
                n5 -= 16;
            } else if (n9 == n3) {
                n6 -= 8;
                ++this.bA;
            } else if (n9 == n2) {
                n6 += 8;
                ++this.fQ;
            } else if (n9 == n) {
                n5 += 16;
                ++this.bA;
                ++this.fQ;
            }
            n6 = n6 - this.hc - 4 * this.cL[this.bA][this.fQ];
            this.de = n5 -= this.cU;
            this.gX = n6;
            int n10 = 0;
            while (n10 <= 3) {
                int n11 = n10;
                this.w[n11] = this.w[n11] - this.cU;
                int n12 = n10++;
                this.dz[n12] = this.dz[n12] - this.hc;
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
        if (!this.fn && (by = this.R[this.bt][this.aT]) != 0) {
            int[] nArray;
            int[] nArray2;
            if (by == 1) {
                if (this.e(this.ek, this.I, 0)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 2) {
                if (this.e(this.ek, this.I, 1)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 3) {
                if (this.e(this.ek, this.I, 2)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 4) {
                if (this.e(this.ek, this.I, 3)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 5) {
                if (this.e(this.ek, this.I, 4)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 6) {
                if (this.e(this.ek, this.I, 5)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
                nArray2 = new int[4];
                nArray = new int[4];
                if (this.I > 0) {
                    this.a(this.ek, this.I - 1, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
                if (this.I < this.hw - 1) {
                    this.a(this.ek, this.I + 1, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
            }
            if (by == 7) {
                if (this.e(this.ek, this.I, 6)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
                nArray2 = new int[4];
                nArray = new int[4];
                if (this.ek > 0) {
                    this.a(this.ek - 1, this.I, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
                if (this.ek < this.hw - 1) {
                    this.a(this.ek + 1, this.I, nArray2, nArray, true);
                    this.a(graphics, nArray2, nArray);
                }
            }
            if (by == 8) {
                if (this.j(this.ek, this.I)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 9) {
                if (this.r(this.ek, this.I)) {
                    graphics.setColor(65280);
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            if (by == 10) {
                graphics.setColor(0xFFFFFF);
            }
            if (by == 11) {
                if (this.b(this.ek, this.I)) {
                    int n = this.ek;
                    int n2 = this.I;
                    int n3 = 0;
                    int n4 = 0;
                    if (this.cI[n][n2] == 7) {
                        n4 = -1;
                    }
                    if (this.cI[n][n2] == 10) {
                        n4 = 1;
                    }
                    if (this.cI[n][n2] == 8) {
                        n3 = -1;
                    }
                    if (this.cI[n][n2] == 9) {
                        n3 = 1;
                    }
                    graphics.setColor(65280);
                    int[] nArray3 = new int[4];
                    int[] nArray4 = new int[4];
                    while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < this.hw && n2 < this.hr) {
                        this.a(n, n2, nArray3, nArray4, true);
                        this.a(graphics, nArray3, nArray4);
                        if (this.cI[n][n2] == 0 && this.ea[n][n2] == 0) continue;
                        break;
                    }
                } else {
                    graphics.setColor(0xFF0000);
                }
            }
            this.a(graphics, this.w, this.dz);
        }
    }

    void e(Graphics graphics) {
        if (this.cP[this.bt][this.aT]) {
            int n = this.db - this.cU;
            int n2 = this.aR - this.hc;
            if (this.fn) {
                this.a(graphics, 200, n, n2 + 15, 20);
                graphics.setColor(0xFF0000);
                graphics.drawArc(n + 5, n2 + 5 + 15, 7, 7, 0, 360);
                graphics.drawLine(n + 7, n2 + 10 + 15, n + 10, n2 + 7 + 15);
            } else {
                if (this.bt == 1 && (this.aT == 5 || this.aT == 6)) {
                    this.a(graphics, 249, this.de - 2, this.gX - 2 + 15, 20);
                }
                this.a(graphics, 200, n, n2 + 15, 20);
                if (this.bt != 0 || this.aT != 0) {
                    this.a(graphics, this.c(this.gI[this.bt][this.aT]), n + 5, n2 + 4 + 15, 20);
                }
                if (this.bt == 3 && this.aT < 4) {
                    int n3;
                    this.a(graphics, this.aT + 230, n + 5, n2 + 4 + 15, 20);
                    int n4 = this.ek;
                    int n5 = this.I;
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
                    if (n8 >= this.hw) {
                        n8 = this.hw - 1;
                    }
                    if (n9 >= this.hr) {
                        n9 = this.hr - 1;
                    }
                    int n10 = 0;
                    boolean bl = false;
                    boolean bl2 = false;
                    boolean bl3 = false;
                    boolean bl4 = false;
                    boolean bl5 = false;
                    for (n3 = n6; n3 <= n8; ++n3) {
                        for (int i = n7; i <= n9; ++i) {
                            if (this.cI[n3][i] >= 50 && this.cI[n3][i] <= 55) {
                                bl2 = true;
                            }
                            if (this.cI[n3][i] >= 56 && this.cI[n3][i] <= 61) {
                                bl3 = true;
                            }
                            if (this.cI[n3][i] >= 62 && this.cI[n3][i] <= 68) {
                                bl = true;
                            }
                            if (this.cI[n3][i] < 34 || this.cI[n3][i] > 40) continue;
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

    void H() {
        if (!this.fn && this.r(this.ek, this.I)) {
            this.bg = 0;
            if (this.cI[this.ek][this.I] >= 81 && this.cI[this.ek][this.I] <= 86) {
                int n = this.ek;
                int n2 = this.I;
                this.d(n, n2, -3);
                if (this.cI[n][n2] == 81 || this.cI[n][n2] == 84 || this.cI[n][n2] == 85) {
                    int n3;
                    for (n3 = n; n3 < this.hw && (this.cI[n3][n2] == 81 || this.cI[n3][n2] == 84 || this.cI[n3][n2] == 85); ++n3) {
                        this.a(n3, n2, true, false);
                    }
                    for (n3 = n - 1; n3 >= 0 && (this.cI[n3][n2] == 81 || this.cI[n3][n2] == 84 || this.cI[n3][n2] == 85); --n3) {
                        this.a(n3, n2, true, false);
                    }
                } else {
                    int n4;
                    for (n4 = n2; n4 < this.hr && (this.cI[n][n4] == 82 || this.cI[n][n4] == 83 || this.cI[n][n4] == 86); ++n4) {
                        this.a(n, n4, true, false);
                    }
                    for (n4 = n2 - 1; n4 >= 0 && (this.cI[n][n4] == 82 || this.cI[n][n4] == 83 || this.cI[n][n4] == 86); --n4) {
                        this.a(n, n4, true, false);
                    }
                }
            } else if (this.cI[this.ek][this.I] >= 87 && this.cI[this.ek][this.I] <= 90) {
                int n = this.s(this.ek, this.I);
                if (n != 5000) {
                    int n5 = this.c(this.aj[n]);
                    int n6 = this.c(this.eq[n]);
                    if ((this.hv[n] & 0x20) == 32) {
                        if ((this.hv[n] & 0x40) == 0) {
                            this.ea[n5][n6 - 1] = 0;
                            this.ea[n5][n6 + 1] = 0;
                            this.a(n5, n6 - 1, true, false);
                            this.a(n5, n6 + 1, true, false);
                        } else {
                            this.ea[n5 - 1][n6] = 0;
                            this.ea[n5 + 1][n6] = 0;
                            this.a(n5 - 1, n6, true, false);
                            this.a(n5 + 1, n6, true, false);
                        }
                    }
                    this.a(n5, n6, true, false);
                    this.d(n5, n6, -6);
                    this.ea[n5][n6] = 0;
                    this.hv[n] = 0;
                }
            } else {
                this.a(this.ek, this.I, true, false);
                this.d(this.ek, this.I, -6);
            }
            this.f();
        }
    }

    void I() {
        if (!this.fn) {
            byte by = this.ea[this.ek][this.I];
            if ((by & 0xC0) == 128) {
                this.gL = (byte)(this.ea[this.ek][this.I] & 0x3F);
                this.d = false;
                this.cq = 20;
                this.t(20);
            }
            if ((by & 0xC0) == 192) {
                this.du = (byte)(this.ea[this.ek][this.I] & 0x3F);
                this.d = false;
                this.cq = 21;
                this.t(20);
            }
            if (by == 65) {
                this.aN = this.c(this.ek, this.I);
                if (this.aN != 5000) {
                    int n;
                    int n2;
                    this.gY = this.g[this.aN];
                    for (n2 = 0; n2 < 5; ++n2) {
                        this.gC[n2] = -1;
                    }
                    n2 = this.Y[this.aN];
                    for (n = 0; n < 5 && n2 != -1; ++n) {
                        switch (this.ak[n2] & 0xF) {
                            case 2: {
                                this.gC[n] = 0;
                                break;
                            }
                            case 3: {
                                this.gC[n] = 1;
                                break;
                            }
                            case 4: {
                                this.gC[n] = 2;
                                break;
                            }
                            case 5: {
                                this.gC[n] = 2;
                                break;
                            }
                            case 6: {
                                this.gC[n] = 3;
                                break;
                            }
                            case 7: {
                                this.gC[n] = 3;
                            }
                        }
                        n2 = this.Y[n2];
                    }
                    for (n = 0; n < 5; ++n) {
                        this.U[n] = this.gC[n];
                    }
                    this.hg = this.gY;
                    this.eS = 100;
                    this.af();
                    this.gv = this.s;
                    this.eS = 100 - 3 * this.aw[this.aN];
                    this.af();
                    this.cfr_renamed_0 = this.s;
                    this.d = false;
                    this.cq = 26;
                    this.t(20);
                } else {
                    this.fD = this.s(this.ek, this.I);
                    if (this.fD != 5000) {
                        this.d = false;
                        this.cq = 22;
                        this.t(20);
                    }
                }
            }
        }
    }

    void E(int n) {
        switch (n) {
            case 0: {
                int n2;
                if (!this.e(this.ek, this.I, 0) || (n2 = this.a(1, this.ek, this.I, false, false)) == 5000 || !this.q(this.aW)) break;
                this.cI[this.ek][this.I] = 87;
                this.ea[this.ek][this.I] = 65;
                this.d(this.ek, this.I, -8);
                this.x(this.aW);
                break;
            }
            case 1: {
                int n3;
                if (!this.e(this.ek, this.I, 0) || (n3 = this.a(1, this.ek, this.I, true, false)) == 5000 || !this.q(this.aW)) break;
                this.cI[this.ek][this.I] = 88;
                this.ea[this.ek][this.I] = 65;
                this.d(this.ek, this.I, -8);
                this.x(this.aW);
                break;
            }
            case 2: {
                int n4;
                if (!this.e(this.ek, this.I, 5) || (n4 = this.a(1, this.ek, this.I, false, true)) == 5000 || !this.q(this.hi)) break;
                this.cI[this.ek][this.I] = 87;
                this.cI[this.ek][this.I - 1] = 89;
                this.cI[this.ek][this.I + 1] = 89;
                this.ea[this.ek][this.I] = 65;
                this.ea[this.ek][this.I - 1] = 65;
                this.ea[this.ek][this.I + 1] = 65;
                this.d(this.ek, this.I, -11);
                this.x(this.hi);
                break;
            }
            case 3: {
                int n5;
                if (!this.e(this.ek, this.I, 6) || (n5 = this.a(1, this.ek, this.I, true, true)) == 5000 || !this.q(this.hi)) break;
                this.cI[this.ek][this.I] = 88;
                this.cI[this.ek - 1][this.I] = 90;
                this.cI[this.ek + 1][this.I] = 90;
                this.ea[this.ek][this.I] = 65;
                this.ea[this.ek - 1][this.I] = 65;
                this.ea[this.ek + 1][this.I] = 65;
                this.d(this.ek, this.I, -11);
                this.x(this.hi);
            }
        }
        this.e(true);
        this.T();
    }

    void ah() {
        byte by = this.az[this.bt][this.aT];
        if (!(!this.bJ || this.bt == 0 && by == 11 || this.bt == 0 && by == 1 || this.bt == 0 && by == 9 || this.bt == 1 && by == 1 || this.bt == 1 && by == 3 || this.bt == 1 && by == 9 || this.bt == 3 && by == 9 || this.bt == 5 && by == 9)) {
            return;
        }
        block0 : switch (this.bt) {
            case 0: {
                switch (by) {
                    case 0: {
                        this.I();
                        break;
                    }
                    case 1: {
                        this.bt = 1;
                        this.bu = true;
                        this.aT = 0;
                        break;
                    }
                    case 2: {
                        this.d = false;
                        this.cq = 30;
                        this.t(20);
                        break;
                    }
                    case 3: {
                        this.d = false;
                        this.cq = 31;
                        this.t(20);
                        break;
                    }
                    case 4: {
                        this.a();
                        this.d = false;
                        this.cq = 60;
                        this.t(20);
                        break;
                    }
                    case 5: {
                        this.d = false;
                        this.cq = 17;
                        this.t(20);
                        break;
                    }
                    case 8: {
                        this.ev = true;
                        break;
                    }
                    case 9: {
                        this.c(this.eF - 2);
                        this.bC = false;
                        this.d = false;
                        this.cq = 1;
                        this.t(20);
                        break;
                    }
                    case 11: {
                        this.bJ = !this.bJ;
                    }
                }
                break;
            }
            case 1: {
                switch (by) {
                    case 0: {
                        if (this.fn || !this.j(this.ek, this.I)) break;
                        this.fV = this.ek;
                        this.bN = this.I;
                        this.cI[this.fV][this.bN] = 127;
                        this.bt = 4;
                        this.bu = true;
                        this.aT = 0;
                        break;
                    }
                    case 1: {
                        this.bt = 5;
                        this.bu = true;
                        this.aT = 0;
                        break;
                    }
                    case 2: {
                        if (this.fn) break;
                        if (this.cI[this.ek][this.I] == 7) {
                            this.a(this.ek, this.I, 0, -1, 10, 83, 86, 82);
                        }
                        if (this.cI[this.ek][this.I] == 10) {
                            this.a(this.ek, this.I, 0, 1, 7, 86, 83, 82);
                        }
                        if (this.cI[this.ek][this.I] == 8) {
                            this.a(this.ek, this.I, -1, 0, 9, 84, 85, 81);
                        }
                        if (this.cI[this.ek][this.I] != 9) break;
                        this.a(this.ek, this.I, 1, 0, 8, 85, 84, 81);
                        break;
                    }
                    case 3: {
                        this.bt = 3;
                        this.bu = true;
                        this.aT = 0;
                        break;
                    }
                    case 4: {
                        this.H();
                        break;
                    }
                    case 5: {
                        if (this.fn) break;
                        this.a(this.bA, this.fQ, false);
                        break;
                    }
                    case 6: {
                        if (this.fn) break;
                        this.a(this.bA, this.fQ, true);
                        break;
                    }
                    case 9: {
                        this.bt = 0;
                        this.bu = true;
                        this.aT = 1;
                    }
                }
                break;
            }
            case 2: {
                switch (by) {
                    case 0: {
                        if (this.dx <= this.eU) {
                            this.n(1);
                            this.eU -= this.dx;
                            this.gK += this.dx;
                            this.p(-this.dx);
                            this.x(this.dx);
                            break;
                        }
                        this.J();
                        this.M();
                        break;
                    }
                    case 9: {
                        if (this.aC) {
                            this.J();
                            break;
                        }
                        this.r();
                    }
                }
                this.bt = 1;
                this.aT = this.aC ? 0 : 2;
                this.bu = true;
                this.bH = -1;
                break;
            }
            case 3: {
                switch (by) {
                    default: {
                        this.E(by);
                        break block0;
                    }
                    case 9: 
                }
                this.bt = 1;
                this.bu = true;
                this.aT = 3;
                break;
            }
            case 4: {
                switch (by) {
                    case 0: {
                        if (this.fn || !this.j(this.ek, this.I)) break;
                        this.fm = this.ek;
                        this.ax = this.I;
                        this.cI[this.fV][this.bN] = 0;
                        this.t(45);
                        break;
                    }
                    case 9: {
                        this.cI[this.fV][this.bN] = 0;
                        this.bt = 1;
                        this.bu = true;
                        this.aT = 0;
                    }
                }
                break;
            }
            case 5: {
                switch (by) {
                    case 0: {
                        this.a(this.fM, 75, 74, -1, -1, -1, -1, false, 0);
                        break block0;
                    }
                    case 1: {
                        this.a(this.cN, 72, 73, -1, -1, -1, -1, false, 0);
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
                        this.bt = 1;
                        this.bu = true;
                        this.aT = 1;
                    }
                }
            }
        }
    }

    boolean q(int n) {
        if (n <= this.eU) {
            this.eU -= n;
            this.bH = -1;
            this.gK += n;
            this.p(-n);
            return true;
        }
        this.M();
        return false;
    }

    void a(int n, int n2, int n3, int n4, int n5, int n6, int n7, int n8) {
        int n9 = n;
        int n10 = n2;
        boolean bl = false;
        int n11 = this.fb;
        this.an = n;
        this.am = n2;
        this.fg = n3;
        this.aL = n4;
        while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < this.hw && n2 < this.hr) {
            n11 += this.ba;
            if (this.cI[n][n2] == n5) {
                bl = true;
            }
            if (this.cI[n][n2] == 0 && this.ea[n][n2] == 0) continue;
        }
        if (bl) {
            this.dx = n11;
            this.bt = 2;
            this.bu = true;
            this.aT = 0;
            this.aC = false;
            this.cI[n][n2] = (byte)n7;
            this.ea[n][n2] = 65;
            this.cI[n9][n10] = (byte)n6;
            this.ea[n9][n10] = 65;
            while ((n9 += n3) != n || (n10 += n4) != n2) {
                this.cI[n9][n10] = (byte)n8;
                this.d(n9, n10, -4);
                this.ea[n9][n10] = 65;
            }
            this.bH = -1;
        }
    }

    void r() {
        byte by;
        int n = this.an;
        int n2 = this.am;
        while ((by = this.cI[n][n2]) == 81 || by == 82 || by == 83 || by == 84 || by == 85 || by == 86) {
            if (by == 83) {
                this.cI[n][n2] = 7;
            }
            if (by == 86) {
                this.cI[n][n2] = 10;
            }
            if (by == 84) {
                this.cI[n][n2] = 8;
            }
            if (by == 85) {
                this.cI[n][n2] = 9;
            }
            if (by == 81 || by == 82) {
                this.cI[n][n2] = 0;
            }
            this.ea[n][n2] = 0;
            if ((n += this.fg) >= 0 && (n2 += this.aL) >= 0 && n < this.hw && n2 < this.hr) continue;
            break;
        }
    }

    void a(boolean bl, int n, int n2, int n3, int n4, int n5, int n6, boolean bl2, int n7) {
        if (!this.fn && this.bK[this.ek][this.I] >= 0 && (this.ea[this.ek][this.I] == 0 || bl2 && this.cI[this.ek][this.I] == n7) && (this.cI[this.ek][this.I] == n7 || this.cI[this.ek][this.I] == n3 || this.cI[this.ek][this.I] == n4 || this.cI[this.ek][this.I] == 0 || this.cI[this.ek][this.I] >= 29 && this.cI[this.ek][this.I] <= 33)) {
            int n8 = 0;
            if (this.cI[this.ek][this.I] != 0) {
                n8 += this.cw;
            }
            n8 += this.eH;
            if (bl2 && this.cI[this.ek][this.I] == n7) {
                if ((n8 += this.cz) <= this.eU) {
                    this.cI[this.ek][this.I] = 76;
                    int n9 = this.ea[this.ek][this.I] & 0x3F;
                    n9 = n7 == 70 ? 0x48 | n9 : 0x41 | n9 << 3;
                    this.ea[this.ek][this.I] = (byte)n9;
                    this.eU -= n8;
                    this.bH = -1;
                    this.p(-n8);
                    this.gK += n8;
                    this.x(n8);
                    this.d(this.ek, this.I, -4);
                } else {
                    this.M();
                }
            } else if (n8 <= this.eU) {
                this.cI[this.ek][this.I] = this.cI[this.ek][this.I] == n3 ? (byte)n5 : (this.cI[this.ek][this.I] == n4 ? (byte)n6 : (byte)(bl ? n : n2));
                this.ea[this.ek][this.I] = 65;
                this.eU -= n8;
                this.gK += n8;
                this.bH = -1;
                this.p(-n8);
                this.x(n8);
                this.d(this.ek, this.I, -4);
            } else {
                this.M();
            }
        }
    }

    void a(int n, int n2, boolean bl, boolean bl2) {
        int n3;
        boolean bl3;
        boolean bl4 = bl3 = !bl;
        if (bl) {
            int n4 = n3 = this.cI[n][n2] >= 29 && this.cI[n][n2] <= 33 ? this.bO : -this.gT;
            if (this.eU >= n3 || this.eU < 0) {
                this.bg += n3;
                bl3 = true;
                this.ck = true;
            } else {
                this.ck = false;
            }
        }
        for (n3 = 49; n3 >= 0; --n3) {
            if (this.ha[n3][0] == 0 || this.c(this.ha[n3][1]) != n || this.c(this.ha[n3][2]) != n2) continue;
            bl3 = false;
        }
        if (bl3) {
            if (this.c(this.cI[n][n2]) < 128) {
                this.cI[n][n2] = (byte)(this.cI[n][n2] + 128);
            }
            n3 = 0;
            for (int i = 49; i >= 0; --i) {
                if (this.ha[i][0] != 0) continue;
                n3 = i;
                break;
            }
            this.ha[n3][0] = 14;
            this.go[n3] = bl2;
            this.ha[n3][1] = (byte)n;
            this.ha[n3][2] = (byte)n2;
        }
    }

    void f() {
        if (this.ck) {
            this.ho += this.bg;
            this.eU -= this.bg;
            this.p(-this.bg);
        } else {
            this.M();
        }
    }

    void d(int n, int n2) {
        this.az[n] = new byte[n2];
        this.gp[n] = new byte[n2];
        this.aa[n] = new byte[n2];
        this.gI[n] = new byte[n2];
        this.R[n] = new byte[n2];
        this.cP[n] = new boolean[n2];
        this.eY = 0;
        this.fN = n;
        this.cf = 0;
    }

    void a(int n, int n2, int n3, boolean bl, boolean bl2, int n4) {
        this.az[this.fN][this.eY] = (byte)n2;
        this.gp[this.fN][this.eY] = (byte)n3;
        this.aa[this.fN][this.eY] = (byte)this.cf;
        this.gI[this.fN][this.eY] = (byte)n;
        this.R[this.fN][this.eY] = (byte)n4;
        this.cP[this.fN][this.eY] = bl2;
        this.cf += 15 + (bl ? 2 : 0);
        ++this.eY;
    }

    void v(int n) {
        if (n != -1) {
            this.aX = (byte)n;
        }
        this.az = new byte[6][];
        this.gp = new byte[6][];
        this.aa = new byte[6][];
        this.gI = new byte[6][];
        this.R = new byte[6][];
        this.cP = new boolean[6][];
        int n2 = 5;
        if ((this.aX & 1) == 1) {
            ++n2;
        }
        if ((this.aX & 2) == 2) {
            ++n2;
        }
        if ((this.aX & 4) == 4) {
            ++n2;
        }
        if (this.eF >= 2) {
            ++n2;
        }
        this.d(0, n2);
        this.a(202, 0, 0, true, true, 10);
        this.a(205, 1, 3, true, false, 0);
        if ((this.aX & 1) == 1) {
            this.a(208, 2, 25, false, false, 0);
        }
        if ((this.aX & 2) == 2) {
            this.a(218, 3, 26, false, false, 0);
        }
        if (this.eF >= 2) {
            this.a(207, 5, 28, (this.aX & 4) != 4, false, 0);
        }
        if ((this.aX & 4) == 4) {
            this.a(206, 4, 27, true, false, 0);
        }
        this.a(236, 11, 31, false, false, 0);
        this.a(234, 8, 29, true, false, 0);
        this.a(210, 9, 5, true, false, 0);
        n2 = 4;
        if ((this.aX & 8) == 8) {
            ++n2;
        }
        if ((this.aX & 0x10) == 16) {
            ++n2;
        }
        if ((this.aX & 0x20) == 32) {
            n2 += 2;
        }
        this.d(1, n2);
        if ((this.aX & 8) == 8) {
            this.a(211, 0, 8, true, true, 8);
        }
        this.a(205, 1, 3, false, false, 0);
        if ((this.aX & 0x10) == 16) {
            this.a(216, 2, 13, false, true, 11);
        }
        this.a(217, 3, 15, true, false, 0);
        this.a(219, 4, 14, true, true, 9);
        if ((this.aX & 0x20) == 32) {
            this.a(203, 5, 2, false, true, 0);
        }
        if ((this.aX & 0x20) == 32) {
            this.a(204, 6, 1, true, true, 0);
        }
        this.a(201, 9, 17, true, false, 0);
        this.d(2, 2);
        this.a(225, 0, 18, true, true, 0);
        this.a(226, 9, 19, true, true, 0);
        n2 = 3;
        if ((this.aX & 0x40) == 64) {
            n2 += 2;
        }
        this.d(3, n2);
        this.a(230, 0, 20, false, true, 1);
        this.a(231, 1, 21, true, true, 1);
        if ((this.aX & 0x40) == 64) {
            this.a(232, 2, 22, false, true, 6);
        }
        if ((this.aX & 0x40) == 64) {
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

    void c(boolean bl) {
        this.aT = bl ? ++this.aT : --this.aT;
        if (this.aT < 0) {
            this.aT = this.az[this.bt].length - 1;
        }
        if (this.aT == this.az[this.bt].length) {
            this.aT = 0;
        }
        this.cb = this.gp[this.bt][this.aT];
        this.bu = true;
        this.dB = 20;
    }

    int a(Graphics graphics, int n, int n2, int n3, boolean bl) {
        this.a(graphics, n, n3, fX, 20);
        if (this.aT == n2) {
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
        graphics.drawRect(18 * this.cR, fX, 14, 14);
        graphics.drawRect(18 * this.cR + 1, fX + 1, 12, 12);
    }

    void h(Graphics graphics) {
        if (this.fr != this.bB || this.fd != this.eQ || this.dN != this.bj || this.cy != this.eU || this.bu) {
            int n;
            graphics.setColor(0x909090);
            graphics.fillRect(0, 0, e, 14);
            graphics.setColor(0xBBBBBB);
            graphics.drawRect(0, 0, e, 14);
            graphics.setColor(0x222222);
            graphics.drawLine(1, 14, e, 14);
            graphics.drawLine(e - 1, 1, e - 1, 14);
            StringBuffer stringBuffer = new StringBuffer();
            stringBuffer.append(Math.abs(this.eU));
            stringBuffer = stringBuffer.reverse();
            int n2 = (stringBuffer.length() - 1) / 3;
            for (int i = 1; i <= n2; ++i) {
                stringBuffer.insert(4 * i - 1, ':');
            }
            stringBuffer = stringBuffer.reverse();
            String string = (this.eU < 0 ? "-" : "") + stringBuffer.toString();
            byte[] byArray = new byte[string.length() + 1];
            byArray[0] = 24;
            for (n = 0; n < string.length(); ++n) {
                byArray[n + 1] = string.charAt(n) == '-' ? 22 : (byte)(string.charAt(n) - 47);
            }
            this.a(graphics, 1, 5, byArray, false);
            string = (String.valueOf(this.bB).length() == 1 ? "0" : "") + this.bB + "." + (String.valueOf(this.eQ).length() == 1 ? "0" : "") + this.eQ + "." + (e < 140 ? this.bj - 1900 : this.bj);
            byArray = new byte[string.length()];
            for (n = 0; n < string.length(); ++n) {
                if (string.charAt(n) >= '0' && string.charAt(n) <= '9') {
                    byArray[n] = (byte)(string.charAt(n) - 47);
                }
                if (string.charAt(n) != '.') continue;
                byArray[n] = 11;
            }
            this.a(graphics, e - (e < 140 ? 49 : 61), 5, byArray, false);
            this.cy = this.eU;
            this.fr = this.bB;
            this.fd = this.eQ;
            this.dN = this.bj;
        }
    }

    void a(Graphics graphics) {
        this.h(this.C);
        if (this.bu) {
            boolean bl = false;
            this.C.setColor(0x606060);
            this.C.fillRect(0, fX, e, 15);
            int n = this.c(this.aa[this.bt][this.aT]);
            int n2 = n + 15 - e;
            if (n2 < 0) {
                n2 = 0;
            }
            for (int i = 0; i < this.az[this.bt].length; ++i) {
                this.a(this.C, this.c(this.gI[this.bt][i]), this.c(this.aa[this.bt][i]) - n2, fX, 20);
                if (i != this.aT) continue;
                this.C.setColor(0xFFFFFF);
                this.C.drawRect(this.c(this.aa[this.bt][i]) - n2, fX, 14, 14);
                this.C.drawRect(this.c(this.aa[this.bt][i]) + 1 - n2, fX + 1, 12, 12);
            }
            if (this.bt == 2) {
                String string = String.valueOf(Math.abs(this.dx));
                byte[] byArray = new byte[string.length() + this.n[this.ds[10]].length + 1];
                for (int i = 0; i < string.length(); ++i) {
                    byArray[i + 1] = (byte)(string.charAt(i) - 47);
                }
                byArray[0] = 24;
                System.arraycopy(this.n[this.ds[10]], 0, byArray, string.length() + 1, this.n[this.ds[10]].length);
                this.a(this.C, 35, fX + 5, byArray, false);
            }
            this.bu = false;
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
        if (this.dB > 0) {
            if (this.cb >= 0) {
                this.a(graphics, 10, 10, 0, 37, this.n[this.ds[120 + this.cb]]);
            }
            --this.dB;
        }
    }

    byte a(byte by, int n, int n2) {
        boolean bl;
        this.dC[this.bw] = 50;
        this.gN[this.bw] = by;
        this.gi[this.bw] = (short)n;
        this.bL[this.bw] = (short)n2;
        short s = 0;
        do {
            byte by2 = (byte)Math.abs(this.gm.nextInt() % 6);
            byte by3 = (byte)Math.abs(this.gm.nextInt() % 14);
            s = (short)(256 * by2 + by3);
            bl = true;
            for (int i = 0; i < this.bw; ++i) {
                if (s != this.di[i]) continue;
                bl = false;
            }
        } while (!bl);
        this.di[this.bw] = s;
        this.bw = (byte)(this.bw + 1);
        return (byte)(this.bw - 1);
    }

    void P() {
        if (this.eQ % 4 == 0) {
            for (int i = 0; i < 64; ++i) {
                if (this.dC[i] <= 0) continue;
                int n = i;
                this.dC[n] = (byte)(this.dC[n] - 1);
            }
        }
    }

    void d(int n, int n2, int n3) {
        Vector<Integer> vector = new Vector<Integer>();
        for (int i = n - 5; i <= n + 5; ++i) {
            for (int j = n2 - 5; j <= n2 + 5; ++j) {
                int n4;
                if (i < 0 || j < 0 || i >= this.hw || j >= this.hr || (this.ea[i][j] & 0xC0) != 128 || vector.indexOf(new Integer(n4 = this.ea[i][j] & 0x3F)) != -1) continue;
                vector.addElement(new Integer(n4));
                int n5 = 0;
                if (n3 < 0) {
                    n5 = this.dC[n4] * n3 * 3 / 200 - 1;
                    if (n5 < n3) {
                        n3 = n5;
                    }
                } else {
                    n5 = (100 - this.dC[n4]) / 30 * n3 / 3 + 1;
                    if (n5 > n3) {
                        n5 = n3;
                    }
                }
                int n6 = n4;
                this.dC[n6] = (byte)(this.dC[n6] + n5);
                if (this.dC[n4] < 0) {
                    this.dC[n4] = 0;
                }
                if (this.dC[n4] <= 100) continue;
                this.dC[n4] = 100;
            }
        }
    }

    void d() {
        int n;
        for (n = 0; n < 64; ++n) {
            this.gN[n] = 0;
        }
        for (n = 0; n < this.hw; ++n) {
            for (int i = 0; i < this.hr; ++i) {
                if (this.cI[n][i] < 34 || this.cI[n][i] > 40) continue;
                int n2 = this.ea[n][i] & 0x3F;
                this.gN[n2] = (short)(this.gN[n2] + 1);
            }
        }
    }

    void a(int n, int n2, int n3) {
        if (this.cI[n][n2] == 0 || this.cI[n][n2] >= 29 && this.cI[n][n2] <= 33) {
            this.cI[n][n2] = this.p();
            this.ea[n][n2] = (byte)n3;
        }
    }

    boolean v() {
        int n;
        int n2 = Math.abs(this.gm.nextInt() % (this.hw - 2)) + 1;
        if (this.cI[n2][n = Math.abs(this.gm.nextInt() % (this.hr - 2)) + 1] == 0 || this.cI[n2][n] >= 29 && this.cI[n2][n] <= 33) {
            boolean bl;
            byte by = this.cI[n2 - 1][n];
            byte by2 = this.cI[n2 + 1][n];
            byte by3 = this.cI[n2][n - 1];
            byte by4 = this.cI[n2][n + 1];
            boolean bl2 = by == 43 || by == 44 || by == 45 || by == 46;
            boolean bl3 = by2 == 43 || by2 == 44 || by2 == 47 || by2 == 48;
            boolean bl4 = by3 == 42 || by3 == 44 || by3 == 46 || by3 == 47;
            boolean bl5 = bl = by4 == 42 || by4 == 44 || by4 == 45 || by4 == 48;
            if (bl2 || bl3 || bl4 || bl) {
                int n3 = -1;
                if (bl2) {
                    n3 = this.ea[n2 - 1][n];
                } else if (bl3) {
                    n3 = this.ea[n2 + 1][n];
                } else if (bl4) {
                    n3 = this.ea[n2][n - 1];
                } else if (bl) {
                    n3 = this.ea[n2][n + 1];
                }
                int n4 = n3 & 0x3F;
                if (n4 > -1) {
                    int n5;
                    if (bl2 && bl3 && bl4 && bl) {
                        this.cI[n2][n] = 44;
                    } else if (bl2 && bl3 && bl4) {
                        this.cI[n2][n] = 44;
                    } else if (bl2 && bl3 && bl) {
                        this.cI[n2][n] = 44;
                    } else if (bl2 && bl4 && bl) {
                        this.cI[n2][n] = 44;
                    } else if (bl3 && bl4 && bl) {
                        this.cI[n2][n] = 44;
                    } else if (bl2 && bl4) {
                        this.cI[n2][n] = 48;
                    } else if (bl2 && bl) {
                        this.cI[n2][n] = 47;
                    } else if (bl3 && bl4) {
                        this.cI[n2][n] = 45;
                    } else if (bl3 && bl) {
                        this.cI[n2][n] = 46;
                    } else {
                        n5 = Math.abs(this.gm.nextInt() % 13);
                        if (n5 == 12) {
                            this.cI[n2][n] = 44;
                        } else if (n5 == 11) {
                            if (bl2) {
                                this.cI[n2][n] = 47;
                            }
                            if (bl3) {
                                this.cI[n2][n] = 46;
                            }
                            if (bl4) {
                                this.cI[n2][n] = 45;
                            }
                            if (bl) {
                                this.cI[n2][n] = 46;
                            }
                        } else if (n5 == 10) {
                            if (bl2) {
                                this.cI[n2][n] = 48;
                            }
                            if (bl3) {
                                this.cI[n2][n] = 45;
                            }
                            if (bl4) {
                                this.cI[n2][n] = 48;
                            }
                            if (bl) {
                                this.cI[n2][n] = 47;
                            }
                        } else {
                            if (bl2 || bl3) {
                                this.cI[n2][n] = 43;
                            }
                            if (bl4 || bl) {
                                this.cI[n2][n] = 42;
                            }
                        }
                    }
                    n5 = this.cI[n2][n];
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
                    this.ea[n2][n] = (byte)n3;
                    return true;
                }
            }
        }
        return false;
    }

    byte f(int n) {
        this.da[this.aE] = (byte)n;
        this.cV[this.aE] = (byte)(50 + this.gm.nextInt() % 20);
        this.be[this.aE] = 0;
        this.aE = (byte)(this.aE + 1);
        return (byte)(this.aE - 1);
    }

    void ag() {
        for (int i = 0; i < 48; ++i) {
            int n = i;
            this.cV[n] = (byte)(this.cV[n] + this.gm.nextInt() % 8);
            if (this.cV[i] < 10) {
                this.cV[i] = 10;
            }
            if (this.cV[i] <= 90) continue;
            this.cV[i] = 90;
        }
    }

    void T() {
        for (int i = 0; i < 48; ++i) {
            this.be[i] = 0;
        }
        Vector<String> vector = new Vector<String>();
        for (int i = 0; i < this.hw; ++i) {
            for (int j = 0; j < this.hr; ++j) {
                byte by = this.ea[i][j];
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
                if (n4 >= this.hw) {
                    n4 = this.hw - 1;
                }
                if (n5 >= this.hr) {
                    n5 = this.hr - 1;
                }
                for (int k = n2; k <= n4; ++k) {
                    for (int i2 = n3; i2 <= n5; ++i2) {
                        if (this.cI[k][i2] != 87 && this.cI[k][i2] != 88 || vector.indexOf(n + "_" + k + "_" + i2) != -1) continue;
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
            if (n2 == 0 && this.gY >= 0 && this.gY < 3) {
                return this.fU[this.c(this.gt[136])];
            }
            if (n2 == 0 && this.gY >= 3 && this.gY < 6) {
                return this.fU[this.c(this.gt[146])];
            }
            if (n2 > 0) {
                switch (this.gC[n2 - 1]) {
                    case -1: {
                        return null;
                    }
                    case 0: {
                        return this.fU[this.c(this.gt[162])];
                    }
                    case 1: {
                        return this.fU[this.c(this.gt[167])];
                    }
                    case 2: {
                        return this.fU[this.c(this.gt[172])];
                    }
                    case 3: {
                        return this.fU[this.c(this.gt[182])];
                    }
                }
            }
        }
        if (n == 26 && this.A(this.aN) > n2) {
            int n3;
            int n4 = this.aN;
            for (n3 = 0; n3 < n2; ++n3) {
                n4 = this.Y[n4];
            }
            n3 = this.ak[n4] & 0xF;
            switch (n3) {
                case -1: {
                    return null;
                }
                case 0: {
                    return this.fU[this.c(this.gt[136])];
                }
                case 1: {
                    return this.fU[this.c(this.gt[146])];
                }
                case 2: {
                    return this.fU[this.c(this.gt[162])];
                }
                case 3: {
                    return this.fU[this.c(this.gt[167])];
                }
                case 4: {
                    return this.fU[this.c(this.gt[172])];
                }
                case 5: {
                    return this.fU[this.c(this.gt[177])];
                }
                case 6: {
                    return this.fU[this.c(this.gt[182])];
                }
                case 7: {
                    return this.fU[this.c(this.gt[187])];
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
            if (n2 == 0 && !this.d) {
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
            if (n2 == 0 && this.da[this.du] % 2 == 0) {
                return true;
            }
            if (n2 == 1 && this.da[this.du] % 2 == 1) {
                return true;
            }
            if (n2 == 2 && this.da[this.du] % 2 == 1) {
                return true;
            }
        }
        if (n == 22) {
            if (n2 == 0 && this.cO[this.fD][0] == 0 && this.cO[this.fD][1] == 0 && this.cO[this.fD][2] == 0 && this.cO[this.fD][3] == 0) {
                return true;
            }
            if (n2 == 1 && this.cO[this.fD][0] > 0) {
                return true;
            }
            if (n2 == 2 && this.cO[this.fD][1] > 0) {
                return true;
            }
            if (n2 == 3 && this.cO[this.fD][2] > 0) {
                return true;
            }
            if (n2 == 4 && this.cO[this.fD][3] > 0) {
                return true;
            }
            if (n2 == 5 && this.cO[this.fD][4] > 0) {
                return true;
            }
            if (n2 == 6 && this.hq[this.fD] == 0) {
                return true;
            }
            if (n2 == 7 && (this.hq[this.fD] & 1) == 1) {
                return true;
            }
            if (n2 == 8 && (this.hq[this.fD] & 2) == 2) {
                return true;
            }
            if (n2 == 9 && (this.hq[this.fD] & 4) == 4) {
                return true;
            }
            if (n2 == 10 && (this.hq[this.fD] & 8) == 8) {
                return true;
            }
            if (n2 == 11 && (this.hq[this.fD] & 0x10) == 16) {
                return true;
            }
        }
        if (n == 24 && n2 <= this.ez) {
            return true;
        }
        if (n == 26) {
            if (n2 < 6 && this.A(this.aN) > n2) {
                return true;
            }
            if (n2 == 6 && this.eL[this.aN] == 7) {
                return true;
            }
            if (n2 == 7 && this.eL[this.aN] != 7) {
                return true;
            }
            if (n2 == 8 && this.aw[this.aN] > 0) {
                return true;
            }
        }
        return n == 60 && (n2 != 0 ? n2 <= this.aO : this.aO == 0);
    }

    int n(int n, int n2) {
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
                switch (this.cn) {
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
                        return this.n[this.ds[30 + this.eG]];
                    }
                    case 3: {
                        return this.n[this.ds[35 + this.dJ]];
                    }
                    case 4: {
                        return this.n[this.ds[40 + this.b]];
                    }
                    case 5: {
                        return this.n[this.ds[40 + this.hy]];
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
                        byte[] byArray = this.n[this.ds[80 + (this.di[this.gL] >> 8 & 0xFF)]];
                        byte[] byArray2 = this.n[this.ds[86 + (this.di[this.gL] & 0xFF)]];
                        byte[] byArray3 = new byte[byArray2.length + byArray.length];
                        System.arraycopy(byArray2, 0, byArray3, 0, byArray2.length);
                        System.arraycopy(byArray, 0, byArray3, byArray2.length, byArray.length);
                        return byArray3;
                    }
                    case 1: {
                        return this.p(this.gN[this.gL] * 13, 0);
                    }
                    case 2: {
                        return this.p(this.dC[this.gL], 29);
                    }
                }
                break;
            }
            case 21: {
                switch (n2) {
                    case 0: {
                        return this.n[this.ds[100 + this.da[this.du]]];
                    }
                    case 1: {
                        return this.n[this.ds[106 + this.da[this.du] / 2]];
                    }
                    case 2: {
                        return this.p(this.fs[this.du], 0);
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
                        return !aP ? this.n[this.ds[20]] : this.n[this.ds[21]];
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
                        return this.b(this.aG);
                    }
                    case 1: {
                        return this.b(this.eI);
                    }
                    case 2: {
                        return this.b(this.au);
                    }
                    case 3: {
                        return this.b(this.bZ);
                    }
                    case 4: {
                        return this.b(this.fC);
                    }
                    case 5: {
                        return this.b(this.ai);
                    }
                }
                break;
            }
            case 23: {
                return this.b(this.s - this.cfr_renamed_0);
            }
            case 22: {
                return this.p(this.cO[this.fD][n2], 0);
            }
            case 26: {
                int n3;
                if (n2 == 0) {
                    return this.n[this.ds[60 + this.g[this.aN]]];
                }
                if (n2 == 6) {
                    return this.b(this.dT[this.aN]);
                }
                if (n2 == 7) {
                    return this.b(this.fS[this.aN]);
                }
                if (n2 == 8) {
                    return this.p(this.aw[this.aN], 0);
                }
                if (n2 == 9) {
                    return this.b(this.cfr_renamed_0 * 70 / 100);
                }
                if (n2 == 10) {
                    return this.b(this.gv - this.cfr_renamed_0);
                }
                if (n2 == 20) {
                    return this.bz[this.aN] == 0 ? this.n[this.ds[21]] : this.n[this.ds[20]];
                }
                if (n2 <= 0 || n2 >= 6) break;
                int n4 = 0;
                if (this.A(this.aN) > n2) {
                    n4 = this.aN;
                    for (n3 = 0; n3 < n2; ++n3) {
                        n4 = this.Y[n4];
                    }
                }
                n3 = this.ak[n4] & 0xF;
                String string = String.valueOf(this.p[n4]);
                if (n3 == 2) {
                    string = string + "/" + this.eT[n4];
                }
                return this.d(string);
            }
            case 30: {
                switch (n2) {
                    case 0: {
                        return this.b(this.hk);
                    }
                    case 1: {
                        return this.b(this.cY);
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
                        return this.b(this.y ? this.gx : this.gK);
                    }
                    case 1: {
                        return this.b(this.y ? this.eM : this.gE);
                    }
                    case 2: {
                        return this.b(this.y ? this.V : this.cF);
                    }
                    case 3: {
                        return this.b(this.y ? this.f : this.ho);
                    }
                    case 4: {
                        return this.b(this.y ? this.dA : this.eg);
                    }
                    case 5: {
                        return this.b(this.y ? this.v : this.eJ);
                    }
                    case 6: {
                        return this.b(this.y ? this.eu : this.gr);
                    }
                    case 7: {
                        return this.b(this.y ? this.cM : this.eX);
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
                            return this.b(this.N[n2 - 40]);
                        }
                        if (this.W == 1) {
                            return this.b(this.cx[n2 - 40]);
                        }
                        if (this.W == 2) {
                            return this.b(this.Z[n2 - 40]);
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
                    return this.b(this.aV[this.cZ]);
                }
                if (n2 == 3) {
                    return this.p(this.a((int)this.cZ), 29);
                }
                if (n2 != 4) break;
                return this.p(this.aV[this.cZ], 0);
            }
            case 60: {
                if (n2 % 2 == 1) {
                    int n5 = n2 / 2;
                    return this.n[this.c(this.ds[this.hm[this.gP[n5]] + 160])];
                }
                int n6 = (n2 - 1) / 2;
                return this.p(this.a((int)this.gP[n6]), 29);
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
                    return this.p(this.X[this.cZ], 29);
                }
                if (n2 == 2) {
                    return this.b(this.cJ[this.cZ]);
                }
                if (n2 != 3) break;
                return this.p(this.X[this.cZ], 0);
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
                        if (this.d) {
                            this.cq = 4;
                            this.o();
                            break;
                        }
                        this.gH = 4;
                        this.t(30);
                        break;
                    }
                    case 2: {
                        this.cn = 1;
                        if (this.d) {
                            this.cq = 14;
                            this.o();
                            break;
                        }
                        this.gH = 14;
                        this.t(30);
                        break;
                    }
                    case 3: {
                        this.cn = 0;
                        if (this.d) {
                            this.cq = 14;
                            this.o();
                            break;
                        }
                        this.gH = 14;
                        this.t(30);
                        break;
                    }
                    case 4: {
                        if (this.d) {
                            this.cq = 10;
                            this.o();
                            break;
                        }
                        this.gH = 10;
                        this.t(30);
                        break;
                    }
                    case 5: {
                        if (this.d) {
                            this.cq = 2;
                            this.o();
                            break;
                        }
                        this.gH = 2;
                        this.t(30);
                        break;
                    }
                    case 6: {
                        if (this.d) {
                            this.cq = 3;
                            this.o();
                            break;
                        }
                        this.gH = 3;
                        this.t(30);
                        break;
                    }
                    case 7: {
                        this.t(30);
                        break;
                    }
                    case 8: {
                        if (this.d) {
                            this.cq = 5;
                            this.o();
                            break;
                        }
                        this.gH = 5;
                        this.t(30);
                        break;
                    }
                    case 9: {
                        if (this.d) {
                            this.cq = 28;
                            this.o();
                            break;
                        }
                        this.gH = 28;
                        this.t(30);
                        break;
                    }
                    case 99: {
                        this.cn = 2;
                        if (this.d) {
                            this.cq = 14;
                            this.o();
                            break;
                        }
                        this.gH = 14;
                        this.t(30);
                    }
                }
                break;
            }
            case 14: {
                if (n2 == 2) {
                    if (this.cn == 4) {
                        this.gH = 26;
                        this.t(30);
                        break;
                    }
                    if (this.cn == 3) {
                        this.t(10);
                        break;
                    }
                    if (this.d) {
                        this.cq = 1;
                        this.o();
                        break;
                    }
                    this.gH = 1;
                    this.t(30);
                    break;
                }
                switch (this.cn) {
                    case 0: {
                        this.t(55);
                        break;
                    }
                    case 1: {
                        this.t(65);
                        break;
                    }
                    case 2: {
                        this.gQ = true;
                        break;
                    }
                    case 3: {
                        this.gR = true;
                        this.ac = true;
                        this.t(10);
                        break;
                    }
                    case 4: {
                        this.eU += this.cfr_renamed_0 * 70 / 100;
                        this.gE -= this.cfr_renamed_0 * 70 / 100;
                        if (this.cfr_renamed_0 != 0) {
                            this.p(this.cfr_renamed_0 * 70 / 100);
                        }
                        short s = this.Y[this.aN];
                        this.o(s);
                        this.ak[this.aN] = 0;
                        while (s != -1) {
                            this.ak[s] = 0;
                            s = this.Y[s];
                        }
                        this.t(30);
                    }
                }
                break;
            }
            case 36: {
                this.t(40);
                break;
            }
            case 17: {
                if (n2 == 1) {
                    this.gH = 80 + this.ao;
                }
                this.t(30);
                break;
            }
            case 2: 
            case 3: {
                if (this.d) {
                    this.cq = 1;
                    this.o();
                    break;
                }
                this.gH = 1;
                this.t(30);
                break;
            }
            case 6: {
                this.t(30);
                break;
            }
            case 7: {
                this.d = false;
                this.dP = true;
                this.cq = 1;
                this.o();
                this.t(20);
                break;
            }
            case 5: {
                if (n2 >= 2) {
                    this.J = n2;
                    this.t(25);
                    break;
                }
                if (this.d) {
                    this.cq = 1;
                    this.o();
                    break;
                }
                this.gH = 1;
                this.t(30);
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
                        if (this.eG == 0) {
                            this.eG = 1;
                            break block0;
                        }
                        this.eG = 0;
                        break block0;
                    }
                    case 3: {
                        if (this.dJ == 0) {
                            this.dJ = 1;
                            break block0;
                        }
                        this.dJ = 0;
                        break block0;
                    }
                    case 4: {
                        if (this.b == 0) {
                            this.b = 1;
                            break block0;
                        }
                        this.b = 0;
                        break block0;
                    }
                    case 5: {
                        if (this.hy == 0) {
                            this.hy = 1;
                            break block0;
                        }
                        this.hy = 0;
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
                        this.t(25);
                        break block0;
                    }
                }
                if (this.d) {
                    this.cq = 1;
                    this.o();
                    break;
                }
                this.gH = 1;
                this.t(30);
                break;
            }
            case 20: {
                switch (n2) {
                    default: 
                }
                this.t(30);
                break;
            }
            case 21: {
                switch (n2) {
                    default: 
                }
                this.t(30);
                break;
            }
            case 22: {
                switch (n2) {
                    case 1: {
                        int n3;
                        this.gY = 0;
                        this.cfr_renamed_0 = 0;
                        this.hg = this.gY;
                        for (n3 = 0; n3 < 5; ++n3) {
                            this.gC[n3] = -1;
                        }
                        for (n3 = 0; n3 < 5; ++n3) {
                            this.U[n3] = -1;
                        }
                        this.eS = 100;
                        this.fq = false;
                        this.af();
                        this.gH = 23;
                        this.t(30);
                        break block0;
                    }
                }
                this.t(30);
                break;
            }
            case 23: {
                if (n2 == 0) {
                    this.gH = 24;
                    this.t(30);
                    break;
                }
                if (n2 == 1) {
                    this.gH = 25;
                    this.t(30);
                    break;
                }
                if (n2 == 2) {
                    if (!this.fq) {
                        if (!this.y()) break;
                        this.t(30);
                        break;
                    }
                    if (!this.a(this.aN, this.gY, this.gC)) break;
                    this.gH = 26;
                    this.t(30);
                    break;
                }
                this.gH = !this.fq ? 22 : 26;
                this.t(30);
                break;
            }
            case 24: {
                if (n2 >= 0 && n2 < 6) {
                    this.gY = n2;
                    this.af();
                }
                this.gH = 23;
                this.t(30);
                break;
            }
            case 25: {
                if (n2 == 3) {
                    this.r(0);
                } else if (n2 == 1) {
                    this.r(1);
                } else if (n2 == 0) {
                    this.r(2);
                } else if (n2 == 2) {
                    this.r(3);
                } else if (n2 == 9) {
                    this.V();
                }
                this.af();
                this.gH = 23;
                this.t(30);
                break;
            }
            case 26: {
                if (n2 == 1) {
                    this.fq = true;
                    this.gH = 23;
                    this.t(30);
                } else if (n2 == 2) {
                    int n4 = this.gv - this.cfr_renamed_0;
                    if (n4 <= this.eU) {
                        this.aw[this.aN] = 0;
                        this.eU -= n4;
                        this.p(-n4);
                        this.gE += n4;
                        this.eS = 100 - 3 * this.aw[this.aN];
                        this.af();
                        this.cfr_renamed_0 = this.s;
                        this.gH = 26;
                        this.t(30);
                    } else {
                        this.M();
                    }
                } else if (n2 == 3) {
                    this.cn = 4;
                    this.gH = 14;
                    this.t(30);
                }
                if (n2 == 4) {
                    this.bz[this.aN] = (byte)(this.bz[this.aN] == 0 ? 1 : 0);
                    break;
                }
                this.t(30);
                break;
            }
            case 10: {
                switch (n2) {
                    case 1: {
                        ch = !ch;
                        break block0;
                    }
                    case 2: {
                        aP = !aP;
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
                            this.ae();
                            break block0;
                        }
                        this.E();
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
                if (this.d) {
                    this.cq = 1;
                    this.o();
                    break;
                }
                this.gH = 1;
                this.t(30);
                break;
            }
            case 18: 
            case 19: {
                if (this.d) {
                    this.cq = 1;
                    this.o();
                    break;
                }
                this.gH = 1;
                this.t(30);
                break;
            }
            case 8: 
            case 9: {
                if (n2 == 1) {
                    this.gH = this.fY;
                    this.t(30);
                    break;
                }
                if (n2 == 2) {
                    this.gH = this.dR;
                    this.t(30);
                    break;
                }
                this.t(30);
                break;
            }
            case 11: 
            case 12: 
            case 13: 
            case 16: {
                this.t(30);
                break;
            }
            case 15: {
                if (n2 == 1) {
                    this.gR = true;
                }
                this.t(10);
                break;
            }
            case 30: {
                switch (n2) {
                    case 1: {
                        if (this.cY - 20000 < this.hk) break;
                        this.hk += 20000;
                        this.eU += 20000;
                        this.z(-20000);
                        break;
                    }
                    case 2: {
                        if (this.hk < 20000 || this.eU < 20000) break;
                        this.hk -= 20000;
                        this.eU -= 20000;
                        this.z(20000);
                        break;
                    }
                    default: {
                        this.t(30);
                        this.bu = true;
                    }
                }
                this.ga = true;
                break;
            }
            case 31: {
                this.dS = false;
                switch (n2) {
                    case 0: {
                        this.y = false;
                        this.ab();
                        this.gH = 32;
                        this.t(30);
                        break block0;
                    }
                    case 1: {
                        this.y = true;
                        this.gH = 32;
                        this.t(30);
                        break block0;
                    }
                    case 2: {
                        this.b(0, false);
                        this.gH = 33;
                        this.t(30);
                        break block0;
                    }
                    case 3: {
                        this.b(0, false);
                        this.gH = 34;
                        this.t(30);
                        break block0;
                    }
                    case 4: {
                        this.b(1, false);
                        this.gH = 33;
                        this.t(30);
                        break block0;
                    }
                    case 5: {
                        this.b(1, false);
                        this.gH = 34;
                        this.t(30);
                        break block0;
                    }
                    case 6: {
                        this.b(2, false);
                        this.gH = 33;
                        this.t(30);
                        break block0;
                    }
                    case 7: {
                        this.b(2, false);
                        this.gH = 34;
                        this.t(30);
                        break block0;
                    }
                    case 8: {
                        this.b(0, true);
                        this.gH = 33;
                        this.t(30);
                        break block0;
                    }
                    case 9: {
                        this.b(0, true);
                        this.gH = 34;
                        this.t(30);
                        break block0;
                    }
                    case 10: {
                        this.b(1, true);
                        this.gH = 33;
                        this.t(30);
                        break block0;
                    }
                    case 11: {
                        this.b(1, true);
                        this.gH = 34;
                        this.t(30);
                        break block0;
                    }
                    case 12: {
                        this.b(2, true);
                        this.gH = 33;
                        this.t(30);
                        break block0;
                    }
                    case 13: {
                        this.b(2, true);
                        this.gH = 34;
                        this.t(30);
                        break block0;
                    }
                    case 14: {
                        this.y = false;
                        this.ab();
                        this.gH = 35;
                        this.t(30);
                        break block0;
                    }
                    case 15: {
                        this.y = true;
                        this.gH = 35;
                        this.t(30);
                        break block0;
                    }
                }
                this.t(30);
                this.bu = true;
                break;
            }
            case 32: 
            case 35: {
                if (!this.dS) {
                    this.gH = 31;
                    this.t(30);
                    break;
                }
                this.t(30);
                this.bu = true;
                break;
            }
            case 33: 
            case 34: {
                this.gH = 31;
                this.t(30);
                break;
            }
            case 40: {
                switch (n2) {
                    case 1: {
                        this.gH = this.hm[this.cZ] + 40;
                        this.cW = (byte)2;
                        break;
                    }
                    case 2: {
                        this.gH = this.fa[this.cZ] + 60;
                        this.cW = (byte)2;
                    }
                }
                this.t(30);
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
                    this.gH = 60;
                }
                if (this.cW == 2) {
                    this.gH = 40;
                }
                if (this.cW == 3) {
                    this.gH = 9;
                }
                if (this.cW == 4) {
                    this.gH = 8;
                }
                this.t(30);
                break;
            }
            case 60: {
                if (n2 % 2 == 1) {
                    this.cZ = this.gP[n2 / 2];
                    this.gH = this.hm[this.cZ] + 40;
                }
                if (n2 % 2 == 0) {
                    this.cZ = this.gP[(n2 - 1) / 2];
                    this.gH = this.fa[this.cZ] + 60;
                }
                this.cW = 1;
                this.t(30);
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
                    this.gH = 60;
                }
                if (this.cW == 2) {
                    this.gH = 40;
                }
                if (this.cW == 3) {
                    this.gH = 9;
                }
                if (this.cW == 4) {
                    this.gH = 8;
                }
                this.t(30);
                break;
            }
            case 28: {
                if (n2 == 41) {
                    this.ad = false;
                    this.t(120);
                    break;
                }
                if (this.d) {
                    this.cq = n2 == 40 ? 27 : 1;
                    this.o();
                    break;
                }
                this.gH = n2 == 40 ? 27 : 1;
                this.t(30);
                break;
            }
            case 27: {
                if (n2 == 40) {
                    this.ad = true;
                    this.t(120);
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
                if (this.d) {
                    this.cq = 28;
                    this.o();
                    break;
                }
                this.gH = 28;
                this.t(30);
            }
        }
        if (n > 80 && n < 100) {
            this.t(30);
        }
    }

    boolean K() {
        this.aQ = new short[this.hw][this.hr];
        this.fl = 10;
        this.dx = 0;
        this.b();
        this.aQ[this.fV][this.bN] = 20;
        this.C();
        if (this.aQ[this.fm][this.ax] < 20) {
            this.aQ = null;
            return false;
        }
        this.S();
        this.aQ = null;
        this.N();
        if (this.et == 0) {
            return false;
        }
        return this.dx != 0;
    }

    void S() {
        int n;
        int n2;
        int n3;
        boolean bl;
        int n4 = this.fm;
        int n5 = this.ax;
        boolean bl2 = true;
        do {
            bl = true;
            this.dX = n4;
            this.gO = n5;
            n3 = this.cI[n4][n5];
            if (n3 == 7 || n3 == 10 || n3 == 21 || n3 == 23 || n3 == 71) {
                this.o(n4 - 1, n5);
                this.o(n4 + 1, n5);
            }
            if (n3 == 8 || n3 == 9 || n3 == 22 || n3 == 24 || n3 == 70) {
                this.o(n4, n5 - 1);
                this.o(n4, n5 + 1);
            }
            if (n4 == this.fV && n5 == this.bN) {
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
            if (this.cI[n4][n5] == 7) {
                n2 = -1;
            }
            if (this.cI[n4][n5] == 8) {
                n = -1;
            }
            if (this.cI[n4][n5] == 9) {
                n = 1;
            }
            if (this.cI[n4][n5] == 10) {
                n2 = 1;
            }
            if (n == 0 && n2 == 0) continue;
            do {
                this.aQ[n4][n5] = 9;
            } while (this.aQ[n4 += n][n5 += n2] == 3);
        } while (bl);
        for (n3 = 0; n3 < this.hw; ++n3) {
            for (n2 = 0; n2 < this.hr; ++n2) {
                if (this.aQ[n3][n2] == 9) continue;
                this.aQ[n3][n2] = 0;
            }
        }
        if (bl2) {
            for (n3 = 0; n3 < this.hw; ++n3) {
                for (n2 = 0; n2 < this.hr; ++n2) {
                    boolean bl3;
                    boolean bl4;
                    boolean bl5;
                    if (this.aQ[n3][n2] != 9) continue;
                    n = n3 > 0 && this.aQ[n3 - 1][n2] == 9 ? 1 : 0;
                    bl5 = n3 < this.hw - 1 && this.aQ[n3 + 1][n2] == 9;
                    bl4 = n2 > 0 && this.aQ[n3][n2 - 1] == 9;
                    bl3 = n2 < this.hr - 1 && this.aQ[n3][n2 + 1] == 9;
                    if ((n != 0 ? 1 : 0) + (bl5 ? 1 : 0) + (bl4 ? 1 : 0) + (bl3 ? 1 : 0) == 1) {
                        n = this.a(n != 0, n3 - 1, n2, 71, 72, 74, 78, 80, 84, 88, 90, true) ? 1 : 0;
                        bl5 = this.a(bl5, n3 + 1, n2, 71, 73, 75, 78, 80, 85, 88, 90, true);
                        bl4 = this.a(bl4, n3, n2 - 1, 70, 73, 74, 77, 79, 83, 87, 89, false);
                        bl3 = this.a(bl3, n3, n2 + 1, 70, 72, 75, 77, 79, 86, 87, 89, false);
                    }
                    byte by = this.cI[n3][n2];
                    boolean bl9 = false;
                    if (this.cI[n3][n2] != 0 && this.cI[n3][n2] > 28) {
                        this.dx += this.cw;
                    }
                    int n7 = this.dx;
                    if (n != 0 && bl5) {
                        if (this.cI[n3][n2] == 22) {
                            this.cI[n3][n2] = 78;
                        } else if (this.cI[n3][n2] == 24) {
                            this.cI[n3][n2] = 80;
                        } else if (this.cI[n3][n2] == 9) {
                            this.cI[n3][n2] = 85;
                            this.dx += this.fb;
                        } else if (this.cI[n3][n2] == 8) {
                            this.cI[n3][n2] = 84;
                            this.dx += this.fb;
                        } else if (this.cI[n3][n2] == 70) {
                            this.cI[n3][n2] = 76;
                            bl9 = true;
                            this.ea[n3][n2] = (byte)(0x40 | this.ea[n3][n2] & 0x3F | this.bW << 3);
                        } else if (this.bK[n3][n2] == -1) {
                            this.cI[n3][n2] = 81;
                            this.dx += this.ba;
                        } else {
                            this.cI[n3][n2] = 71;
                        }
                    } else if (bl4 && bl3) {
                        if (this.cI[n3][n2] == 21) {
                            this.cI[n3][n2] = 77;
                        } else if (this.cI[n3][n2] == 23) {
                            this.cI[n3][n2] = 79;
                        } else if (this.cI[n3][n2] == 7) {
                            this.cI[n3][n2] = 83;
                            this.dx += this.fb;
                        } else if (this.cI[n3][n2] == 10) {
                            this.cI[n3][n2] = 86;
                            this.dx += this.fb;
                        } else if (this.cI[n3][n2] == 71) {
                            this.cI[n3][n2] = 76;
                            bl9 = true;
                            this.ea[n3][n2] = (byte)(0x40 | (this.ea[n3][n2] & 0x3F) << 3 | this.bW);
                        } else if (this.bK[n3][n2] == -1) {
                            this.cI[n3][n2] = 82;
                            this.dx += this.ba;
                        } else {
                            this.cI[n3][n2] = 70;
                        }
                    } else if (n != 0 && bl4) {
                        this.cI[n3][n2] = 75;
                    } else if (bl5 && bl4) {
                        this.cI[n3][n2] = 72;
                    } else if (n != 0 && bl3) {
                        this.cI[n3][n2] = 73;
                    } else if (bl5 && bl3) {
                        this.cI[n3][n2] = 74;
                    }
                    if (!bl9 && by != this.cI[n3][n2]) {
                        this.ea[n3][n2] = (byte)(0x40 | this.bW);
                        if (n7 == this.dx) {
                            this.dx += this.eH;
                        }
                    }
                    if (!bl9) continue;
                    this.dx += this.cz + this.eH + this.cw;
                }
            }
        }
    }

    boolean a(boolean bl, int n, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, boolean bl2) {
        if (!bl && n >= 0 && n <= this.hw - 1 && n2 >= 0 && n2 <= this.hr - 1) {
            byte by = this.cI[n][n2];
            int n11 = this.ea[n][n2] & 0x3F;
            n11 = by == 76 && bl2 ? (n11 >>= 3) : (n11 &= 7);
            if (n11 == this.df && (by == n3 || by == n4 || by == n5 || by == n6 || by == n7 || by == n8 || by == n9 || by == n10 || by == 76)) {
                bl = true;
            }
        }
        return bl;
    }

    boolean b(int n, int n2, int n3) {
        boolean bl = false;
        if (n >= 0 && n < this.hw && n2 >= 0 && n2 < this.hr && this.aQ[n][n2] >= 20 && this.aQ[n][n2] < n3) {
            bl = true;
            this.dX = n;
            this.gO = n2;
        }
        return bl;
    }

    void o(int n, int n2) {
        if (n > 0 && n < this.hw && n2 > 0 && n2 < this.hr && (this.cI[n][n2] >= 7 && this.cI[n][n2] <= 10 || this.cI[n][n2] >= 21 && this.cI[n][n2] <= 24 || this.cI[n][n2] == 70 || this.cI[n][n2] == 71) && this.aQ[n][n2] > 0) {
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
            for (n3 = this.fV; n3 >= 0; --n3) {
                for (n2 = this.bN; n2 >= 0; --n2) {
                    n = this.h(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
            for (n3 = this.fV; n3 < this.hw; ++n3) {
                for (n2 = this.bN; n2 < this.hr; ++n2) {
                    n = this.h(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
            for (n3 = this.fV; n3 >= 0; --n3) {
                for (n2 = this.bN; n2 < this.hr; ++n2) {
                    n = this.h(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
            for (n3 = this.fV; n3 < this.hw; ++n3) {
                for (n2 = this.bN; n2 >= 0; --n2) {
                    n = this.h(n3, n2);
                    bl |= n == 0;
                    bl2 |= n == 2;
                }
            }
        }
    }

    int h(int n, int n2) {
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
                    if (this.cI[n][n2] == 7) {
                        n5 = -1;
                    }
                    if (this.cI[n][n2] == 8) {
                        n6 = -1;
                    }
                    if (this.cI[n][n2] == 9) {
                        n6 = 1;
                    }
                    if (this.cI[n][n2] == 10) {
                        n5 = 1;
                    }
                    if (n6 != 0 || n5 != 0) {
                        int n9 = n4;
                        do {
                            n9 = (short)(n9 + 1);
                        } while (this.aQ[n7 += n6][n8 += n5] == 3);
                        this.aQ[n7][n8] = (short)n9;
                        bl = true;
                    }
                }
                this.aQ[n][n2] = (short)n4;
            }
        } else {
            bl = true;
        }
        return this.aQ[this.fm][this.ax] >= 20 ? 2 : (bl ? 0 : 1);
    }

    short h(int n, int n2, int n3) {
        short s = (short)n3;
        if (n >= 0 && n < this.hw && n2 >= 0 && n2 < this.hr && this.aQ[n][n2] >= 20 && this.aQ[n][n2] < s) {
            s = this.aQ[n][n2];
        }
        return s;
    }

    void J() {
        for (int i = 0; i < this.hw; ++i) {
            for (int j = 0; j < this.hr; ++j) {
                if (this.ea[i][j] == 64) {
                    this.cI[i][j] = (byte)(this.cI[i][j] == 78 ? 22 : (this.cI[i][j] == 80 ? 24 : (this.cI[i][j] == 85 ? 9 : (this.cI[i][j] == 84 ? 8 : (this.cI[i][j] == 77 ? 21 : (this.cI[i][j] == 79 ? 23 : (this.cI[i][j] == 83 ? 7 : (this.cI[i][j] == 86 ? 10 : 0))))))));
                    this.ea[i][j] = 0;
                }
                if (this.cI[i][j] != 76) continue;
                int n = this.ea[i][j] & 7;
                int n2 = (this.ea[i][j] & 0x3F) >> 3;
                if (n == 0 && n2 == 0) {
                    this.cI[i][j] = 0;
                    this.ea[i][j] = 0;
                    continue;
                }
                if (n == 0) {
                    this.cI[i][j] = 71;
                    this.ea[i][j] = (byte)(0x40 | n2);
                    continue;
                }
                if (n2 != 0) continue;
                this.cI[i][j] = 70;
                this.ea[i][j] = (byte)(0x40 | n);
            }
        }
    }

    void N() {
        this.et = 0;
        for (int i = 0; i < this.hw; ++i) {
            for (int j = 0; j < this.hr; ++j) {
                if (this.ea[i][j] == 64) {
                    ++this.et;
                }
                if (this.cI[i][j] != 76) continue;
                int n = this.ea[i][j] & 7;
                int n2 = (this.ea[i][j] & 0x3F) >> 3;
                if (n == 0) {
                    ++this.et;
                }
                if (n2 != 0) continue;
                ++this.et;
            }
        }
    }

    void n(int n) {
        for (int i = 0; i < this.hw; ++i) {
            for (int j = 0; j < this.hr; ++j) {
                if (this.ea[i][j] == 64) {
                    if (n == 1) {
                        this.d(i, j, -4);
                    }
                    this.ea[i][j] = (byte)(0x40 | n);
                }
                if (this.cI[i][j] != 76) continue;
                int n2 = this.ea[i][j] & 7;
                int n3 = (this.ea[i][j] & 0x3F) >> 3;
                if (n2 == 0) {
                    n2 = n;
                }
                if (n3 == 0) {
                    n3 = n;
                }
                this.ea[i][j] = (byte)(0x40 | n2 | n3 << 3);
            }
        }
    }

    void b() {
        int n;
        int n2;
        for (n2 = 0; n2 < this.hw; ++n2) {
            for (n = 0; n < this.hr; ++n) {
                this.aQ[n2][n] = 0;
            }
        }
        for (n2 = 0; n2 < this.hw; ++n2) {
            for (n = 0; n < this.hr; ++n) {
                int n3;
                boolean bl;
                boolean bl2;
                int n4;
                if (this.aQ[n2][n] != 0) continue;
                short s = 0;
                if (this.ea[n2][n] == 0) {
                    s = 1;
                }
                byte by = this.cI[n2][n];
                if (s == 1) {
                    if (this.bK[n2][n] == -1) {
                        s = 0;
                    }
                    if (by != 0 && by != 21 && by != 22 && by != 23 && by != 24 && by != 29 && by != 30 && by != 31 && by != 32 && by != 33) {
                        s = 0;
                    }
                }
                if (by == 71 && n2 > 0 && n2 < this.hw - 1 && this.ea[n2][n] == this.ea[n2 - 1][n] && this.ea[n2][n] == this.ea[n2 + 1][n]) {
                    s = 1;
                }
                if (by == 70 && n > 0 && n < this.hr - 1 && this.ea[n2][n] == this.ea[n2][n - 1] && this.ea[n2][n] == this.ea[n2][n + 1]) {
                    s = 1;
                }
                this.aQ[n2][n] = s;
                if (this.cI[n2][n] == 7) {
                    n4 = n;
                    bl2 = false;
                    do {
                        bl = false;
                        if (this.bK[n2][--n4] != -1 || this.cI[n2][n4] != 0 || this.ea[n2][n4] != 0) {
                            bl = true;
                        }
                        if (n4 == 0) {
                            bl = true;
                        }
                        if (this.cI[n2][n4] != 10) continue;
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
                if (this.cI[n2][n] != 8) continue;
                n4 = n2;
                bl2 = false;
                do {
                    bl = false;
                    if (this.bK[--n4][n] != -1 || this.cI[n4][n] != 0 || this.ea[n4][n] != 0) {
                        bl = true;
                    }
                    if (n4 == 0) {
                        bl = true;
                    }
                    if (this.cI[n4][n] != 9) continue;
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
        while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < this.hw && n2 < this.hr) {
            if (this.cI[n][n2] == n5) {
                bl = true;
            }
            if (this.cI[n][n2] == 0 && this.ea[n][n2] == 0) continue;
            break;
        }
        return bl;
    }

    boolean b(int n, int n2) {
        byte by = this.cI[n][n2];
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
        return this.bK[n][n2] >= 0 && (this.cI[n][n2] == 0 || this.cI[n][n2] >= 29 && this.cI[n][n2] <= 33) && this.ea[n][n2] == 0;
    }

    boolean r(int n, int n2) {
        if (this.cI[n][n2] >= 29 && this.cI[n][n2] <= 33 || this.cI[n][n2] >= 70 && this.cI[n][n2] <= 80 && this.ea[n][n2] == 65 || this.cI[n][n2] == 76 && (this.ea[n][n2] & 0x47) == 65 || this.cI[n][n2] == 76 && (this.ea[n][n2] & 0x78) == 72 || (this.cI[n][n2] == 87 || this.cI[n][n2] == 88 || this.cI[n][n2] == 89 || this.cI[n][n2] == 90) && this.ea[n][n2] == 65) {
            int n3 = this.s(n, n2);
            for (int i = 0; i < 500; ++i) {
                if ((this.ak[i] & 0x80) != 128) continue;
                int n4 = this.c(this.eo[i]);
                int n5 = this.c(this.af[i]);
                if (n == n4 && n2 == n5) {
                    return false;
                }
                if (n3 == 5000 || n3 != this.s(n4, n5)) continue;
                return false;
            }
            return true;
        }
        if (this.cI[n][n2] >= 81 && this.cI[n][n2] <= 86 && this.ea[n][n2] == 65) {
            for (int i = 0; i < 500; ++i) {
                int n6;
                if ((this.ak[i] & 0x80) != 128) continue;
                if (this.cI[n][n2] == 81 || this.cI[n][n2] == 84 || this.cI[n][n2] == 85) {
                    for (n6 = n; n6 < this.hw && (this.cI[n6][n2] == 81 || this.cI[n6][n2] == 84 || this.cI[n6][n2] == 85); ++n6) {
                        if (this.c(this.eo[i]) != n6 || this.c(this.af[i]) != n2) continue;
                        return false;
                    }
                    for (n6 = n - 1; n6 >= 0 && (this.cI[n6][n2] == 81 || this.cI[n6][n2] == 84 || this.cI[n6][n2] == 85); --n6) {
                        if (this.c(this.eo[i]) != n6 || this.c(this.af[i]) != n2) continue;
                        return false;
                    }
                    continue;
                }
                for (n6 = n2; n6 < this.hr && (this.cI[n][n6] == 82 || this.cI[n][n6] == 83 || this.cI[n][n6] == 86); ++n6) {
                    if (this.c(this.eo[i]) != n || this.c(this.af[i]) != n6) continue;
                    return false;
                }
                for (n6 = n2 - 1; n6 >= 0 && (this.cI[n][n6] == 82 || this.cI[n][n6] == 83 || this.cI[n][n6] == 86); --n6) {
                    if (this.c(this.eo[i]) != n || this.c(this.af[i]) != n6) continue;
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
        if (n < 0 || n2 < 0 || n >= this.hw || n2 >= this.hr) {
            return false;
        }
        byte by = this.cI[n][n2];
        boolean bl = false;
        if (this.bK[n][n2] >= 0 && (by == 0 || by >= 29 && by <= 33) && this.ea[n][n2] == 0) {
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

    int s(int n, int n2) {
        for (int i = 0; i < 250; ++i) {
            if ((this.hv[i] & 0x80) != 128) continue;
            if (this.c(this.aj[i]) == n && this.c(this.eq[i]) == n2) {
                return i;
            }
            if ((this.c(this.aj[i]) == n - 1 || this.c(this.aj[i]) == n + 1) && this.c(this.eq[i]) == n2 && (this.hv[i] & 0xE0) == 224) {
                return i;
            }
            if (this.c(this.eq[i]) != n2 - 1 && this.c(this.eq[i]) != n2 + 1 || this.c(this.aj[i]) != n || (this.hv[i] & 0xE0) != 160) continue;
            return i;
        }
        return 5000;
    }

    int a(int n, int n2, int n3, boolean bl, boolean bl2) {
        int n4;
        int n5 = 5000;
        for (n4 = 0; n4 < 250; ++n4) {
            if ((this.hv[n4] & 0x80) != 0) continue;
            n5 = n4;
            break;
        }
        if (n5 != 5000) {
            this.hv[n5] = 0;
            int n6 = n5;
            this.hv[n6] = (byte)(this.hv[n6] | 0x80);
            int n7 = n5;
            this.hv[n7] = (byte)(this.hv[n7] | (bl ? 64 : 0));
            int n8 = n5;
            this.hv[n8] = (byte)(this.hv[n8] | (bl2 ? 32 : 0));
            int n9 = n5;
            this.hv[n9] = (byte)(this.hv[n9] | n);
            this.aj[n5] = (byte)n2;
            this.eq[n5] = (byte)n3;
            this.hq[n5] = 0;
            for (n4 = 0; n4 < 5; ++n4) {
                this.cO[n5][n4] = 0;
            }
        }
        return n5;
    }

    void af() {
        switch (this.gY) {
            case 0: {
                this.s = this.aG;
                break;
            }
            case 1: {
                this.s = this.eI;
                break;
            }
            case 2: {
                this.s = this.au;
                break;
            }
            case 3: {
                this.s = this.bZ;
                break;
            }
            case 4: {
                this.s = this.fC;
                break;
            }
            case 5: {
                this.s = this.ai;
            }
        }
        if (this.gY == this.hg) {
            this.s = this.s * this.eS / 100;
        }
        block14: for (int i = 0; i < 5; ++i) {
            switch (this.gC[i]) {
                case 0: {
                    this.s += this.fH * (this.gC[i] == this.U[i] ? this.eS : 1) / (this.gC[i] == this.U[i] ? 100 : 1);
                    continue block14;
                }
                case 1: {
                    this.s += this.F * (this.gC[i] == this.U[i] ? this.eS : 1) / (this.gC[i] == this.U[i] ? 100 : 1);
                    continue block14;
                }
                case 2: {
                    this.s += this.eP * (this.gC[i] == this.U[i] ? this.eS : 1) / (this.gC[i] == this.U[i] ? 100 : 1);
                    continue block14;
                }
                case 3: {
                    this.s += this.fB * (this.gC[i] == this.U[i] ? this.eS : 1) / (this.gC[i] == this.U[i] ? 100 : 1);
                }
            }
        }
    }

    void r(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.gC[i] != -1) continue;
            this.gC[i] = n;
            break;
        }
    }

    void V() {
        for (int i = 4; i >= 0; --i) {
            if (this.gC[i] == -1) continue;
            this.gC[i] = -1;
            break;
        }
    }

    boolean y() {
        if (this.eU >= this.s) {
            int n = this.b(this.ek, this.I, this.gY < 3 ? 0 : 1, this.gY, (this.hv[this.fD] & 0x40) == 64 ? 2 : 0, 8 - this.gY);
            for (int i = 0; i < 5; ++i) {
                if (this.gC[i] < 0) continue;
                int n2 = 2 + this.gC[i];
                if (n2 == 5) {
                    ++n2;
                }
                n = this.l(n, n2);
                this.w(n2);
            }
            this.eU -= this.s;
            this.gE += this.s;
            this.p(-this.s);
            ++this.gu;
            return true;
        }
        this.M();
        return false;
    }

    void e(boolean bl) {
        for (int i = 0; i < 250; ++i) {
            int n;
            int n2;
            if ((this.hv[i] & 0x80) != 128) continue;
            int n3 = this.c(this.aj[i]);
            int n4 = this.c(this.eq[i]);
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
            if (n7 >= this.hw) {
                n7 = this.hw - 1;
            }
            if (n8 >= this.hr) {
                n8 = this.hr - 1;
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
                    if (this.cI[n2][n] >= 34 && this.cI[n2][n] <= 40) {
                        if ((this.ea[n2][n] & 0xC0) == 128 && !bl && vector.indexOf(new Integer(n11 = this.ea[n2][n] & 0x3F)) == -1) {
                            vector.addElement(new Integer(n11));
                            if ((this.hq[i] & 1) == 1) {
                                short[] sArray = this.cO[i];
                                sArray[0] = (short)(sArray[0] + this.gN[n11] * (this.dC[n11] + 10 + this.gN[n11] / 10 + this.gm.nextInt() % 40) / 170);
                            }
                            if ((this.hq[i] & 0x10) == 16) {
                                short[] sArray = this.cO[i];
                                sArray[4] = (short)(sArray[4] + this.gN[n11] / 2 * (this.dC[n11] + 10 + this.gN[n11] / 10 + Math.abs(this.gm.nextInt() % 40)) / 170);
                            }
                            int n12 = n11;
                            this.gN[n12] = (short)(this.gN[n12] + this.gm.nextInt() % 2);
                        }
                        ++n9;
                    }
                    if (this.cI[n2][n] >= 65 && this.cI[n2][n] <= 68 && !bl && (this.ea[n2][n] & 0xC0) == 192 && vector2.indexOf(new Integer(n11 = this.ea[n2][n] & 0x3F)) == -1) {
                        vector2.addElement(new Integer(n11));
                        n10 = this.cV[n11] * 4 / 30;
                        n10 = n10 / this.be[n11] == 0 ? 1 : n10 / this.be[n11];
                        short[] sArray = this.cO[i];
                        sArray[1] = (short)(sArray[1] + n10);
                        int n13 = n11;
                        this.bb[n13] = (short)(this.bb[n13] + n10);
                        int n14 = i;
                        this.gW[n14] = (byte)(this.gW[n14] | 4);
                    }
                    if (this.cI[n2][n] >= 53 && this.cI[n2][n] <= 55 && !bl && (this.ea[n2][n] & 0xC0) == 192 && vector3.indexOf(new Integer(n11 = this.ea[n2][n] & 0x3F)) == -1) {
                        vector3.addElement(new Integer(n11));
                        n10 = this.cV[n11] * 4 / 30;
                        n10 = n10 / this.be[n11] == 0 ? 1 : n10 / this.be[n11];
                        short[] sArray = this.cO[i];
                        sArray[2] = (short)(sArray[2] + n10);
                        int n15 = n11;
                        this.bb[n15] = (short)(this.bb[n15] + n10);
                        int n16 = i;
                        this.gW[n16] = (byte)(this.gW[n16] | 8);
                    }
                    if (this.cI[n2][n] >= 59 && this.cI[n2][n] <= 61 && !bl && (this.ea[n2][n] & 0xC0) == 192 && vector4.indexOf(new Integer(n11 = this.ea[n2][n] & 0x3F)) == -1) {
                        vector4.addElement(new Integer(n11));
                        n10 = this.cV[n11] * 4 / 30;
                        n10 = n10 / this.be[n11] == 0 ? 1 : n10 / this.be[n11];
                        short[] sArray = this.cO[i];
                        sArray[3] = (short)(sArray[3] + n10);
                        int n17 = n11;
                        this.bb[n17] = (short)(this.bb[n17] + n10);
                        int n18 = i;
                        this.gW[n18] = (byte)(this.gW[n18] | 2);
                    }
                    if (this.cI[n2][n] >= 50 && this.cI[n2][n] <= 52) {
                        int n19 = i;
                        this.hq[n19] = (byte)(this.hq[n19] | 4);
                    }
                    if (this.cI[n2][n] >= 56 && this.cI[n2][n] <= 58) {
                        int n20 = i;
                        this.hq[n20] = (byte)(this.hq[n20] | 8);
                    }
                    if (this.cI[n2][n] < 62 || this.cI[n2][n] > 64) continue;
                    int n21 = i;
                    this.hq[n21] = (byte)(this.hq[n21] | 2);
                }
            }
            if (n9 / 3 > 0) {
                int n22 = i;
                this.hq[n22] = (byte)(this.hq[n22] | 1);
                int n23 = i;
                this.gW[n23] = (byte)(this.gW[n23] | 1);
            }
            if (n9 / 7 > 0) {
                int n24 = i;
                this.hq[n24] = (byte)(this.hq[n24] | 0x10);
                int n25 = i;
                this.gW[n25] = (byte)(this.gW[n25] | 0x10);
            }
            n2 = (this.hv[i] & 0x20) == 32 ? 1 : 0;
            for (n = 0; n < 5; ++n) {
                if (n2 != 0) {
                    if (n == 0) {
                        if (this.cO[i][n] <= 250) continue;
                        this.cO[i][n] = 250;
                        continue;
                    }
                    if (n == 4) {
                        if (this.cO[i][n] <= 325) continue;
                        this.cO[i][n] = 325;
                        continue;
                    }
                    if (this.cO[i][n] <= 450) continue;
                    this.cO[i][n] = 450;
                    continue;
                }
                if (n == 0) {
                    if (this.cO[i][n] <= 100) continue;
                    this.cO[i][n] = 100;
                    continue;
                }
                if (n == 4) {
                    if (this.cO[i][n] <= 150) continue;
                    this.cO[i][n] = 150;
                    continue;
                }
                if (this.cO[i][n] <= 200) continue;
                this.cO[i][n] = 200;
            }
        }
    }

    int l(int n, int n2) {
        int n3 = 500;
        for (int i = 0; i < 500; ++i) {
            if ((this.ak[i] & 0x80) != 0) continue;
            n3 = i;
            break;
        }
        if (n3 != 500) {
            this.Y[n] = (short)n3;
            this.ak[n3] = -128;
            int n4 = n3;
            this.ak[n4] = (byte)(this.ak[n4] | n2);
            this.Y[n3] = -1;
            this.eo[n3] = this.eo[n];
            this.af[n3] = this.af[n];
            this.ey[n3] = this.ey[n];
            this.u[n3] = (byte)(this.u[n] - 8);
            this.p[n3] = 0;
            this.eT[n3] = 0;
            this.aI[n3] = -1;
        }
        this.bH = -1;
        return n3;
    }

    int b(int n, int n2, int n3, int n4, int n5, int n6) {
        int n7 = 500;
        for (int i = 0; i < 500; ++i) {
            if ((this.ak[i] & 0x80) != 0) continue;
            n7 = i;
            break;
        }
        if (n7 != 500) {
            this.ak[n7] = -64;
            int n8 = n7;
            this.ak[n8] = (byte)(this.ak[n8] | n3);
            this.Y[n7] = -1;
            this.eo[n7] = (byte)n;
            this.af[n7] = (byte)n2;
            this.ey[n7] = (byte)n5;
            this.eT[n7] = (byte)n6;
            this.u[n7] = 0;
            this.p[n7] = 1;
            this.aI[n7] = -1;
            this.g[n7] = (byte)n4;
            this.aw[n7] = 0;
            this.eL[n7] = 7;
            this.bz[n7] = 0;
        }
        this.bH = -1;
        return n7;
    }

    void w() {
        int n = 0;
        while (n < 500) {
            int n2 = n++;
            this.aw[n2] = (byte)(this.aw[n2] + 1);
        }
    }

    void q() {
        if (this.eA[0] == 0) {
            for (int i = 0; i < 500; ++i) {
                int n;
                int n2;
                if ((this.ak[i] & 0x80) != 128 || (this.ak[i] & 0x40) != 64 || this.ea[n2 = this.c(this.eo[i])][n = this.c(this.af[i])] != 65) continue;
                int n3 = this.A(i) * 2;
                if ((this.ak[i] & 0xF) == 0) {
                    n3 += 3;
                } else if ((this.ak[i] & 0xF) == 1) {
                    n3 += 7;
                }
                int n4 = i;
                this.dT[n4] = this.dT[n4] - n3;
                this.eU -= n3;
                this.cF += n3;
            }
        }
    }

    boolean i(int n, int n2) {
        short s = this.Y[n];
        if (s != -1) {
            if ((this.ak[s] & 0x40) == 0) {
                int n3 = this.ak[s] & 0xF;
                switch (n3) {
                    case 2: {
                        if ((this.gW[n2] & 1) == 1 && this.p[s] < 25) {
                            return true;
                        }
                        if ((this.gW[n2] & 0x10) != 16 || this.eT[s] >= 15) break;
                        return true;
                    }
                    case 3: {
                        if ((this.gW[n2] & 4) != 4 || this.p[s] >= 25) break;
                        return true;
                    }
                    case 4: 
                    case 5: {
                        if ((this.gW[n2] & 8) != 8 || this.p[s] >= 30) break;
                        return true;
                    }
                    case 6: 
                    case 7: {
                        if ((this.gW[n2] & 2) != 2 || this.p[s] >= 30) break;
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

    void y(int n) {
        int n2;
        if ((this.ak[n] & 0x40) == 0 || this.c(this.p[n]) <= 0) {
            int n3;
            int n4;
            int n5;
            int n6;
            int n7;
            int n8;
            n2 = 0;
            int n9 = n;
            this.ak[n9] = (byte)(this.ak[n9] & 0xEF);
            if ((this.ak[n] & 0x40) == 64) {
                this.p[n] = (byte)(this.eT[n] + this.B(n) / 2 + this.A(n) / 2 + this.aw[n] / 3);
            }
            int n10 = this.c(this.eo[n]);
            int n11 = this.c(this.af[n]);
            if ((this.ak[n] & 0x40) == 64 && this.u[n] == 4 && (this.cI[n10][n11] == 87 || this.cI[n10][n11] == 88) && this.ea[n10][n11] == 65) {
                n8 = this.s(n10, n11);
                n7 = 0;
                if (n8 != this.aI[n]) {
                    n7 = this.x(n, n8);
                }
                if (this.eL[n] != 7) {
                    this.ak[n] = -64;
                    int n12 = n;
                    this.ak[n12] = (byte)(this.ak[n12] | (byte)((this.eL[n] & 7) >= 3 ? 1 : 0));
                    this.eT[n] = (byte)(8 - (this.eL[n] & 7));
                    this.g[n] = (byte)(this.eL[n] & 7);
                    this.aw[n] = 0;
                    n6 = this.Y[n];
                    while (n6 != -1) {
                        this.ak[n6] = 0;
                        n6 = this.Y[n6];
                    }
                    n5 = n;
                    for (n4 = 0; n4 < 5; ++n4) {
                        n3 = this.eL[n] >> 3 + 3 * n4 & 7;
                        if (n3 >= 7) continue;
                        int n13 = 2 + n3;
                        if (n13 == 5) {
                            ++n13;
                        }
                        n5 = this.l(n5, n13);
                    }
                    this.eL[n] = 7;
                    this.bH = -1;
                }
                this.b(n, n8, false);
                if (this.bz[n] == 1 && this.i(n, n8)) {
                    this.u[n] = 3;
                    n2 = 1;
                }
                if (n8 != this.aI[n]) {
                    if (n7 > 0) {
                        this.eU += n7;
                        this.p(n7);
                        this.gr += n7;
                        int n14 = n;
                        this.dT[n14] = this.dT[n14] + n7;
                        this.h(n7);
                    }
                    this.p[n] = (byte)(this.A(n) * 40);
                }
                this.aI[n] = (byte)n8;
                int n15 = n;
                this.ak[n15] = (byte)(this.ak[n15] | 0x10);
            }
            if ((this.ak[n] & 0x40) == 64 && this.u[n] == 4 && (this.cI[n10][n11] == 87 || this.cI[n10][n11] == 88) && this.ea[n10][n11] != 65) {
                this.p[n] = (byte)(this.A(n) * 40);
            }
            n8 = 0;
            if (this.u[n] < 8) {
                int n16 = n;
                this.u[n16] = (byte)(this.u[n16] + 1);
            } else {
                byte[] byArray;
                if (this.c(this.A[n]) < 200) {
                    int n17 = n;
                    this.A[n17] = (byte)(this.A[n17] + 1);
                }
                n10 = this.eo[n];
                n11 = this.af[n];
                n7 = this.ey[n];
                this.u[n] = 0;
                n6 = this.c(this.eo[n]);
                n5 = this.c(this.af[n]);
                n4 = this.ea[n6][n5];
                n3 = this.cI[n6][n5] - 70;
                if (n3 == 6 && (this.ey[n] == 0 || this.ey[n] == 1)) {
                    n4 = 0x40 | n4 & 7;
                    n3 = 0;
                }
                if (n3 == 6 && (this.ey[n] == 2 || this.ey[n] == 3)) {
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
                if ((n6 += (byArray = this.eR[n3][0] == this.ey[n] ? this.eR[n3] : this.aH[n3])[2]) >= 0 && (n5 += byArray[3]) >= 0 && n6 < this.hw && n5 < this.hr) {
                    int n18;
                    boolean bl = false;
                    int n19 = this.ea[n6][n5];
                    int n20 = this.cI[n6][n5] - 70;
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
                        this.eo[n] = (byte)n6;
                        this.af[n] = (byte)n5;
                        this.ey[n] = byArray[2 * n18 + 1];
                        bl = true;
                    }
                    if (!bl) {
                        this.ey[n] = byArray[1];
                        this.f(n, this.Y[n], 0);
                    }
                    if (bl && (this.ak[n] & 0x40) == 64 && n20 == 6) {
                        n18 = 0;
                        for (int i = 0; i < 500; ++i) {
                            if ((this.ak[i] & 0x80) != 128 || i == n || this.eo[i] != this.eo[n] || this.af[i] != this.af[n]) continue;
                            n18 = 1;
                            break;
                        }
                        if (n18 != 0) {
                            this.eo[n] = (byte)n10;
                            this.af[n] = (byte)n11;
                            this.ey[n] = (byte)n7;
                            this.u[n] = 8;
                            n8 = 1;
                        }
                    }
                } else {
                    if (this.eR[n3][0] == this.ey[n]) {
                        this.ey[n] = this.eR[n3][1];
                    } else if (this.aH[n3][0] == this.ey[n]) {
                        this.ey[n] = this.aH[n3][1];
                    }
                    this.f(n, this.Y[n], 0);
                }
            }
            if (n2 == 0 && n8 == 0 && this.Y[n] != -1) {
                this.y(this.Y[n]);
            }
        }
        if ((this.ak[n] & 0x40) == 64) {
            n2 = this.c(this.p[n]);
            if ((n2 -= 3) < 0) {
                n2 = 0;
            }
            this.p[n] = (byte)n2;
        }
    }

    void f(int n, int n2, int n3) {
        if (n2 != -1) {
            this.f(n, this.Y[n2], n3 - 8);
            this.eo[n2] = this.eo[n];
            this.af[n2] = this.af[n];
            this.ey[n2] = this.ey[n];
            this.u[n2] = (byte)(n3 - 8);
        }
    }

    int A(int n) {
        if (n != -1) {
            return this.A(this.Y[n]) + 1;
        }
        return 0;
    }

    int B(int n) {
        if (n != -1) {
            byte by = this.cI[this.c(this.eo[n])][this.c(this.af[n])];
            int n2 = 0;
            if (by == 77) {
                int n3 = n2 = this.ey[n] == 0 ? -1 : 1;
            }
            if (by == 79) {
                int n4 = n2 = this.ey[n] == 0 ? 1 : -1;
            }
            if (by == 78) {
                int n5 = n2 = this.ey[n] == 2 ? 1 : -1;
            }
            if (by == 80) {
                n2 = this.ey[n] == 2 ? -1 : 1;
            }
            return this.B(this.Y[n]) + n2;
        }
        return 0;
    }

    void a(int n, boolean bl) {
        if (n != -1) {
            this.a((int)this.Y[n], bl);
            int n2 = n;
            this.ak[n2] = (byte)(this.ak[n2] | 0xE0);
            this.Y[n] = -1;
            this.p[n] = 7;
            if (bl) {
                this.a(this.c(this.eo[n]), this.c(this.af[n]), false, true);
            }
            this.bH = -1;
            this.d = false;
            this.cq = 16;
            this.t(20);
        }
    }

    void b(Graphics graphics, int n) {
        int n2;
        int n3;
        int n4;
        short[] sArray = new short[8];
        System.arraycopy(this.cs[this.ak[n] & 0xF], 0, sArray, 0, 8);
        int n5 = this.c(this.eo[n]);
        int n6 = this.c(this.af[n]);
        if ((this.ak[n] & 0x40) == 64) {
            int n7;
            n4 = this.cI[n5][n6];
            n3 = this.c(this.ea[n5][n6]);
            n2 = 0;
            if (n4 != 76 && n3 > 65) {
                n2 = 1;
            }
            if (n4 == 76) {
                n7 = n3 & 7;
                int n8 = n3 >> 3 & 7;
                if ((this.ey[n] == 0 || this.ey[n] == 1) && n7 > 1) {
                    n2 = 1;
                }
                if ((this.ey[n] == 2 || this.ey[n] == 3) && n8 > 1) {
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
        n4 = this.c(this.cI[n5][n6]);
        n3 = n6 * 16 + n5 * 16 + 16 - this.cU;
        n2 = this.hj - n5 * 8 + n6 * 8 + 16 - this.hc - 4 * this.bK[n5][n6] + 15;
        if (this.u[n] >= 0) {
            switch (this.ey[n]) {
                case 0: {
                    if (n4 == 82) {
                        n2 -= 4;
                    }
                    if (n4 == 79) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[3]])], n3 - 8 + this.u[n] * 2, n2 - 11 + 4 + this.u[n] - this.u[n] / 2, 33);
                        break;
                    }
                    if (n4 == 77) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[3]])], n3 - 8 + this.u[n] * 2, n2 - 11 + this.u[n] + this.u[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[3]])], n3 - 8 + this.u[n] * 2, n2 - 11 + this.u[n], 33);
                    break;
                }
                case 1: {
                    if (n4 == 82) {
                        n2 -= 4;
                    }
                    if (n4 == 79) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[7]])], n3 + 8 - this.u[n] * 2, n2 - 3 - this.u[n] + this.u[n] / 2, 33);
                        break;
                    }
                    if (n4 == 77) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[7]])], n3 + 8 - this.u[n] * 2, n2 + 1 - this.u[n] - this.u[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[7]])], n3 + 8 - this.u[n] * 2, n2 - 3 - this.u[n], 33);
                    break;
                }
                case 2: {
                    if (n4 == 81) {
                        n2 -= 4;
                    }
                    if (n4 == 78) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[1]])], n3 - 8 + this.u[n] * 2, n2 + 1 - this.u[n] - this.u[n] / 2, 33);
                        break;
                    }
                    if (n4 == 80) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[1]])], n3 - 8 + this.u[n] * 2, n2 - 3 - this.u[n] + this.u[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[1]])], n3 - 8 + this.u[n] * 2, n2 - 3 - this.u[n], 33);
                    break;
                }
                case 3: {
                    if (n4 == 81) {
                        n2 -= 4;
                    }
                    if (n4 == 78) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[5]])], n3 + 8 - this.u[n] * 2, n2 - 11 + this.u[n] + this.u[n] / 2, 33);
                        break;
                    }
                    if (n4 == 80) {
                        graphics.drawImage(this.fU[this.c(this.gt[sArray[5]])], n3 + 8 - this.u[n] * 2, n2 - 11 + 4 + this.u[n] - this.u[n] / 2, 33);
                        break;
                    }
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[5]])], n3 + 8 - this.u[n] * 2, n2 - 11 + this.u[n], 33);
                    break;
                }
                case 4: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[2]])], n3 - 8 + this.u[n] * 2, n2 - 12, 33);
                    break;
                }
                case 5: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[6]])], n3 + 8 - this.u[n] * 2, n2 - 12, 33);
                    break;
                }
                case 6: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[2]])], n3 - 8 + this.u[n] * 2, n2 - 4, 33);
                    break;
                }
                case 7: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[6]])], n3 + 8 - this.u[n] * 2, n2 - 4, 33);
                    break;
                }
                case 8: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[0]])], n3 + 8, n2 - 11 + this.u[n], 33);
                    break;
                }
                case 9: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[4]])], n3 + 8, n2 - 3 - this.u[n], 33);
                    break;
                }
                case 10: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[0]])], n3 - 8, n2 - 11 + this.u[n], 33);
                    break;
                }
                case 11: {
                    graphics.drawImage(this.fU[this.c(this.gt[sArray[4]])], n3 - 8, n2 - 3 - this.u[n], 33);
                }
            }
        }
    }

    boolean j(int n, int n2, int n3) {
        if (n != -1) {
            if (this.c(this.eo[n]) == n2 && this.c(this.af[n]) == n3) {
                return true;
            }
            return this.j(this.Y[n], n2, n3);
        }
        return false;
    }

    void i(int n) {
        int n2 = -1;
        for (int i = 0; i < 500; ++i) {
            if (n == i || (this.ak[i] & 0x80) != 128 || (this.ak[i] & 0x40) != 64 || !this.j(i, this.c(this.eo[n]), this.c(this.af[n]))) continue;
            this.a(i, true);
            this.a(n, true);
        }
    }

    boolean i(int n, int n2, int n3) {
        if (n != -1) {
            if (this.c(this.eo[n]) == n2 && this.c(this.af[n]) == n3) {
                return true;
            }
            if (this.i(this.Y[n], n2, n3)) {
                return true;
            }
        }
        return false;
    }

    int c(int n, int n2) {
        for (int i = 0; i < 500; ++i) {
            if ((this.ak[i] & 0x80) != 128 || (this.ak[i] & 0x40) != 64 || !this.i(i, n, n2)) continue;
            return i;
        }
        return 5000;
    }

    int x(int n, int n2) {
        if (n != -1) {
            int n3 = 0;
            if ((this.ak[n] & 0x40) == 0) {
                int n4 = this.ak[n] & 0xF;
                if (this.aI[n] != n2 && (this.p[n] > 0 || this.eT[n] > 0)) {
                    switch (n4) {
                        case 2: {
                            int n5;
                            if ((this.hq[n2] & 1) == 1) {
                                n5 = this.p[n] * 3 * (this.c(this.A[n]) / 15 + 1);
                                if (this.eA[4] > 0) {
                                    n5 *= 2;
                                }
                                n3 += n5;
                                this.t(4, this.p[n]);
                                this.p[n] = 0;
                            }
                            if ((this.hq[n2] & 0x10) != 16) break;
                            n5 = this.eT[n] * 8 * (this.c(this.A[n]) / 15 + 1);
                            if (this.eA[5] > 0) {
                                n5 *= 2;
                            }
                            n3 += n5;
                            this.t(5, this.eT[n]);
                            this.eT[n] = 0;
                            break;
                        }
                        case 3: {
                            if ((this.hq[n2] & 2) != 2) break;
                            int n6 = this.p[n] * 10 * (this.c(this.A[n]) / 15 + 1);
                            if (this.eA[3] > 0) {
                                n6 *= 2;
                            }
                            n3 += n6;
                            this.t(3, this.p[n]);
                            this.p[n] = 0;
                            break;
                        }
                        case 4: 
                        case 5: {
                            if ((this.hq[n2] & 4) != 4) break;
                            int n7 = this.p[n] * 12 * (this.c(this.A[n]) / 15 + 1);
                            if (this.eA[2] > 0) {
                                n7 *= 2;
                            }
                            n3 += n7;
                            this.t(2, this.p[n]);
                            this.p[n] = 0;
                            break;
                        }
                        case 6: 
                        case 7: {
                            if ((this.hq[n2] & 8) != 8) break;
                            int n8 = this.p[n] * 13 * (this.c(this.A[n]) / 15 + 1);
                            if (this.eA[1] > 0) {
                                n8 *= 2;
                            }
                            n3 += n8;
                            this.t(1, this.p[n]);
                            this.p[n] = 0;
                        }
                    }
                }
            }
            return this.x(this.Y[n], n2) + n3;
        }
        return 0;
    }

    int g(int n, int n2, int n3) {
        int n4 = this.cO[n][n2] > n3 ? n3 : this.cO[n][n2];
        short[] sArray = this.cO[n];
        int n5 = n2;
        sArray[n5] = (short)(sArray[n5] - n4);
        return n4;
    }

    int c(int n, int n2, int n3) {
        int n4 = this.g(n, n2, n3);
        for (int i = 0; i < 250; ++i) {
            if (n == i || (this.hv[i] & 0x80) != 128 || (this.hv[i] & 7) != (this.hv[n] & 7) || Math.abs(this.aj[n] - this.aj[i]) >= 3 || Math.abs(this.eq[n] - this.eq[i]) >= 3) continue;
            n4 += this.g(i, n2, n3 - n4);
        }
        return n4;
    }

    void b(int n, int n2, boolean bl) {
        boolean bl2 = bl;
        if (n != -1) {
            boolean bl3 = false;
            if ((this.ak[n] & 0x40) == 0) {
                int n3 = this.ak[n] & 0xF;
                switch (n3) {
                    case 2: {
                        int n4 = n;
                        this.p[n4] = (byte)(this.p[n4] + this.c(n2, 0, 25 - this.p[n]));
                        int n5 = n;
                        this.eT[n5] = (byte)(this.eT[n5] + this.c(n2, 4, 15 - this.eT[n]));
                        this.A[n] = 0;
                        if (n2 == this.aI[n] || bl2) break;
                        this.d(this.c(this.eo[n]), this.c(this.af[n]), 4);
                        bl2 = true;
                        break;
                    }
                    case 3: {
                        int n6 = n;
                        this.p[n6] = (byte)(this.p[n6] + this.c(n2, 1, 25 - this.p[n]));
                        this.A[n] = 0;
                        break;
                    }
                    case 4: 
                    case 5: {
                        int n7 = n;
                        this.p[n7] = (byte)(this.p[n7] + this.c(n2, 2, 30 - this.p[n]));
                        this.A[n] = 0;
                        break;
                    }
                    case 6: 
                    case 7: {
                        int n8 = n;
                        this.p[n8] = (byte)(this.p[n8] + this.c(n2, 3, 30 - this.p[n]));
                        this.A[n] = 0;
                    }
                }
                if (n3 == 4 || n3 == 5) {
                    this.ak[n] = this.p[n] >= 10 ? (byte)(this.ak[n] & 0xF0 | 5) : (byte)(this.ak[n] & 0xF0 | 4);
                }
                if (n3 == 6 || n3 == 7) {
                    this.ak[n] = this.p[n] >= 10 ? (byte)(this.ak[n] & 0xF0 | 7) : (byte)(this.ak[n] & 0xF0 | 6);
                }
                this.aI[n] = (byte)n2;
            } else {
                this.A[n] = 0;
            }
            this.b((int)this.Y[n], n2, bl2);
        }
    }

    void o(int n) {
        if (n != -1) {
            this.l(this.ak[n] & 7);
            this.o(this.Y[n]);
        }
    }

    boolean a(int n, int n2, int[] nArray) {
        if (this.eU >= this.s - this.cfr_renamed_0) {
            this.eL[n] = (short)n2;
            this.o(this.Y[n]);
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
            this.eU -= this.s - this.cfr_renamed_0;
            this.gE += this.s - this.cfr_renamed_0;
            if (this.s - this.cfr_renamed_0 != 0) {
                this.p(-(this.s - this.cfr_renamed_0));
            }
            return true;
        }
        this.M();
        return false;
    }

    void y(int n, int n2) {
        this.fc = n;
        this.fL = n2;
        this.dl = 0;
        this.eK = 0;
        this.dd = false;
        if (this.fL == 1) {
            try {
                RecordStore recordStore = RecordStore.openRecordStore((String)"TTStore", (boolean)true);
                RecordEnumeration recordEnumeration = recordStore.enumerateRecords(null, null, false);
                for (int i = 0; i < recordEnumeration.numRecords(); ++i) {
                    int n3 = recordEnumeration.nextRecordId();
                    byte by = recordStore.getRecord(n3)[0];
                    if (by != n) continue;
                    this.dv = new byte[recordStore.getRecordSize(n3) - 1];
                    System.arraycopy(recordStore.getRecord(n3), 1, this.dv, 0, recordStore.getRecordSize(n3) - 1);
                    this.eK = recordStore.getRecordSize(n3) - 1;
                    this.dd = true;
                }
                recordEnumeration.destroy();
                recordStore.closeRecordStore();
            }
            catch (Exception exception) {}
        } else {
            this.dd = true;
        }
    }

    void k(int n) {
        if (this.eK > 0) {
            byte[] byArray = new byte[this.eK + n];
            System.arraycopy(this.dv, 0, byArray, 0, this.eK);
            this.eK += n;
            this.dv = new byte[this.eK];
            System.arraycopy(byArray, 0, this.dv, 0, this.eK);
        } else {
            this.dv = new byte[n];
            this.eK += n;
        }
    }

    private int s() {
        return this.dv[this.dl] >= 0 ? this.dv[this.dl++] : 256 + this.dv[this.dl++];
    }

    public void W() {
        if (this.fL == 0) {
            try {
                RecordStore recordStore = RecordStore.openRecordStore((String)"TTStore", (boolean)true);
                RecordEnumeration recordEnumeration = recordStore.enumerateRecords(null, null, false);
                for (int i = 0; i < recordEnumeration.numRecords(); ++i) {
                    int n = recordEnumeration.nextRecordId();
                    byte by = recordStore.getRecord(n)[0];
                    if (by != this.fc) continue;
                    recordStore.deleteRecord(n);
                }
                byte[] byArray = new byte[this.eK + 1];
                byArray[0] = (byte)this.fc;
                System.arraycopy(this.dv, 0, byArray, 1, this.eK);
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
        for (int i = 0; i < this.eK; ++i) {
            l += (long)this.c(this.dv[i]);
        }
    }

    void Y() {
        this.c("Compress pre");
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        byte by = this.dv[0];
        int n = 1;
        for (int i = 1; i < this.eK; ++i) {
            if (by != this.dv[i] || n == 254) {
                this.a(byteArrayOutputStream, (int)by, n);
                by = this.dv[i];
                n = 1;
                continue;
            }
            ++n;
        }
        this.a(byteArrayOutputStream, (int)by, n);
        this.bm += this.eK;
        this.dv = byteArrayOutputStream.toByteArray();
        this.eK = this.dv.length;
        this.fw += this.eK;
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
        for (int i = 0; i < this.eK; ++i) {
            if (this.dv[i] == -1) {
                for (int j = 0; j < this.c(this.dv[i + 1]); ++j) {
                    byteArrayOutputStream.write(this.dv[i + 2]);
                }
                i += 2;
                continue;
            }
            byteArrayOutputStream.write(this.dv[i]);
        }
        this.fw += this.eK;
        this.dv = byteArrayOutputStream.toByteArray();
        this.eK = this.dv.length;
        this.bm += this.eK;
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
        if (this.fL == 2) {
            return 0;
        }
        byte by2 = by;
        if (this.fL == 0) {
            this.k(1);
            this.dv[this.dl++] = by;
        } else if (this.dd) {
            by2 = this.dv[this.dl++];
        }
        return by2;
    }

    int s(int n) {
        if (this.fL == 2) {
            return 0;
        }
        int n2 = n;
        if (this.fL == 0) {
            this.k(4);
            this.dv[this.dl++] = (byte)(n & 0xFF);
            this.dv[this.dl++] = (byte)(n >> 8 & 0xFF);
            this.dv[this.dl++] = (byte)(n >> 16 & 0xFF);
            this.dv[this.dl++] = (byte)(n >> 24 & 0xFF);
        } else if (this.dd) {
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
        if (this.fL == 2) {
            for (int i = 0; i < byArray.length; ++i) {
                byArray[i] = 0;
            }
        } else if (this.fL == 0) {
            this.k(byArray.length);
            System.arraycopy(byArray, 0, this.dv, this.dl, byArray.length);
            this.dl += byArray.length;
        } else if (this.dd) {
            System.arraycopy(this.dv, this.dl, byArray, 0, byArray.length);
            this.dl += byArray.length;
        }
    }

    void a(short[] sArray) {
        block4: {
            block5: {
                block3: {
                    if (this.fL != 2) break block3;
                    for (int i = 0; i < sArray.length; ++i) {
                        sArray[i] = 0;
                    }
                    break block4;
                }
                if (this.fL != 0) break block5;
                this.k(2 * sArray.length);
                for (int i = 0; i < sArray.length; ++i) {
                    this.dv[this.dl++] = (byte)(sArray[i] & 0xFF);
                    this.dv[this.dl++] = (byte)(sArray[i] >> 8 & 0xFF);
                }
                break block4;
            }
            if (!this.dd) break block4;
            for (int i = 0; i < sArray.length; ++i) {
                sArray[i] = (short)(this.s() + 256 * this.s());
            }
        }
    }

    void a(int[] nArray) {
        block4: {
            block5: {
                block3: {
                    if (this.fL != 2) break block3;
                    for (int i = 0; i < nArray.length; ++i) {
                        nArray[i] = 0;
                    }
                    break block4;
                }
                if (this.fL != 0) break block5;
                this.k(4 * nArray.length);
                for (int i = 0; i < nArray.length; ++i) {
                    this.dv[this.dl++] = (byte)(nArray[i] & 0xFF);
                    this.dv[this.dl++] = (byte)(nArray[i] >> 8 & 0xFF);
                    this.dv[this.dl++] = (byte)(nArray[i] >> 16 & 0xFF);
                    this.dv[this.dl++] = (byte)(nArray[i] >> 24 & 0xFF);
                }
                break block4;
            }
            if (!this.dd) break block4;
            for (int i = 0; i < nArray.length; ++i) {
                nArray[i] = this.s() + 256 * this.s() + 65536 * this.s() + 0x1000000 * this.s();
            }
        }
    }

    void g(int n) {
        if (n == 0) {
            this.y(0, 0);
        }
        if (n == 1) {
            this.y(0, 1);
        }
        if (n == 2) {
            this.fL = 2;
        }
        if (n == 4) {
            this.eK = 0;
            this.dv = null;
            this.fL = 0;
        }
        if (n == 5) {
            this.dl = 0;
            this.fL = 1;
            this.dd = true;
        }
        if (n == 1) {
            this.j();
        }
        this.hw = this.s(this.hw);
        this.hr = this.s(this.hr);
        this.cU = this.s(this.cU);
        this.hc = this.s(this.hc);
        this.eU = this.s(this.eU);
        this.bB = this.s(this.bB);
        this.eQ = this.s(this.eQ);
        this.bj = this.s(this.bj);
        this.bI = this.s(this.bI);
        this.db = this.s(this.db);
        this.aR = this.s(this.aR);
        this.t = this.s(this.t);
        this.fj = this.s(this.fj);
        this.a(this.bU);
        this.es = this.s(this.es);
        this.cl = this.s(this.cl);
        this.gM = this.s(this.gM);
        this.fE = this.s(this.fE);
        this.eO = this.s(this.eO);
        this.hk = this.s(this.hk);
        this.cY = this.s(this.cY);
        this.aD = this.b(this.aD);
        this.eC = this.s(this.eC);
        this.a(this.N);
        this.a(this.cx);
        this.a(this.Z);
        this.a(this.dG);
        this.a(this.ef);
        this.a(this.L);
        this.a(this.dT);
        this.a(this.fS);
        this.a(this.bb);
        this.a(this.fs);
        this.fe = this.s(this.fe);
        this.eh = this.s(this.eh);
        this.gK = this.s(this.gK);
        this.gE = this.s(this.gE);
        this.cF = this.s(this.cF);
        this.ho = this.s(this.ho);
        this.gr = this.s(this.gr);
        this.eg = this.s(this.eg);
        this.eJ = this.s(this.eJ);
        this.eX = this.s(this.eX);
        this.gx = this.s(this.gx);
        this.eM = this.s(this.eM);
        this.V = this.s(this.V);
        this.f = this.s(this.f);
        this.eu = this.s(this.eu);
        this.dA = this.s(this.dA);
        this.v = this.s(this.v);
        this.cM = this.s(this.cM);
        this.bG = this.s(this.bG);
        this.he = this.s(this.he);
        this.bP = this.s(this.bP);
        this.ht = this.s(this.ht);
        this.cr = this.s(this.cr);
        this.cg = this.s(this.cg);
        this.cm = this.s(this.cm);
        this.gq = this.s(this.gq);
        this.h = this.s(this.h);
        this.B = this.s(this.B);
        this.hn = this.s(this.hn);
        this.ep = this.s(this.ep);
        this.bo = this.s(this.bo);
        this.eb = this.s(this.eb);
        this.dF = this.s(this.dF);
        this.aq = this.s(this.aq);
        this.aE = this.b(this.aE);
        this.a(this.da);
        this.a(this.cV);
        this.a(this.be);
        this.a(this.hv);
        this.a(this.aj);
        this.a(this.eq);
        this.a(this.hq);
        this.a(this.cO);
        this.bw = this.b(this.bw);
        this.fp = this.d(this.fp);
        this.a(this.dC);
        this.a(this.gN);
        this.a(this.di);
        this.a(this.gi);
        this.a(this.bL);
        this.ez = this.s(this.ez);
        this.a(this.eo);
        this.a(this.af);
        this.a(this.ey);
        this.a(this.u);
        this.a(this.ak);
        this.a(this.eT);
        this.a(this.p);
        this.a(this.aI);
        this.a(this.A);
        this.a(this.Y);
        this.a(this.g);
        this.a(this.aw);
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
        this.a(this.hm);
        this.a(this.bh);
        this.a(this.bY);
        this.a(this.dn);
        this.a(this.dE);
        this.a(this.fW);
        this.a(this.fF);
        this.a(this.aV);
        this.a(this.M);
        this.a(this.fa);
        this.a(this.X);
        this.a(this.cJ);
        this.a(this.dU);
        this.er = this.s(this.er);
        this.bq = this.s(this.bq);
        this.bv = this.s(this.bv);
        this.dy = this.d(this.dy);
        this.aX = this.b(this.aX);
        this.ap = this.b(this.ap);
        this.en = this.d(this.en);
        this.bs = this.s(this.bs);
        this.fT = this.s(this.fT);
        this.bD = this.s(this.bD);
        this.cC = this.s(this.cC);
        this.hb = this.s(this.hb);
        this.dV = this.s(this.dV);
        this.fu = this.s(this.fu);
        this.cT = this.s(this.cT);
        this.gu = this.s(this.gu);
        this.by = this.s(this.by);
        this.eF = this.s(this.eF);
        this.ao = this.s(this.ao);
        this.fb = this.s(this.fb);
        this.ba = this.s(this.ba);
        this.cw = this.s(this.cw);
        this.eH = this.s(this.eH);
        this.cz = this.s(this.cz);
        this.bO = this.s(this.bO);
        this.gT = this.s(this.gT);
        this.aW = this.s(this.aW);
        this.hi = this.s(this.hi);
        this.c = this.s(this.c);
        this.hp = this.s(this.hp);
        this.fK = this.s(this.fK);
        this.aG = this.s(this.aG);
        this.eI = this.s(this.eI);
        this.au = this.s(this.au);
        this.bZ = this.s(this.bZ);
        this.fC = this.s(this.fC);
        this.ai = this.s(this.ai);
        this.fH = this.s(this.fH);
        this.F = this.s(this.F);
        this.eP = this.s(this.eP);
        this.fB = this.s(this.fB);
        if (n == 0) {
            this.Y();
            this.W();
        }
    }

    void A() {
        ch = this.d(ch);
        aP = this.d(aP);
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
        this.y(5, 0);
        this.A();
        this.W();
    }

    void Z() {
        this.y(5, 1);
        this.A();
        this.C(this.gh);
    }

    void t() {
        this.bm = 0;
        this.fw = 0;
        this.g(0);
        this.y(1, 0);
        this.a(this.cI);
        this.Y();
        this.W();
        this.y(2, 0);
        this.a(this.ea);
        this.Y();
        this.W();
        this.y(3, 0);
        this.a(this.bK);
        this.Y();
        this.W();
        this.y(4, 0);
        this.a(this.cL);
        this.Y();
        this.W();
    }

    boolean X() {
        this.y(0, 1);
        return this.dd;
    }

    void u() {
        this.bm = 0;
        this.fw = 0;
        this.g(1);
        this.cI = new byte[this.hw + 1][this.hr + 1];
        this.ea = new byte[this.hw + 1][this.hr + 1];
        this.bK = new byte[this.hw + 1][this.hr + 1];
        this.cL = new byte[this.hw + 2][this.hr + 2];
        this.hj = (this.hw - 1) * 8;
        this.eV = (this.hw + this.hr) * 16;
        this.bQ = (this.hw + this.hr) * 8;
        this.gU = this.eV / 2;
        this.fO = this.bQ / 2;
        this.v(-1);
        this.y(1, 1);
        this.j();
        this.a(this.cI);
        this.y(2, 1);
        this.j();
        this.a(this.ea);
        this.y(3, 1);
        this.j();
        this.a(this.bK);
        this.y(4, 1);
        this.j();
        this.a(this.cL);
        this.bH = -1;
    }

    void L() {
        this.Z[0] = this.h();
        this.L[0] = this.Z[0];
    }

    void l() {
        this.ab();
        this.N[0] = this.eX;
        this.cx[0] = this.gr;
        this.dG[0] = this.eh + this.eX;
        this.ef[0] = this.fe + this.gr;
        this.br = this.h + this.gK;
        this.gV = this.B + this.gE;
        this.cj = this.hn + this.cF;
        this.aA = this.ep + this.ho;
        this.dg = this.bo + this.gr;
        this.bM = this.aq + this.eX;
        this.dq = this.dF + this.eJ;
    }

    void R() {
        int n;
        for (n = 0; n < 500; ++n) {
            this.fS[n] = this.dT[n];
            this.dT[n] = 0;
        }
        for (n = 0; n < 48; ++n) {
            this.fs[n] = this.bb[n];
            this.bb[n] = 0;
        }
        this.ab();
        this.gx = this.gK;
        this.eM = this.gE;
        this.V = this.cF;
        this.f = this.ho;
        this.eu = this.gr;
        this.dA = this.eg;
        this.v = this.eJ;
        this.cM = this.eX;
        for (n = 10; n >= 0; --n) {
            this.N[n + 1] = this.N[n];
            this.cx[n + 1] = this.cx[n];
            this.Z[n + 1] = this.Z[n];
        }
        this.N[0] = 0;
        this.cx[0] = 0;
        this.Z[0] = 0;
        this.h += this.gK;
        this.B += this.gE;
        this.hn += this.cF;
        this.ep += this.ho;
        this.bo += this.gr;
        this.aq += this.eX;
        this.dF += this.eJ;
        this.eh += this.eX;
        this.fe += this.gr;
        if (this.eQ == 13) {
            this.bG = this.br;
            this.he = this.gV;
            this.bP = this.cj;
            this.ht = this.aA;
            this.cr = this.dg;
            this.cg = this.gs;
            this.cm = this.dq;
            this.gq = this.bM;
            this.h = 0;
            this.B = 0;
            this.hn = 0;
            this.ep = 0;
            this.bo = 0;
            this.aq = 0;
            this.dF = 0;
            for (n = 10; n >= 0; --n) {
                this.dG[n + 1] = this.dG[n];
                this.ef[n + 1] = this.ef[n];
                this.L[n + 1] = this.L[n];
            }
            this.eh = 0;
            this.fe = 0;
            this.dG[0] = 0;
            this.ef[0] = 0;
            this.L[0] = 0;
        }
        this.L[0] = this.h();
        this.eU -= this.eg;
        if (this.eg > 0) {
            this.p(-this.eg);
        }
        this.eU -= this.eJ;
        if (this.eJ > 0) {
            this.p(-this.eJ);
        }
        this.gK = 0;
        this.gE = 0;
        this.cF = 0;
        this.ho = 0;
        this.gr = 0;
        this.eX = 0;
        this.eJ = 0;
        this.ga = true;
        this.y = true;
        this.dS = true;
        if (this.eQ == 13) {
            if (q) {
                this.d = false;
                this.cq = 35;
                this.t(20);
            }
        } else if (cc) {
            this.d = false;
            this.cq = 32;
            this.t(20);
        }
    }

    void ab() {
        this.eg = this.hk * this.aD / 1200;
        this.eJ = this.gr * this.eC / 100;
        this.eX = this.gr - this.eg - this.gK - this.gE - this.cF - this.ho - this.eJ;
    }

    int h() {
        int n;
        int n2;
        int n3;
        int n4 = 0;
        for (n3 = 0; n3 < this.hw; ++n3) {
            for (n2 = 0; n2 < this.hr; ++n2) {
                if (this.ea[n3][n2] != 65) continue;
                n = this.cI[n3][n2];
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
                    n4 += this.eH;
                }
                if (n == 87 || n == 88) {
                    n4 += this.aW;
                }
                if (n != 89 && n != 90) continue;
                n4 += this.aW / 2;
            }
        }
        for (n3 = 0; n3 < 500; ++n3) {
            if ((this.ak[n3] & 0x80) != 128 || this.ea[n2 = this.c(this.eo[n3])][n = this.c(this.af[n3])] != 65) continue;
            if ((this.ak[n3] & 0x40) == 64) {
                switch (this.g[n3]) {
                    case 0: {
                        n4 += this.aG;
                        break;
                    }
                    case 1: {
                        n4 += this.eI;
                        break;
                    }
                    case 2: {
                        n4 += this.au;
                        break;
                    }
                    case 3: {
                        n4 += this.bZ;
                        break;
                    }
                    case 4: {
                        n4 += this.fC;
                        break;
                    }
                    case 5: {
                        n4 += this.ai;
                    }
                }
                continue;
            }
            n4 += (this.fH + this.F + this.eP + this.fB) / 4;
        }
        return n4 -= this.hk;
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
                        this.o[n3] = (byte)(this.bj - 1940);
                        this.dm[n3] = (byte)(this.eQ - n3);
                        if (this.dm[n3] < 1) {
                            this.dm[n3] = (byte)(12 + this.dm[n3]);
                            int n7 = n3;
                            this.o[n7] = (byte)(this.o[n7] - 1);
                        }
                        if (n == 0) {
                            if (this.N[n3] > n6) {
                                n6 = this.N[n3];
                            }
                            if (this.N[n3] < n5) {
                                n5 = this.N[n3];
                            }
                        }
                        if (n == 1) {
                            if (this.cx[n3] > n6) {
                                n6 = this.cx[n3];
                            }
                            if (this.cx[n3] < n5) {
                                n5 = this.cx[n3];
                            }
                        }
                        if (n != 2) continue;
                        if (this.Z[n3] > n6) {
                            n6 = this.Z[n3];
                        }
                        if (this.Z[n3] >= n5) continue;
                        n5 = this.Z[n3];
                        continue;
                    }
                    this.dm[n3] = (byte)(this.bj - n3 - 1940);
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
                        this.fA[n4] = this.N[n4] == 0 ? (byte)0 : (byte)(30 * this.N[n4] / n3);
                    }
                }
                if (n == 1) {
                    for (n4 = 0; n4 < 12; ++n4) {
                        this.fA[n4] = this.cx[n4] == 0 ? (byte)0 : (byte)(60 * this.cx[n4] / n3);
                    }
                }
                if (n != 2) break block29;
                for (n4 = 0; n4 < 12; ++n4) {
                    this.fA[n4] = this.N[n4] == 0 ? (byte)0 : (byte)(30 * this.Z[n4] / n3);
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
        if (this.bt != 2 && this.bt != 4 && this.gm.nextInt() % this.cd > this.fP) {
            this.t(105);
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
                this.fV = this.gS;
                this.bN = this.bp;
                this.fm = this.dD;
                this.ax = this.k;
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

    void ad() {
        for (int i = 0; i < 500; ++i) {
            int n;
            int n2;
            if ((this.ak[i] & 0x80) != 128 || (this.ak[i] & 0x40) != 64 || this.ea[n2 = this.c(this.eo[i])][n = this.c(this.af[i])] != 66 && this.ea[n2][n] != 67 && this.ea[n2][n] != 68 || this.g[i] >= this.ez) continue;
            this.g[i] = (byte)this.ez;
            this.ak[i] = -64;
            int n3 = i;
            this.ak[n3] = (byte)(this.ak[n3] | this.ez / 3);
            this.eT[i] = (byte)(8 - this.ez);
            break;
        }
    }

    void e() {
        for (int i = 0; i < 500; ++i) {
            int n;
            int n2;
            if ((this.ak[i] & 0x80) != 128 || this.ea[n2 = this.c(this.eo[i])][n = this.c(this.af[i])] != 66 && this.ea[n2][n] != 67 && this.ea[n2][n] != 68) continue;
            this.aw[i] = 0;
        }
    }

    void m(int n, int n2) {
        int n3 = this.ez / 3;
        int n4 = this.ez;
        int n5 = this.dx / 8000 + 1;
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
        int n7 = this.b(n, n2, n3, n4, this.cI[n][n2] == 87 ? 0 : 2, 8 - n4);
        for (int i = 0; i < n5; ++i) {
            n7 = this.l(n7, n6);
        }
    }

    void g(int n, int n2) {
        boolean bl;
        boolean bl2;
        boolean bl3;
        boolean bl4;
        bl4 = n > 0 && this.ea[n - 1][n2] == 64;
        bl3 = n < this.hw - 1 && this.ea[n + 1][n2] == 64;
        bl2 = n2 > 0 && this.ea[n][n2 - 1] == 64;
        bl = n2 < this.hr - 1 && this.ea[n][n2 + 1] == 64;
        if (bl4 || bl3) {
            this.cI[n][n2] = 88;
            this.ea[n][n2] = (byte)this.hu;
            if (bl3 && n > 1 && (this.cI[n - 1][n2] == 0 || this.cI[n - 1][n2] >= 29 && this.cI[n - 1][n2] <= 33) && (this.cI[n - 2][n2] == 0 || this.cI[n - 2][n2] >= 29 && this.cI[n - 2][n2] <= 33)) {
                this.cI[n][n2] = 90;
                this.cI[n - 1][n2] = 88;
                this.cI[n - 2][n2] = 90;
                this.ea[n - 1][n2] = (byte)this.hu;
                this.ea[n - 2][n2] = (byte)this.hu;
            }
            if (bl4 && n < this.hw - 2 && (this.cI[n + 1][n2] == 0 || this.cI[n + 1][n2] >= 29 && this.cI[n + 1][n2] <= 33) && (this.cI[n + 2][n2] == 0 || this.cI[n + 2][n2] >= 29 && this.cI[n + 2][n2] <= 33)) {
                this.cI[n][n2] = 90;
                this.cI[n + 1][n2] = 88;
                this.cI[n + 2][n2] = 90;
                this.ea[n + 1][n2] = (byte)this.hu;
                this.ea[n + 2][n2] = (byte)this.hu;
            }
        }
        if (bl2 || bl) {
            this.cI[n][n2] = 87;
            this.ea[n][n2] = (byte)this.hu;
            if (bl2 && n2 > 1 && (this.cI[n][n2 - 1] == 0 || this.cI[n][n2 - 1] >= 29 && this.cI[n][n2 - 1] <= 33) && (this.cI[n][n2 - 2] == 0 || this.cI[n][n2 - 2] >= 29 && this.cI[n][n2 - 2] <= 33)) {
                this.cI[n][n2] = 89;
                this.cI[n][n2 - 1] = 87;
                this.cI[n][n2 - 2] = 89;
                this.ea[n][n2 - 1] = (byte)this.hu;
                this.ea[n][n2 - 2] = (byte)this.hu;
            }
            if (bl2 && n2 < this.hr - 2 && (this.cI[n][n2 + 1] == 0 || this.cI[n][n2 + 1] >= 29 && this.cI[n][n2 + 1] <= 33) && (this.cI[n][n2 + 2] == 0 || this.cI[n][n2 + 2] >= 29 && this.cI[n][n2 + 2] <= 33)) {
                this.cI[n][n2] = 89;
                this.cI[n][n2 + 1] = 87;
                this.cI[n][n2 + 2] = 89;
                this.ea[n][n2 + 1] = (byte)this.hu;
                this.ea[n][n2 + 2] = (byte)this.hu;
            }
        }
    }

    void F() {
        int n = Math.abs(this.gm.nextInt() % this.hw);
        int n2 = Math.abs(this.gm.nextInt() % this.hr);
        this.cA = -1;
        if (this.cI[n][n2] == 0 || this.cI[n][n2] >= 29 && this.cI[n][n2] <= 33) {
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
            if (n5 >= this.hw) {
                n5 = this.hw - 1;
            }
            if (n6 >= this.hr) {
                n6 = this.hr - 1;
            }
            block0: for (int i = n3; i <= n5; ++i) {
                for (int j = n4; j <= n6; ++j) {
                    byte by = this.cI[i][j];
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
        int n = Math.abs(this.gm.nextInt() % this.hw);
        int n2 = Math.abs(this.gm.nextInt() % this.hr);
        this.aY = false;
        if (this.cI[n][n2] == 0 || this.cI[n][n2] >= 29 && this.cI[n][n2] <= 33) {
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
            if (n5 >= this.hw) {
                n5 = this.hw - 1;
            }
            if (n6 >= this.hr) {
                n6 = this.hr - 1;
            }
            block0: for (int i = n3; i <= n5; ++i) {
                for (int j = n4; j <= n6; ++j) {
                    byte by = this.cI[i][j];
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
        int n3 = this.bs;
        if (this.ap >= 6 && this.ap <= 11) {
            n3 += this.fT;
        }
        switch (this.ap) {
            case 1: {
                n2 = this.cC;
                break;
            }
            case 2: {
                n2 = this.hb;
                break;
            }
            case 3: {
                n2 = this.dV;
                break;
            }
            case 4: {
                n2 = this.fu;
                break;
            }
            case 5: {
                n2 = this.cT;
                break;
            }
            case 12: {
                n2 = this.bD;
                break;
            }
            case 15: {
                n2 = this.gu;
                break;
            }
            case 14: {
                n2 = this.bj - 1950;
                n3 = this.bs - 1950;
                break;
            }
            case 13: {
                n2 = this.cY - this.hk;
                n3 = this.cY;
                break;
            }
            case 6: {
                n2 = this.cC + this.hb;
                break;
            }
            case 7: {
                n2 = this.cC + this.dV;
                break;
            }
            case 8: {
                n2 = this.cC + this.fu;
                break;
            }
            case 9: {
                n2 = this.hb + this.dV;
                break;
            }
            case 10: {
                n2 = this.hb + this.fu;
                break;
            }
            case 11: {
                n2 = this.fu + this.cT;
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

    void z() {
        boolean bl = false;
        if (!this.en) {
            switch (this.ap) {
                case 1: {
                    if (this.bs > this.cC) break;
                    bl = true;
                    break;
                }
                case 2: {
                    if (this.bs > this.hb) break;
                    bl = true;
                    break;
                }
                case 3: {
                    if (this.bs > this.dV) break;
                    bl = true;
                    break;
                }
                case 4: {
                    if (this.bs > this.fu) break;
                    bl = true;
                    break;
                }
                case 5: {
                    if (this.bs > this.cT) break;
                    bl = true;
                    break;
                }
                case 6: {
                    if (this.bs > this.cC || this.fT > this.hb) break;
                    bl = true;
                    break;
                }
                case 7: {
                    if (this.bs > this.cC || this.fT > this.dV) break;
                    bl = true;
                    break;
                }
                case 8: {
                    if (this.bs > this.cC || this.fT > this.fu) break;
                    bl = true;
                    break;
                }
                case 9: {
                    if (this.bs > this.hb || this.fT > this.dV) break;
                    bl = true;
                    break;
                }
                case 10: {
                    if (this.bs > this.hb || this.fT > this.fu) break;
                    bl = true;
                    break;
                }
                case 11: {
                    if (this.bs > this.fu || this.fT > this.cT) break;
                    bl = true;
                    break;
                }
                case 12: {
                    if (this.bs > this.bD) break;
                    bl = true;
                    break;
                }
                case 13: {
                    if (this.hk != 0) break;
                    bl = true;
                    break;
                }
                case 14: {
                    if (this.bj != this.bs) break;
                    bl = true;
                    break;
                }
                case 15: {
                    if (this.bs > this.gu) break;
                    bl = true;
                }
            }
        }
        if (bl) {
            this.d = false;
            this.cq = 6;
            this.en = true;
            if (this.eF >= 2) {
                if (this.aM < this.eF) {
                    this.aM = this.eF;
                }
                this.O();
                this.gJ = this.c(this.eF - 2);
            }
            this.t(20);
        }
    }

    void a() {
        this.aO = 0;
        for (int i = 0; i < 5; ++i) {
            if (this.hm[i] == 0) continue;
            this.gP[this.aO] = (byte)i;
            ++this.aO;
        }
    }

    byte a(int n) {
        if (this.hm[n] == 1) {
            int n2 = 100 * this.M[n] / this.aV[n];
            if (n2 > 100) {
                n2 = 100;
            }
            if (n2 < 0) {
                n2 = 0;
            }
            return (byte)n2;
        }
        if (this.hm[n] == 2) {
            int n3 = this.eU > 0 ? 100 * this.eU / this.aV[n] : 100 * this.aV[n] / this.eU;
            if (n3 > 100) {
                n3 = 100;
            }
            if (n3 < 0) {
                n3 = 0;
            }
            return (byte)n3;
        }
        if (this.hm[n] >= 4 && this.hm[n] <= 14) {
            int n4 = 100 * this.M[n] / this.aV[n];
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
        int n3 = n * 60 + 20 + this.gm.nextInt() % 20;
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
        if (this.fa[n] == 1 || this.fa[n] == 2) {
            this.eU += this.cJ[n];
            this.p(this.cJ[n]);
        }
        if (this.fa[n] == 3) {
            this.cY += this.cJ[n];
        }
        if (this.fa[n] == 4 && this.ez < 5) {
            ++this.ez;
        }
        if (this.fa[n] >= 5 && this.fa[n] <= 10) {
            int n2 = this.fa[n] - 5;
            this.eA[n2] = (byte)(this.eA[n2] + this.X[n]);
        }
        if (this.fa[n] == 11) {
            this.hk -= this.hk * this.X[n] / 100;
            if (this.hk < 0) {
                this.hk = 0;
            }
            if (this.hk % 20000 != 0) {
                this.hk = this.hk % 20000 < 10000 ? (this.hk -= this.hk % 20000) : (this.hk += 20000 - this.hk % 20000);
            }
        }
        if (this.fa[n] == 12 && this.aD > 1) {
            this.aD = (byte)(this.aD - 1);
        }
        this.fY = this.hm[n] + 40;
        this.dR = this.fa[n] + 60;
        this.cW = (byte)3;
        this.cZ = (byte)5;
        this.hm[5] = this.hm[n];
        this.aV[5] = this.aV[n];
        this.M[5] = this.M[n];
        this.fa[5] = this.fa[n];
        this.X[5] = this.X[n];
        this.cJ[5] = this.cJ[n];
        this.bh[5] = this.bh[n];
        this.dn[5] = this.dn[n];
        this.fW[5] = this.fW[n];
        this.bY[5] = this.bY[n];
        this.dE[5] = this.dE[n];
        this.fF[5] = this.fF[n];
        this.hm[n] = 0;
        this.aV[n] = 0;
        this.M[n] = 0;
        this.fa[n] = 0;
        this.X[n] = 0;
        this.cJ[n] = 0;
        this.d = false;
        this.cq = 9;
        this.t(20);
    }

    void e(int n) {
        this.fY = this.hm[n] + 40;
        this.dR = this.fa[n] + 60;
        this.cW = (byte)4;
        this.cZ = (byte)5;
        this.hm[5] = this.hm[n];
        this.aV[5] = this.aV[n];
        this.M[5] = this.M[n];
        this.fa[5] = this.fa[n];
        this.X[5] = this.X[n];
        this.cJ[5] = this.cJ[n];
        this.bh[5] = this.bh[n];
        this.dn[5] = this.dn[n];
        this.fW[5] = this.fW[n];
        this.bY[5] = this.bY[n];
        this.dE[5] = this.dE[n];
        this.fF[5] = this.fF[n];
        this.hm[n] = 0;
        this.aV[n] = 0;
        this.M[n] = 0;
        this.fa[n] = 0;
        this.X[n] = 0;
        this.cJ[n] = 0;
        this.d = false;
        this.cq = 8;
        this.t(20);
    }

    void Q() {
        int n;
        if (!this.dy) {
            return;
        }
        for (n = 0; n < 6; ++n) {
            if (this.eA[n] <= 0) continue;
            int n2 = n;
            this.eA[n2] = (byte)(this.eA[n2] - 1);
        }
        for (n = 0; n < 5; ++n) {
            if ((this.hm[n] == 1 || this.hm[n] >= 4 && this.hm[n] <= 14) && this.aV[n] <= this.M[n]) {
                this.D(n);
            }
            if (this.bY[n] != this.bB || this.dE[n] != this.eQ || this.fF[n] != this.bj - 1950) continue;
            if (this.hm[n] == 2 && this.aV[n] <= this.eU) {
                this.D(n);
            }
            if (this.hm[n] == 0) continue;
            this.e(n);
        }
        n = this.bB + 32 * this.eQ + 370 * (this.bj - 1950);
        if (n - this.bv > 150 + this.gm.nextInt() % 20) {
            int n3;
            int n4 = -1;
            for (n3 = 0; n3 < 5; ++n3) {
                if (this.hm[n3] != 0) continue;
                n4 = n3;
                break;
            }
            if (n4 > -1) {
                int n5;
                this.bh[n4] = (byte)this.bB;
                this.dn[n4] = (byte)this.eQ;
                this.fW[n4] = (byte)(this.bj - 1950);
                n3 = 4 + Math.abs(this.gm.nextInt() % 12);
                this.f(n3, n4);
                int n6 = Math.abs(this.gm.nextInt() % 16);
                int n7 = 0;
                int n8 = 0;
                int n9 = 0;
                int n10 = 0;
                int n11 = 0;
                int n12 = 0;
                block9: for (n5 = 0; n5 < 500; ++n5) {
                    if ((this.ak[n5] & 0x80) != 128 || this.ea[this.c(this.eo[n5])][this.c(this.af[n5])] != 65) continue;
                    ++n8;
                    if ((this.ak[n5] & 0x40) == 64) {
                        ++n7;
                        continue;
                    }
                    switch (this.ak[n5] & 7) {
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
                this.hm[n4] = n6 > 0 && n6 < 15 ? (byte)n6 : (byte)0;
                if (n6 == 1) {
                    this.aV[n4] = n3 * (n7 + 2) * (3000 + this.gm.nextInt() % 1500);
                }
                if (n6 == 2) {
                    this.aV[n4] = this.eU + n3 * (n7 + 2) * (2000 + this.gm.nextInt() % 1000);
                }
                if (n6 == 3) {
                    this.hm[n4] = 0;
                }
                if (n6 == 4) {
                    this.aV[n4] = n3 * (n9 * 2 + 5) * (25 + this.gm.nextInt() % 3);
                }
                if (n6 == 5) {
                    this.aV[n4] = n3 * (n10 * 2 + 5) * (25 + this.gm.nextInt() % 3);
                }
                if (n6 == 6) {
                    this.aV[n4] = n3 * (n11 * 2 + 5) * (25 + this.gm.nextInt() % 3);
                }
                if (n6 == 7) {
                    this.aV[n4] = n3 * (n12 * 2 + 5) * (18 + this.gm.nextInt() % 3);
                }
                if (n6 == 8) {
                    this.aV[n4] = n3 * (n12 * 2 + 5) * (10 + this.gm.nextInt() % 3);
                }
                if (n6 == 9) {
                    this.aV[n4] = 3 + Math.abs(this.gm.nextInt() % 8);
                }
                if (n6 == 10) {
                    this.aV[n4] = 3 + Math.abs(this.gm.nextInt() % 8);
                }
                if (n6 == 11) {
                    this.aV[n4] = 3 + Math.abs(this.gm.nextInt() % 8);
                }
                if (n6 == 12) {
                    this.aV[n4] = 3 + Math.abs(this.gm.nextInt() % 8);
                }
                if (n6 == 13) {
                    this.aV[n4] = (13324 + Math.abs(this.gm.nextInt() % 5000)) * n3;
                }
                if (n6 == 14) {
                    if (this.hk > 40000) {
                        this.aV[n4] = this.hk * (75 + Math.abs(this.gm.nextInt() % 15)) / 100;
                    } else {
                        this.hm[n4] = 0;
                    }
                }
                if (n6 != 0) {
                    n5 = 0;
                    while (n5 == 0) {
                        n5 = 1 + Math.abs(this.gm.nextInt() % 12);
                        byte[] byArray = this.aK[n5 - 1];
                        boolean bl = false;
                        for (int i = 0; i < byArray.length; ++i) {
                            if (byArray[i] != n6) continue;
                            bl = true;
                        }
                        if (n5 == 12 && this.aD <= 1) {
                            bl = false;
                        }
                        if (n5 == 3 && this.cY >= 1000000) {
                            bl = false;
                        }
                        if (n5 == 4 && this.ez >= 5) {
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
                        this.X[n4] = (byte)(30 + Math.abs(this.gm.nextInt() % 50));
                        this.cJ[n4] = this.aV[n4] * this.X[n4] / 100;
                    }
                    if (n5 == 2) {
                        this.cJ[n4] = 5000 * (6 + Math.abs(this.gm.nextInt() % 17));
                    }
                    if (n5 == 3) {
                        this.cJ[n4] = 20000 * (3 + Math.abs(this.gm.nextInt() % 3));
                    }
                    if (n5 >= 5 && n5 <= 10) {
                        this.X[n4] = (byte)(25 + Math.abs(this.gm.nextInt() % 45));
                    }
                    if (n5 == 11) {
                        this.X[n4] = (byte)(15 + Math.abs(this.gm.nextInt() % 30));
                    }
                    this.fa[n4] = (byte)n5;
                }
                if (this.hm[n4] != 0) {
                    this.d = false;
                    this.cq = 40;
                    this.cZ = (byte)n4;
                    this.t(20);
                    ++this.er;
                }
            }
            this.bv = n;
        }
    }

    void h(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.hm[i] != 1) continue;
            int n2 = i;
            this.M[n2] = this.M[n2] + n;
        }
        this.bD += n;
        this.by += n / 73;
    }

    void t(int n, int n2) {
        for (int i = 0; i < 5; ++i) {
            if (this.hm[i] != 3 + n) continue;
            int n3 = i;
            this.M[n3] = this.M[n3] + n2;
        }
        switch (n) {
            case 1: {
                this.cC += n2;
                break;
            }
            case 2: {
                this.hb += n2;
                break;
            }
            case 3: {
                this.dV += n2;
                break;
            }
            case 4: {
                this.fu += n2;
                break;
            }
            case 5: {
                this.cT += n2;
            }
        }
    }

    void w(int n) {
        for (int i = 0; i < 5; ++i) {
            if (n == 6 && this.hm[i] == 9) {
                int n2 = i;
                this.M[n2] = this.M[n2] + 1;
            }
            if (n == 4 && this.hm[i] == 10) {
                int n3 = i;
                this.M[n3] = this.M[n3] + 1;
            }
            if (n == 3 && this.hm[i] == 11) {
                int n4 = i;
                this.M[n4] = this.M[n4] + 1;
            }
            if (n != 2 || this.hm[i] != 12) continue;
            int n5 = i;
            this.M[n5] = this.M[n5] + 1;
        }
    }

    void l(int n) {
        for (int i = 0; i < 5; ++i) {
            if (n == 7 && this.hm[i] == 9) {
                int n2 = i;
                this.M[n2] = this.M[n2] - 1;
            }
            if (n == 6 && this.hm[i] == 9) {
                int n3 = i;
                this.M[n3] = this.M[n3] - 1;
            }
            if (n == 5 && this.hm[i] == 10) {
                int n4 = i;
                this.M[n4] = this.M[n4] - 1;
            }
            if (n == 4 && this.hm[i] == 10) {
                int n5 = i;
                this.M[n5] = this.M[n5] - 1;
            }
            if (n == 3 && this.hm[i] == 11) {
                int n6 = i;
                this.M[n6] = this.M[n6] - 1;
            }
            if (n != 2 || this.hm[i] != 12) continue;
            int n7 = i;
            this.M[n7] = this.M[n7] - 1;
        }
    }

    void x(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.hm[i] != 13) continue;
            int n2 = i;
            this.M[n2] = this.M[n2] + n;
        }
    }

    void z(int n) {
        for (int i = 0; i < 5; ++i) {
            if (this.hm[i] != 14) continue;
            int n2 = i;
            this.M[n2] = this.M[n2] + n;
        }
    }

    void x() {
        if (this.bi != null) {
            this.bi.deallocate();
            this.bi = null;
        }
        try {
            InputStream inputStream = this.getClass().getResourceAsStream("/6");
            this.bi = Manager.createPlayer((InputStream)inputStream, (String)"audio/midi");
            this.bi.prefetch();
            Control[] controlArray = this.bi.getControls();
            for (int i = 0; i < controlArray.length; ++i) {
                if (!(controlArray[i] instanceof VolumeControl)) continue;
                this.E = (VolumeControl)controlArray[i];
            }
            this.E.setLevel(20 * this.gh);
            this.cG = false;
            this.ay = true;
        }
        catch (Exception exception) {
            this.ay = false;
        }
    }

    void ae() {
        try {
            if (this.ay && this.bi != null && !this.cG && this.ac) {
                this.bi.prefetch();
                this.bi.setMediaTime(0L);
                this.bi.setLoopCount(-1);
                this.bi.start();
                this.cG = true;
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    void C(int n) {
        try {
            if (this.E != null) {
                this.E.setLevel(n * 20);
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    void E() {
        try {
            if (this.ay && this.bi != null && this.cG) {
                this.bi.stop();
                this.cG = false;
            }
        }
        catch (Exception exception) {
            // empty catch block
        }
    }

    int c(int n) {
        int n2 = this.by;
        for (int i = 0; i < this.hw; ++i) {
            for (int j = 0; j < this.hr; ++j) {
                if (this.ea[i][j] != 65) continue;
                n2 += 3;
                for (int k = 0; k < 500; ++k) {
                    if ((this.ak[k] & 0x80) != 128 || this.c(this.eo[k]) != i || this.c(this.af[k]) != j) continue;
                    if ((this.ak[k] & 0x40) == 64) {
                        n2 += 400;
                        continue;
                    }
                    n2 += 55;
                }
            }
        }
        n2 += this.bB + 30 * this.eQ + 365 * (this.bj - 1950);
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

    String b(byte[] byArray) {
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

    boolean b(boolean bl) {
        try {
            int n;
            int n2;
            HttpConnection httpConnection = (HttpConnection)Connector.open((String)this.b(ec));
            httpConnection.setRequestMethod("GET");
            httpConnection.setRequestProperty(this.b(dO), this.b(i));
            httpConnection.setRequestProperty(this.b(el), this.b(G));
            httpConnection.setRequestProperty(this.b(fz), "en");
            httpConnection.setRequestProperty(this.b(dM), this.n());
            httpConnection.setRequestProperty(this.b(cQ), bl ? "S" : "D");
            StringBuffer stringBuffer = new StringBuffer(String.valueOf(this.H));
            for (n2 = 0; n2 < this.gk.length; ++n2) {
                stringBuffer.append(",");
                stringBuffer.append(this.gk[n2]);
            }
            n2 = 0;
            int n3 = 0;
            int n4 = 0;
            String string = stringBuffer.toString();
            httpConnection.setRequestProperty(this.b(l), string);
            for (int i = 0; i < string.length(); ++i) {
                n2 += string.charAt(i);
                if (i > 0) {
                    n3 += string.charAt(i) ^ string.charAt(i - 1);
                }
                n4 += (i + 1) * string.charAt(i);
            }
            httpConnection.setRequestProperty(this.b(ag), String.valueOf(n2 += n3 + n4));
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
            this.gl = byteArrayOutputStream.toByteArray();
            this.av = byteArrayOutputStream2.toByteArray();
            this.bS = this.gl.length;
            byteArrayOutputStream.close();
            byteArrayOutputStream2.close();
            do {
                n5 = 0;
                for (n4 = 0; n4 < this.bS - 1; ++n4) {
                    if (this.av[n4] <= this.av[n4 + 1]) continue;
                    n3 = this.av[n4];
                    this.av[n4] = this.av[n4 + 1];
                    this.av[n4 + 1] = (byte)n3;
                    n3 = this.gl[n4];
                    this.gl[n4] = this.gl[n4 + 1];
                    this.gl[n4 + 1] = (byte)n3;
                    n5 = 1;
                }
            } while (n5 != 0);
            do {
                n5 = 0;
                for (n4 = 1; n4 < this.bS; ++n4) {
                    if (this.gl[n4] <= this.gl[n4 - 1] || this.av[n4] != this.av[n4 - 1]) continue;
                    n3 = this.av[n4];
                    this.av[n4] = this.av[n4 - 1];
                    this.av[n4 - 1] = (byte)n3;
                    n3 = this.gl[n4];
                    this.gl[n4] = this.gl[n4 - 1];
                    this.gl[n4 - 1] = (byte)n3;
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

    void M() {
        this.d = false;
        this.cq = 11;
        this.t(20);
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
        this.ft = 0;
        this.bf = 15;
        this.bl = e;
        this.r = eN;
        this.a(graphics, n + 4, n2 + 3, byArray3, true);
    }

    void a(int n, int n2) {
        if (this.eD != n || this.aJ != n2) {
            e = n;
            hs = n2;
            this.fG = Image.createImage((int)e, (int)hs);
            this.C = this.fG.getGraphics();
            eN = hs - 30;
            fX = hs - 15;
            this.k(n, n2);
            this.eD = n;
            this.aJ = n2;
            this.bu = true;
            this.bH = -1;
            if (this.bF == 20 && !this.d) {
                if (this.hx == 30) {
                    this.gH = this.cq;
                    this.bF = 30;
                }
                if (this.hx == 40) {
                    this.gH = this.cq;
                    this.bF = 40;
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
            if (this.bF != 40 && this.bF != 35 && this.bF != 45 && this.bF != 55 && this.bF != 65 && this.bF != 105 && this.bF != 120 && (this.bF != 20 || this.d)) {
                graphics.setColor(0);
                graphics.fillRect(0, 0, n, n2);
            }
            graphics.setClip(0, 0, e, hs);
            switch (this.bF) {
                case 0: 
                case 1: 
                case 2: 
                case 3: {
                    graphics.setColor(0xFFFFFF);
                    if (this.bF == 0) {
                        graphics.setColor(0);
                    }
                    graphics.fillRect(0, 0, e, hs);
                    int n3 = e / 2;
                    int n4 = hs / 2;
                    int n5 = this.P.getWidth() / 2;
                    int n6 = this.P.getHeight();
                    int n7 = n6 / 10 + 1;
                    for (int i = 0; i < 10; ++i) {
                        int n8 = (n3 + n5 + 30) * (this.gd - 5 * (i / 2)) / 10;
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
                    if (this.d) {
                        graphics.setColor(0xFFFFFF);
                        graphics.fillRect(0, 0, e, hs);
                        if (this.P != null) {
                            graphics.drawImage(this.P, e / 2, hs / 2, 3);
                        }
                    }
                    if (this.cq == 36) {
                        this.bF = 40;
                        this.paint(graphics);
                        this.bF = 20;
                    }
                    this.a(graphics, this.cq);
                    break;
                }
                case 25: {
                    graphics.setColor(0);
                    graphics.fillRect(0, 0, e, hs);
                    this.a(graphics, e / 2, hs / 2, 0, 3, this.n[this.ds[0]]);
                    graphics.setColor(215, 222, 238);
                    graphics.fillRect(10, hs - 23, e - 21, 9);
                    int n11 = this.dh < 2 ? 2 : this.dh;
                    int n12 = this.bV * (e - 20) / (n11 - 1);
                    graphics.setColor(60, 105, 144);
                    graphics.fillRect(10, hs - 23, n12, 9);
                    graphics.setColor(0xFFFFFF);
                    graphics.drawRect(10, hs - 23, e - 21, 9);
                    break;
                }
                case 30: {
                    this.em = System.currentTimeMillis();
                    this.f(graphics);
                    this.ah = System.currentTimeMillis();
                    if (this.ev) {
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
                    if (this.bJ) {
                        this.a(graphics, 236, 0, 15, 0);
                    }
                    if (this.gH > -1) {
                        this.cq = this.gH;
                        this.o();
                        this.gH = -1;
                        this.a(graphics, this.cq);
                        this.t(20);
                    }
                    if (this.fZ <= 0) break;
                    this.fZ = (byte)(this.fZ - 1);
                    if (this.fZ != 0) break;
                    this.cq = 80 + this.ff;
                    this.o();
                    this.d = false;
                    this.a(graphics, this.cq);
                    this.t(20);
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
                    int n15 = this.cU / 16;
                    int n16 = this.hc / 8;
                    int n17 = this.hw + this.hr;
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
                    graphics.drawImage(this.fG, n18 + n19, 15 + n14 + n20, 20);
                    int n21 = this.cU - 2 * this.hc + 16 + 2 * this.hj;
                    int n22 = this.cU + 2 * this.hc - 16 - 2 * this.hj;
                    int n23 = this.hw - (n21 /= 32) + (n22 /= 32);
                    int n24 = n21 + n22;
                    graphics.setColor(0xFFFFFF);
                    graphics.drawRect(n24 + n18 + n19, 15 + n23 + n14 + n20, e / 16, eN / 8);
                    graphics.setClip(0, 0, e, hs);
                    this.b(graphics);
                    this.d(graphics);
                    if (this.ee) {
                        this.d = false;
                        this.cq = 36;
                        this.t(20);
                        this.ee = false;
                    }
                    if (this.gH <= -1) break;
                    this.cq = this.gH;
                    this.o();
                    this.gH = -1;
                    this.a(graphics, this.cq);
                    this.t(20);
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

    public void t(int n) {
        this.hx = this.bF;
        if (!this.d) {
            this.P = null;
        }
        this.S = false;
        this.bu = true;
        switch (n) {
            case 0: {
                this.a("/1");
                this.b("/4");
                this.gd = 0;
            }
            case 1: 
            case 2: {
                this.P = this.aa();
                this.gD = System.currentTimeMillis();
                this.dZ = false;
                this.gd = 0;
                break;
            }
            case 4: {
                try {
                    b b2 = new b("/3");
                    int n2 = b2.b();
                    int n3 = b2.b();
                    this.n = new byte[n2][];
                    this.ds = new byte[n3 + 1];
                    for (int i = 0; i < n2; ++i) {
                        int n4 = b2.b();
                        int n5 = b2.b();
                        this.n[i] = new byte[n5];
                        this.ds[n4] = (byte)i;
                        for (int j = 0; j < n5; ++j) {
                            this.n[i][j] = (byte)b2.b();
                        }
                    }
                    b2.a();
                }
                catch (Exception exception) {}
                break;
            }
            case 10: {
                this.eW = 0;
                try {
                    b b3 = new b("/2");
                    this.al = b3.b();
                    for (int i = 0; i < this.al; ++i) {
                        int n6 = b3.b();
                        this.gt[n6] = (byte)(i - 8);
                    }
                    b3.a();
                }
                catch (Exception exception) {
                    // empty catch block
                }
                this.al -= 8;
                this.fU = new Image[this.al];
                this.gt[68] = this.gt[66];
                break;
            }
            case 20: {
                this.o();
                if (!this.bC) {
                    this.gF = 2;
                }
                this.bC = true;
                break;
            }
            case 25: {
                this.ct = 0;
                break;
            }
            case 30: {
                this.fG = null;
                System.gc();
                this.fG = Image.createImage((int)e, (int)hs);
                this.C = this.fG.getGraphics();
                this.bH = -1;
                this.bu = true;
                break;
            }
            case 35: {
                this.ct = 4;
                this.cR = 0;
                break;
            }
            case 45: {
                this.ct = 4;
                break;
            }
            case 55: {
                this.ct = 4;
                break;
            }
            case 65: {
                this.ct = 4;
                break;
            }
            case 105: {
                this.ct = 4;
                break;
            }
            case 120: {
                this.ct = 4;
            }
        }
        this.bF = n;
    }

    public void m() {
        switch (this.bF) {
            case 0: 
            case 1: 
            case 2: {
                if (System.currentTimeMillis() - this.gD > 5000L) {
                    this.dZ = true;
                }
                ++this.gd;
                if (!this.dZ) break;
                if (this.bF == 1) {
                    for (int i = 0; i < 6; ++i) {
                        this.ca[i] = this.aa();
                    }
                    this.t(4);
                    break;
                }
                this.t(this.bF + 1);
                break;
            }
            case 4: {
                this.d = true;
                this.cn = 3;
                this.cq = 14;
                this.o();
                this.t(20);
                break;
            }
            case 10: {
                this.fU[this.eW] = this.aa();
                ++this.eW;
                if (this.eW == this.al / 2) {
                    this.x();
                    if (this.gR) {
                        this.ae();
                        this.ac = true;
                    } else {
                        this.ac = false;
                    }
                }
                if (this.eW != this.al) break;
                this.d = true;
                this.cq = 1;
                this.t(20);
                break;
            }
            case 25: {
                ++this.ct;
                if (this.ct == 2) {
                    this.g(2);
                    this.bB = 1;
                    this.eQ = 1;
                    this.bj = 1950;
                    this.en = false;
                    this.bJ = false;
                    this.dP = false;
                    this.fb = 974;
                    this.ba = 974;
                    this.cw = 173;
                    this.eH = 342;
                    this.cz = 311;
                    this.bO = 173;
                    this.gT = 234;
                    this.aW = 4759;
                    this.hi = 9845;
                    this.c = 347;
                    this.hp = 5982;
                    this.fK = 481;
                    this.aG = 37894;
                    this.eI = 43247;
                    this.au = 51872;
                    this.bZ = 60149;
                    this.fC = 71423;
                    this.ai = 82675;
                    this.fH = 4578;
                    this.F = 5196;
                    this.eP = 4314;
                    this.fB = 5072;
                    try {
                        b b2 = new b("/5");
                        int n = b2.b();
                        for (int i = 0; i < n * 36; ++i) {
                            int n2 = b2.b();
                            int n3 = b2.b();
                            int n4 = b2.b();
                            int n5 = b2.b();
                            int n6 = b2.b() + 256 * b2.b();
                            int n7 = b2.b();
                            int n8 = b2.b();
                            int n9 = b2.b();
                            int n10 = b2.b();
                            int n11 = b2.b();
                            int n12 = b2.b();
                            int n13 = b2.b();
                            int n14 = b2.b();
                            int n15 = b2.b();
                            int n16 = b2.b() + 256 * b2.b() + 65536 * b2.b();
                            int n17 = b2.b() + 256 * b2.b() + 65536 * b2.b();
                            int n18 = b2.b() + 256 * b2.b() + 65536 * b2.b();
                            int n19 = b2.b();
                            int n20 = b2.b();
                            int n21 = b2.b();
                            int n22 = b2.b();
                            int n23 = b2.b() + 256 * b2.b() + 65536 * b2.b();
                            int n24 = b2.b() + 256 * b2.b() + 65536 * b2.b();
                            int n25 = b2.b();
                            int n26 = b2.b();
                            if (n2 != this.J) continue;
                            this.eF = n2;
                            this.hw = this.hr = n3;
                            this.hj = (this.hw - 1) * 8;
                            this.eV = (this.hw + this.hr) * 16;
                            this.bQ = (this.hw + this.hr) * 8;
                            this.gU = this.eV / 2;
                            this.fO = this.bQ / 2;
                            this.cU = this.gU;
                            this.hc = this.fO;
                            this.db = this.cU;
                            this.aR = this.hc;
                            this.t = this.db * 2;
                            this.fj = this.aR * 2;
                            this.dQ = n6;
                            this.hf = n4;
                            this.ab = n5;
                            this.cK = n7;
                            this.gG = n8;
                            this.co = n9;
                            this.dK = n10;
                            this.bX = n11;
                            this.gf = n12;
                            this.fo = n13;
                            this.ew = n14;
                            this.aB = n15;
                            this.eU = n16;
                            this.cY = n17;
                            this.hk = n18;
                            this.aD = (byte)n19;
                            this.ez = n20 - 1;
                            this.ap = (byte)n22;
                            this.bs = n23;
                            this.fT = n24;
                            this.v(n25);
                            this.fp = (n26 & 2) == 2;
                            this.dy = (n26 & 4) == 4;
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
                        b2.a();
                    }
                    catch (Exception exception) {
                        // empty catch block
                    }
                    if (this.J <= 1) {
                        if (this.dJ == 0) {
                            this.cd = (byte)32;
                            this.fP = (byte)29;
                        }
                        if (this.dJ == 1) {
                            this.cd = (byte)25;
                            this.fP = (byte)22;
                        }
                        if (this.eG == 0) {
                            this.aD = (byte)4;
                            this.eC = 2;
                        }
                        if (this.eG == 1) {
                            this.aD = (byte)8;
                            this.eC = 5;
                        }
                        if (this.b == 0) {
                            this.gG /= 2;
                            this.co /= 2;
                        }
                        if (this.hy == 0) {
                            this.dK /= 2;
                            this.bX /= 2;
                            this.gf /= 2;
                            this.fo /= 2;
                            this.ew /= 2;
                            this.aB /= 2;
                        }
                    }
                }
                if (this.ct == 3) {
                    this.aT = 0;
                    this.bt = 0;
                    this.F(0);
                }
                if (this.ct <= 3) break;
                this.F(1);
                if (this.bV != this.dh) break;
                this.bV = 0;
                this.t(30);
                break;
            }
            case 30: {
                int n;
                int n27;
                this.em = System.currentTimeMillis();
                ++this.fy;
                if (this.fy == 2) {
                    this.fy = 0;
                    ++this.fh;
                    if (this.fh == 3) {
                        this.fh = 0;
                    }
                }
                ++this.gB;
                if (this.gB == 2) {
                    this.gB = 0;
                    ++this.gb;
                    if (this.gb == 3) {
                        this.gb = 0;
                    }
                }
                ++this.ei;
                if (this.ei == 3) {
                    this.ei = 0;
                    ++this.bc;
                    if (this.bc == 3) {
                        this.bc = 0;
                    }
                }
                ++this.eE;
                if (this.eE == 150) {
                    for (n27 = 0; n27 < this.hw; ++n27) {
                        for (n = 0; n < this.hr; ++n) {
                            if (this.cI[n27][n] < 59 || this.cI[n27][n] > 61 || this.gm.nextInt() % 3 != 0) continue;
                            byte[] byArray = this.cI[n27];
                            int n28 = n;
                            byArray[n28] = (byte)(byArray[n28] + 1);
                            if (this.cI[n27][n] != 62) continue;
                            this.cI[n27][n] = 59;
                        }
                    }
                    this.eE = 0;
                }
                ++this.dj;
                if (this.dj == 10) {
                    this.dj = 0;
                }
                for (n27 = 0; n27 < 50; ++n27) {
                    int n29;
                    if (this.ha[n27][0] <= 0) continue;
                    byte[] byArray = this.ha[n27];
                    byArray[0] = (byte)(byArray[0] - 1);
                    if (this.ha[n27][0] == 0) {
                        n = this.c(this.ha[n27][1]);
                        n29 = this.c(this.ha[n27][2]);
                        if (n >= 0 && n29 >= 0 && n < this.hw && n29 < this.hr) {
                            this.cI[n][n29] = (byte)(this.c(this.cI[n][n29]) - 128);
                        }
                    }
                    if (this.ha[n27][0] != 7) continue;
                    n = this.c(this.ha[n27][1]);
                    n29 = this.c(this.ha[n27][2]);
                    if (n < 0 || n29 < 0 || n >= this.hw || n29 >= this.hr) continue;
                    int n30 = this.c(this.cI[n][n29]) - 128;
                    if (this.go[n27]) continue;
                    if (n30 >= 29 && n30 <= 33) {
                        this.cI[n][n29] = -128;
                    }
                    if (n30 >= 70 && n30 <= 75 || n30 >= 87 && n30 <= 90) {
                        this.ea[n][n29] = 0;
                        this.cI[n][n29] = -128;
                        this.bH = -1;
                    }
                    if (n30 >= 77 && n30 <= 80) {
                        this.cI[n][n29] = (byte)(128 + n30 - 56);
                        this.ea[n][n29] = 0;
                        this.bH = -1;
                    }
                    if (n30 == 81 || n30 == 82) {
                        this.cI[n][n29] = -128;
                        this.ea[n][n29] = 0;
                        this.bH = -1;
                    }
                    if (n30 >= 83 && n30 <= 86) {
                        this.cI[n][n29] = (byte)(128 + n30 - 76);
                        this.ea[n][n29] = 0;
                        this.bH = -1;
                    }
                    if (n30 != 76) continue;
                    int n31 = this.ea[n][n29] & 7;
                    int n32 = (this.ea[n][n29] & 0x3F) >> 3;
                    if (n31 == 1 && n32 == 1) {
                        this.cI[n][n29] = -128;
                        this.ea[n][n29] = 0;
                    } else if (n31 == 1) {
                        this.cI[n][n29] = -57;
                        this.ea[n][n29] = (byte)(0x40 | n32);
                    } else if (n32 == 1) {
                        this.cI[n][n29] = -58;
                        this.ea[n][n29] = (byte)(0x40 | n31);
                    }
                    this.bH = -1;
                }
                for (n27 = 0; n27 < 500; ++n27) {
                    if ((this.ak[n27] & 0x80) != 128 || (this.ak[n27] & 0x40) != 64) continue;
                    if ((this.ak[n27] & 0x20) == 32) {
                        if (this.p[n27] > 0) {
                            int n33 = n27;
                            this.p[n33] = (byte)(this.p[n33] - 1);
                            continue;
                        }
                        this.u[n27] = 0;
                        this.ak[n27] = 0;
                        continue;
                    }
                    this.y(n27);
                    this.y(n27);
                    if (this.u[n27] != 0) continue;
                    this.i(n27);
                }
                if (this.bI >= this.dH) {
                    this.l();
                    if (this.gg > 0) {
                        --this.gg;
                    }
                    this.bI = 0;
                    ++this.bB;
                    this.Q();
                    if (this.bB % 4 == 0) {
                        this.g();
                    }
                    if (this.bB % 2 == 0) {
                        if (this.bB % 10 == 0) {
                            this.L();
                        }
                        this.q();
                        this.z();
                        if (this.fp) {
                            n27 = 0;
                            for (n = 0; n < 30; ++n) {
                                n27 = n27 != 0 || this.v() ? 1 : 0;
                            }
                            if (n27 != 0) {
                                this.d();
                            }
                        }
                        this.e(false);
                    }
                    n27 = this.eQ;
                    switch (this.eQ) {
                        case 1: 
                        case 3: 
                        case 5: 
                        case 7: 
                        case 8: 
                        case 10: 
                        case 12: {
                            if (this.bB != 32) break;
                            ++this.eQ;
                            break;
                        }
                        case 2: {
                            if ((this.bj % 4 != 0 || this.bB != 30) && (this.bj % 4 == 0 || this.bB != 29)) break;
                            ++this.eQ;
                            break;
                        }
                        case 4: 
                        case 6: 
                        case 9: 
                        case 11: {
                            if (this.bB != 31) break;
                            ++this.eQ;
                        }
                    }
                    if (n27 != this.eQ) {
                        this.bB = 1;
                        this.R();
                        this.P();
                        this.ag();
                        this.ad();
                        n = Math.abs(this.gm.nextInt() % 7);
                        switch (n) {
                            case 0: {
                                this.fb += 9;
                                this.ba += 9;
                                break;
                            }
                            case 1: {
                                ++this.cw;
                                this.eH += 3;
                                this.cz += 3;
                                break;
                            }
                            case 2: {
                                ++this.bO;
                                this.gT += 2;
                                break;
                            }
                            case 3: {
                                this.aW += 47;
                                this.hi += 98;
                                break;
                            }
                            case 4: {
                                this.c += 3;
                                this.hp += 59;
                                this.fK += 4;
                                break;
                            }
                            case 5: {
                                this.aG += 378;
                                this.eI += 432;
                                this.au += 518;
                                this.bZ += 601;
                                this.fC += 714;
                                this.ai += 826;
                                break;
                            }
                            case 6: {
                                this.fH += 45;
                                this.F += 51;
                                this.eP += 43;
                                this.fB += 50;
                            }
                        }
                    }
                    if (this.eQ == 13) {
                        this.w();
                        this.e();
                        this.eQ = 1;
                        ++this.bj;
                        if (this.eU < 0) {
                            this.d = false;
                            this.cq = 7;
                            this.t(20);
                        }
                    }
                    n = this.ez;
                    if (this.bB == 5 && this.eQ == 3 && this.bj == 1951 && this.ez < 2) {
                        this.ez = 2;
                    }
                    if (this.bB == 17 && this.eQ == 7 && this.bj == 1953 && this.ez < 3) {
                        this.ez = 3;
                    }
                    if (this.bB == 4 && this.eQ == 12 && this.bj == 1957 && this.ez < 4) {
                        this.ez = 4;
                    }
                    if (this.bB == 26 && this.eQ == 2 && this.bj == 1962 && this.ez < 5) {
                        this.ez = 5;
                    }
                    if (n != this.ez) {
                        this.d = false;
                        this.cq = 13;
                        this.t(20);
                    }
                } else {
                    ++this.bI;
                }
                this.ah = System.currentTimeMillis();
                break;
            }
            case 35: {
                if (this.ct > 0) {
                    --this.ct;
                }
                if (this.ct != 2) break;
                this.fG = null;
                System.gc();
                int n = this.hr + this.hw;
                this.fG = Image.createImage((int)n, (int)n);
                this.C = this.fG.getGraphics();
                this.C.setColor(0);
                this.C.fillRect(0, 0, n, n);
                int n34 = -1;
                int n35 = 0;
                for (int i = 0; i < this.hw; ++i) {
                    for (int j = 0; j < this.hr; ++j) {
                        int n36 = this.hw - i + j;
                        int n37 = i + j;
                        byte by = this.cI[i][j];
                        byte by2 = this.ea[i][j];
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
                                    if (this.bK[i][j] == -1) {
                                        this.C.setColor(3167360);
                                    }
                                    if (this.bK[i][j] == 0) {
                                        this.C.setColor(5274640);
                                    }
                                    if (this.bK[i][j] == 1) {
                                        this.C.setColor(6327328);
                                    }
                                    if (this.bK[i][j] == 2) {
                                        this.C.setColor(7380016);
                                    }
                                    if (this.bK[i][j] != 3) break;
                                    this.C.setColor(8432704);
                                    break;
                                }
                                case 2: {
                                    this.C.setColor(6327312);
                                    break;
                                }
                                case 3: {
                                    this.C.setColor(0x909040);
                                    break;
                                }
                                case 4: {
                                    this.C.setColor(0x606060);
                                    break;
                                }
                                case 5: {
                                    this.C.setColor(0x404040);
                                    break;
                                }
                                case 6: {
                                    this.C.setColor(252, 0, 0);
                                    break;
                                }
                                case 7: {
                                    this.C.setColor(16, 16, 16);
                                    break;
                                }
                                case 8: {
                                    this.C.setColor(252, 156, 0);
                                    break;
                                }
                                case 9: {
                                    this.C.setColor(120, 220, 30);
                                    break;
                                }
                                case 10: {
                                    this.C.setColor(252, 252, 0);
                                    break;
                                }
                                case 11: {
                                    this.C.setColor(128, 196, 252);
                                    break;
                                }
                                case 12: {
                                    this.C.setColor(0);
                                    break;
                                }
                                case 13: {
                                    this.C.setColor(14287063);
                                    break;
                                }
                                case 14: {
                                    this.C.setColor(16738557);
                                    break;
                                }
                                case 15: {
                                    this.C.setColor(16040947);
                                }
                            }
                        }
                        n34 = n35;
                        this.C.drawLine(n37, n36, n37 + 1, n36);
                    }
                    n34 = -1;
                }
                this.t(40);
                break;
            }
            case 40: {
                this.k();
                break;
            }
            case 45: {
                if (this.ct > 0) {
                    --this.ct;
                }
                if (this.ct != 2) break;
                if (this.K()) {
                    this.t(30);
                    this.aT = 0;
                    this.aC = true;
                    this.bt = 2;
                    this.bu = true;
                    break;
                }
                this.aT = 0;
                this.aC = true;
                this.bt = 1;
                this.bu = true;
                this.d = false;
                this.cq = 12;
                this.t(20);
                break;
            }
            case 55: {
                if (this.ct > 0) {
                    --this.ct;
                }
                if (this.ct != 2) break;
                this.t();
                this.aT = 0;
                this.bu = true;
                this.t(30);
                break;
            }
            case 65: {
                if (this.ct > 0) {
                    --this.ct;
                }
                if (this.ct != 2) break;
                this.u();
                this.bJ = false;
                this.aT = 0;
                this.bu = true;
                this.t(30);
                break;
            }
            case 105: {
                if (this.ct > 0) {
                    --this.ct;
                }
                if (this.ct != 2) break;
                if (this.hu - 66 < this.T) {
                    this.c();
                    this.t(30);
                    break;
                }
                ++this.hu;
                if (this.hu == 69) {
                    this.hu = 66;
                }
                this.t(30);
                break;
            }
            case 120: {
                if (this.ct > 0) {
                    --this.ct;
                }
                if (this.ct != 2) break;
                this.cq = this.b(this.ad) ? 19 : 18;
                this.t(20);
            }
        }
    }

    public void f(Graphics graphics) {
        int n;
        int n2;
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        int n8;
        int n9 = this.cU - 2 * this.hc + 16 + 2 * this.hj;
        int n10 = this.cU + 2 * this.hc - 16 - 2 * this.hj;
        n9 /= 32;
        n10 /= 32;
        if (this.cU != this.bH || this.hc != this.dW) {
            this.C.setClip(0, 15, e, eN);
            this.C.setColor(0);
            this.C.fillRect(0, 15, e, eN);
            this.es = 0;
            this.cl = 2 * this.hw;
            this.gM = 2 * this.hr;
            this.fE = -this.hw;
            this.eO = -this.hw;
            for (n8 = 0; n8 < this.bS; ++n8) {
                n7 = n9 + this.gl[n8];
                int n11 = n10 + this.av[n8];
                if (n7 >= 0 && n7 < this.hw && n11 >= 0 && n11 < this.hr) {
                    n6 = this.c(this.cI[n7][n11]);
                    if (n6 >= 128) {
                        n6 -= 128;
                    }
                    if (!(n6 != 0 && n6 <= 28 || n6 >= 77 && n6 <= 86)) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.cU;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.hc - 4 * this.bK[n7][n11] + 15;
                        if (this.bK[n7][n11] > -1) {
                            if (n6 >= 42 && n6 <= 48 || n6 >= 70 && n6 <= 76) {
                                this.C.drawImage(this.fU[this.gt[n6]], n5, n4, 33);
                            } else {
                                this.C.drawImage(this.fU[this.gt[1]], n5, n4, 33);
                            }
                        } else {
                            this.C.drawImage(this.fU[this.gt[2]], n5, n4, 33);
                        }
                    }
                    if (n6 >= 3 && n6 <= 28) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.cU;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.hc - 4 * this.bK[n7][n11] + 15;
                        this.C.drawImage(this.fU[this.gt[n6]], n5, n4 + this.gc[n6 - 3], 33);
                    }
                    if (n6 >= 77 && n6 <= 80) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.cU;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.hc - 4 * this.bK[n7][n11] + 15;
                        this.C.drawImage(this.fU[this.gt[n6]], n5, n4 + this.gc[n6 - 56 - 3], 33);
                    }
                    if (n6 >= 80 && n6 <= 86) {
                        n5 = n11 * 16 + n7 * 16 + 16 - this.cU;
                        n4 = this.hj - n7 * 8 + n11 * 8 + 16 - this.hc - 4 * this.bK[n7][n11] + 15;
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
                        this.C.drawImage(this.fU[this.gt[n6]], n5, n4 + n3, 33);
                    }
                }
                if (n7 > this.fE) {
                    this.fE = n7;
                }
                if (n11 > this.eO) {
                    this.eO = n11;
                }
                if (n7 < this.cl) {
                    this.cl = n7;
                }
                if (n11 >= this.gM) continue;
                this.gM = n11;
            }
            if (this.cl < 0) {
                this.cl = 0;
            }
            if (this.gM < 0) {
                this.gM = 0;
            }
            if (this.fE >= this.hw) {
                this.fE = this.hw - 1;
            }
            if (this.eO >= this.hr) {
                this.eO = this.hr - 1;
            }
            this.bH = this.cU;
            this.dW = this.hc;
            this.C.setClip(0, 0, e, hs);
        }
        this.a(graphics);
        graphics.drawImage(this.fG, 0, 0, 20);
        graphics.setClip(0, 15, e, eN);
        this.c(graphics);
        n8 = this.fE - this.cl + 1;
        n7 = this.eO - this.gM + 1;
        if (n8 < 1) {
            n8 = 1;
        }
        if (n7 < 1) {
            n7 = 1;
        }
        boolean[][] blArray = new boolean[n8][n7];
        this.es = 0;
        for (n6 = 0; n6 < 500; ++n6) {
            if ((this.ak[n6] & 0x80) != 128) continue;
            n5 = this.c(this.eo[n6]);
            n4 = this.c(this.af[n6]);
            if (n5 < this.cl || n4 < this.gM || n5 > this.fE || n4 > this.eO) continue;
            this.bU[this.es++] = (short)n6;
            blArray[n5 - this.cl][n4 - this.gM] = true;
        }
        for (n6 = 0; n6 < this.bS; ++n6) {
            int n12;
            n5 = n9 + this.gl[n6];
            n4 = n10 + this.av[n6];
            if (n5 < 0 || n5 >= this.hw || n4 < 0 || n4 >= this.hr) continue;
            n3 = this.c(this.cI[n5][n4]);
            n2 = n3;
            if (n2 >= 128) {
                n2 -= 128;
            }
            if (n2 > 28 && n2 < 128 && ((n2 < 42 || n2 > 48) && n2 < 70 || n2 > 86)) {
                n = n4 * 16 + n5 * 16 + 16 - this.cU;
                n12 = this.hj - n5 * 8 + n4 * 8 + 16 - this.hc - 4 * this.bK[n5][n4] + 15;
                if (!ch) {
                    if (n2 == 54 || n2 == 55) {
                        graphics.drawImage(this.fU[this.gt[53]], n, n12, 33);
                    }
                    if (this.dj == 1 && n2 >= 65 && n2 <= 68) {
                        byte[] byArray = this.cI[n5];
                        int n13 = n4;
                        byArray[n13] = (byte)(byArray[n13] + 1);
                        if (this.cI[n5][n4] == 69) {
                            this.cI[n5][n4] = 65;
                        }
                    }
                    if (n2 != 55) {
                        graphics.drawImage(this.fU[this.gt[n2]], n, n12, 33);
                    } else {
                        graphics.drawImage(this.fU[this.gt[243 + this.bc]], n, n12, 33);
                    }
                    if (n2 == 50) {
                        graphics.drawImage(this.fU[this.gt[240 + this.fh]], n + 12, n12 - 28, 33);
                    }
                    if (n2 == 64) {
                        graphics.drawImage(this.fU[this.gt[246 + this.gb]], n + 3, n12 - 39, 33);
                    }
                } else {
                    graphics.drawImage(this.fU[this.gt[41]], n, n12, 33);
                }
            }
            if (blArray[n5 - this.cl][n4 - this.gM]) {
                for (n = 0; n < this.es; ++n) {
                    n12 = this.bU[n];
                    if ((this.ak[n12] & 0x80) != 128 || n5 != this.c(this.eo[n12]) || n4 != this.c(this.af[n12])) continue;
                    this.b(graphics, n12);
                }
            }
            if (n3 < 128) continue;
            n = n4 * 16 + n5 * 16 + 16 - this.cU;
            n12 = this.hj - n5 * 8 + n4 * 8 + 16 - this.hc - 4 * this.bK[n5][n4] + 15;
            int n14 = -1;
            for (int i = 0; i < 50; ++i) {
                if (this.c(this.ha[i][1]) != n5 || this.c(this.ha[i][2]) != n4) continue;
                n14 = this.ha[i][0];
            }
            if (n14 <= 0) continue;
            graphics.drawImage(this.fU[this.c(this.gt[250 + n14 / 3])], n, n12 - 3, 33);
        }
        if (!aP) {
            for (n6 = 0; n6 < this.bS; ++n6) {
                n5 = n9 + this.gl[n6];
                n4 = n10 + this.av[n6];
                for (n3 = 0; n3 < this.bw; ++n3) {
                    if (this.gi[n3] != n5 || this.bL[n3] != n4) continue;
                    n2 = n4 * 16 + n5 * 16 + 16 - this.cU;
                    n = this.hj - n5 * 8 + n4 * 8 + 16 - this.hc - 4 * this.bK[n5][n4] + 15;
                    this.a(graphics, n2, n - 16, (byte)n3);
                }
            }
        }
        this.e(graphics);
        graphics.setClip(0, 0, e, hs);
        this.d(graphics);
    }

    public void hideNotify() {
        this.cv = true;
        if (this.bF > 10 && this.bF != 120) {
            this.E();
        }
        if (this.bF == 30) {
            this.d = false;
            this.cq = 1;
            this.t(20);
        }
    }

    public void showNotify() {
        if (this.bF > 10 && this.bF != 120) {
            this.ae();
        }
        this.cv = false;
    }
}
