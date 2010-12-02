import os, subprocess

def which(name):
    """like the unix `which -a` but for windows"""
    name = name.lower()
    if not name.endswith(".exe"):
        name += ".exe"
    results = []
    for folder in os.getenv("PATH").split(";"):
        try:
            items = os.listdir(folder)
        except OSError:
            continue
        lowerToProper = {item.lower(): item for item in items}
        try:
            results.append(os.path.join(folder, lowerToProper[name]))
        except KeyError:
            pass
    return results


def make_thumbnail(in_file, out_file, new_width):
    # on josh's VirtualBox, python insisted on using the system32 convert.exe
    # instead of the ImageMagic one even though it was on the path in front of
    # system32.
    for convert_path in which("convert"):
        if convert_path.lower().find("system32") != -1:
            continue
        the_right_convert = convert_path
        break
    else:
        assert False, "ImageMagick is not installed and on the path"
    process = subprocess.Popen([the_right_convert, in_file, '-thumbnail', str(new_width), '-strip', out_file])
    process.wait()

