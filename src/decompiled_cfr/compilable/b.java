/*
 * Decompiled with CFR 0.152.
 */
import java.io.InputStream;

public class b {
    private InputStream a;
    private int b;

    public b(String string) {
        this.a = this.getClass().getResourceAsStream(string);
        this.b = 1;
    }

    public int b() {
        try {
            int n = this.a.read();
            if (n < 0) {
                return n;
            }
            n ^= this.b;
            ++this.b;
            if (this.b == 128) {
                this.b = 1;
            }
            return n;
        }
        catch (Exception exception) {
            return -1;
        }
    }

    public void a() {
        try {
            this.a.close();
        }
        catch (Exception exception) {
            // empty catch block
        }
    }
}
