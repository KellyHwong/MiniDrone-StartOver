def EvalChecksum(chars):
    checksum = 0x00
    for char in chars:
        checksum ^= char # xor
    return checksum

def main():
    chars = [0xA5,0x5A,0x05,0xF1,0xC3]
    print hex(EvalChecksum(chars))

if __name__ == '__main__':
    main()
