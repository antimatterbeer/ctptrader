from conan import ConanFile


class Ctptrader(ConanFile):
    generators = "CMakeToolchain", "CMakeDeps"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("boost/1.75.0")
        self.requires("fmt/10.1.1")
        self.requires("gtest/1.10.0")
        self.requires("libcurl/8.4.0")
        self.requires("rapidjson/1.1.0")
        self.requires("loguru/cci.20230406")

    def build_requirements(self):
        self.build_requires("cmake/[>=3.20]")

    def configure(self):
        self.options["boost"].shared = True

    def layout(self):
        self.folders.generators = ""
