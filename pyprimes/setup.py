from distutils.core import setup, Extension

module_name = 'primes'

setup(
    name=module_name,
    version="1.0",
    ext_modules=[Extension(module_name, [f"{module_name}.c"])]
)
