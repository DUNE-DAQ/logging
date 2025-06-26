import re


def get_version_from_cmake(cmake_file="../CMakeLists.txt"):
    with open(cmake_file) as f:
        for line in f:
            m = re.match(
                r'project\([^\)]*VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)', line)
            if m:
                return m.group(1)
    raise RuntimeError("Version not found in CMakeLists.txt")


if __name__ == "__main__":
    version = get_version_from_cmake()
    with open("version.py", "w") as f:
        f.write(f'__version__ = "{version}"\n')
