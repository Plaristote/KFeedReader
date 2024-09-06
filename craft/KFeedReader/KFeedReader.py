import info
from CraftCore import CraftCore

class subinfo(info.infoclass):
    def setTargets(self):
        self.displayName = "FluxKap"
        self.description = "RSS/Atom/JSON feed aggregator"
        self.svnTargets["master"] = "https://github.com/Plaristote/KFeedReader.git|master"
        self.svnTargets["kf5"] = "https://github.com/Plaristote/KFeedReader.git|kf5"
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
    def __init__(self, **kwargs):
        CMakePackageBase.__init__(self, **kwargs)
        CMakePackageBase.buildTests = False

    def createPackage(self):
        self.defines["appname"] = "FluxKap"
        self.defines["icon"] = self.blueprintDir() / "KFeedReader.ico"                                                     
        self.defines["icon_png"] = self.sourceDir() / "src/contents/icons/app.png"
        #self.defines["shortcuts"]
        if not CraftCore.compiler.isLinux:
            self.ignoredPackages.append("libs/dbus")
        return super().createPackage()
