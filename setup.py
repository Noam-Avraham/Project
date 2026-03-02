from setuptools import setup, Extension

setup(
    name='symnmf',
    version='1.0',
    description='symnmf clustering algorithm',
    ext_modules=[
        Extension(
            'symnmf', 
            sources=['symnmfmodule.c'] 
        )
    ]
)