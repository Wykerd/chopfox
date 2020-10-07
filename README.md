# Chopfox

A library for extracting information from comic strips using OpenCV and Tesseract.

# Features

- Panel detection
- Text ROI detection
- Text recognition
- Panel chopping
- XML export of extracted data

# Dependencies

- OpenCV
- Tesseract
- TinyXML

# Building

Use CMake to build this project.

```
mkdir build
cd build
cmake ..
make
```

# Web version

A minimal client side web version of Chopfox is also provided in this repo. It uses a WASM version of opencv to extract the panels of the comic strip.

See the `web/` directory for the source to the web version.

A prebundled version of the library can be downloaded in the releases tab.

# License

This project is licensed under the GNU Affero General Public License.

```
Chopfox is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Chopfox is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with Chopfox.  If not, see <https://www.gnu.org/licenses/>.
```

![GNU Affero General Public License](https://www.gnu.org/graphics/agplv3-88x31.png)

__NOTE:__ This software depends on other packages that may be licensed under different open source licenses.