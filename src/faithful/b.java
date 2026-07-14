import java.io.InputStream;

/** Bytecode-faithful reconstruction. Original identifiers are preserved. */
public class b {
    private InputStream a;
    private int b;

    public b(String resourceName) {
        a = getClass().getResourceAsStream(resourceName);
        b = 1;
    }

    public int b() {
        try {
            int value = a.read();
            if (value < 0) {
                return value;
            }

            value ^= b;
            if (++b == 128) {
                b = 1;
            }
            return value;
        } catch (Exception ignored) {
            return -1;
        }
    }

    public void a() {
        try {
            a.close();
        } catch (Exception ignored) {
        }
    }
}
