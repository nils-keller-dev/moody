# <img src="./assets/preview.gif" width="48" height="24" /> Moody

Moody is an Arduino project that displays a range of facial expressions on a small OLED display.


<br/>

## How It Works

The Moody project features a small OLED display that can show a variety of facial expressions. Each facial expression consists of two animation frames, and the display changes to a different facial expression after a certain amount of time. The facial expressions are arranged in a particular order so that only certain facial expressions can follow one another.

<br/>

# If you just want to build a Moody with the default configuration, head over to the [`moody-arduino`](https://github.com/tsomic/moody-arduino) repository
If you are interested in creating your own individual Moody with custom facial expressions and their order, you should continue reading.

<br/>

## Overview

The Moody project consists of three main components, each of which is contained in a separate sub-repository:

1. [`moody-arduino`](https://github.com/tsomic/moody-arduino): This repository contains all the code for the Arduino microcontroller, ready to be uploaded to an Arduino.
2. [`moody-images`](https://github.com/tsomic/moody-images): This repository includes all the images of the facial expressions.
3. [`moody-mapper`](https://github.com/tsomic/moody-mapper): This repository is a web-based tool for managing all the facial expressions and their order. Using it you can generate files to be used by the Arduino board.

<br/>

If you want to get started with creating custom facial expressions and their configuration, I recommend cloning this repository including its submodules

```bash
git clone --recurse-submodules git@github.com:tsomic/moody.git
```

and read through the `README` file of the [`moody-mapper`](https://github.com/tsomic/moody-mapper) to get a better understanding of what to do next.

<br/>

## Contributing

Contributions are welcome! If you have ideas for new features, find any bugs, or would like to make improvements, please open an issue or submit a pull request to this repository or to any of the submodules.

<br/>

## License

The Moody project and all its submodules are licensed under the GNU GPLv3 license. See the [`LICENSE`](https://github.com/tsomic/moody/blob/main/LICENSE) file for more information.
