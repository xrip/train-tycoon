import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.commons.ClassRemapper;
import org.objectweb.asm.commons.SimpleRemapper;

import java.io.*;
import java.nio.file.*;
import java.util.*;

/**
 * Descriptor-aware bytecode remapper for the Train Tycoon deobfuscation.
 *
 * Renames are keyed by member identity (owner + name + descriptor), so an
 * overloaded method `a(...)`, a field `a`, and a class `a` are three distinct
 * things and never collide. This is a pure rename: behaviour is unchanged, so
 * the verified faithfulness of the decompilation is preserved by construction.
 *
 * Usage: java Remap <mappings.txt> <in-dir-of-.class> <out-dir>
 *
 * mappings.txt line formats (whitespace separated, # = comment):
 *   class  <oldInternal>            <newInternal>
 *   field  <owner> <name>           <newName>
 *   method <owner> <name> <descr>   <newName>
 */
public class Remap {
    public static void main(String[] args) throws IOException {
        if (args.length != 3) {
            System.err.println("usage: Remap <mappings.txt> <in-dir> <out-dir>");
            System.exit(2);
        }
        Map<String, String> m = new HashMap<>();
        int n = 0;
        for (String raw : Files.readAllLines(Paths.get(args[0]))) {
            String line = raw.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] t = line.split("\\s+");
            switch (t[0]) {
                case "class":  m.put(t[1], t[2]); n++; break;
                case "field":  m.put(t[1] + "." + t[2], t[3]); n++; break;
                case "method": m.put(t[1] + "." + t[2] + t[3], t[4]); n++; break;
                default: System.err.println("skip: " + line);
            }
        }
        SimpleRemapper remapper = new SimpleRemapper(m);

        Path in = Paths.get(args[1]), out = Paths.get(args[2]);
        Files.createDirectories(out);
        int classes = 0;
        for (Path p : (Iterable<Path>) Files.list(in)::iterator) {
            if (!p.toString().endsWith(".class")) continue;
            byte[] src = Files.readAllBytes(p);
            ClassReader cr = new ClassReader(src);
            ClassWriter cw = new ClassWriter(0);
            cr.accept(new ClassRemapper(cw, remapper), 0);
            // output file name follows the (possibly renamed) class name
            String oldInternal = cr.getClassName();
            String newInternal = m.getOrDefault(oldInternal, oldInternal);
            Files.write(out.resolve(newInternal + ".class"), cw.toByteArray());
            classes++;
        }
        System.out.println("applied " + n + " mappings to " + classes + " classes");
    }
}
