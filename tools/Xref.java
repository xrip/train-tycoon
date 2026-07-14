import org.objectweb.asm.*;
import org.objectweb.asm.tree.*;

import java.io.*;
import java.nio.file.*;
import java.util.*;

/**
 * Cross-reference report for the obfuscated GameCanvas (class `a`).
 *
 * Descriptor-aware (uses member identity, no text matching), so it is free of
 * the substring/scope false positives that plague grep-based analysis.
 *
 * For every FIELD  : which methods read it / write it.
 * For every METHOD : its callers, its callees, the fields it reads/writes,
 *                    and the string / int constants it references.
 * Members are auto-tagged by association with known strings
 * (RMS / net / audio / resources) to seed subsystem discovery.
 *
 * Usage: java Xref <in-dir-of-.class> <targetInternalName> <out.md>
 */
public class Xref {
    // ---- readable descriptor rendering ----
    static String type(String d) {
        int arr = 0;
        while (d.startsWith("[")) { arr++; d = d.substring(1); }
        String base;
        switch (d.charAt(0)) {
            case 'V': base = "void"; break;
            case 'Z': base = "boolean"; break;
            case 'B': base = "byte"; break;
            case 'C': base = "char"; break;
            case 'S': base = "short"; break;
            case 'I': base = "int"; break;
            case 'J': base = "long"; break;
            case 'F': base = "float"; break;
            case 'D': base = "double"; break;
            case 'L': base = d.substring(1, d.indexOf(';')).replace('/', '.'); break;
            default:  base = d;
        }
        StringBuilder b = new StringBuilder(base);
        for (int i = 0; i < arr; i++) b.append("[]");
        return b.toString();
    }
    static String[] params(String desc) {
        List<String> out = new ArrayList<>();
        int i = 1;
        while (desc.charAt(i) != ')') {
            int s = i;
            while (desc.charAt(i) == '[') i++;
            if (desc.charAt(i) == 'L') i = desc.indexOf(';', i);
            out.add(type(desc.substring(s, i + 1)));
            i++;
        }
        return out.toArray(new String[0]);
    }
    static String ret(String desc) { return type(desc.substring(desc.indexOf(')') + 1)); }
    // compact, human-readable + unique method label
    static String sig(String name, String desc) {
        return name + "(" + String.join(",", params(desc)) + ")";
    }

    static String tagFor(Set<String> strings, Set<String> calls) {
        List<String> t = new ArrayList<>();
        for (String s : strings) {
            if (s.equals("TTStore")) t.add("RMS/save");
            if (s.equals("GET") || s.equals("audio/midi")) { /* handled below */ }
        }
        if (strings.contains("TTStore")) t.add("RMS");
        if (strings.contains("GET") || calls.stream().anyMatch(c -> c.contains("HttpConnection") || c.contains("Connector")))
            t.add("net");
        if (strings.contains("audio/midi") || calls.stream().anyMatch(c -> c.contains("media/Manager") || c.contains("Player")))
            t.add("audio");
        for (String s : strings) if (s.matches("/[1-6]")) { t.add("resource"); break; }
        if (calls.stream().anyMatch(c -> c.startsWith("Graphics."))) t.add("render");
        LinkedHashSet<String> uniq = new LinkedHashSet<>(t);
        return uniq.isEmpty() ? "" : " [" + String.join(",", uniq) + "]";
    }

    public static void main(String[] args) throws IOException {
        String inDir = args[0], target = args[1], outPath = args[2];
        ClassNode cn = new ClassNode();
        try (InputStream is = Files.newInputStream(Paths.get(inDir, target + ".class"))) {
            new ClassReader(is).accept(cn, ClassReader.SKIP_FRAMES);
        }

        // maps keyed by member label
        Map<String, Set<String>> fieldReaders = new TreeMap<>();
        Map<String, Set<String>> fieldWriters = new TreeMap<>();
        Map<String, String>      fieldType    = new TreeMap<>();
        for (FieldNode f : cn.fields) {
            fieldType.put(f.name, type(f.desc));
            fieldReaders.put(f.name, new TreeSet<>());
            fieldWriters.put(f.name, new TreeSet<>());
        }

        Map<String, Set<String>> methCallers  = new TreeMap<>();
        Map<String, Set<String>> methCalls    = new TreeMap<>();
        Map<String, Set<String>> methReads     = new TreeMap<>();
        Map<String, Set<String>> methWrites    = new TreeMap<>();
        Map<String, Set<String>> methStrings   = new TreeMap<>();
        Map<String, String>      methRet       = new TreeMap<>();
        Map<String, Integer>     nameCount     = new TreeMap<>();

        for (MethodNode mn : cn.methods) {
            String self = sig(mn.name, mn.desc);
            nameCount.merge(mn.name, 1, Integer::sum);
            methRet.putIfAbsent(self, ret(mn.desc));
            methCallers.putIfAbsent(self, new TreeSet<>());
            methCalls.putIfAbsent(self, new TreeSet<>());
            methReads.putIfAbsent(self, new TreeSet<>());
            methWrites.putIfAbsent(self, new TreeSet<>());
            methStrings.putIfAbsent(self, new TreeSet<>());

            for (AbstractInsnNode in : mn.instructions) {
                if (in instanceof FieldInsnNode) {
                    FieldInsnNode fi = (FieldInsnNode) in;
                    if (!fi.owner.equals(cn.name)) continue;
                    boolean write = fi.getOpcode() == Opcodes.PUTFIELD || fi.getOpcode() == Opcodes.PUTSTATIC;
                    (write ? methWrites : methReads).get(self).add(fi.name);
                    (write ? fieldWriters : fieldReaders).get(fi.name).add(self);
                } else if (in instanceof MethodInsnNode) {
                    MethodInsnNode mi = (MethodInsnNode) in;
                    if (mi.owner.equals(cn.name)) {
                        String callee = sig(mi.name, mi.desc);
                        methCalls.get(self).add(callee);
                        methCallers.computeIfAbsent(callee, k -> new TreeSet<>()).add(self);
                    } else {
                        // external call: record short owner.method for tagging
                        String owner = mi.owner.substring(mi.owner.lastIndexOf('/') + 1);
                        methCalls.get(self).add(owner + "." + mi.name + " (ext)");
                    }
                } else if (in instanceof LdcInsnNode) {
                    Object c = ((LdcInsnNode) in).cst;
                    if (c instanceof String) methStrings.get(self).add((String) c);
                }
            }
        }

        StringBuilder sb = new StringBuilder();
        sb.append("# GameCanvas (obf `").append(target).append("`) cross-reference\n\n");
        sb.append(cn.fields.size()).append(" fields, ").append(cn.methods.size()).append(" methods.\n\n");

        sb.append("## Overloaded method-name groups\n\n");
        nameCount.entrySet().stream().filter(e -> e.getValue() > 1)
            .sorted((x, y) -> y.getValue() - x.getValue())
            .forEach(e -> sb.append("- `").append(e.getKey()).append("` x").append(e.getValue()).append("\n"));

        sb.append("\n## Methods\n\n");
        for (MethodNode mn : cn.methods) {
            String self = sig(mn.name, mn.desc);
            String tag = tagFor(methStrings.get(self), methCalls.get(self));
            sb.append("### ").append(methRet.get(self)).append(" ").append(self).append(tag).append("\n");
            sb.append("- callers: ").append(fmt(methCallers.get(self))).append("\n");
            sb.append("- reads: ").append(fmt(methReads.get(self))).append("\n");
            sb.append("- writes: ").append(fmt(methWrites.get(self))).append("\n");
            if (!methStrings.get(self).isEmpty())
                sb.append("- strings: ").append(methStrings.get(self)).append("\n");
            sb.append("- calls: ").append(fmt(methCalls.get(self))).append("\n\n");
        }

        sb.append("## Fields\n\n");
        for (FieldNode f : cn.fields) {
            sb.append("- `").append(fieldType.get(f.name)).append(" ").append(f.name).append("`")
              .append("  R=").append(fieldReaders.get(f.name).size())
              .append(" W=").append(fieldWriters.get(f.name).size()).append("\n");
            sb.append("    - read by: ").append(fmt(fieldReaders.get(f.name))).append("\n");
            sb.append("    - written by: ").append(fmt(fieldWriters.get(f.name))).append("\n");
        }

        Files.write(Paths.get(outPath), sb.toString().getBytes("UTF-8"));
        System.out.println("wrote " + outPath + " (" + sb.length() + " bytes)");
    }

    static String fmt(Set<String> s) { return s.isEmpty() ? "-" : String.join(", ", s); }
}
