import javax.microedition.midlet.MIDlet;

/** Entry point of Train Tycoon. */
public final class GameMIDlet extends MIDlet {
    private TrainTycoonCanvas game;
    private boolean started;

    protected void startApp() {
        if (started) {
            return;
        }

        started = true;
        game = new TrainTycoonCanvas();
        game.start(this);
    }

    protected void pauseApp() {
        if (game != null) {
            game.onApplicationHidden();
        }
    }

    protected void destroyApp(boolean unconditional) {
        // The original MIDlet leaves shutdown to the Canvas thread.
    }
}
