/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  javax.microedition.midlet.MIDlet
 */
import javax.microedition.midlet.MIDlet;

public class GameMIDlet
extends MIDlet {
    GameCanvas canvas;
    boolean started = false;

    public void startApp() {
        if (!this.started) {
            this.started = true;
            this.canvas = new GameCanvas();
            this.canvas.start(this);
        }
    }

    public void pauseApp() {
        this.canvas.hideNotify();
    }

    public void destroyApp(boolean bl) {
    }
}
