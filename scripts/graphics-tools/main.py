import argparse
import PIL.Image
import bdfparser
import makeelf.elf as elf

def to_beatrix_bmp (im: PIL.Image.Image) -> bytes:
    return im.convert("1").tobytes("raw", "1")

def to_beatrix_font (f: bdfparser.Font, codemap: str) -> bytes:
    raw = f.draw(codemap, linelimit=8)
    im = PIL.Image.frombytes(
            "1",
            (raw.width(), raw.height()),
            raw.tobytes("1")
        ).crop(
            (0, 0, 8, 16 * len(codemap))
        )
    return to_beatrix_bmp(im)

def to_objfile (data: bytes, symbol_name: str) -> elf.ELF:
    objfile = elf.ELF(e_data = elf.ELFDATA.ELFDATA2LSB, e_type = elf.ET.ET_REL, e_machine = elf.EM.EM_ARM)
    sec_id = objfile.append_section(".rodata", data, 0x0)
    objfile.Elf.Shdr_table[sec_id].sh_flage = 0x2 # Allocatable
    objfile.append_symbol(symbol_name, sec_id, 0x0, len(data), sym_binding = elf.STB.STB_GLOBAL, sym_type = elf.STT.STT_OBJECT)
    objfile.Elf.Shdr_table[-1].sh_info = 1 # Make sure we don't get .symtab error at link time
    return objfile

def main():
    codemap = "".join([chr(x) for x in range(32, 127)] + [chr(x) for x in range(0x400, 0x480)])
    fontdata = to_beatrix_font(bdfparser.Font("../../resources/unifont-16.0.04.bdf"), codemap)
    with open("unifont.o", "wb") as f:
        f.write(bytes(to_objfile(fontdata, "bea_unifont")))
    logodata = to_beatrix_bmp(PIL.Image.open("../../resources/logo.png").resize((64, 64)))
    with open("logo.o", "wb") as l:
        l.write(bytes(to_objfile(logodata, "bea_logo")))

if __name__ == "__main__":
    main()
