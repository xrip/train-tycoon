import javax.microedition.lcdui.Image;

/** Reconstructs PNG images stored in the decoded /1 resource stream. */
public final class PackedImageReader {
    private static final byte[] PNG_SIGNATURE = {
        (byte) 0x89, 'P', 'N', 'G', 13, 10, 26, 10
    };

    private final XorResourceInput input;

    public PackedImageReader(XorResourceInput input) {
        this.input = input;
    }

    public Image readImage() {
        byte[] png = readPngBytes();
        return Image.createImage(png, 0, png.length);
    }

    public byte[] readPngBytes() {
        int storageType = readByte();
        if (storageType == 2) {
            return readAlmostCompletePng();
        }
        if (storageType == 1) {
            return rebuildIndexedPng();
        }
        throw new IllegalStateException("Unknown packed image type: " + storageType);
    }

    /** Type 2 stores a complete PNG except for the first four signature bytes. */
    private byte[] readAlmostCompletePng() {
        int length = readUnsignedShort();
        byte[] png = new byte[length];
        png[0] = (byte) 0x89;
        png[1] = 'P';
        png[2] = 'N';
        png[3] = 'G';
        readFully(png, 4, length - 4);
        return png;
    }

    /**
     * Type 1 stores the IHDR fields, PLTE chunk and IDAT chunk separately.
     * Chunk CRCs are already present in the resource; this method rebuilds the
     * standard PNG envelope exactly as the original game does.
     */
    private byte[] rebuildIndexedPng() {
        int idatLength = readUnsignedShort();
        int paletteLength = readByte() * 3;
        boolean hasTransparency = readByte() == 1;

        int totalLength = idatLength + paletteLength + 69
                + (hasTransparency ? 13 : 0);
        byte[] png = new byte[totalLength];
        copy(PNG_SIGNATURE, png, 0);

        // IHDR: 13-byte payload. Width and height are stored as unsigned shorts.
        png[11] = 13;
        writeAscii(png, 12, "IHDR");
        png[18] = (byte) readByte();
        png[19] = (byte) readByte();
        png[22] = (byte) readByte();
        png[23] = (byte) readByte();
        png[24] = (byte) readByte(); // bit depth
        png[25] = 3;                // indexed colour
        readFully(png, 29, 4);      // IHDR CRC

        // PLTE chunk.
        png[35] = (byte) (paletteLength >>> 8);
        png[36] = (byte) paletteLength;
        writeAscii(png, 37, "PLTE");
        readFully(png, 41, paletteLength + 4); // palette + CRC

        int offset = 45 + paletteLength;
        if (hasTransparency) {
            offset += 3;             // high bytes of chunk length are zero
            png[offset++] = 1;       // one-byte tRNS payload
            writeAscii(png, offset, "tRNS");
            offset += 4;
            offset++;                // transparent palette index is zero
            png[offset++] = 0x40;
            png[offset++] = (byte) 0xE6;
            png[offset++] = (byte) 0xD8;
            png[offset++] = 0x66;    // fixed CRC for tRNS {0}
        }

        // IDAT chunk.
        offset += 2;                 // high bytes of chunk length are zero
        png[offset++] = (byte) (idatLength >>> 8);
        png[offset++] = (byte) idatLength;
        writeAscii(png, offset, "IDAT");
        offset += 4;
        readFully(png, offset, idatLength + 4); // compressed data + CRC
        offset += idatLength + 4;

        // IEND chunk.
        offset += 4;                 // zero payload length
        writeAscii(png, offset, "IEND");
        offset += 4;
        png[offset++] = (byte) 0xAE;
        png[offset++] = 0x42;
        png[offset++] = 0x60;
        png[offset] = (byte) 0x82;
        return png;
    }

    private int readUnsignedShort() {
        return (readByte() << 8) | readByte();
    }

    private int readByte() {
        int value = input.read();
        if (value < 0) {
            throw new IllegalStateException("Unexpected end of packed resource");
        }
        return value;
    }

    private void readFully(byte[] target, int offset, int length) {
        for (int i = 0; i < length; i++) {
            target[offset + i] = (byte) readByte();
        }
    }

    private static void writeAscii(byte[] target, int offset, String text) {
        for (int i = 0; i < text.length(); i++) {
            target[offset + i] = (byte) text.charAt(i);
        }
    }

    private static void copy(byte[] source, byte[] target, int offset) {
        for (int i = 0; i < source.length; i++) {
            target[offset + i] = source[i];
        }
    }
}
