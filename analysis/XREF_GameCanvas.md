# GameCanvas (obf `a`) cross-reference

441 fields, 198 methods.

## Overloaded method-name groups

- `a` x39
- `b` x15
- `c` x9
- `d` x7
- `e` x6
- `f` x5
- `g` x5
- `h` x5
- `i` x4
- `j` x4
- `k` x3
- `l` x3
- `m` x3
- `n` x3
- `o` x3
- `p` x3
- `q` x3
- `r` x3
- `s` x3
- `t` x3
- `u` x3
- `v` x3
- `w` x3
- `x` x3
- `y` x3
- `z` x3
- `A` x2
- `B` x2
- `C` x2
- `D` x2
- `E` x2
- `F` x2

## Methods

### void <init>()
- callers: -
- reads: bQ, e, eV, fG, fO, gU, hr, hs, hw
- writes: A, B, C, D, E, H, K, L, M, N, O, S, T, U, V, W, X, Y, Z, a, aA, aC, aD, aE, aF, aH, aI, aJ, aK, aM, aO, aR, aS, aT, aU, aV, aZ, ac, ad, ae, af, aj, ak, al, ao, aq, ar, as, at, av, aw, ay, b, bB, bC, bD, bE, bG, bH, bI, bJ, bL, bM, bP, bQ, bR, bS, bT, bU, bV, bW, bY, bb, bc, bd, be, bh, bi, bj, bk, bm, bo, br, bt, bu, bv, bw, bx, by, bz, cB, cC, cD, cE, cF, cG, cH, cJ, cM, cO, cR, cT, cU, cV, cW, cY, cZ, ca, cb, cd, ce, cg, ci, cj, cm, cq, cr, cs, ct, cu, cv, cx, cy, d, dA, dB, dC, dE, dF, dG, dH, dI, dJ, dN, dP, dS, dT, dU, dV, dW, da, db, dc, dd, df, dg, dh, di, dj, dk, dl, dm, dn, do, dp, dq, dr, du, dv, dw, dx, dz, eA, eB, eC, eD, eE, eF, eG, eJ, eK, eL, eM, eQ, eR, eS, eT, eU, eV, eX, eb, ed, ee, ef, eg, eh, ei, ej, eo, ep, eq, er, es, et, eu, ev, ey, ez, f, fA, fD, fF, fG, fI, fJ, fL, fO, fP, fR, fS, fW, fa, fc, fd, fe, fh, fi, fj, fk, fn, fp, fq, fr, fs, fu, fw, fx, fy, g, gA, gB, gC, gE, gF, gH, gK, gL, gN, gP, gQ, gR, gU, gV, gW, gY, ga, gb, gc, gd, ge, gg, gh, gi, gj, gk, gl, gm, go, gq, gr, gs, gt, gu, gv, gw, gx, gy, h, ha, hb, hc, he, hg, hh, hj, hk, hl, hm, hn, ho, hq, ht, hu, hv, hx, hy, j, m, o, p, s, t, u, v, w, x, y, z
- calls: Canvas.<init> (ext), Image.createImage (ext), Image.getGraphics (ext), Random.<init> (ext), System.currentTimeMillis (ext)

### int c(byte)
- callers: B(int), H(), Q(), a(int,boolean), a(int,int,boolean,boolean), a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,int,int), ad(), b(int,int,boolean), b(javax.microedition.lcdui.Graphics,int), c(int), c(java.lang.String), e(), e(boolean), e(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), h(), i(int), i(int,int,int), j(), j(int,int,int), m(), q(), q(int,int), r(int,int), s(int,int), u(int,int), x(int,int), y(int)
- reads: -
- writes: -
- calls: -

### void a(GameMIDlet)
- callers: -
- reads: ex, gn
- writes: ex, gn
- calls: Display.getDisplay (ext), Display.setCurrent (ext), Thread.<init> (ext), Thread.start (ext), Z(), t(int)

### void run()
- callers: -
- reads: aF, aS, aT, aZ, bF, bJ, bt, cP, cS, cX, cp, cv, dw, ev, ex, gQ, ge, gg, gn, gy, z
- writes: aF, aS, cS, cX, cp, dL, dw, ev, ge, gg, gy
- calls: B(), Canvas.repaint (ext), Canvas.serviceRepaints (ext), Display.getDisplay (ext), Display.setCurrent (ext), E(), MIDlet.notifyDestroyed (ext), System.currentTimeMillis (ext), Thread.sleep (ext), Thread.yield (ext), d(int), j(int), m()

### void pointerPressed(int,int)
- callers: -
- reads: ev
- writes: -
- calls: -

### void pointerReleased(int,int)
- callers: -
- reads: ev
- writes: ev
- calls: -

### void pointerDragged(int,int)
- callers: -
- reads: ev
- writes: -
- calls: -

### void keyPressed(int)
- callers: -
- reads: ev
- writes: aS, ev
- calls: -

### void keyReleased(int)
- callers: -
- reads: -
- writes: aS
- calls: -

### void a(javax.microedition.lcdui.Graphics,int,int,int,int) [render]
- callers: a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int,int,int,boolean), e(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics)
- reads: fU, gt
- writes: -
- calls: Graphics.drawImage (ext), c(byte)

### void j(int)
- callers: run()
- reads: bF, bt, cR, dw, fI
- writes: aF, aS, aT, bk, bt, bu, cE, cR, cb, cq, d, dB, dZ, dw, ej, gF, z
- calls: Canvas.getGameAction (ext), a(byte), ah(), c(boolean), t(int)

### void d(int)
- callers: run()
- reads: -
- writes: aF, aS, bk, dw, ej, z
- calls: -

### void a(java.lang.String)
- callers: t(int)
- reads: -
- writes: aU
- calls: b.<init> (ext)

### javax.microedition.lcdui.Image aa()
- callers: m(), t(int)
- reads: aU
- writes: -
- calls: Image.createImage (ext), b.b (ext)

### void o()
- callers: m(), paint(javax.microedition.lcdui.Graphics), t(int), v(int,int)
- reads: -
- writes: eB, eZ, fk, gF, gZ, hd, m, x
- calls: -

### void b(java.lang.String)
- callers: t(int)
- reads: dY, gz
- writes: dY, gz
- calls: b.<init> (ext), b.b (ext)

### byte[] m(int)
- callers: a(javax.microedition.lcdui.Graphics,int)
- reads: gz
- writes: -
- calls: System.arraycopy (ext)

### void a(javax.microedition.lcdui.Graphics,int) [render]
- callers: paint(javax.microedition.lcdui.Graphics)
- reads: Q, bf, bl, ci, dY, e, eB, eZ, fk, ft, gA, gF, gZ, ga, gz, hd, hs, m, r, x
- writes: Q, bf, bl, bu, cE, ci, eZ, fI, fk, ft, gA, gF, gZ, ga, hd, m, r, x
- calls: Graphics.drawArc (ext), Graphics.drawImage (ext), Graphics.drawLine (ext), Graphics.drawRect (ext), Graphics.fillArc (ext), Graphics.fillRect (ext), Graphics.setClip (ext), Graphics.setColor (ext), Image.getHeight (ext), Image.getWidth (ext), Math.max (ext), Math.min (ext), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean), c(byte), e(int,int), f(javax.microedition.lcdui.Graphics), m(int), n(int,int), q(int,int), u(int,int)

### void a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean) [render]
- callers: a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte), a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[]), h(javax.microedition.lcdui.Graphics)
- reads: bf, bl, ca, e, ft, hs, r
- writes: -
- calls: Graphics.clipRect (ext), Graphics.drawImage (ext), Graphics.drawLine (ext), Graphics.setClip (ext), Graphics.setColor (ext)

### void a(byte)
- callers: j(int)
- reads: Q, cE, eB, fk, gZ, hd, x
- writes: eB, gF
- calls: v(int,int)

### void z(int,int)
- callers: a(int,int,boolean), c(int,int,int,int,int)
- reads: bn, cL, dt, fv, hr, hw
- writes: as, bn, dt, fv
- calls: System.arraycopy (ext)

### boolean a(int,int,int,boolean)
- callers: a(int,int,boolean), a(int,int,int,boolean), c(int,int,int,int,int)
- reads: as, bn, c, cI, dt, fK, fv, hp
- writes: as
- calls: Math.abs (ext), a(int,int,int,boolean)

### void ac()
- callers: a(boolean), c(int,int,int,int,int)
- reads: bn, cL, dt, fv
- writes: -
- calls: System.arraycopy (ext)

### void a(boolean)
- callers: F(int), a(int,int,boolean)
- reads: bK, bn, cI, cL, fv, hr, hw
- writes: dt
- calls: ac()

### void a(int,int,boolean)
- callers: a(int,int,int,int), ah(), b(int,int,int,int,int), w(int,int)
- reads: as, cL, eU, hl, ho
- writes: bH, eU, ho
- calls: M(), a(boolean), a(int,int,int,boolean), p(int), z(int,int)

### void c(int,int,int,int,int)
- callers: F(int)
- reads: gm, hr, hw
- writes: -
- calls: Random.nextInt (ext), a(int,int,int,boolean), ac(), z(int,int)

### void b(int,int,int,int,int)
- callers: F(int)
- reads: gm, hr, hw
- writes: -
- calls: Random.nextInt (ext), a(int,int,boolean)

### void F(int)
- callers: m()
- reads: O, aB, ab, bK, bV, bX, cI, co, dK, dQ, dr, ew, fo, fx, gG, gf, gm, hf, hr, hw
- writes: O, bK, bV, cI, cL, ct, dh, dr, ea, fx, hl
- calls: Math.abs (ext), Random.nextInt (ext), a(boolean), b(int,int,int,int,int), c(int,int,int,int,int), u(int), w(int,int)

### byte p()
- callers: a(int,int,int), w(int,int)
- reads: gm
- writes: -
- calls: Math.abs (ext), Random.nextInt (ext)

### void w(int,int)
- callers: F(int)
- reads: cI, cK, cL, ea, gm, hr, hw
- writes: fx
- calls: Math.abs (ext), Random.nextInt (ext), System.currentTimeMillis (ext), a(byte,int,int), a(int,int,boolean), p()

### void a(int,int,int,int)
- callers: u(int)
- reads: cL, hr, hw
- writes: -
- calls: a(int,int,boolean)

### void b(int,int,int,int)
- callers: u(int)
- reads: cI, ea
- writes: -
- calls: -

### void u(int)
- callers: F(int)
- reads: bK, cI, fx, gm, hr, hw
- writes: fx
- calls: Math.abs (ext), Random.nextInt (ext), System.currentTimeMillis (ext), a(int,int,int,int), b(int,int,int,int), f(int)

### int G()
- callers: B()
- reads: bk, hh
- writes: bk
- calls: -

### void k()
- callers: m()
- reads: aR, bQ, cU, db, e, eN, eV, ej, hc
- writes: aR, cU, db, fj, hc, t
- calls: -

### void B()
- callers: run()
- reads: aR, bQ, cU, db, e, eN, eV, ej, fj, hc, t
- writes: aR, bk, cU, db, fj, hc, t
- calls: G(), i()

### int a(int,int,int,int,int,int)
- callers: i()
- reads: -
- writes: -
- calls: -

### void a(int,int,int[],int[],boolean)
- callers: c(javax.microedition.lcdui.Graphics), i()
- reads: cL, cU, hc, hj
- writes: -
- calls: -

### void i()
- callers: B()
- reads: aR, bA, cL, cU, db, dz, fQ, fn, hc, hj, hr, hw, w
- writes: I, bA, cN, de, ek, fM, fQ, fn, gX
- calls: Math.abs (ext), Math.min (ext), a(int,int,int,int,int,int), a(int,int,int[],int[],boolean)

### void a(javax.microedition.lcdui.Graphics,int[],int[]) [render]
- callers: c(javax.microedition.lcdui.Graphics)
- reads: -
- writes: -
- calls: Graphics.drawLine (ext)

### void c(javax.microedition.lcdui.Graphics) [render]
- callers: f(javax.microedition.lcdui.Graphics)
- reads: I, R, aT, bt, cI, dz, ea, ek, fn, hr, hw, w
- writes: -
- calls: Graphics.setColor (ext), a(int,int,int[],int[],boolean), a(javax.microedition.lcdui.Graphics,int[],int[]), b(int,int), e(int,int,int), j(int,int), r(int,int)

### void e(javax.microedition.lcdui.Graphics) [render]
- callers: f(javax.microedition.lcdui.Graphics)
- reads: I, aR, aT, bt, cI, cP, cU, db, de, ek, fn, gI, gX, hc, hr, hw
- writes: -
- calls: Graphics.drawArc (ext), Graphics.drawLine (ext), Graphics.setColor (ext), a(javax.microedition.lcdui.Graphics,int,int,int,int), c(byte)

### void H()
- callers: ah()
- reads: I, aj, cI, ea, ek, eq, fn, hr, hv, hw
- writes: bg
- calls: a(int,int,boolean,boolean), c(byte), d(int,int,int), f(), r(int,int), s(int,int)

### void I()
- callers: ah()
- reads: I, U, Y, aN, ak, aw, ea, ek, fn, g, gC, gY, s
- writes: aN, cq, d, do, du, eS, fD, gL, gY, gv, hg
- calls: af(), c(int,int), s(int,int), t(int)

### void E(int)
- callers: ah()
- reads: I, aW, cI, ea, ek, hi
- writes: -
- calls: T(), a(int,int,int,boolean,boolean), d(int,int,int), e(boolean), e(int,int,int), q(int), x(int)

### void ah()
- callers: j(int)
- reads: I, aC, aT, az, bA, bJ, bN, bt, cI, cN, dx, eF, eU, ek, fM, fQ, fV, fn, gK
- writes: aT, ax, bC, bH, bJ, bN, bt, bu, cq, d, eU, ev, fV, fm, gK
- calls: E(int), H(), I(), J(), M(), a(), a(boolean,int,int,int,int,int,int,boolean,int), a(int,int,boolean), a(int,int,int,int,int,int,int,int), c(int), j(int,int), n(int), p(int), r(), t(int), x(int)

### boolean q(int)
- callers: E(int)
- reads: eU, gK
- writes: bH, eU, gK
- calls: M(), p(int)

### void a(int,int,int,int,int,int,int,int)
- callers: ah()
- reads: ba, cI, ea, fb, hr, hw
- writes: aC, aL, aT, am, an, bH, bt, bu, dx, fg
- calls: d(int,int,int)

### void r()
- callers: ah()
- reads: aL, am, an, cI, ea, fg, hr, hw
- writes: -
- calls: -

### void a(boolean,int,int,int,int,int,int,boolean,int)
- callers: ah()
- reads: I, bK, cI, cw, cz, eH, eU, ea, ek, fn, gK
- writes: bH, eU, gK
- calls: M(), d(int,int,int), p(int), x(int)

### void a(int,int,boolean,boolean)
- callers: H(), a(int,boolean)
- reads: bO, bg, cI, eU, gT, go, ha
- writes: bg, ck
- calls: c(byte)

### void f()
- callers: H()
- reads: bg, ck, eU, ho
- writes: eU, ho
- calls: M(), p(int)

### void d(int,int)
- callers: v(int)
- reads: R, aa, az, cP, gI, gp
- writes: cf, eY, fN
- calls: -

### void a(int,int,int,boolean,boolean,int)
- callers: v(int)
- reads: R, aa, az, cP, cf, eY, fN, gI, gp
- writes: cf, eY
- calls: -

### void v(int)
- callers: m(), u()
- reads: aX, eF
- writes: R, aX, aa, az, cP, gI, gp
- calls: a(int,int,int,boolean,boolean,int), d(int,int)

### void c(boolean)
- callers: j(int)
- reads: aT, az, bt, gp
- writes: aT, bu, cb, dB
- calls: -

### int a(javax.microedition.lcdui.Graphics,int,int,int,boolean)
- callers: b(javax.microedition.lcdui.Graphics)
- reads: aT, fX
- writes: gj
- calls: a(javax.microedition.lcdui.Graphics,int,int,int,int)

### void b(javax.microedition.lcdui.Graphics) [render]
- callers: paint(javax.microedition.lcdui.Graphics)
- reads: cR, e, fX
- writes: -
- calls: Graphics.drawRect (ext), Graphics.fillRect (ext), Graphics.setColor (ext), a(javax.microedition.lcdui.Graphics,int,int,int,boolean), h(javax.microedition.lcdui.Graphics)

### void h(javax.microedition.lcdui.Graphics) [render]
- callers: a(javax.microedition.lcdui.Graphics), b(javax.microedition.lcdui.Graphics)
- reads: bB, bj, bu, cy, dN, e, eQ, eU, fd, fr
- writes: cy, dN, fd, fr
- strings: [, -, ., 0]
- calls: Graphics.drawLine (ext), Graphics.drawRect (ext), Graphics.fillRect (ext), Graphics.setColor (ext), Math.abs (ext), String.charAt (ext), String.length (ext), String.valueOf (ext), StringBuffer.<init> (ext), StringBuffer.append (ext), StringBuffer.insert (ext), StringBuffer.length (ext), StringBuffer.reverse (ext), StringBuffer.toString (ext), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)

### void a(javax.microedition.lcdui.Graphics) [render]
- callers: f(javax.microedition.lcdui.Graphics)
- reads: C, aT, aa, az, bt, bu, ds, dx, e, fX, gI, n
- writes: bu
- calls: Graphics.drawRect (ext), Graphics.fillRect (ext), Graphics.setColor (ext), Math.abs (ext), String.charAt (ext), String.length (ext), String.valueOf (ext), System.arraycopy (ext), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean), a(javax.microedition.lcdui.Graphics,int,int,int,int), c(byte), h(javax.microedition.lcdui.Graphics)

### void p(int)
- callers: D(int), R(), a(boolean,int,int,int,int,int,int,boolean,int), a(int,int,boolean), a(int,int,int[]), ah(), f(), q(int), v(int,int), y(), y(int)
- reads: K, fJ
- writes: -
- calls: -

### int a(javax.microedition.lcdui.Graphics,int,int)
- callers: g(javax.microedition.lcdui.Graphics)
- reads: e
- writes: -
- calls: Math.abs (ext), String.charAt (ext), String.length (ext), String.valueOf (ext), a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[])

### void g(javax.microedition.lcdui.Graphics) [render]
- callers: paint(javax.microedition.lcdui.Graphics)
- reads: K, fJ
- writes: -
- calls: Graphics.setColor (ext), a(javax.microedition.lcdui.Graphics,int,int)

### void d(javax.microedition.lcdui.Graphics)
- callers: f(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics)
- reads: cb, dB, ds, n
- writes: dB
- calls: a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[])

### byte a(byte,int,int)
- callers: w(int,int)
- reads: bL, bw, dC, di, gN, gi, gm
- writes: bw
- calls: Math.abs (ext), Random.nextInt (ext)

### void P()
- callers: m()
- reads: dC, eQ
- writes: -
- calls: -

### void d(int,int,int)
- callers: E(int), H(), a(boolean,int,int,int,int,int,int,boolean,int), a(int,int,int,int,int,int,int,int), b(int,int,boolean), n(int)
- reads: dC, ea, hr, hw
- writes: -
- calls: Integer.<init> (ext), Vector.<init> (ext), Vector.addElement (ext), Vector.indexOf (ext)

### void d()
- callers: m()
- reads: cI, ea, gN, hr, hw
- writes: -
- calls: -

### void a(int,int,int)
- callers: v()
- reads: cI, ea
- writes: -
- calls: p()

### boolean v()
- callers: m()
- reads: cI, ea, gm, hr, hw
- writes: -
- calls: Math.abs (ext), Random.nextInt (ext), a(int,int,int)

### byte f(int)
- callers: u(int)
- reads: aE, be, cV, da, gm
- writes: aE
- calls: Random.nextInt (ext)

### void ag()
- callers: m()
- reads: cV, gm
- writes: -
- calls: Random.nextInt (ext)

### void T()
- callers: E(int)
- reads: be, cI, ea, hr, hw
- writes: -
- strings: [_]
- calls: StringBuffer.<init> (ext), StringBuffer.append (ext), StringBuffer.toString (ext), Vector.<init> (ext), Vector.addElement (ext), Vector.indexOf (ext)

### byte[] b(int)
- callers: q(int,int)
- reads: -
- writes: -
- strings: [, -]
- calls: Math.abs (ext), String.charAt (ext), String.length (ext), StringBuffer.<init> (ext), StringBuffer.append (ext), StringBuffer.insert (ext), StringBuffer.length (ext), StringBuffer.reverse (ext), StringBuffer.toString (ext)

### byte[] p(int,int)
- callers: q(int,int)
- reads: -
- writes: -
- calls: String.charAt (ext), String.length (ext), String.valueOf (ext)

### byte[] d(java.lang.String)
- callers: q(int,int)
- reads: -
- writes: -
- calls: String.charAt (ext), String.length (ext)

### javax.microedition.lcdui.Image u(int,int)
- callers: a(javax.microedition.lcdui.Graphics,int)
- reads: Y, aN, ak, fU, gC, gY, gt
- writes: -
- calls: A(int), c(byte)

### boolean e(int,int)
- callers: a(javax.microedition.lcdui.Graphics,int)
- reads: S, aM, aN, aO, aw, cH, cO, d, dP, da, du, eL, ez, fD, hq
- writes: S, cH
- calls: A(int), X()

### int n(int,int)
- callers: a(javax.microedition.lcdui.Graphics,int)
- reads: W, fA
- writes: -
- calls: -

### byte[] q(int,int)
- callers: a(javax.microedition.lcdui.Graphics,int)
- reads: H, L, N, T, V, W, X, Y, Z, aA, aD, aG, aN, aP, aV, ac, ae, ai, ak, au, aw, b, bE, bG, bM, bP, bR, bY, bZ, bd, br, bz, cD, cF, cJ, cM, cO, cY, cZ, cc, cg, ch, cj, cm, cn, cr, cu, cx, dA, dC, dE, dG, dJ, dT, da, dg, di, dm, do, dq, ds, du, eG, eI, eJ, eM, eT, eX, ef, eg, eu, f, fC, fD, fF, fS, fs, g, gE, gJ, gK, gL, gN, gP, gV, gh, gk, gq, gr, gs, gv, gx, he, hk, hm, ho, ht, hy, n, o, p, q, s, v, y
- writes: cD
- strings: [/]
- calls: A(int), String.valueOf (ext), StringBuffer.<init> (ext), StringBuffer.append (ext), StringBuffer.toString (ext), System.arraycopy (ext), U(), a(int), b(int), c(byte), d(java.lang.String), p(int,int)

### void v(int,int)
- callers: a(byte)
- reads: T, U, Y, aN, aP, ac, ae, ak, ao, aw, b, bE, bz, cW, cY, cZ, cc, ch, cn, cu, d, dJ, dR, dS, do, eG, eU, fY, fa, fi, fq, gC, gE, gP, gY, gh, gv, hk, hm, hy, q, s
- writes: J, T, aP, ac, ad, ae, b, bu, cW, cZ, cc, ch, cn, cq, cu, d, dJ, dP, dS, do, eG, eS, eU, fi, fq, gE, gH, gQ, gR, gY, ga, gh, hg, hk, hy, q, y
- calls: C(int), E(), M(), O(), V(), a(int,int,int[]), ab(), ae(), af(), b(int,boolean), o(), o(int), p(int), r(int), t(int), y(), z(int)

### boolean K()
- callers: c(), m()
- reads: aQ, ax, bN, dx, et, fV, fm, hr, hw
- writes: aQ, dx, fl
- calls: C(), N(), S(), b()

### void S()
- callers: K()
- reads: aQ, ax, bK, bN, bW, ba, cI, cw, cz, dX, dx, eH, ea, ed, fV, fb, fm, gO, hr, hw
- writes: dX, dx, ed, gO
- calls: a(boolean,int,int,int,int,int,int,int,int,int,int,boolean), b(int,int,int), o(int,int)

### boolean a(boolean,int,int,int,int,int,int,int,int,int,int,boolean)
- callers: S()
- reads: cI, df, ea, hr, hw
- writes: -
- calls: -

### boolean b(int,int,int)
- callers: S()
- reads: aQ, hr, hw
- writes: dX, gO
- calls: -

### void o(int,int)
- callers: S()
- reads: aQ, cI, hr, hw
- writes: -
- calls: -

### void C()
- callers: K()
- reads: bN, fV, hr, hw
- writes: -
- calls: h(int,int)

### int h(int,int)
- callers: C()
- reads: aQ, ax, cI, fm
- writes: -
- calls: h(int,int,int)

### short h(int,int,int)
- callers: h(int,int)
- reads: aQ, hr, hw
- writes: -
- calls: -

### void J()
- callers: ah()
- reads: cI, ea, hr, hw
- writes: -
- calls: -

### void N()
- callers: K()
- reads: cI, ea, et, hr, hw
- writes: et
- calls: -

### void n(int)
- callers: ah(), c()
- reads: cI, ea, hr, hw
- writes: -
- calls: d(int,int,int)

### void b()
- callers: K()
- reads: aQ, bK, cI, ea, hr, hw
- writes: -
- calls: -

### boolean a(int,int,int,int,int)
- callers: b(int,int)
- reads: cI, ea, hr, hw
- writes: -
- calls: -

### boolean b(int,int)
- callers: c(javax.microedition.lcdui.Graphics)
- reads: cI
- writes: -
- calls: a(int,int,int,int,int)

### boolean j(int,int)
- callers: ah(), c(javax.microedition.lcdui.Graphics)
- reads: bK, cI, ea
- writes: -
- calls: -

### boolean r(int,int)
- callers: H(), c(javax.microedition.lcdui.Graphics)
- reads: af, ak, cI, ea, eo, hr, hw
- writes: -
- calls: c(byte), s(int,int)

### boolean e(int,int,int)
- callers: E(int), c(javax.microedition.lcdui.Graphics), e(int,int,int)
- reads: bK, cI, ea, hr, hw
- writes: -
- calls: e(int,int,int)

### int s(int,int)
- callers: H(), I(), r(int,int), y(int)
- reads: aj, eq, hv
- writes: -
- calls: c(byte)

### int a(int,int,int,boolean,boolean)
- callers: E(int)
- reads: aj, cO, eq, hq, hv
- writes: -
- calls: -

### void af()
- callers: I(), v(int,int)
- reads: F, U, aG, ai, au, bZ, eI, eP, eS, fB, fC, fH, gC, gY, hg, s
- writes: s
- calls: -

### void r(int)
- callers: v(int,int)
- reads: gC
- writes: -
- calls: -

### void V()
- callers: v(int,int)
- reads: gC
- writes: -
- calls: -

### boolean y()
- callers: v(int,int)
- reads: I, eU, ek, fD, gC, gE, gY, gu, hv, s
- writes: eU, gE, gu
- calls: M(), b(int,int,int,int,int,int), l(int,int), p(int), w(int)

### void e(boolean)
- callers: E(int), m()
- reads: aj, bb, be, cI, cO, cV, dC, ea, eq, gN, gW, gm, hq, hr, hv, hw
- writes: -
- calls: Integer.<init> (ext), Math.abs (ext), Random.nextInt (ext), Vector.<init> (ext), Vector.addElement (ext), Vector.indexOf (ext), c(byte)

### int l(int,int)
- callers: m(int,int), y(), y(int)
- reads: Y, aI, af, ak, eT, eo, ey, p, u
- writes: bH
- calls: -

### int b(int,int,int,int,int,int)
- callers: m(int,int), y()
- reads: Y, aI, af, ak, aw, bz, eL, eT, eo, ey, g, p, u
- writes: bH
- calls: -

### void w()
- callers: m()
- reads: aw
- writes: -
- calls: -

### void q()
- callers: m()
- reads: af, ak, cF, dT, eA, eU, ea, eo
- writes: cF, eU
- calls: A(int), c(byte)

### boolean i(int,int)
- callers: i(int,int), y(int)
- reads: Y, ak, eT, gW, p
- writes: -
- calls: i(int,int)

### void y(int)
- callers: m(), y(int)
- reads: A, Y, aH, aI, af, ak, aw, bz, cI, dT, eL, eR, eT, eU, ea, eo, ey, g, gr, hr, hw, p, u
- writes: bH, eU, gr
- calls: A(int), B(int), b(int,int,boolean), c(byte), f(int,int,int), h(int), i(int,int), l(int,int), p(int), s(int,int), x(int,int), y(int)

### void f(int,int,int)
- callers: f(int,int,int), y(int)
- reads: Y, af, eo, ey, u
- writes: -
- calls: f(int,int,int)

### int A(int)
- callers: A(int), e(int,int), q(), q(int,int), u(int,int), y(int)
- reads: Y
- writes: -
- calls: A(int)

### int B(int)
- callers: B(int), y(int)
- reads: Y, af, cI, eo, ey
- writes: -
- calls: B(int), c(byte)

### void a(int,boolean)
- callers: a(int,boolean), i(int)
- reads: Y, af, ak, eo, p
- writes: bH, cq, d
- calls: a(int,boolean), a(int,int,boolean,boolean), c(byte), t(int)

### void b(javax.microedition.lcdui.Graphics,int) [render]
- callers: f(javax.microedition.lcdui.Graphics)
- reads: af, ak, bK, cI, cU, cs, ea, eo, ey, fU, gt, hc, hj, u
- writes: -
- calls: Graphics.drawImage (ext), System.arraycopy (ext), c(byte)

### boolean j(int,int,int)
- callers: i(int), j(int,int,int)
- reads: Y, af, eo
- writes: -
- calls: c(byte), j(int,int,int)

### void i(int)
- callers: m()
- reads: af, ak, eo
- writes: -
- calls: a(int,boolean), c(byte), j(int,int,int)

### boolean i(int,int,int)
- callers: c(int,int), i(int,int,int)
- reads: Y, af, eo
- writes: -
- calls: c(byte), i(int,int,int)

### int c(int,int)
- callers: I()
- reads: ak
- writes: -
- calls: i(int,int,int)

### int x(int,int)
- callers: x(int,int), y(int)
- reads: A, Y, aI, ak, eA, eT, hq, p
- writes: -
- calls: c(byte), t(int,int), x(int,int)

### int g(int,int,int)
- callers: c(int,int,int)
- reads: cO
- writes: -
- calls: -

### int c(int,int,int)
- callers: b(int,int,boolean)
- reads: aj, eq, hv
- writes: -
- calls: Math.abs (ext), g(int,int,int)

### void b(int,int,boolean)
- callers: b(int,int,boolean), y(int)
- reads: A, Y, aI, af, ak, eT, eo, p
- writes: -
- calls: b(int,int,boolean), c(byte), c(int,int,int), d(int,int,int)

### void o(int)
- callers: a(int,int,int[]), o(int), v(int,int)
- reads: Y, ak
- writes: -
- calls: l(int), o(int)

### boolean a(int,int,int[])
- callers: v(int,int)
- reads: Y, do, eL, eU, gE, s
- writes: eU, gE
- calls: M(), o(int), p(int), w(int)

### void y(int,int) [RMS/save,RMS]
- callers: O(), X(), Z(), g(int), t(), u()
- reads: dv, fL
- writes: dd, dl, dv, eK, fL, fc
- strings: [TTStore]
- calls: RecordEnumeration.destroy (ext), RecordEnumeration.nextRecordId (ext), RecordEnumeration.numRecords (ext), RecordStore.closeRecordStore (ext), RecordStore.enumerateRecords (ext), RecordStore.getRecord (ext), RecordStore.getRecordSize (ext), RecordStore.openRecordStore (ext), System.arraycopy (ext)

### void k(int)
- callers: a(byte[]), a(int[]), a(short[]), b(byte), s(int)
- reads: dv, eK
- writes: dv, eK
- calls: System.arraycopy (ext)

### int s()
- callers: a(int[]), a(short[]), s(int)
- reads: dl, dv
- writes: dl
- calls: -

### void W() [RMS/save,RMS]
- callers: O(), g(int), t()
- reads: dv, eK, fL, fc
- writes: -
- strings: [TTStore]
- calls: RecordEnumeration.destroy (ext), RecordEnumeration.nextRecordId (ext), RecordEnumeration.numRecords (ext), RecordStore.addRecord (ext), RecordStore.closeRecordStore (ext), RecordStore.deleteRecord (ext), RecordStore.enumerateRecords (ext), RecordStore.getRecord (ext), RecordStore.openRecordStore (ext), System.arraycopy (ext)

### void a(java.io.ByteArrayOutputStream,int,int)
- callers: Y()
- reads: -
- writes: -
- calls: ByteArrayOutputStream.write (ext)

### void c(java.lang.String)
- callers: Y(), j()
- reads: dv, eK
- writes: -
- calls: c(byte)

### void Y()
- callers: g(int), t()
- reads: bm, dv, eK, fw
- writes: bm, dv, eK, fw
- strings: [Compress post, Compress pre]
- calls: ByteArrayOutputStream.<init> (ext), ByteArrayOutputStream.close (ext), ByteArrayOutputStream.toByteArray (ext), a(java.io.ByteArrayOutputStream,int,int), c(java.lang.String)

### void j()
- callers: g(int), u()
- reads: bm, dv, eK, fw
- writes: bm, dv, eK, fw
- strings: [Decompress post, Decompress pre]
- calls: ByteArrayOutputStream.<init> (ext), ByteArrayOutputStream.close (ext), ByteArrayOutputStream.toByteArray (ext), ByteArrayOutputStream.write (ext), c(byte), c(java.lang.String)

### boolean d(boolean)
- callers: A(), g(int)
- reads: -
- writes: -
- calls: b(byte)

### byte b(byte)
- callers: d(boolean), g(int)
- reads: dd, dl, dv, fL
- writes: dl
- calls: k(int)

### int s(int)
- callers: A(), g(int)
- reads: dd, dl, dv, fL
- writes: dl
- calls: k(int), s()

### void a(byte[][])
- callers: t(), u()
- reads: -
- writes: -
- calls: a(byte[])

### void a(short[][])
- callers: g(int)
- reads: -
- writes: -
- calls: a(short[])

### void a(byte[])
- callers: A(), a(byte[][]), g(int)
- reads: dd, dl, dv, fL
- writes: dl
- calls: System.arraycopy (ext), k(int)

### void a(short[])
- callers: a(short[][]), g(int)
- reads: dd, dl, dv, fL
- writes: dl
- calls: k(int), s()

### void a(int[])
- callers: A(), g(int)
- reads: dd, dl, dv, fL
- writes: dl
- calls: k(int), s()

### void g(int)
- callers: m(), t(), u()
- reads: A, B, F, L, M, N, T, V, X, Y, Z, aD, aE, aG, aI, aR, aV, aW, aX, aY, af, ai, aj, ak, ao, ap, aq, au, aw, bB, bD, bG, bI, bL, bO, bP, bU, bY, bZ, ba, bb, be, bh, bj, bo, bq, bs, bv, bw, by, bz, c, cA, cC, cF, cJ, cM, cO, cT, cU, cV, cY, cd, cg, cl, cm, cr, cw, cx, cz, dA, dC, dE, dF, dG, dT, dU, dV, da, db, di, dn, dy, eC, eF, eH, eI, eJ, eL, eM, eO, eP, eQ, eT, eU, eX, eb, ef, eg, eh, en, eo, ep, eq, er, es, eu, ey, ez, f, fB, fC, fE, fF, fH, fK, fP, fS, fT, fW, fa, fb, fe, fj, fp, fs, fu, g, gE, gK, gM, gN, gT, gi, gq, gr, gu, gx, h, hb, hc, he, hi, hk, hm, hn, ho, hp, hq, hr, ht, hu, hv, hw, p, t, u, v
- writes: B, F, T, V, aD, aE, aG, aR, aW, aX, aY, ai, ao, ap, aq, au, bB, bD, bG, bI, bO, bP, bZ, ba, bj, bo, bq, bs, bv, bw, by, c, cA, cC, cF, cM, cT, cU, cY, cd, cg, cl, cm, cr, cw, cz, dA, dF, dV, db, dd, dl, dv, dy, eC, eF, eH, eI, eJ, eK, eM, eO, eP, eQ, eU, eX, eb, eg, eh, en, ep, er, es, eu, ez, f, fB, fC, fE, fH, fK, fL, fP, fT, fb, fe, fj, fp, fu, gE, gK, gM, gT, gq, gr, gu, gx, h, hb, hc, he, hi, hk, hn, ho, hp, hr, ht, hu, hw, t, v
- calls: W(), Y(), a(byte[]), a(int[]), a(short[]), a(short[][]), b(byte), d(boolean), j(), s(int), y(int,int)

### void A()
- callers: O(), Z()
- reads: H, aM, aP, bE, bd, cc, ch, gh, gk, q
- writes: H, aM, aP, cc, ch, gh, q
- calls: a(byte[]), a(int[]), d(boolean), s(int)

### void O()
- callers: b(boolean), c(int), v(int,int), z()
- reads: -
- writes: -
- calls: A(), W(), y(int,int)

### void Z()
- callers: a(GameMIDlet)
- reads: gh
- writes: -
- calls: A(), C(int), y(int,int)

### void t()
- callers: m()
- reads: bK, cI, cL, ea
- writes: bm, fw
- calls: W(), Y(), a(byte[][]), g(int), y(int,int)

### boolean X()
- callers: e(int,int)
- reads: dd
- writes: -
- calls: y(int,int)

### void u()
- callers: m()
- reads: bK, bQ, cI, cL, eV, ea, hr, hw
- writes: bH, bK, bQ, bm, cI, cL, eV, ea, fO, fw, gU, hj
- calls: a(byte[][]), g(int), j(), v(int), y(int,int)

### void L()
- callers: m()
- reads: L, Z
- writes: -
- calls: h()

### void l()
- callers: m()
- reads: B, N, aq, bo, cF, cx, dF, dG, eJ, eX, ef, eh, ep, fe, gE, gK, gr, h, hn, ho
- writes: aA, bM, br, cj, dg, dq, gV
- calls: ab()

### void R()
- callers: m()
- reads: B, L, N, Z, aA, aq, bM, bb, bo, br, cF, cc, cj, cx, dF, dG, dT, dg, dq, eJ, eQ, eU, eX, ef, eg, eh, ep, fS, fe, fs, gE, gK, gV, gr, gs, h, hn, ho, q
- writes: B, V, aq, bG, bP, bo, cF, cM, cg, cm, cq, cr, d, dA, dF, dS, eJ, eM, eU, eX, eh, ep, eu, f, fe, gE, gK, ga, gq, gr, gx, h, he, hn, ho, ht, v, y
- calls: ab(), h(), p(int), t(int)

### void ab()
- callers: R(), l(), v(int,int)
- reads: aD, cF, eC, eJ, eg, gE, gK, gr, hk, ho
- writes: eJ, eX, eg
- calls: -

### int h()
- callers: L(), R()
- reads: F, aG, aW, af, ai, ak, au, bZ, cI, eH, eI, eP, ea, eo, fB, fC, fH, g, hk, hr, hw
- writes: -
- calls: c(byte)

### void b(int,boolean)
- callers: v(int,int)
- reads: L, N, Z, bj, cx, dG, dm, eQ, ef, fA, o
- writes: W, bR
- calls: Math.max (ext)

### void g()
- callers: m()
- reads: bt, cd, fP, gm
- writes: -
- calls: Random.nextInt (ext), t(int)

### void c()
- callers: m()
- reads: aY, bp, cA, dD, gS, hu, k
- writes: ax, bN, fV, fm, hu
- calls: D(), F(), K(), g(int,int), m(int,int), n(int)

### void ad()
- callers: m()
- reads: af, ak, eT, ea, eo, ez, g
- writes: -
- calls: c(byte)

### void e()
- callers: m()
- reads: af, ak, aw, ea, eo
- writes: -
- calls: c(byte)

### void m(int,int)
- callers: c()
- reads: cA, cI, dx, ez
- writes: -
- calls: b(int,int,int,int,int,int), l(int,int)

### void g(int,int)
- callers: c()
- reads: cI, ea, hr, hu, hw
- writes: -
- calls: -

### void F()
- callers: c()
- reads: cA, cI, gm, hr, hw
- writes: bp, cA, gS
- calls: Math.abs (ext), Random.nextInt (ext)

### void D()
- callers: c()
- reads: aY, cA, cI, gm, hr, hw
- writes: aY, dD, k
- calls: Math.abs (ext), Random.nextInt (ext)

### int U()
- callers: q(int,int)
- reads: ap, bD, bj, bs, cC, cT, cY, dV, fT, fu, gu, hb, hk
- writes: -
- calls: -

### void z()
- callers: m()
- reads: aM, ap, bD, bj, bs, cC, cT, dV, eF, en, fT, fu, gu, hb, hk
- writes: aM, cq, d, en, gJ
- calls: O(), c(int), t(int)

### void a()
- callers: ah()
- reads: aO, gP, hm
- writes: aO
- calls: -

### byte a(int)
- callers: q(int,int)
- reads: M, aV, eU, hm
- writes: -
- calls: -

### void f(int,int)
- callers: Q()
- reads: bY, bh, dE, dn, fF, fW, gm
- writes: -
- calls: Random.nextInt (ext)

### void D(int)
- callers: Q()
- reads: M, X, aD, aV, bY, bh, cJ, cY, dE, dn, eA, eU, ez, fF, fW, fa, hk, hm
- writes: aD, cW, cY, cZ, cq, d, dR, eU, ez, fY, hk
- calls: p(int), t(int)

### void e(int)
- callers: Q()
- reads: M, X, aV, bY, bh, cJ, dE, dn, fF, fW, fa, hm
- writes: cW, cZ, cq, d, dR, fY
- calls: t(int)

### void Q()
- callers: m()
- reads: M, X, a, aD, aK, aV, af, ak, bB, bY, bh, bj, bv, cJ, cY, dE, dU, dn, dy, eA, eQ, eU, ea, eo, er, ez, fF, fW, fa, gm, hk, hm
- writes: bv, cZ, cq, d, er
- calls: D(int), Math.abs (ext), Random.nextInt (ext), c(byte), e(int), f(int,int), t(int)

### void h(int)
- callers: y(int)
- reads: M, bD, by, hm
- writes: bD, by
- calls: -

### void t(int,int)
- callers: x(int,int)
- reads: M, cC, cT, dV, fu, hb, hm
- writes: cC, cT, dV, fu, hb
- calls: -

### void w(int)
- callers: a(int,int,int[]), y()
- reads: M, hm
- writes: -
- calls: -

### void l(int)
- callers: o(int)
- reads: M, hm
- writes: -
- calls: -

### void x(int)
- callers: E(int), a(boolean,int,int,int,int,int,int,boolean,int), ah()
- reads: M, hm
- writes: -
- calls: -

### void z(int)
- callers: v(int,int)
- reads: M, hm
- writes: -
- calls: -

### void x() [audio,resource]
- callers: m()
- reads: E, bi, gh
- writes: E, ay, bi, cG
- strings: [/6, audio/midi]
- calls: Class.getResourceAsStream (ext), Controllable.getControls (ext), Manager.createPlayer (ext), Object.getClass (ext), Player.deallocate (ext), Player.prefetch (ext), VolumeControl.setLevel (ext)

### void ae() [audio]
- callers: m(), showNotify(), v(int,int)
- reads: ac, ay, bi, cG
- writes: cG
- calls: Player.prefetch (ext), Player.setLoopCount (ext), Player.setMediaTime (ext), Player.start (ext)

### void C(int)
- callers: Z(), v(int,int)
- reads: E
- writes: -
- calls: VolumeControl.setLevel (ext)

### void E() [audio]
- callers: hideNotify(), run(), v(int,int)
- reads: ay, bi, cG
- writes: cG
- calls: Player.stop (ext)

### int c(int)
- callers: ah(), z()
- reads: H, af, ak, bB, bj, by, eQ, ea, eo, gk, hr, hw
- writes: H
- calls: O(), c(byte)

### java.lang.String b(byte[])
- callers: b(boolean)
- reads: -
- writes: -
- calls: String.<init> (ext)

### java.lang.String n()
- callers: b(boolean)
- reads: bE
- writes: -
- calls: StringBuffer.<init> (ext), StringBuffer.append (ext), StringBuffer.toString (ext)

### boolean b(boolean) [net]
- callers: m()
- reads: G, H, ag, bd, cQ, dM, dO, ec, el, fz, gk, i, l
- writes: -
- strings: [,, D, GET, S, en]
- calls: Connection.close (ext), Connector.open (ext), HttpConnection.getResponseCode (ext), HttpConnection.setRequestMethod (ext), HttpConnection.setRequestProperty (ext), InputConnection.openInputStream (ext), InputStream.close (ext), InputStream.read (ext), O(), String.charAt (ext), String.length (ext), String.valueOf (ext), StringBuffer.<init> (ext), StringBuffer.append (ext), StringBuffer.toString (ext), b(byte[]), n()

### void a(java.io.ByteArrayOutputStream,java.io.ByteArrayOutputStream,int,int)
- callers: k(int,int)
- reads: -
- writes: -
- calls: ByteArrayOutputStream.write (ext)

### void k(int,int)
- callers: a(int,int)
- reads: av, bS, gl
- writes: av, bS, gl
- calls: ByteArrayOutputStream.<init> (ext), ByteArrayOutputStream.close (ext), ByteArrayOutputStream.toByteArray (ext), a(java.io.ByteArrayOutputStream,java.io.ByteArrayOutputStream,int,int)

### void a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[]) [render]
- callers: a(javax.microedition.lcdui.Graphics,int,int), d(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics)
- reads: dc, eN
- writes: -
- calls: Graphics.drawRect (ext), Graphics.fillRect (ext), Graphics.setColor (ext), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)

### void M()
- callers: a(boolean,int,int,int,int,int,int,boolean,int), a(int,int,boolean), a(int,int,int[]), ah(), f(), q(int), v(int,int), y()
- reads: -
- writes: cq, d
- calls: t(int)

### void a(javax.microedition.lcdui.Graphics,int,int,byte) [render]
- callers: f(javax.microedition.lcdui.Graphics)
- reads: di, ds, e, eN, n
- writes: bf, bl, ft, r
- calls: Graphics.drawRect (ext), Graphics.fillRect (ext), Graphics.setColor (ext), System.arraycopy (ext), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)

### void a(int,int)
- callers: paint(javax.microedition.lcdui.Graphics)
- reads: aJ, bF, cq, d, e, eD, fG, hs, hx
- writes: C, aJ, bF, bH, bu, e, eD, eN, fG, fX, gH, hs
- calls: Image.createImage (ext), Image.getGraphics (ext), k(int,int)

### void paint(javax.microedition.lcdui.Graphics) [render]
- callers: paint(javax.microedition.lcdui.Graphics)
- reads: P, al, bF, bJ, bV, cU, ce, cq, d, dh, ds, e, eN, eW, ee, ev, fG, fX, fZ, ff, gH, gd, hc, hj, hr, hs, hw, n
- writes: ah, bF, cq, d, ee, em, fZ, gH
- calls: Canvas.setFullScreenMode (ext), Displayable.getHeight (ext), Displayable.getWidth (ext), Graphics.drawImage (ext), Graphics.drawLine (ext), Graphics.drawRect (ext), Graphics.fillRect (ext), Graphics.setClip (ext), Graphics.setColor (ext), Image.getHeight (ext), Image.getWidth (ext), System.currentTimeMillis (ext), a(int,int), a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,int,int), a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[]), b(javax.microedition.lcdui.Graphics), d(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), g(javax.microedition.lcdui.Graphics), o(), paint(javax.microedition.lcdui.Graphics), t(int)

### void t(int) [resource]
- callers: D(int), I(), M(), Q(), R(), a(GameMIDlet), a(int,boolean), ah(), e(int), g(), hideNotify(), j(int), m(), paint(javax.microedition.lcdui.Graphics), v(int,int), z()
- reads: al, bC, bF, d, ds, e, fG, gt, hs, n
- writes: C, P, S, al, bC, bF, bH, bu, cR, ct, dZ, ds, eW, fG, fU, gD, gF, gd, hx, n
- strings: [/1, /2, /3, /4]
- calls: Image.createImage (ext), Image.getGraphics (ext), System.currentTimeMillis (ext), System.gc (ext), a(java.lang.String), aa(), b(java.lang.String), b.<init> (ext), b.a (ext), b.b (ext), o()

### void m() [resource,render]
- callers: run()
- reads: C, F, J, T, aB, aG, aR, aW, ad, ai, ak, al, au, b, bB, bF, bI, bK, bO, bQ, bV, bX, bZ, ba, bc, bj, c, cI, cU, ca, co, ct, cw, cz, dH, dJ, dK, dZ, db, dh, dj, eE, eG, eH, eI, eP, eQ, eU, eV, eW, ea, ei, ew, ez, fB, fC, fG, fH, fK, fO, fU, fb, fh, fo, fp, fy, gB, gD, gG, gR, gT, gU, gb, gd, gf, gg, gm, go, ha, hc, hi, hp, hr, hu, hw, hy, p, u
- writes: C, F, aB, aC, aD, aG, aR, aT, aW, ab, ac, ah, ai, ao, ap, au, bB, bH, bI, bJ, bO, bQ, bV, bX, bZ, ba, bc, bj, bs, bt, bu, c, cK, cU, cY, cd, cn, co, cq, ct, cw, cz, d, dK, dP, dQ, dZ, db, dj, dy, eC, eE, eF, eH, eI, eP, eQ, eU, eV, eW, ei, em, en, ew, ez, fB, fC, fG, fH, fK, fO, fP, fT, fZ, fb, ff, fh, fj, fo, fp, fy, gB, gG, gT, gU, gb, gd, gf, gg, hc, hf, hi, hj, hk, hp, hr, hu, hw, t
- strings: [/5, AI OFF]
- calls: F(int), Graphics.drawLine (ext), Graphics.fillRect (ext), Graphics.setColor (ext), Image.createImage (ext), Image.getGraphics (ext), K(), L(), Math.abs (ext), P(), PrintStream.println (ext), Q(), R(), Random.nextInt (ext), System.currentTimeMillis (ext), System.gc (ext), aa(), ad(), ae(), ag(), b(boolean), b.<init> (ext), b.a (ext), b.b (ext), c(), c(byte), d(), e(), e(boolean), g(), g(int), i(int), k(), l(), o(), q(), t(), t(int), u(), v(), v(int), w(), x(), y(int), z()

### void f(javax.microedition.lcdui.Graphics) [render]
- callers: a(javax.microedition.lcdui.Graphics,int), paint(javax.microedition.lcdui.Graphics)
- reads: C, aP, af, ak, av, bH, bK, bL, bS, bU, bc, bw, cI, cU, ch, cl, dW, dj, e, eN, eO, eo, es, fE, fG, fU, fh, gM, gb, gc, gi, gl, gt, ha, hc, hj, hr, hs, hw
- writes: bH, cl, dW, eO, es, fE, gM
- calls: Graphics.drawImage (ext), Graphics.fillRect (ext), Graphics.setClip (ext), Graphics.setColor (ext), a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int,int,byte), b(javax.microedition.lcdui.Graphics,int), c(byte), c(javax.microedition.lcdui.Graphics), d(javax.microedition.lcdui.Graphics), e(javax.microedition.lcdui.Graphics)

### void hideNotify()
- callers: -
- reads: bF
- writes: cq, cv, d
- calls: E(), t(int)

### void showNotify()
- callers: -
- reads: bF
- writes: cv
- calls: ae()

### void <clinit>()
- callers: -
- reads: hs
- writes: G, aP, ag, cQ, cc, ch, dM, dO, e, eN, ec, el, fX, fz, hs, i, l, q
- calls: -

## Fields

- `GameMIDlet ex`  R=2 W=1
    - read by: a(GameMIDlet), run()
    - written by: a(GameMIDlet)
- `boolean gQ`  R=1 W=2
    - read by: run()
    - written by: <init>(), v(int,int)
- `int at`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int cB`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int ar`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int D`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int hw`  R=45 W=2
    - read by: <init>(), C(), D(), F(), F(int), H(), J(), K(), N(), S(), T(), a(boolean), a(boolean,int,int,int,int,int,int,int,int,int,int,boolean), a(int,int,int,int), a(int,int,int,int,int), a(int,int,int,int,int,int,int,int), b(), b(int,int,int), b(int,int,int,int,int), c(int), c(int,int,int,int,int), c(javax.microedition.lcdui.Graphics), d(), d(int,int,int), e(boolean), e(int,int,int), e(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), g(int), g(int,int), h(), h(int,int,int), i(), m(), n(int), o(int,int), paint(javax.microedition.lcdui.Graphics), r(), r(int,int), u(), u(int), v(), w(int,int), y(int), z(int,int)
    - written by: g(int), m()
- `int hr`  R=45 W=2
    - read by: <init>(), C(), D(), F(), F(int), H(), J(), K(), N(), S(), T(), a(boolean), a(boolean,int,int,int,int,int,int,int,int,int,int,boolean), a(int,int,int,int), a(int,int,int,int,int), a(int,int,int,int,int,int,int,int), b(), b(int,int,int), b(int,int,int,int,int), c(int), c(int,int,int,int,int), c(javax.microedition.lcdui.Graphics), d(), d(int,int,int), e(boolean), e(int,int,int), e(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), g(int), g(int,int), h(), h(int,int,int), i(), m(), n(int), o(int,int), paint(javax.microedition.lcdui.Graphics), r(), r(int,int), u(), u(int), v(), w(int,int), y(int), z(int,int)
    - written by: g(int), m()
- `int hj`  R=5 W=3
    - read by: a(int,int,int[],int[],boolean), b(javax.microedition.lcdui.Graphics,int), f(javax.microedition.lcdui.Graphics), i(), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), m(), u()
- `int eV`  R=5 W=3
    - read by: <init>(), B(), k(), m(), u()
    - written by: <init>(), m(), u()
- `int bQ`  R=5 W=3
    - read by: <init>(), B(), k(), m(), u()
    - written by: <init>(), m(), u()
- `int gU`  R=2 W=3
    - read by: <init>(), m()
    - written by: <init>(), m(), u()
- `int fO`  R=2 W=3
    - read by: <init>(), m()
    - written by: <init>(), m(), u()
- `int dQ`  R=1 W=1
    - read by: F(int)
    - written by: m()
- `int hf`  R=1 W=1
    - read by: F(int)
    - written by: m()
- `int ab`  R=1 W=1
    - read by: F(int)
    - written by: m()
- `int gG`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int co`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int cK`  R=1 W=1
    - read by: w(int,int)
    - written by: m()
- `int dK`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int bX`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int gf`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int fo`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int ew`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int aB`  R=2 W=1
    - read by: F(int), m()
    - written by: m()
- `int fb`  R=4 W=2
    - read by: S(), a(int,int,int,int,int,int,int,int), g(int), m()
    - written by: g(int), m()
- `int ba`  R=4 W=2
    - read by: S(), a(int,int,int,int,int,int,int,int), g(int), m()
    - written by: g(int), m()
- `int cw`  R=4 W=2
    - read by: S(), a(boolean,int,int,int,int,int,int,boolean,int), g(int), m()
    - written by: g(int), m()
- `int eH`  R=5 W=2
    - read by: S(), a(boolean,int,int,int,int,int,int,boolean,int), g(int), h(), m()
    - written by: g(int), m()
- `int cz`  R=4 W=2
    - read by: S(), a(boolean,int,int,int,int,int,int,boolean,int), g(int), m()
    - written by: g(int), m()
- `int bO`  R=3 W=2
    - read by: a(int,int,boolean,boolean), g(int), m()
    - written by: g(int), m()
- `int gT`  R=3 W=2
    - read by: a(int,int,boolean,boolean), g(int), m()
    - written by: g(int), m()
- `int aW`  R=4 W=2
    - read by: E(int), g(int), h(), m()
    - written by: g(int), m()
- `int hi`  R=3 W=2
    - read by: E(int), g(int), m()
    - written by: g(int), m()
- `int c`  R=3 W=2
    - read by: a(int,int,int,boolean), g(int), m()
    - written by: g(int), m()
- `int hp`  R=3 W=2
    - read by: a(int,int,int,boolean), g(int), m()
    - written by: g(int), m()
- `int fK`  R=3 W=2
    - read by: a(int,int,int,boolean), g(int), m()
    - written by: g(int), m()
- `int aG`  R=5 W=2
    - read by: af(), g(int), h(), m(), q(int,int)
    - written by: g(int), m()
- `int eI`  R=5 W=2
    - read by: af(), g(int), h(), m(), q(int,int)
    - written by: g(int), m()
- `int au`  R=5 W=2
    - read by: af(), g(int), h(), m(), q(int,int)
    - written by: g(int), m()
- `int bZ`  R=5 W=2
    - read by: af(), g(int), h(), m(), q(int,int)
    - written by: g(int), m()
- `int fC`  R=5 W=2
    - read by: af(), g(int), h(), m(), q(int,int)
    - written by: g(int), m()
- `int ai`  R=5 W=2
    - read by: af(), g(int), h(), m(), q(int,int)
    - written by: g(int), m()
- `int fH`  R=4 W=2
    - read by: af(), g(int), h(), m()
    - written by: g(int), m()
- `int F`  R=4 W=2
    - read by: af(), g(int), h(), m()
    - written by: g(int), m()
- `int eP`  R=4 W=2
    - read by: af(), g(int), h(), m()
    - written by: g(int), m()
- `int fB`  R=4 W=2
    - read by: af(), g(int), h(), m()
    - written by: g(int), m()
- `java.lang.Thread gn`  R=2 W=1
    - read by: a(GameMIDlet), run()
    - written by: a(GameMIDlet)
- `int aZ`  R=1 W=1
    - read by: run()
    - written by: <init>()
- `long cX`  R=1 W=1
    - read by: run()
    - written by: run()
- `long cp`  R=1 W=1
    - read by: run()
    - written by: run()
- `int cS`  R=1 W=1
    - read by: run()
    - written by: run()
- `int ge`  R=1 W=2
    - read by: run()
    - written by: <init>(), run()
- `int gg`  R=2 W=3
    - read by: m(), run()
    - written by: <init>(), m(), run()
- `boolean ev`  R=6 W=5
    - read by: keyPressed(int), paint(javax.microedition.lcdui.Graphics), pointerDragged(int,int), pointerPressed(int,int), pointerReleased(int,int), run()
    - written by: <init>(), ah(), keyPressed(int), pointerReleased(int,int), run()
- `boolean bJ`  R=3 W=3
    - read by: ah(), paint(javax.microedition.lcdui.Graphics), run()
    - written by: <init>(), ah(), m()
- `boolean cv`  R=1 W=3
    - read by: run()
    - written by: <init>(), hideNotify(), showNotify()
- `long dL`  R=0 W=1
    - read by: -
    - written by: run()
- `boolean ce`  R=1 W=1
    - read by: paint(javax.microedition.lcdui.Graphics)
    - written by: <init>()
- `long bT`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int aF`  R=1 W=4
    - read by: run()
    - written by: <init>(), d(int), j(int), run()
- `int z`  R=1 W=3
    - read by: run()
    - written by: <init>(), d(int), j(int)
- `boolean gy`  R=1 W=2
    - read by: run()
    - written by: <init>(), run()
- `int dw`  R=2 W=4
    - read by: j(int), run()
    - written by: <init>(), d(int), j(int), run()
- `int aS`  R=1 W=6
    - read by: run()
    - written by: <init>(), d(int), j(int), keyPressed(int), keyReleased(int), run()
- `boolean ee`  R=1 W=2
    - read by: paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), paint(javax.microedition.lcdui.Graphics)
- `int dp`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int e`  R=14 W=2
    - read by: <init>(), B(), a(int,int), a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int), a(javax.microedition.lcdui.Graphics,int,int,byte), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean), b(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), h(javax.microedition.lcdui.Graphics), k(), paint(javax.microedition.lcdui.Graphics), t(int)
    - written by: <clinit>(), a(int,int)
- `int hs`  R=8 W=2
    - read by: <clinit>(), <init>(), a(int,int), a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean), f(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics), t(int)
    - written by: <clinit>(), a(int,int)
- `int eN`  R=6 W=2
    - read by: B(), a(javax.microedition.lcdui.Graphics,int,int,byte), a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[]), f(javax.microedition.lcdui.Graphics), k(), paint(javax.microedition.lcdui.Graphics)
    - written by: <clinit>(), a(int,int)
- `int fX`  R=4 W=2
    - read by: a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int,int,int,boolean), b(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics)
    - written by: <clinit>(), a(int,int)
- `b aU`  R=1 W=2
    - read by: aa()
    - written by: <init>(), a(java.lang.String)
- `javax.microedition.lcdui.Image[] ca`  R=2 W=1
    - read by: a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean), m()
    - written by: <init>()
- `short[] dY`  R=2 W=1
    - read by: a(javax.microedition.lcdui.Graphics,int), b(java.lang.String)
    - written by: b(java.lang.String)
- `byte[] gz`  R=3 W=1
    - read by: a(javax.microedition.lcdui.Graphics,int), b(java.lang.String), m(int)
    - written by: b(java.lang.String)
- `int eB`  R=2 W=3
    - read by: a(byte), a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(byte), o()
- `int gF`  R=1 W=6
    - read by: a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(byte), a(javax.microedition.lcdui.Graphics,int), j(int), o(), t(int)
- `int x`  R=2 W=3
    - read by: a(byte), a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int), o()
- `int fk`  R=2 W=3
    - read by: a(byte), a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int), o()
- `boolean gZ`  R=2 W=2
    - read by: a(byte), a(javax.microedition.lcdui.Graphics,int)
    - written by: a(javax.microedition.lcdui.Graphics,int), o()
- `boolean hd`  R=2 W=2
    - read by: a(byte), a(javax.microedition.lcdui.Graphics,int)
    - written by: a(javax.microedition.lcdui.Graphics,int), o()
- `int cE`  R=1 W=3
    - read by: a(byte)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int), j(int)
- `boolean ga`  R=1 W=4
    - read by: a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), R(), a(javax.microedition.lcdui.Graphics,int), v(int,int)
- `boolean bC`  R=1 W=3
    - read by: t(int)
    - written by: <init>(), ah(), t(int)
- `boolean fI`  R=1 W=2
    - read by: j(int)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int)
- `int eZ`  R=1 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int)
    - written by: a(javax.microedition.lcdui.Graphics,int), o()
- `boolean m`  R=1 W=3
    - read by: a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int), o()
- `int ft`  R=2 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)
    - written by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte)
- `int bf`  R=2 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)
    - written by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte)
- `int bl`  R=2 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)
    - written by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte)
- `int r`  R=2 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte[],boolean)
    - written by: a(javax.microedition.lcdui.Graphics,int), a(javax.microedition.lcdui.Graphics,int,int,byte)
- `int Q`  R=2 W=1
    - read by: a(byte), a(javax.microedition.lcdui.Graphics,int)
    - written by: a(javax.microedition.lcdui.Graphics,int)
- `int ci`  R=1 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int)
- `int gA`  R=1 W=2
    - read by: a(javax.microedition.lcdui.Graphics,int)
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int)
- `byte[][] bK`  R=13 W=2
    - read by: F(int), S(), a(boolean), a(boolean,int,int,int,int,int,int,boolean,int), b(), b(javax.microedition.lcdui.Graphics,int), e(int,int,int), f(javax.microedition.lcdui.Graphics), j(int,int), m(), t(), u(), u(int)
    - written by: F(int), u()
- `byte[][] cL`  R=10 W=2
    - read by: a(boolean), a(int,int,boolean), a(int,int,int,int), a(int,int,int[],int[],boolean), ac(), i(), t(), u(), w(int,int), z(int,int)
    - written by: F(int), u()
- `int as`  R=2 W=3
    - read by: a(int,int,boolean), a(int,int,int,boolean)
    - written by: <init>(), a(int,int,int,boolean), z(int,int)
- `boolean hl`  R=1 W=2
    - read by: a(int,int,boolean)
    - written by: <init>(), F(int)
- `byte[][] dt`  R=3 W=2
    - read by: a(int,int,int,boolean), ac(), z(int,int)
    - written by: a(boolean), z(int,int)
- `int fv`  R=4 W=1
    - read by: a(boolean), a(int,int,int,boolean), ac(), z(int,int)
    - written by: z(int,int)
- `int bn`  R=4 W=1
    - read by: a(boolean), a(int,int,int,boolean), ac(), z(int,int)
    - written by: z(int,int)
- `boolean fx`  R=2 W=4
    - read by: F(int), u(int)
    - written by: <init>(), F(int), u(int), w(int,int)
- `int dr`  R=1 W=2
    - read by: F(int)
    - written by: <init>(), F(int)
- `int O`  R=1 W=2
    - read by: F(int)
    - written by: <init>(), F(int)
- `int dh`  R=2 W=2
    - read by: m(), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), F(int)
- `int bV`  R=3 W=3
    - read by: F(int), m(), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), F(int), m()
- `int db`  R=6 W=5
    - read by: B(), e(javax.microedition.lcdui.Graphics), g(int), i(), k(), m()
    - written by: <init>(), B(), g(int), k(), m()
- `int aR`  R=6 W=5
    - read by: B(), e(javax.microedition.lcdui.Graphics), g(int), i(), k(), m()
    - written by: <init>(), B(), g(int), k(), m()
- `int t`  R=2 W=5
    - read by: B(), g(int)
    - written by: <init>(), B(), g(int), k(), m()
- `int fj`  R=2 W=5
    - read by: B(), g(int)
    - written by: <init>(), B(), g(int), k(), m()
- `int bk`  R=1 W=5
    - read by: G()
    - written by: <init>(), B(), G(), d(int), j(int)
- `int ej`  R=2 W=3
    - read by: B(), k()
    - written by: <init>(), d(int), j(int)
- `int hh`  R=1 W=1
    - read by: G()
    - written by: <init>()
- `int an`  R=1 W=1
    - read by: r()
    - written by: a(int,int,int,int,int,int,int,int)
- `int am`  R=1 W=1
    - read by: r()
    - written by: a(int,int,int,int,int,int,int,int)
- `int fg`  R=1 W=1
    - read by: r()
    - written by: a(int,int,int,int,int,int,int,int)
- `int aL`  R=1 W=1
    - read by: r()
    - written by: a(int,int,int,int,int,int,int,int)
- `int aT`  R=7 W=6
    - read by: a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int,int,int,boolean), ah(), c(boolean), c(javax.microedition.lcdui.Graphics), e(javax.microedition.lcdui.Graphics), run()
    - written by: <init>(), a(int,int,int,int,int,int,int,int), ah(), c(boolean), j(int), m()
- `int[] w`  R=2 W=1
    - read by: c(javax.microedition.lcdui.Graphics), i()
    - written by: <init>()
- `int[] dz`  R=2 W=1
    - read by: c(javax.microedition.lcdui.Graphics), i()
    - written by: <init>()
- `int de`  R=1 W=1
    - read by: e(javax.microedition.lcdui.Graphics)
    - written by: i()
- `int gX`  R=1 W=1
    - read by: e(javax.microedition.lcdui.Graphics)
    - written by: i()
- `int bA`  R=2 W=1
    - read by: ah(), i()
    - written by: i()
- `int fQ`  R=2 W=1
    - read by: ah(), i()
    - written by: i()
- `int ek`  R=8 W=1
    - read by: E(int), H(), I(), a(boolean,int,int,int,int,int,int,boolean,int), ah(), c(javax.microedition.lcdui.Graphics), e(javax.microedition.lcdui.Graphics), y()
    - written by: i()
- `int I`  R=8 W=1
    - read by: E(int), H(), I(), a(boolean,int,int,int,int,int,int,boolean,int), ah(), c(javax.microedition.lcdui.Graphics), e(javax.microedition.lcdui.Graphics), y()
    - written by: i()
- `boolean fM`  R=1 W=1
    - read by: ah()
    - written by: i()
- `boolean cN`  R=1 W=1
    - read by: ah()
    - written by: i()
- `boolean fn`  R=7 W=2
    - read by: H(), I(), a(boolean,int,int,int,int,int,int,boolean,int), ah(), c(javax.microedition.lcdui.Graphics), e(javax.microedition.lcdui.Graphics), i()
    - written by: <init>(), i()
- `int bg`  R=2 W=2
    - read by: a(int,int,boolean,boolean), f()
    - written by: H(), a(int,int,boolean,boolean)
- `boolean ck`  R=1 W=1
    - read by: f()
    - written by: a(int,int,boolean,boolean)
- `boolean bu`  R=2 W=11
    - read by: a(javax.microedition.lcdui.Graphics), h(javax.microedition.lcdui.Graphics)
    - written by: <init>(), a(int,int), a(int,int,int,int,int,int,int,int), a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int), ah(), c(boolean), j(int), m(), t(int), v(int,int)
- `int bt`  R=8 W=5
    - read by: a(javax.microedition.lcdui.Graphics), ah(), c(boolean), c(javax.microedition.lcdui.Graphics), e(javax.microedition.lcdui.Graphics), g(), j(int), run()
    - written by: <init>(), a(int,int,int,int,int,int,int,int), ah(), j(int), m()
- `int gj`  R=0 W=2
    - read by: -
    - written by: <init>(), a(javax.microedition.lcdui.Graphics,int,int,int,boolean)
- `byte[][] az`  R=5 W=1
    - read by: a(int,int,int,boolean,boolean,int), a(javax.microedition.lcdui.Graphics), ah(), c(boolean), d(int,int)
    - written by: v(int)
- `byte[][] gp`  R=3 W=1
    - read by: a(int,int,int,boolean,boolean,int), c(boolean), d(int,int)
    - written by: v(int)
- `byte[][] aa`  R=3 W=1
    - read by: a(int,int,int,boolean,boolean,int), a(javax.microedition.lcdui.Graphics), d(int,int)
    - written by: v(int)
- `byte[][] gI`  R=4 W=1
    - read by: a(int,int,int,boolean,boolean,int), a(javax.microedition.lcdui.Graphics), d(int,int), e(javax.microedition.lcdui.Graphics)
    - written by: v(int)
- `byte[][] R`  R=3 W=1
    - read by: a(int,int,int,boolean,boolean,int), c(javax.microedition.lcdui.Graphics), d(int,int)
    - written by: v(int)
- `boolean[][] cP`  R=4 W=1
    - read by: a(int,int,int,boolean,boolean,int), d(int,int), e(javax.microedition.lcdui.Graphics), run()
    - written by: v(int)
- `int eY`  R=1 W=2
    - read by: a(int,int,int,boolean,boolean,int)
    - written by: a(int,int,int,boolean,boolean,int), d(int,int)
- `int fN`  R=1 W=1
    - read by: a(int,int,int,boolean,boolean,int)
    - written by: d(int,int)
- `int cf`  R=1 W=2
    - read by: a(int,int,int,boolean,boolean,int)
    - written by: a(int,int,int,boolean,boolean,int), d(int,int)
- `byte aX`  R=2 W=2
    - read by: g(int), v(int)
    - written by: g(int), v(int)
- `int cR`  R=2 W=3
    - read by: b(javax.microedition.lcdui.Graphics), j(int)
    - written by: <init>(), j(int), t(int)
- `int[] K`  R=2 W=1
    - read by: g(javax.microedition.lcdui.Graphics), p(int)
    - written by: <init>()
- `byte[] fJ`  R=2 W=1
    - read by: g(javax.microedition.lcdui.Graphics), p(int)
    - written by: <init>()
- `int dB`  R=1 W=4
    - read by: d(javax.microedition.lcdui.Graphics)
    - written by: <init>(), c(boolean), d(javax.microedition.lcdui.Graphics), j(int)
- `int cb`  R=1 W=3
    - read by: d(javax.microedition.lcdui.Graphics)
    - written by: <init>(), c(boolean), j(int)
- `boolean fp`  R=2 W=3
    - read by: g(int), m()
    - written by: <init>(), g(int), m()
- `byte gL`  R=1 W=2
    - read by: q(int,int)
    - written by: <init>(), I()
- `byte bw`  R=3 W=3
    - read by: a(byte,int,int), f(javax.microedition.lcdui.Graphics), g(int)
    - written by: <init>(), a(byte,int,int), g(int)
- `byte[] dC`  R=6 W=1
    - read by: P(), a(byte,int,int), d(int,int,int), e(boolean), g(int), q(int,int)
    - written by: <init>()
- `short[] gN`  R=5 W=1
    - read by: a(byte,int,int), d(), e(boolean), g(int), q(int,int)
    - written by: <init>()
- `short[] di`  R=4 W=1
    - read by: a(byte,int,int), a(javax.microedition.lcdui.Graphics,int,int,byte), g(int), q(int,int)
    - written by: <init>()
- `short[] gi`  R=3 W=1
    - read by: a(byte,int,int), f(javax.microedition.lcdui.Graphics), g(int)
    - written by: <init>()
- `short[] bL`  R=3 W=1
    - read by: a(byte,int,int), f(javax.microedition.lcdui.Graphics), g(int)
    - written by: <init>()
- `byte du`  R=2 W=2
    - read by: e(int,int), q(int,int)
    - written by: <init>(), I()
- `byte aE`  R=2 W=3
    - read by: f(int), g(int)
    - written by: <init>(), f(int), g(int)
- `byte[] da`  R=4 W=1
    - read by: e(int,int), f(int), g(int), q(int,int)
    - written by: <init>()
- `byte[] cV`  R=4 W=1
    - read by: ag(), e(boolean), f(int), g(int)
    - written by: <init>()
- `byte[] be`  R=4 W=1
    - read by: T(), e(boolean), f(int), g(int)
    - written by: <init>()
- `int gH`  R=1 W=4
    - read by: paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), a(int,int), paint(javax.microedition.lcdui.Graphics), v(int,int)
- `boolean fq`  R=1 W=2
    - read by: v(int,int)
    - written by: <init>(), v(int,int)
- `int gv`  R=2 W=2
    - read by: q(int,int), v(int,int)
    - written by: <init>(), I()
- `byte ae`  R=2 W=2
    - read by: q(int,int), v(int,int)
    - written by: <init>(), v(int,int)
- `byte eG`  R=3 W=2
    - read by: m(), q(int,int), v(int,int)
    - written by: <init>(), v(int,int)
- `byte dJ`  R=3 W=2
    - read by: m(), q(int,int), v(int,int)
    - written by: <init>(), v(int,int)
- `byte b`  R=3 W=2
    - read by: m(), q(int,int), v(int,int)
    - written by: <init>(), v(int,int)
- `byte hy`  R=3 W=2
    - read by: m(), q(int,int), v(int,int)
    - written by: <init>(), v(int,int)
- `byte cZ`  R=2 W=5
    - read by: q(int,int), v(int,int)
    - written by: <init>(), D(int), Q(), e(int), v(int,int)
- `byte cW`  R=1 W=4
    - read by: v(int,int)
    - written by: <init>(), D(int), e(int), v(int,int)
- `int J`  R=1 W=1
    - read by: m()
    - written by: v(int,int)
- `int fY`  R=1 W=2
    - read by: v(int,int)
    - written by: D(int), e(int)
- `int dR`  R=1 W=2
    - read by: v(int,int)
    - written by: D(int), e(int)
- `int gJ`  R=1 W=1
    - read by: q(int,int)
    - written by: z()
- `boolean ad`  R=1 W=2
    - read by: m()
    - written by: <init>(), v(int,int)
- `byte[] bE`  R=4 W=1
    - read by: A(), n(), q(int,int), v(int,int)
    - written by: <init>()
- `byte[] cD`  R=1 W=2
    - read by: q(int,int)
    - written by: <init>(), q(int,int)
- `byte cu`  R=2 W=2
    - read by: q(int,int), v(int,int)
    - written by: <init>(), v(int,int)
- `byte fi`  R=1 W=2
    - read by: v(int,int)
    - written by: <init>(), v(int,int)
- `boolean S`  R=1 W=3
    - read by: e(int,int)
    - written by: <init>(), e(int,int), t(int)
- `boolean cH`  R=1 W=2
    - read by: e(int,int)
    - written by: <init>(), e(int,int)
- `boolean dP`  R=1 W=3
    - read by: e(int,int)
    - written by: <init>(), m(), v(int,int)
- `int cn`  R=2 W=2
    - read by: q(int,int), v(int,int)
    - written by: m(), v(int,int)
- `short[][] aQ`  R=7 W=1
    - read by: K(), S(), b(), b(int,int,int), h(int,int), h(int,int,int), o(int,int)
    - written by: K()
- `int fV`  R=4 W=2
    - read by: C(), K(), S(), ah()
    - written by: ah(), c()
- `int bN`  R=4 W=2
    - read by: C(), K(), S(), ah()
    - written by: ah(), c()
- `int fm`  R=3 W=2
    - read by: K(), S(), h(int,int)
    - written by: ah(), c()
- `int ax`  R=3 W=2
    - read by: K(), S(), h(int,int)
    - written by: ah(), c()
- `int fl`  R=0 W=1
    - read by: -
    - written by: K()
- `int dX`  R=1 W=2
    - read by: S()
    - written by: S(), b(int,int,int)
- `int gO`  R=1 W=2
    - read by: S()
    - written by: S(), b(int,int,int)
- `int ed`  R=1 W=2
    - read by: S()
    - written by: <init>(), S()
- `byte bW`  R=1 W=1
    - read by: S()
    - written by: <init>()
- `byte df`  R=1 W=1
    - read by: a(boolean,int,int,int,int,int,int,int,int,int,int,boolean)
    - written by: <init>()
- `int dx`  R=5 W=4
    - read by: K(), S(), a(javax.microedition.lcdui.Graphics), ah(), m(int,int)
    - written by: <init>(), K(), S(), a(int,int,int,int,int,int,int,int)
- `boolean aC`  R=1 W=3
    - read by: ah()
    - written by: <init>(), a(int,int,int,int,int,int,int,int), m()
- `int et`  R=2 W=2
    - read by: K(), N()
    - written by: <init>(), N()
- `int[] dk`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int[] j`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int fD`  R=3 W=2
    - read by: e(int,int), q(int,int), y()
    - written by: <init>(), I()
- `int gY`  R=5 W=3
    - read by: I(), af(), u(int,int), v(int,int), y()
    - written by: <init>(), I(), v(int,int)
- `int hg`  R=1 W=3
    - read by: af()
    - written by: <init>(), I(), v(int,int)
- `int[] gC`  R=7 W=1
    - read by: I(), V(), af(), r(int), u(int,int), v(int,int), y()
    - written by: <init>()
- `int[] U`  R=3 W=1
    - read by: I(), af(), v(int,int)
    - written by: <init>()
- `int s`  R=6 W=2
    - read by: I(), a(int,int,int[]), af(), q(int,int), v(int,int), y()
    - written by: <init>(), af()
- `int do`  R=3 W=3
    - read by: a(int,int,int[]), q(int,int), v(int,int)
    - written by: <init>(), I(), v(int,int)
- `int eS`  R=1 W=3
    - read by: af()
    - written by: <init>(), I(), v(int,int)
- `byte[] hv`  R=7 W=1
    - read by: H(), a(int,int,int,boolean,boolean), c(int,int,int), e(boolean), g(int), s(int,int), y()
    - written by: <init>()
- `byte[] aj`  R=6 W=1
    - read by: H(), a(int,int,int,boolean,boolean), c(int,int,int), e(boolean), g(int), s(int,int)
    - written by: <init>()
- `byte[] eq`  R=6 W=1
    - read by: H(), a(int,int,int,boolean,boolean), c(int,int,int), e(boolean), g(int), s(int,int)
    - written by: <init>()
- `short[][] cO`  R=6 W=1
    - read by: a(int,int,int,boolean,boolean), e(boolean), e(int,int), g(int), g(int,int,int), q(int,int)
    - written by: <init>()
- `byte[] hq`  R=5 W=1
    - read by: a(int,int,int,boolean,boolean), e(boolean), e(int,int), g(int), x(int,int)
    - written by: <init>()
- `byte[] gW`  R=2 W=1
    - read by: e(boolean), i(int,int)
    - written by: <init>()
- `byte[] eo`  R=20 W=1
    - read by: B(int), Q(), a(int,boolean), ad(), b(int,int,boolean), b(int,int,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), c(int), e(), f(int,int,int), f(javax.microedition.lcdui.Graphics), g(int), h(), i(int), i(int,int,int), j(int,int,int), l(int,int), q(), r(int,int), y(int)
    - written by: <init>()
- `byte[] af`  R=20 W=1
    - read by: B(int), Q(), a(int,boolean), ad(), b(int,int,boolean), b(int,int,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), c(int), e(), f(int,int,int), f(javax.microedition.lcdui.Graphics), g(int), h(), i(int), i(int,int,int), j(int,int,int), l(int,int), q(), r(int,int), y(int)
    - written by: <init>()
- `byte[] ey`  R=7 W=1
    - read by: B(int), b(int,int,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), f(int,int,int), g(int), l(int,int), y(int)
    - written by: <init>()
- `byte[] u`  R=7 W=1
    - read by: b(int,int,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), f(int,int,int), g(int), l(int,int), m(), y(int)
    - written by: <init>()
- `byte[] ak`  R=25 W=1
    - read by: I(), Q(), a(int,boolean), ad(), b(int,int,boolean), b(int,int,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), c(int), c(int,int), e(), f(javax.microedition.lcdui.Graphics), g(int), h(), i(int), i(int,int), l(int,int), m(), o(int), q(), q(int,int), r(int,int), u(int,int), v(int,int), x(int,int), y(int)
    - written by: <init>()
- `byte[] g`  R=7 W=1
    - read by: I(), ad(), b(int,int,int,int,int,int), g(int), h(), q(int,int), y(int)
    - written by: <init>()
- `byte[] aw`  R=9 W=1
    - read by: I(), b(int,int,int,int,int,int), e(), e(int,int), g(int), q(int,int), v(int,int), w(), y(int)
    - written by: <init>()
- `byte[] eT`  R=9 W=1
    - read by: ad(), b(int,int,boolean), b(int,int,int,int,int,int), g(int), i(int,int), l(int,int), q(int,int), x(int,int), y(int)
    - written by: <init>()
- `byte[] p`  R=10 W=1
    - read by: a(int,boolean), b(int,int,boolean), b(int,int,int,int,int,int), g(int), i(int,int), l(int,int), m(), q(int,int), x(int,int), y(int)
    - written by: <init>()
- `byte[] aI`  R=6 W=1
    - read by: b(int,int,boolean), b(int,int,int,int,int,int), g(int), l(int,int), x(int,int), y(int)
    - written by: <init>()
- `byte[] A`  R=4 W=1
    - read by: b(int,int,boolean), g(int), x(int,int), y(int)
    - written by: <init>()
- `short[] Y`  R=19 W=1
    - read by: A(int), B(int), I(), a(int,boolean), a(int,int,int[]), b(int,int,boolean), b(int,int,int,int,int,int), f(int,int,int), g(int), i(int,int), i(int,int,int), j(int,int,int), l(int,int), o(int), q(int,int), u(int,int), v(int,int), x(int,int), y(int)
    - written by: <init>()
- `int[] eL`  R=5 W=1
    - read by: a(int,int,int[]), b(int,int,int,int,int,int), e(int,int), g(int), y(int)
    - written by: <init>()
- `byte[] bz`  R=5 W=1
    - read by: b(int,int,int,int,int,int), g(int), q(int,int), v(int,int), y(int)
    - written by: <init>()
- `int aN`  R=5 W=1
    - read by: I(), e(int,int), q(int,int), u(int,int), v(int,int)
    - written by: I()
- `int ez`  R=7 W=4
    - read by: D(int), Q(), ad(), e(int,int), g(int), m(), m(int,int)
    - written by: <init>(), D(int), g(int), m()
- `byte[][] eR`  R=1 W=1
    - read by: y(int)
    - written by: <init>()
- `byte[][] aH`  R=1 W=1
    - read by: y(int)
    - written by: <init>()
- `int dI`  R=0 W=1
    - read by: -
    - written by: <init>()
- `short[][] cs`  R=1 W=1
    - read by: b(javax.microedition.lcdui.Graphics,int)
    - written by: <init>()
- `byte[] dv`  R=12 W=6
    - read by: W(), Y(), a(byte[]), a(int[]), a(short[]), b(byte), c(java.lang.String), j(), k(int), s(), s(int), y(int,int)
    - written by: <init>(), Y(), g(int), j(), k(int), y(int,int)
- `int dl`  R=6 W=9
    - read by: a(byte[]), a(int[]), a(short[]), b(byte), s(), s(int)
    - written by: <init>(), a(byte[]), a(int[]), a(short[]), b(byte), g(int), s(), s(int), y(int,int)
- `int fc`  R=1 W=2
    - read by: W()
    - written by: <init>(), y(int,int)
- `int fL`  R=7 W=3
    - read by: W(), a(byte[]), a(int[]), a(short[]), b(byte), s(int), y(int,int)
    - written by: <init>(), g(int), y(int,int)
- `int eK`  R=5 W=6
    - read by: W(), Y(), c(java.lang.String), j(), k(int)
    - written by: <init>(), Y(), g(int), j(), k(int), y(int,int)
- `boolean dd`  R=6 W=3
    - read by: X(), a(byte[]), a(int[]), a(short[]), b(byte), s(int)
    - written by: <init>(), g(int), y(int,int)
- `int bm`  R=2 W=5
    - read by: Y(), j()
    - written by: <init>(), Y(), j(), t(), u()
- `int fw`  R=2 W=5
    - read by: Y(), j()
    - written by: <init>(), Y(), j(), t(), u()
- `int[] dT`  R=5 W=1
    - read by: R(), g(int), q(), q(int,int), y(int)
    - written by: <init>()
- `int[] fS`  R=3 W=1
    - read by: R(), g(int), q(int,int)
    - written by: <init>()
- `short[] bb`  R=3 W=1
    - read by: R(), e(boolean), g(int)
    - written by: <init>()
- `short[] fs`  R=3 W=1
    - read by: R(), g(int), q(int,int)
    - written by: <init>()
- `boolean y`  R=1 W=3
    - read by: q(int,int)
    - written by: <init>(), R(), v(int,int)
- `int W`  R=2 W=2
    - read by: n(int,int), q(int,int)
    - written by: <init>(), b(int,boolean)
- `boolean bR`  R=1 W=2
    - read by: q(int,int)
    - written by: <init>(), b(int,boolean)
- `boolean dS`  R=1 W=3
    - read by: v(int,int)
    - written by: <init>(), R(), v(int,int)
- `int gK`  R=8 W=6
    - read by: R(), a(boolean,int,int,int,int,int,int,boolean,int), ab(), ah(), g(int), l(), q(int), q(int,int)
    - written by: <init>(), R(), a(boolean,int,int,int,int,int,int,boolean,int), ah(), g(int), q(int)
- `int gE`  R=8 W=6
    - read by: R(), a(int,int,int[]), ab(), g(int), l(), q(int,int), v(int,int), y()
    - written by: <init>(), R(), a(int,int,int[]), g(int), v(int,int), y()
- `int cF`  R=6 W=4
    - read by: R(), ab(), g(int), l(), q(), q(int,int)
    - written by: <init>(), R(), g(int), q()
- `int ho`  R=7 W=5
    - read by: R(), a(int,int,boolean), ab(), f(), g(int), l(), q(int,int)
    - written by: <init>(), R(), a(int,int,boolean), f(), g(int)
- `int gr`  R=6 W=4
    - read by: R(), ab(), g(int), l(), q(int,int), y(int)
    - written by: <init>(), R(), g(int), y(int)
- `int eg`  R=4 W=3
    - read by: R(), ab(), g(int), q(int,int)
    - written by: <init>(), ab(), g(int)
- `int eJ`  R=5 W=4
    - read by: R(), ab(), g(int), l(), q(int,int)
    - written by: <init>(), R(), ab(), g(int)
- `int eX`  R=4 W=4
    - read by: R(), g(int), l(), q(int,int)
    - written by: <init>(), R(), ab(), g(int)
- `int br`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int gV`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int cj`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int aA`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int dg`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int gs`  R=2 W=1
    - read by: R(), q(int,int)
    - written by: <init>()
- `int dq`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int bM`  R=2 W=2
    - read by: R(), q(int,int)
    - written by: <init>(), l()
- `int gx`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int eM`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int V`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int f`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int eu`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int dA`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int v`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int cM`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int bG`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int he`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int bP`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int ht`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int cr`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int cg`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int cm`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int gq`  R=2 W=3
    - read by: g(int), q(int,int)
    - written by: <init>(), R(), g(int)
- `int h`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int B`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int hn`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int ep`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int bo`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int eb`  R=1 W=2
    - read by: g(int)
    - written by: <init>(), g(int)
- `int dF`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int aq`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int fe`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int eh`  R=3 W=3
    - read by: R(), g(int), l()
    - written by: <init>(), R(), g(int)
- `int[] N`  R=5 W=1
    - read by: R(), b(int,boolean), g(int), l(), q(int,int)
    - written by: <init>()
- `int[] cx`  R=5 W=1
    - read by: R(), b(int,boolean), g(int), l(), q(int,int)
    - written by: <init>()
- `int[] Z`  R=5 W=1
    - read by: L(), R(), b(int,boolean), g(int), q(int,int)
    - written by: <init>()
- `int[] dG`  R=5 W=1
    - read by: R(), b(int,boolean), g(int), l(), q(int,int)
    - written by: <init>()
- `int[] ef`  R=5 W=1
    - read by: R(), b(int,boolean), g(int), l(), q(int,int)
    - written by: <init>()
- `int[] L`  R=5 W=1
    - read by: L(), R(), b(int,boolean), g(int), q(int,int)
    - written by: <init>()
- `byte[] o`  R=2 W=1
    - read by: b(int,boolean), q(int,int)
    - written by: <init>()
- `byte[] dm`  R=2 W=1
    - read by: b(int,boolean), q(int,int)
    - written by: <init>()
- `byte[] fA`  R=2 W=1
    - read by: b(int,boolean), n(int,int)
    - written by: <init>()
- `int cA`  R=5 W=2
    - read by: D(), F(), c(), g(int), m(int,int)
    - written by: F(), g(int)
- `boolean aY`  R=3 W=2
    - read by: D(), c(), g(int)
    - written by: D(), g(int)
- `int gS`  R=1 W=1
    - read by: c()
    - written by: F()
- `int bp`  R=1 W=1
    - read by: c()
    - written by: F()
- `int dD`  R=1 W=1
    - read by: c()
    - written by: D()
- `int k`  R=1 W=1
    - read by: c()
    - written by: D()
- `byte T`  R=4 W=3
    - read by: g(int), m(), q(int,int), v(int,int)
    - written by: <init>(), g(int), v(int,int)
- `int hu`  R=4 W=4
    - read by: c(), g(int), g(int,int), m()
    - written by: <init>(), c(), g(int), m()
- `byte cd`  R=2 W=3
    - read by: g(), g(int)
    - written by: <init>(), g(int), m()
- `byte fP`  R=2 W=3
    - read by: g(), g(int)
    - written by: <init>(), g(int), m()
- `byte[] eA`  R=4 W=1
    - read by: D(int), Q(), q(), x(int,int)
    - written by: <init>()
- `byte[][] aK`  R=1 W=1
    - read by: Q()
    - written by: <init>()
- `byte[][] a`  R=1 W=1
    - read by: Q()
    - written by: <init>()
- `byte[] hm`  R=14 W=1
    - read by: D(int), Q(), a(), a(int), e(int), g(int), h(int), l(int), q(int,int), t(int,int), v(int,int), w(int), x(int), z(int)
    - written by: <init>()
- `byte[] bh`  R=5 W=1
    - read by: D(int), Q(), e(int), f(int,int), g(int)
    - written by: <init>()
- `byte[] bY`  R=6 W=1
    - read by: D(int), Q(), e(int), f(int,int), g(int), q(int,int)
    - written by: <init>()
- `byte[] dn`  R=5 W=1
    - read by: D(int), Q(), e(int), f(int,int), g(int)
    - written by: <init>()
- `byte[] dE`  R=6 W=1
    - read by: D(int), Q(), e(int), f(int,int), g(int), q(int,int)
    - written by: <init>()
- `byte[] fW`  R=5 W=1
    - read by: D(int), Q(), e(int), f(int,int), g(int)
    - written by: <init>()
- `byte[] fF`  R=6 W=1
    - read by: D(int), Q(), e(int), f(int,int), g(int), q(int,int)
    - written by: <init>()
- `int[] aV`  R=6 W=1
    - read by: D(int), Q(), a(int), e(int), g(int), q(int,int)
    - written by: <init>()
- `int[] M`  R=11 W=1
    - read by: D(int), Q(), a(int), e(int), g(int), h(int), l(int), t(int,int), w(int), x(int), z(int)
    - written by: <init>()
- `byte[] fa`  R=5 W=1
    - read by: D(int), Q(), e(int), g(int), v(int,int)
    - written by: <init>()
- `byte[] X`  R=5 W=1
    - read by: D(int), Q(), e(int), g(int), q(int,int)
    - written by: <init>()
- `int[] cJ`  R=5 W=1
    - read by: D(int), Q(), e(int), g(int), q(int,int)
    - written by: <init>()
- `int er`  R=2 W=3
    - read by: Q(), g(int)
    - written by: <init>(), Q(), g(int)
- `short[] dU`  R=2 W=1
    - read by: Q(), g(int)
    - written by: <init>()
- `boolean dy`  R=2 W=2
    - read by: Q(), g(int)
    - written by: g(int), m()
- `int bq`  R=1 W=1
    - read by: g(int)
    - written by: g(int)
- `int bv`  R=2 W=3
    - read by: Q(), g(int)
    - written by: <init>(), Q(), g(int)
- `int aO`  R=2 W=2
    - read by: a(), e(int,int)
    - written by: <init>(), a()
- `byte[] gP`  R=3 W=1
    - read by: a(), q(int,int), v(int,int)
    - written by: <init>()
- `byte ap`  R=3 W=2
    - read by: U(), g(int), z()
    - written by: g(int), m()
- `int bs`  R=3 W=2
    - read by: U(), g(int), z()
    - written by: g(int), m()
- `int fT`  R=3 W=2
    - read by: U(), g(int), z()
    - written by: g(int), m()
- `int bD`  R=4 W=3
    - read by: U(), g(int), h(int), z()
    - written by: <init>(), g(int), h(int)
- `int cC`  R=4 W=3
    - read by: U(), g(int), t(int,int), z()
    - written by: <init>(), g(int), t(int,int)
- `int hb`  R=4 W=3
    - read by: U(), g(int), t(int,int), z()
    - written by: <init>(), g(int), t(int,int)
- `int dV`  R=4 W=3
    - read by: U(), g(int), t(int,int), z()
    - written by: <init>(), g(int), t(int,int)
- `int fu`  R=4 W=3
    - read by: U(), g(int), t(int,int), z()
    - written by: <init>(), g(int), t(int,int)
- `int cT`  R=4 W=3
    - read by: U(), g(int), t(int,int), z()
    - written by: <init>(), g(int), t(int,int)
- `int gu`  R=4 W=3
    - read by: U(), g(int), y(), z()
    - written by: <init>(), g(int), y()
- `boolean en`  R=2 W=3
    - read by: g(int), z()
    - written by: g(int), m(), z()
- `boolean cG`  R=2 W=4
    - read by: E(), ae()
    - written by: <init>(), E(), ae(), x()
- `boolean ay`  R=2 W=2
    - read by: E(), ae()
    - written by: <init>(), x()
- `boolean gR`  R=1 W=2
    - read by: m()
    - written by: <init>(), v(int,int)
- `boolean ac`  R=3 W=3
    - read by: ae(), q(int,int), v(int,int)
    - written by: <init>(), m(), v(int,int)
- `int gh`  R=5 W=3
    - read by: A(), Z(), q(int,int), v(int,int), x()
    - written by: <init>(), A(), v(int,int)
- `javax.microedition.media.Player bi`  R=3 W=2
    - read by: E(), ae(), x()
    - written by: <init>(), x()
- `javax.microedition.media.control.VolumeControl E`  R=2 W=2
    - read by: C(int), x()
    - written by: <init>(), x()
- `int by`  R=3 W=3
    - read by: c(int), g(int), h(int)
    - written by: <init>(), g(int), h(int)
- `int H`  R=4 W=3
    - read by: A(), b(boolean), c(int), q(int,int)
    - written by: <init>(), A(), c(int)
- `int[] gk`  R=4 W=1
    - read by: A(), b(boolean), c(int), q(int,int)
    - written by: <init>()
- `int[] bd`  R=3 W=1
    - read by: A(), b(boolean), q(int,int)
    - written by: <init>()
- `byte[] dO`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] el`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] fz`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] dM`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] l`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] ag`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] i`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] G`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] cQ`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] ec`  R=1 W=1
    - read by: b(boolean)
    - written by: <clinit>()
- `byte[] gl`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), k(int,int)
    - written by: <init>(), k(int,int)
- `byte[] av`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), k(int,int)
    - written by: <init>(), k(int,int)
- `int bS`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), k(int,int)
    - written by: <init>(), k(int,int)
- `boolean ch`  R=4 W=3
    - read by: A(), f(javax.microedition.lcdui.Graphics), q(int,int), v(int,int)
    - written by: <clinit>(), A(), v(int,int)
- `boolean aP`  R=4 W=3
    - read by: A(), f(javax.microedition.lcdui.Graphics), q(int,int), v(int,int)
    - written by: <clinit>(), A(), v(int,int)
- `boolean cc`  R=4 W=3
    - read by: A(), R(), q(int,int), v(int,int)
    - written by: <clinit>(), A(), v(int,int)
- `boolean q`  R=4 W=3
    - read by: A(), R(), q(int,int), v(int,int)
    - written by: <clinit>(), A(), v(int,int)
- `int bF`  R=8 W=3
    - read by: a(int,int), hideNotify(), j(int), m(), paint(javax.microedition.lcdui.Graphics), run(), showNotify(), t(int)
    - written by: a(int,int), paint(javax.microedition.lcdui.Graphics), t(int)
- `byte[][] cI`  R=45 W=2
    - read by: B(int), D(), E(int), F(), F(int), H(), J(), N(), S(), T(), a(boolean), a(boolean,int,int,int,int,int,int,boolean,int), a(boolean,int,int,int,int,int,int,int,int,int,int,boolean), a(int,int,boolean,boolean), a(int,int,int), a(int,int,int,boolean), a(int,int,int,int,int), a(int,int,int,int,int,int,int,int), ah(), b(), b(int,int), b(int,int,int,int), b(javax.microedition.lcdui.Graphics,int), c(javax.microedition.lcdui.Graphics), d(), e(boolean), e(int,int,int), e(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), g(int,int), h(), h(int,int), j(int,int), m(), m(int,int), n(int), o(int,int), r(), r(int,int), t(), u(), u(int), v(), w(int,int), y(int)
    - written by: F(int), u()
- `byte[][] ea`  R=37 W=2
    - read by: E(int), H(), I(), J(), N(), Q(), S(), T(), a(boolean,int,int,int,int,int,int,boolean,int), a(boolean,int,int,int,int,int,int,int,int,int,int,boolean), a(int,int,int), a(int,int,int,int,int), a(int,int,int,int,int,int,int,int), ad(), b(), b(int,int,int,int), b(javax.microedition.lcdui.Graphics,int), c(int), c(javax.microedition.lcdui.Graphics), d(), d(int,int,int), e(), e(boolean), e(int,int,int), g(int,int), h(), j(int,int), m(), n(int), q(), r(), r(int,int), t(), u(), v(), w(int,int), y(int)
    - written by: F(int), u()
- `int cU`  R=10 W=5
    - read by: B(), a(int,int,int[],int[],boolean), b(javax.microedition.lcdui.Graphics,int), e(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), g(int), i(), k(), m(), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), B(), g(int), k(), m()
- `int hc`  R=10 W=5
    - read by: B(), a(int,int,int[],int[],boolean), b(javax.microedition.lcdui.Graphics,int), e(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), g(int), i(), k(), m(), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), B(), g(int), k(), m()
- `int eU`  R=18 W=15
    - read by: D(int), Q(), R(), a(boolean,int,int,int,int,int,int,boolean,int), a(int), a(int,int,boolean), a(int,int,boolean,boolean), a(int,int,int[]), ah(), f(), g(int), h(javax.microedition.lcdui.Graphics), m(), q(), q(int), v(int,int), y(), y(int)
    - written by: <init>(), D(int), R(), a(boolean,int,int,int,int,int,int,boolean,int), a(int,int,boolean), a(int,int,int[]), ah(), f(), g(int), m(), q(), q(int), v(int,int), y(), y(int)
- `int cy`  R=1 W=2
    - read by: h(javax.microedition.lcdui.Graphics)
    - written by: <init>(), h(javax.microedition.lcdui.Graphics)
- `int hk`  R=9 W=5
    - read by: D(int), Q(), U(), ab(), g(int), h(), q(int,int), v(int,int), z()
    - written by: <init>(), D(int), g(int), m(), v(int,int)
- `int cY`  R=6 W=4
    - read by: D(int), Q(), U(), g(int), q(int,int), v(int,int)
    - written by: <init>(), D(int), g(int), m()
- `byte aD`  R=5 W=4
    - read by: D(int), Q(), ab(), g(int), q(int,int)
    - written by: <init>(), D(int), g(int), m()
- `int bB`  R=5 W=3
    - read by: Q(), c(int), g(int), h(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), g(int), m()
- `int eQ`  R=8 W=3
    - read by: P(), Q(), R(), b(int,boolean), c(int), g(int), h(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), g(int), m()
- `int bj`  R=8 W=3
    - read by: Q(), U(), b(int,boolean), c(int), g(int), h(javax.microedition.lcdui.Graphics), m(), z()
    - written by: <init>(), g(int), m()
- `int fr`  R=1 W=2
    - read by: h(javax.microedition.lcdui.Graphics)
    - written by: <init>(), h(javax.microedition.lcdui.Graphics)
- `int fd`  R=1 W=2
    - read by: h(javax.microedition.lcdui.Graphics)
    - written by: <init>(), h(javax.microedition.lcdui.Graphics)
- `int dN`  R=1 W=2
    - read by: h(javax.microedition.lcdui.Graphics)
    - written by: <init>(), h(javax.microedition.lcdui.Graphics)
- `int bI`  R=2 W=3
    - read by: g(int), m()
    - written by: <init>(), g(int), m()
- `int eC`  R=2 W=3
    - read by: ab(), g(int)
    - written by: <init>(), g(int), m()
- `int aM`  R=3 W=3
    - read by: A(), e(int,int), z()
    - written by: <init>(), A(), z()
- `int eF`  R=4 W=3
    - read by: ah(), g(int), v(int), z()
    - written by: <init>(), g(int), m()
- `int ao`  R=2 W=3
    - read by: g(int), v(int,int)
    - written by: <init>(), g(int), m()
- `java.util.Random gm`  R=17 W=1
    - read by: D(), F(), F(int), Q(), a(byte,int,int), ag(), b(int,int,int,int,int), c(int,int,int,int,int), e(boolean), f(int), f(int,int), g(), m(), p(), u(int), v(), w(int,int)
    - written by: <init>()
- `javax.microedition.lcdui.Image fG`  R=6 W=4
    - read by: <init>(), a(int,int), f(javax.microedition.lcdui.Graphics), m(), paint(javax.microedition.lcdui.Graphics), t(int)
    - written by: <init>(), a(int,int), m(), t(int)
- `javax.microedition.lcdui.Graphics C`  R=3 W=4
    - read by: a(javax.microedition.lcdui.Graphics), f(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), a(int,int), m(), t(int)
- `javax.microedition.lcdui.Image[] fU`  R=5 W=1
    - read by: a(javax.microedition.lcdui.Graphics,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), f(javax.microedition.lcdui.Graphics), m(), u(int,int)
    - written by: t(int)
- `byte[] gt`  R=5 W=1
    - read by: a(javax.microedition.lcdui.Graphics,int,int,int,int), b(javax.microedition.lcdui.Graphics,int), f(javax.microedition.lcdui.Graphics), t(int), u(int,int)
    - written by: <init>()
- `javax.microedition.lcdui.Image P`  R=1 W=1
    - read by: paint(javax.microedition.lcdui.Graphics)
    - written by: t(int)
- `long gD`  R=1 W=1
    - read by: m()
    - written by: t(int)
- `boolean dZ`  R=1 W=3
    - read by: m()
    - written by: j(int), m(), t(int)
- `int eW`  R=2 W=2
    - read by: m(), paint(javax.microedition.lcdui.Graphics)
    - written by: m(), t(int)
- `int al`  R=3 W=2
    - read by: m(), paint(javax.microedition.lcdui.Graphics), t(int)
    - written by: <init>(), t(int)
- `int cq`  R=2 W=15
    - read by: a(int,int), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), D(int), I(), M(), Q(), R(), a(int,boolean), ah(), e(int), hideNotify(), j(int), m(), paint(javax.microedition.lcdui.Graphics), v(int,int), z()
- `boolean d`  R=5 W=15
    - read by: a(int,int), e(int,int), paint(javax.microedition.lcdui.Graphics), t(int), v(int,int)
    - written by: <init>(), D(int), I(), M(), Q(), R(), a(int,boolean), ah(), e(int), hideNotify(), j(int), m(), paint(javax.microedition.lcdui.Graphics), v(int,int), z()
- `int ct`  R=1 W=4
    - read by: m()
    - written by: <init>(), F(int), m(), t(int)
- `byte[] ds`  R=6 W=1
    - read by: a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int,int,byte), d(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics), q(int,int), t(int)
    - written by: t(int)
- `byte[][] n`  R=6 W=1
    - read by: a(javax.microedition.lcdui.Graphics), a(javax.microedition.lcdui.Graphics,int,int,byte), d(javax.microedition.lcdui.Graphics), paint(javax.microedition.lcdui.Graphics), q(int,int), t(int)
    - written by: t(int)
- `int fy`  R=1 W=2
    - read by: m()
    - written by: <init>(), m()
- `int fh`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), m()
- `int gB`  R=1 W=2
    - read by: m()
    - written by: <init>(), m()
- `int gb`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), m()
- `int ei`  R=1 W=2
    - read by: m()
    - written by: <init>(), m()
- `int bc`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), m()
- `int eE`  R=1 W=2
    - read by: m()
    - written by: <init>(), m()
- `int dj`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), m()
    - written by: <init>(), m()
- `byte[][] ha`  R=3 W=1
    - read by: a(int,int,boolean,boolean), f(javax.microedition.lcdui.Graphics), m()
    - written by: <init>()
- `boolean[] go`  R=2 W=1
    - read by: a(int,int,boolean,boolean), m()
    - written by: <init>()
- `int[] dc`  R=1 W=1
    - read by: a(javax.microedition.lcdui.Graphics,int,int,int,int,byte[])
    - written by: <init>()
- `int bx`  R=0 W=1
    - read by: -
    - written by: <init>()
- `int fR`  R=0 W=1
    - read by: -
    - written by: <init>()
- `byte fZ`  R=1 W=2
    - read by: paint(javax.microedition.lcdui.Graphics)
    - written by: m(), paint(javax.microedition.lcdui.Graphics)
- `byte ff`  R=1 W=1
    - read by: paint(javax.microedition.lcdui.Graphics)
    - written by: m()
- `int eD`  R=1 W=2
    - read by: a(int,int)
    - written by: <init>(), a(int,int)
- `int aJ`  R=1 W=2
    - read by: a(int,int)
    - written by: <init>(), a(int,int)
- `int hx`  R=1 W=2
    - read by: a(int,int)
    - written by: <init>(), t(int)
- `int gd`  R=2 W=3
    - read by: m(), paint(javax.microedition.lcdui.Graphics)
    - written by: <init>(), m(), t(int)
- `int dH`  R=1 W=1
    - read by: m()
    - written by: <init>()
- `long em`  R=0 W=2
    - read by: -
    - written by: m(), paint(javax.microedition.lcdui.Graphics)
- `long ah`  R=0 W=2
    - read by: -
    - written by: m(), paint(javax.microedition.lcdui.Graphics)
- `long gw`  R=0 W=1
    - read by: -
    - written by: <init>()
- `byte[] gc`  R=1 W=1
    - read by: f(javax.microedition.lcdui.Graphics)
    - written by: <init>()
- `int bH`  R=1 W=15
    - read by: f(javax.microedition.lcdui.Graphics)
    - written by: <init>(), a(boolean,int,int,int,int,int,int,boolean,int), a(int,boolean), a(int,int), a(int,int,boolean), a(int,int,int,int,int,int,int,int), ah(), b(int,int,int,int,int,int), f(javax.microedition.lcdui.Graphics), l(int,int), m(), q(int), t(int), u(), y(int)
- `int dW`  R=1 W=2
    - read by: f(javax.microedition.lcdui.Graphics)
    - written by: <init>(), f(javax.microedition.lcdui.Graphics)
- `short[] bU`  R=2 W=1
    - read by: f(javax.microedition.lcdui.Graphics), g(int)
    - written by: <init>()
- `int es`  R=2 W=3
    - read by: f(javax.microedition.lcdui.Graphics), g(int)
    - written by: <init>(), f(javax.microedition.lcdui.Graphics), g(int)
- `int fE`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), g(int)
    - written by: f(javax.microedition.lcdui.Graphics), g(int)
- `int eO`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), g(int)
    - written by: f(javax.microedition.lcdui.Graphics), g(int)
- `int cl`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), g(int)
    - written by: f(javax.microedition.lcdui.Graphics), g(int)
- `int gM`  R=2 W=2
    - read by: f(javax.microedition.lcdui.Graphics), g(int)
    - written by: f(javax.microedition.lcdui.Graphics), g(int)
