import info

class subinfo(info.infoclass):
    def selfTargets(self):
        self.displayName = "KFeedReader"
        self.description = "RSS/Atom/JSON feed aggregator"
        self.svnTargets["master"] = "https://github.com/Plaristote/KFeedReader.git|master"
        self.defaultTarget = "master"
    def setDependencies(self):
        self.buildDependencies["dev-utils/pkg-config"] = "default"
        self.runtimeDependencies["libs/qt/qtbase"] = "default"
        self.runtimeDependencies["libs/qt/qtwebengine"] = "default"
        self.runtimeDependencies["kirigami"] = "default"
        self.runtimeDependencies["kcoreaddons"] = "default"
        self.runtimeDependencies["ki18n"] = "default"
        self.runtimeDependencies["breeze"] = "default"
        self.runtimeDependencies["kiconthemes"] = "default"
        if CraftCore.compiler.isMinGW():
            self.runtimeDependencies["libs/runtime"] = None

from Package.CMakePackageBase import *

class Package(CMakePackageBase):
    def __init__(self):
        CMakePackageBase.__init__(self)
        CMakePackageBase.buildTests = False
    def createPackage(self):
        self.defines["appname"] = "KFeedReader"
        #self.defines["icon"]
        #self.defines["shortcuts"]
        if not CraftCore.compiler.isLinux:
            self.ignoredPackages.append("libs/dbus")
        return super().createPackage()
