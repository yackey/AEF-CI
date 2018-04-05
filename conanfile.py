from conans import ConanFile
from conans import tools
#import os

class AefLibConan(ConanFile):
    name = "AEFLib"
    version = "1.0"
    settings = "os", "build_type", "arch"
    description = "AEFLib contains the Avtec Embedded Framework shared libraries and include files"
    url = "None"
    license = "None"
    options = {"shared": [True, False]}
    default_options = "shared=True"
    no_copy_source=False

    def package(self):
#       consumers will not have a directory structure in the include directory
        self.copy("*.h", "include", "include", keep_path=False)
#        print('yo from package')
#        cwd = os.getcwd()
#        print("cwd [", cwd, "]")
#       adir = cwd + "/x86_64/bin/Release"
#       print("adir [", adir, "]")
#       if os.path.isdir(adir):
#           items = os.listdir(adir)
#           for name in items:
#               print(name)
#       else:
#           print("no dir")
#
        if self.settings.os == "Linux" and self.settings.arch == "x86_64":
            if self.settings.build_type == "Release":
                self.copy("*.so*", "lib", "build/x86_64/bin/Release", symlinks=True)
            else:
                self.copy("*.so*", "lib", "build/x86_64/bin/Debug", symlinks=True)
        elif self.settings.os == "Linux" and self.settings.arch == "armv7":
            if self.settings.build_type == "Release":
                self.copy("*.so*", "lib", "build/ARM/bin/Release", symlinks=True)
            else:
                self.copy("*.so*", "lib", "build/ARM/bin/Debug", symlinks=True)
        else:
            raise Exception("Package settings")


    def package_info(self):
        #print("yo from package_info")
        #cwd = os.getcwd()
        #print("cwd [", cwd, "]")
        if self.settings.os == "Linux" and self.settings.arch == "x86_64":
            if self.settings.build_type == "Release":
                self.cpp_info.libs = ["libAEF_ARM.so"]
            else:
                self.cpp_info.libs = ["libAEF_ARM.so"]
        elif self.settings.os == "Linux" and self.settings.arch == "armv7":
            if self.settings.build_type == "Release":
                self.cpp_info.libs = ["libAEF_ARM.so"]
            else:
                self.cpp_info.libs = ["libAEF_ARM.so"]
        else:
            raise Exception("Package does not exist for these settings")
