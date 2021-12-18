from PIL import Image
 
frames = []
 
for frame_number in range(1, 500):
    frame = Image.open(f'dir/{frame_number}.bmp')
    frames.append(frame)
 
frames[0].save(
    'homer.gif',
    save_all=True,
    append_images=frames[1:], 
    optimize=True,
    duration=30,
    loop=0
)
