import sys
import os
import struct
import time
from subprocess import run
from tkinter import *
from PIL import Image,ImageTk

class Tester:
    def __init__(self, paths):
        self.i = 1
        self.paths = paths

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

st = time.time()

if len(sys.argv) < 2:
    print('not enough args:')
    print('test <path>: path to the image to encode and test')
    print('train: encode samples to learn')
    print('suit <path>: path to the folder containing images')
    exit(1)

index = 0

def nextImage(t : Tester, label : Label, label2 : Label, labelpath : Label, dirs):
    if t.i < len(dirs):
        grayscaleImage(os.path.join(t.paths, dirs[t.i]))
        os.chdir("../")
        v = run(['./tests', 'eval'])
        os.chdir("tester")
        img = ImageTk.PhotoImage(Image.open(os.path.join(t.paths, dirs[t.i])))
        label2["text"]=str(v.returncode)
        labelpath["text"]=dirs[t.i]
        label.configure(image=img)
        label.image = img
        t.i+=1
    else:
        labelpath["text"]="All images readed"


if sys.argv[1] == 'test' and len(sys.argv) == 3:
    grayscaleImage(sys.argv[2])
    os.chdir("../")
    v = run(['./tests', 'eval'])
    print("result = ",str(v.returncode))
    os.chdir("tester")
elif sys.argv[1] == 'train':
    writeTrainingSet()
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
    
    next = Button(text="Next",command=lambda: nextImage(a, label, label2, labelpath, dirs))
    next.pack()
    window.mainloop()

et = time.time()
print('Executions time:',(et-st)*1000,'ms')