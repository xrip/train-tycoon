from pathlib import Path
import struct, zlib

src=Path('/mnt/data/train_tycoon_decompile/decoded_resources/1.bin')
out=Path('/mnt/data/train_tycoon_decompile/extracted_images')
out.mkdir(exist_ok=True)
data=src.read_bytes(); p=0; n=0

def take(k):
    global p
    if p+k>len(data): raise EOFError((p,k,len(data)))
    b=data[p:p+k]; p+=k; return b

def u8(): return take(1)[0]
def u16(): return (u8()<<8)|u8()

while p < len(data):
    start=p
    try: typ=u8()
    except EOFError: break
    try:
        if typ==2:
            total=u16()
            if total<8: raise ValueError('bad full PNG length')
            png=b'\x89PNG'+take(total-4)
        elif typ==1:
            idat_len=u16()
            palette_len=u8()*3
            has_trns=u8()
            total=idat_len+palette_len+69+has_trns*13
            png=bytearray(total)
            png[0:8]=b'\x89PNG\r\n\x1a\n'
            png[11]=13
            png[12:16]=b'IHDR'
            png[18]=u8(); png[19]=u8()
            png[22]=u8(); png[23]=u8()
            png[24]=u8(); png[25]=3
            png[29:33]=take(4)
            png[35]=(palette_len>>8)&255; png[36]=palette_len&255
            png[37:41]=b'PLTE'
            png[41:41+palette_len+4]=take(palette_len+4)
            i=45+palette_len
            if has_trns==1:
                i+=3; png[i]=1; i+=1
                png[i:i+4]=b'tRNS'; i+=4
                i+=1
                png[i:i+4]=bytes([0x40,0xE6,0xD8,0x66]); i+=4
            i+=2
            png[i]=(idat_len>>8)&255; i+=1
            png[i]=idat_len&255; i+=1
            png[i:i+4]=b'IDAT'; i+=4
            png[i:i+idat_len+4]=take(idat_len+4); i+=idat_len+4
            i+=4
            png[i:i+4]=b'IEND'; i+=4
            png[i:i+4]=bytes([0xAE,0x42,0x60,0x82]); i+=4
            png=bytes(png)
        else:
            print('stop unknown type',typ,'at',start,'remaining',len(data)-p)
            break
        fn=out/f'image_{n:03d}_type{typ}.png'
        fn.write_bytes(png)
        # basic PNG chunk scan / CRC check
        ok=png.startswith(b'\x89PNG\r\n\x1a\n')
        if ok:
            q=8
            try:
                while q+12<=len(png):
                    ln=int.from_bytes(png[q:q+4],'big'); ct=png[q+4:q+8]; body=png[q+8:q+8+ln]; crc=int.from_bytes(png[q+8+ln:q+12+ln],'big')
                    if zlib.crc32(ct+body)&0xffffffff != crc: ok=False; break
                    q+=12+ln
                    if ct==b'IEND': break
            except Exception: ok=False
        print(n,typ,len(png),'OK' if ok else 'BAD', 'src',start,'..',p)
        n+=1
    except Exception as e:
        print('error at',start,'type',typ,e)
        break
print('images',n,'consumed',p,'of',len(data))
