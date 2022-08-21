from PIL import Image
import os
from textwrap import wrap

maindir = os.getcwd()
imagesdir = os.path.join(maindir, "images")

directory = os.fsencode(imagesdir)

array = "static const byte allFaces[][2][64] PROGMEM = {\n"

for index, folder in enumerate(sorted(os.listdir(directory))):
    foldername = folder.decode("utf-8")
    fullfolder = os.path.join(imagesdir, foldername)
    faces = ""
    for file in sorted(os.listdir(fullfolder)):
        im = Image.open(os.path.join(fullfolder, file))
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

f = open(os.path.join(maindir, "arduino/moody/faces.h"), "w")
f.write(array)
f.close()

print("done")
