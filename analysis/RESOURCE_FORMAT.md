# Resource and image format

## XOR wrapper

For each byte, the game computes `decoded = encoded XOR key`. The key begins at 1, increments after every byte, wraps from 127 back to 1, and is reset for each newly opened resource.

## `/1` image stream

The decoded stream contains 200 consecutive image records.

### Record type 2

```text
byte  type = 2
u16   totalPngLength
byte[totalPngLength - 4] PNG data starting at signature byte 4
```

The game prepends `89 50 4E 47`.

### Record type 1

A compact indexed-colour PNG representation containing:

- IDAT payload length;
- palette entry count;
- optional one-byte `tRNS` flag;
- width, height and bit depth;
- precomputed IHDR CRC;
- PLTE payload and CRC;
- IDAT payload and CRC.

The game recreates the PNG signature, chunk lengths/names, optional fixed `tRNS` chunk and `IEND` chunk in memory. `PackedImageReader.java` is a direct readable translation. All generated files passed PNG CRC validation.
