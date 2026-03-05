from setuptools import setup, Extension

setup(
    name='symnmf_module',
    version='1.0',
    description='symnmf clustering algorithm',
    ext_modules=[
        Extension(
            'symnmf_module', 
            sources=['symnmfmodule.c', 'symnmf.c'], 
        )
    ]
)