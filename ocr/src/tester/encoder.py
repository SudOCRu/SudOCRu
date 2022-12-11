import sys,os,struct,time,random
from subprocess import run
from tkinter import *
from PIL import Image,ImageTk,ImageFont,ImageDraw

class Tester:
    def __init__(self, paths):
        self.i = 0
        self.paths = paths

def writeNewTrainingSet():
    samples_base = "train/"
    num = []
    pixs = []
    with open("../training", "wb") as f:
        for i in range(10):
            folder = os.path.join(samples_base, str(i))
            for image_path in os.listdir(folder):
                image_path = os.path.join(folder, image_path)
                if os.path.isfile(image_path):
                    num.append(i)
                    img = Image.open(image_path)
                    bands = img.getbands()
                    for j in range(img.width*img.height):
                        if bands == ('P',):
                            p = img.getpixel((j%img.width, j//img.width))
                            normalized = (p*255)//9
                        else:
                            r,g,b = img.getpixel((j%img.width, j//img.width))
                            normalized = (r+g+b)//3
                        pixs.append(normalized)
        f.write((2051).to_bytes(4, byteorder='little', signed=True))
        f.write((len(num)).to_bytes(4, byteorder='little', signed=True))
        f.write((28).to_bytes(4, byteorder='little', signed=True))
        f.write((28).to_bytes(4, byteorder='little', signed=True))
        f.write(bytearray(pixs))

    with open('../labels', 'wb') as f:
        f.write((2049).to_bytes(4, byteorder='little', signed=True))
        f.write((len(num)).to_bytes(4, byteorder='little', signed=True))
        f.write(bytearray(num))
    print('total images encoded:',len(num))


def writeTrainingSet():
    samples_base = "train/"
    samples_paths = []
    for path in os.listdir(samples_base):
        if os.path.isdir(os.path.join(samples_base, path)) and path.__contains__("sample"):
            samples_paths.append(os.path.join(samples_base, path))
    
    num = []
    pixs = []
    with open('../training', 'wb') as f:
        for folder in samples_paths:
            for image_path in os.listdir(folder):
                image_path = os.path.join(folder, image_path)
                if os.path.isfile(image_path):
                    num.append(int(os.path.basename(os.path.splitext(image_path)[0])))
                    img = Image.open(image_path)
                    bands = img.getbands()
                    for i in range(img.width*img.height):
                        if bands == ('P',):
                            p = img.getpixel((i%img.width, i//img.width))
                            normalized = (p*255)//9
                        else:
                            r,g,b = img.getpixel((i%img.width, i//img.width))
                            normalized = (r+g+b)//3
                        pixs.append(normalized)
        f.write((2051).to_bytes(4, byteorder='little', signed=True))
        f.write((len(num)).to_bytes(4, byteorder='little', signed=True))
        f.write((28).to_bytes(4, byteorder='little', signed=True))
        f.write((28).to_bytes(4, byteorder='little', signed=True))
        f.write(bytearray(pixs))


    with open('../labels', 'wb') as f:
        f.write((2049).to_bytes(4, byteorder='little', signed=True))
        f.write((len(num)).to_bytes(4, byteorder='little', signed=True))
        f.write(bytearray(num))
    print('total images encoded:',len(num))

def grayscaleImage(imagePath):
    img = Image.open(imagePath)
    bands = img.getbands()
    with open('../data', 'wb') as f:
        for i in range(img.width*img.height):
            x,y = i%img.width, i//img.width
            if bands == ('P',):
                pixel = img.getpixel((x, y))
                f.write(struct.pack('d', (pixel*255)/9))
            else:
                r,g,b = img.getpixel((x, y))
                t = ((r+g+b)//3)
                f.write(struct.pack('d', t))

def isWhiteAtLine(line : int, img):
    for i in range(img.width):
        r, g, b = img.getpixel((i, line))
        bw = (r+g+b)//3
        if bw > 100:
            return True
    return False

def MaxImage(f : ImageFont, text):
    img = Image.new(mode="RGB", size=(28,28), color=(0, 0, 0))
    drawer = ImageDraw.Draw(img)
    size = f.size
    l, t, r, b = 0, 0, 0, 0
    iterations = 0
    while (b-t) != 28 and iterations < 1000:
        size += 1
        f = ImageFont.truetype(f.path, size)
        l, t, r, b = drawer.textbbox((-l, -t), text, font=f)
        iterations += 1
    l, t, r, b = drawer.textbbox((0, 0), text, font=f)
    width = r-l
    drawer.text((-l+(14-width//2), -t), text, (255, 255, 255), font=f)
    return img

def SaveImageWithRotations(img : Image, j, currPath):
    imgPath = os.path.join(currPath, "v"+str(j)+".png")
    img.save(imgPath)
    j+=1
    imgPath = os.path.join(currPath, "v"+str(j)+".png")
    img.rotate(-5).save(imgPath)
    j+=1
    imgPath = os.path.join(currPath, "v"+str(j)+".png")
    img.rotate(5).save(imgPath)
    return j+1

def test(fontPaths):
    fonts = []
    size = 10
    for fontPath in fontPaths:
        fonts.append(ImageFont.truetype(fontPath, size))
    
    for i in range(10):
        currPath = "train/"+str(i)+"/"
        if not os.path.exists(currPath):
            os.mkdir(currPath)
        j = 0
        print("writing image: ", str(i))
        for font in fonts:
            print("using font: ",font.path)
            img = MaxImage(font, str(i))
            j = SaveImageWithRotations(img, j, currPath)
            new = img.transform(img.size, Image.AFFINE, (1, 0, 0, 0, 1, -3))
            j = SaveImageWithRotations(new, j, currPath)
            new = img.transform(img.size, Image.AFFINE, (1, 0, 0, 0, 1, 3))
            j = SaveImageWithRotations(new, j, currPath)

def generateImgs(fonts):
    rotId = 0
    for offset in range(-5, 6, 5):
        for rots in range(-5, 6, 5):
            length = len(fonts)
            for id in range(len(fonts)):
                size = 20
                img = Image.open("base.png")
                font = ImageFont.truetype(fonts[id], size)
                I1 = ImageDraw.Draw(img)
                y = 5
                I1.text((4, y), "0", font=font, fill=(255,255,255))
                for i in range (5):
                    while not isWhiteAtLine(0, img):
                        y-=1
                        img = Image.open("base.png")
                        I1 = ImageDraw.Draw(img)
                        I1.text((4, y), "0", font=font, fill=(255,255,255))
                    while not isWhiteAtLine(img.height-1, img):
                        img = Image.open("base.png")
                        I1 = ImageDraw.Draw(img)
                        size += 1
                        font = ImageFont.truetype(fonts[id], size)
                        I1.text((4, y), "0", font=font, fill=(255,255,255))
                
                if not os.path.exists("train/sample"+str(rotId*length+id)+"/"):
                    os.mkdir("train/sample"+str(rotId*length+id))
                for i in range(10):
                    img = Image.open("base.png")
                    I1 = ImageDraw.Draw(img)
                    _, _, w, h = I1.textbbox((0, 0), str(i), font=font)
                    I1.text(((28-w)/2, y), str(i), font=font, fill=(255,255,255))
                    pixels = img.load()
                    for j in range(120):
                        pixel = random.randrange(0, 28*28-1)
                        pixels[pixel%28, pixel//28] = (0, 0, 0)
                    """if rots == 1:
                        img.rotate(5).save("train/sample"+str(rots*length+id)+"/"+str(i)+".png")
                    elif rots == 2:"""
                    img.rotate(rots).save("train/sample"+str(rotId*length+id)+"/"+str(i)+".png")
                    print("rotation: ",rots,"degs")
                    """else:
                        img.save("train/sample"+str(rots*length+id)+"/"+str(i)+".png")"""
            rotId+=1

def RotateImg(img : Image, deg):
    img.rotate(-deg)

st = time.time()

if len(sys.argv) < 2:
    print('not enough args:')
    print('test <path>: path to the image to encode and test')
    print('train: encode samples to learn')
    print('suit <path>: path to the folder containing images')
    print('generator: generate image samples for training')
    exit(1)

index = 0

def nextImage(t : Tester, label : Label, label2 : Label, labelpath : Label, dirs, offset):
    if t.i == 0 and offset < 0:
        labelpath["text"] = "Beginning of the folder"
    elif t.i < len(dirs):
        t.i+=offset
        grayscaleImage(os.path.join(t.paths, dirs[t.i]))
        os.chdir("../")
        v = run(['./tests', 'eval'])
        os.chdir("tester")
        img = ImageTk.PhotoImage(Image.open(os.path.join(t.paths, dirs[t.i])))
        label2["text"]=str(v.returncode)
        labelpath["text"]=dirs[t.i]
        label.configure(image=img)
        label.image = img
        print("offset=",offset," t.i=",t.i)
    else:
        labelpath["text"]="All images readed"

if sys.argv[1] == 'test' and len(sys.argv) == 3:
    grayscaleImage(sys.argv[2])
    os.chdir("../")
    v = run(['./tests', 'eval'])
    print("result = ",str(v.returncode))
    os.chdir("tester")
elif sys.argv[1] == 'train':
    writeNewTrainingSet()
    #writeTrainingSet()
elif sys.argv[1] == 'suit' and len(sys.argv) == 3:
    dirs = os.listdir(sys.argv[2])

    window = Tk(className='Neural network result')
    window.geometry("200x200")
    frame = Frame(window)
    frame.pack()
    frame.place(anchor='center',relx=0.5,rely=0.5)

    grayscaleImage(os.path.join(sys.argv[2], dirs[0]))
    
    os.chdir("../")
    v = run(['./tests', 'eval'])
    os.chdir("tester")
    img = ImageTk.PhotoImage(Image.open(os.path.join(sys.argv[2], dirs[0])))
    
    labelpath = Label(frame, text=dirs[0])
    labelpath.pack()
    label = Label(frame, image=img)
    label.pack()
    label2 = Label(frame, text=str(v.returncode))
    label2.pack()
    a = Tester(sys.argv[2])
    
    next = Button(text="Next",command=lambda: nextImage(a, label, label2, labelpath, dirs, 1))
    next.pack()
    back = Button(text="Back",command=lambda: nextImage(a, label, label2, labelpath, dirs, -1))
    back.pack()
    window.mainloop()
elif sys.argv[1] == 'generator':
    fonts = []
    font_path = "fonts/"
    for path in os.listdir(font_path):
        if os.path.isfile(os.path.join(font_path, path)):
            fonts.append(font_path+path)
    #generateImgs(fonts)
    test(fonts)

et = time.time()
print('Executions time:',(et-st)*1000,'ms')