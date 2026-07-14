/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  javax.microedition.midlet.MIDlet
 */
import javax.microedition.midlet.MIDlet;

public class GameMIDlet
extends MIDlet {
    a a;
    boolean b = false;

    public void startApp() {
        if (!this.b) {
            this.b = true;
            this.a = new a();
            this.a.a(this);
        }
    }

    public void pauseApp() {
        this.a.hideNotify();
    }

    public void destroyApp(boolean bl) {
    }
}
