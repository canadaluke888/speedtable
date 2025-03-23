from setuptools import setup, Extension

module = Extension(
    "speedtable.speedtable",
    sources=["speedtable/speedtable.c"]
)

setup(
    name="speedtable",
    version="1.0.2",
    description="Ultra-fast terminal table renderer written in C",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    author="Luke Canada",
    author_email="canadaluke888@gmail.com",
    url="https://github.com/canadaluke888/speedtable",
    packages=["speedtable"],
    ext_modules=[module],
    include_package_data=True,
    zip_safe=False,
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: C",
        "Operating System :: OS Independent",
        "License :: OSI Approved :: MIT License",
        "Intended Audience :: Developers",
        "Topic :: Utilities",
    ],
    python_requires=">=3.7",
)
