# EVE-MCU-Dev Arduino Example Sketches

[Back](../README.md)

A sketch needs to have additional files copied to setup it up in the Arduino IDE. 
This module `sketchlib.py` provides a mechanism for copying and "normalising" the source files of the example into an Arduino sketch.

## Usage

To use this include this library into the python script `sketchbuild.py` which is included in the `arduino` directory of examples which support Arduino.

### Add Files to Copy

To generate correct paths to source files to add use the `add_files` function.

The parameters are `(src_dir, dest_dir, file_list)`.

- `src_dir` the path to the directory containing the source files to add.
- `dest_dir` the path to the generated sketch directory.
- `file_list` a list of files in the source directory to add to the list of files to copy.

The return list is contains tuples for the source and destination files.

### Copy Files

To copy and normalise the files for the sketch call the `copy_norm` function.

The parameters are `(src, dest, flatten_filter)`.

- `src` the source file to copy.
- `dest` the destination path for the file.
- `flatten_filter` a filter to remove directory paths to produce a sketch in a single directory.

## Considerations

The sketch must be in a single flat directory. Therefore ensure filenames do not clash.
