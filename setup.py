from setuptools import setup, Extension

module = Extension("table_render", sources=["table_render.c"])

setup(
    name="table_render",
    version="1.0",
    ext_modules=[module]
)