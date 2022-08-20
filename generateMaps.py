from PIL import Image
import os
from textwrap import wrap

maindir = os.getcwd()
imagesdir = r"{}\{}".format(maindir, "images")

directory = os.fsencode(imagesdir)

defines = "#define NUMBER_FACES {}\n".format(len(next(os.walk(directory))[1]))

defines += "#define INVALID_FACE -1\n"
array = "\nstatic const byte allFaces[][2][64] PROGMEM = {\n"

for index, folder in enumerate(os.listdir(directory)):
    foldername = folder.decode("utf-8")
    fullfolder = r"{}\{}".format(imagesdir, foldername)
    defines += "#define {} {}\n".format(foldername.upper(), index)
    faces = ""
    for file in os.listdir(fullfolder):
        im = Image.open(r"{}\{}".format(fullfolder, file))
        px = im.load()

        data = ""

        for y in range(16):
            for x in range(32):
                data += str(int(px[x, y][0] == 0))

        face = ""
        octets = wrap(data, 8)
        for i in range(64):
            face += "0b" + octets[i] + ", "

        face = face[:len(face) - 2]
        faces += "    {" + face + "},\n"
    faces = faces[:len(faces) - 2]
    array += "  {\n" + faces + "\n  },\n"

array = array[:len(array) - 2] + "\n};\n"

f = open(r"{}\{}".format(maindir, "arduino/faces.h"), "w")
f.write(defines + array)
f.close()

print("done")
