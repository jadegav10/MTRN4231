import os
from setuptools import setup

# Replace 'my_marker_package' with your actual package name
package_name = 'demo_launch_param'

# Define a list of launch files to include
launch_files = [
    'launch/marker_launch.py',
    'launch/rviz_launch.py',
    # Add more launch files as needed
]

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    package_dir={'': '.'},  # Assuming your Python code is in the 'src' directory
    data_files=[
        (os.path.join('share', package_name, 'launch'), launch_files),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    author='Mitchell',
    author_email='mitch.torok@gmail.com',
    maintainer='Mitchell',
    maintainer_email='mitch.torok@gmail.com',
    description='Demonstation of launch files and parameters',
    license='Apache License 2.0',
    entry_points={
        'console_scripts': [
            'marker_node = demo_launch_param.marker_node:main',
        ],
    },
)
