"""
image_compress.py
"""

from PIL import Image

def compress(path, out_path):
	im = Image.open(path)
	pix = im.load()
	print im.size
	print pix[0, 0]
	im.save(out_path)

if __name__ == "__main__":
	in_path = ""