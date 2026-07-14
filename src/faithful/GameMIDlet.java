import javax.microedition.midlet.MIDlet;

/** Bytecode-faithful reconstruction. Original identifiers are preserved. */
public class GameMIDlet extends MIDlet {
    a a;
    boolean b = false;

    public GameMIDlet() {
    }

    public void startApp() {
        if (!b) {
            b = true;
            a = new a();
            a.a(this);
        }
    }

    public void pauseApp() {
        a.hideNotify();
    }

    public void destroyApp(boolean unconditional) {
        // Empty in the original bytecode.
    }
}
