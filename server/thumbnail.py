import subprocess

def make_thumbnail(in_file, out_file, new_width):
    proc = subprocess.Popen(['convert', in_file, '-thumbnail', str(new_width), '-strip', out_file])
    proc.communicate()
