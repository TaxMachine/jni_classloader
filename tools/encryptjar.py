from random import randint
from typing import List


def xor(data: List[int], key: List[int]) -> List[int]:
    return [data[i] ^ key[i % len(key)] for i in range(len(data))]


def generate_key(length: int) -> List[int]:
    return [randint(0, 255) for _ in range(length)]


def encode(data: List[int]) -> List[str]:
    return ["0x" + hex(byte)[2:].zfill(2) for byte in data]


def toCpp(var_type: str, name: str, data: List[str]) -> str:
    return f"{var_type} {name}[] = {{{', '.join(data)}}};"


def toStackString(var_name: str, data: List[str]) -> str:
    result = f"unsigned char {var_name}[{len(data)}];\n"
    result += f"void init_{var_name}() {{\n"
    for i in range(len(data)):
        result += f"    {var_name}[OBF({i})] = OBF({data[i]});\n"
    result += "}"
    return result


f = open("testjni2.jar", "rb")
jar_data = list(f.read())

xor_key = generate_key(6)
encrypted = xor(jar_data, xor_key)

print(toCpp("unsigned char", "key", encode(xor_key)))
print(toCpp("unsigned char", "jar", encode(encrypted)))

#print(toStackString("key", encode(xor_key)))
#print(toStackString("jar", encode(encrypted)))
