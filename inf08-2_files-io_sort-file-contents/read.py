with open("file", "rb") as f:
    while (byte := f.read(4)):
        print(int.from_bytes(byte, "little"))
