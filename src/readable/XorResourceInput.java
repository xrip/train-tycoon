import java.io.InputStream;

/**
 * Reader used by packed resources /1 through /5.
 *
 * Each source byte is XORed with a key that cycles through 1..127. This is
 * obfuscation rather than compression or cryptography. The implementation is
 * behaviorally equivalent to the original class {@code b}.
 */
public final class XorResourceInput {
    private final InputStream input;
    private int xorKey = 1;

    public XorResourceInput(String resourceName) {
        input = getClass().getResourceAsStream(resourceName);
    }

    public int read() {
        try {
            int value = input.read();
            if (value < 0) {
                return -1;
            }

            value ^= xorKey;
            xorKey++;
            if (xorKey == 128) {
                xorKey = 1;
            }
            return value;
        } catch (Exception ignored) {
            return -1;
        }
    }

    public void close() {
        try {
            input.close();
        } catch (Exception ignored) {
        }
    }
}
