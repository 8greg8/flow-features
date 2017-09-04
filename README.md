# Flow features

Application for flow features calculation


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 


### Prerequisites

Before installing this software you need:

* [OpenCV](http://opencv.org/) - OpenCV library >v3.1
  
  Must be compiled with contrib extra modules. Links:
    * [github - opencv](https://github.com/opencv/opencv) 
    * [github - contrib](https://github.com/opencv/opencv_contrib)


* [Boost](http://www.boost.org/) - Boost library >v1.53.0
* [CUDA]() - Cuda library >v7.5


### Installing

1. Download or clone this project
2. Change `installDir` in [`build.sh`](build.sh)
3. Run 'build.sh'


### Running programs

Run `opticalflowfeatures2` or `sceneflowfeatures2`


### System

Tested only on CentOS Linux 7.3


## Contributing

For contributing contact authors.


## Versioning

For the versions available, see the [tags on this repository](https://github.com/8greg8/flow-features/tags). 


## Authors

* **Janez Perš** - *Basic idea behind this project and team leader* - [jpers1](https://github.com/jpers1)
* **Gregor Koporec** - *Developed and tested main part of software* - [8greg8](https://github.com/8greg8)


## License

This project is licensed under the GNU GPL License - see the [LICENSE.md](LICENSE.md) file for details.


## Acknowledgments

Parts from project [PD-Flow](https://github.com/MarianoJT88/PD-Flow) were used here. Used code was modified for usage with Microsoft Kinect V2 sensor. Modifications were documented. See [modified files in pdflow](pdflow) folder for details.

Thanks to [MarianoJT88](https://github.com/MarianoJT88) for helping with modifications.
