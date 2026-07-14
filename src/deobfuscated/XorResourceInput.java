/*
 * Decompiled with CFR 0.152.
 */
import java.io.InputStream;

public class XorResourceInput {
    private InputStream in;
    private int key;

    public XorResourceInput(String string) {
        this.in = this.getClass().getResourceAsStream(string);
        this.key = 1;
    }

    public int readByte() {
        try {
            int n = this.in.read();
            if (n < 0) {
                return n;
            }
            n ^= this.key;
            ++this.key;
            if (this.key == 128) {
                this.key = 1;
            }
            return n;
        }
        catch (Exception exception) {
            return -1;
        }
    }

    public void close() {
        try {
            this.in.close();
        }
        catch (Exception exception) {
            // empty catch block
        }
    }
}
